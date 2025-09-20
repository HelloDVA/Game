#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "asynclogger.h"

AsyncLogger::AsyncLogger(const std::string& basename, size_t rollSize, int flushInterval)
  : basename_(basename),
    basepath_("../bin/log"),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    running_(false),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    fp_(nullptr) {
  currentBuffer_->reserve(4 * 1024);  // 4KB初始缓冲区
  nextBuffer_->reserve(4 * 1024);
}

AsyncLogger::~AsyncLogger() {
  if (running_) {
      Stop();
  }
}

void AsyncLogger::Start() {
  running_ = true;
  thread_ = std::thread(&AsyncLogger::ThreadFunc, this);  
}

void AsyncLogger::Stop() {
  running_ = false;
  if (thread_.joinable())
    thread_.join();
  cv_.notify_all();
}

void AsyncLogger::Append(const char* data, size_t len) {
  // small data stack process 
  // big data heap process
  if (len <= 256) {
    char temp[256];
    memcpy(temp, data, len);
    Log(temp, len);
  } else {
    std::string big_temp = (data); 
    Log(big_temp.data(), big_temp.size());
  }
}

void AsyncLogger::Flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (fp_) {
      fflush(fp_);
    }
}

void AsyncLogger::Log(const char* data, size_t len) {
  std::unique_lock<std::mutex> lock(mutex_);
  
  // space enough process or exchange use the second  
  int space = currentBuffer_->capacity() - currentBuffer_->size();
  if (space >= len)
    currentBuffer_->insert(currentBuffer_->end(), data, data + len);
  else {
    buffers_.push_back(currentBuffer_);
    currentBuffer_.reset();   // give out the own shared_ptr 
    
    if (nextBuffer_)
      currentBuffer_ = std::move(nextBuffer_);
    else {
      currentBuffer_.reset(new Buffer);
      currentBuffer_->reserve(4 * 1024);
    }
    
    currentBuffer_->insert(currentBuffer_->end(), data, data + len);
    cv_.notify_one();
  }
}

void AsyncLogger::ThreadFunc() {
  
  // prepare for write data from buffer to file 
  BufferPtr new_buffer1(new Buffer);   
  BufferPtr new_buffer2(new Buffer);
  new_buffer1->reserve(4 * 1024);
  new_buffer2->reserve(4 * 1024);
  BufferVector write_buffers;
  write_buffers.reserve(16);
  time_t last_roll = time(nullptr);

  // regiester log event use loop
  while (running_) {
    // get data
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (buffers_.empty()) 
        cv_.wait_for(lock, std::chrono::seconds(1));
      
      // before the write, clloect all log data
      buffers_.push_back(currentBuffer_);
      currentBuffer_ = std::move(new_buffer1);    // use prepared buffer to exchange. more faster.
      write_buffers.swap(buffers_);
    
      // check the second buffer, and add timely.
      if (!nextBuffer_)
        nextBuffer_ = std::move(new_buffer2);
    }
    
    // when the size is too big, delete them only keep two for use.
    // write down this message to log
    if (write_buffers.size() > 25) {
      char time_buf[80];
      std::time_t t = std::time(nullptr);
      std::strftime(time_buf, sizeof(time_buf), "%F %T", std::localtime(&t)); 

      char buf[256];
      snprintf(buf, sizeof buf, "Dropped log messages at %s\n",
              time_buf);
      fputs(buf, fp_);
      write_buffers.erase(write_buffers.begin() + 2, write_buffers.end());
    }
    
    // write data
    for (const auto& buffer : write_buffers) {
      if (!fp_) {
        RollFile();
        last_roll = time(nullptr);
      }
      fwrite(buffer->data(), 1, buffer->size(), fp_);

      // whether need roll log. one day or full.
      if (ftell(fp_) > rollSize_) {
        RollFile();
        last_roll = time(nullptr);
      } else if (time(nullptr) - last_roll >= 86400) {
        RollFile();
        last_roll = time(nullptr);
      }
    }

    // reset spare buffer
    if (!new_buffer1) {
      new_buffer1 = write_buffers.back();
      write_buffers.pop_back();
      new_buffer1->clear();
    }
    if (!new_buffer2) {
      new_buffer2 = write_buffers.back();
      write_buffers.pop_back();
      new_buffer2->clear();
    }

    write_buffers.clear();
    fflush(fp_);
  }
  
  // log close.
  fflush(fp_);
  if (fp_)
    fclose(fp_);
}

void AsyncLogger::RollFile() {
  time_t now = time(nullptr);
  char filename[256];
  strftime(filename, sizeof filename, "%Y%m%d-%H%M%S", localtime(&now));

  // make sure the basepath_ is right
  if (basepath_.back() != '/')
    basepath_ += '/';

  // make sure the dir is right
  struct stat st;
  if (stat(basepath_.c_str(), &st) != 0) {
      // 目录不存在，创建它
      mkdir(basepath_.c_str(), 0755);
  }
  
  std::string log_file = basepath_ + basename_ + "." + filename + ".lgo";
  FILE* fp = fopen(log_file.c_str(), "ae");
  if (fp) {
    if (fp_)
      fclose(fp_);
    fp_ = fp;
  }
}


