#pragma once

#include <cstddef>
#include <vector>
// @class Buffer
// @brief Buffer for socket read and write.
//
// Use a vector<char> as the buffer.
// Three indexs for use, prependable_index, reader_index, writer_index. 1, 2, 3.
// prependable_index is the true begin. 1-2 read aleady.
// reader_index_ is the read data begin. 2-3 read data space. write aleady.
// writer_index_ is the write data begin. 3- write data space.
class Buffer {
public:
    static const size_t kCheapPrepend = 8;  
    static const size_t kInitialSize = 1024;

    explicit Buffer()
        : buffer_(kInitialSize),
          reader_index_(kCheapPrepend),
          writer_index_(kCheapPrepend) {
    }
    ~Buffer() {}

    // Read data from fd and save the error;
    size_t ReadFd(int fd, int* saved_errno);

    // Find the crf in write data.
    const char* FindCRLF();
    
    void Append(const char* data, size_t len);
    void EnsureWritableBytes(size_t len);
    void MakeSpace(size_t len);
    
    // Clean space.
    void Retrieve(size_t len);
    void RetrieveAll();
    
    // Operations about size.
    size_t PrependableBytes() const;
    size_t ReadableBytes() const;
    size_t WriteableBytes() const;
    
    // Read and Write ptr.
    char* BeginWrite();
    const char* Peek() const;
    
private:
    std::vector<char> buffer_;
    size_t reader_index_;
    size_t writer_index_;
};
