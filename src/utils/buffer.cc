#include"buffer.h"


Buffer::Buffer(){}
Buffer::~Buffer(){
    buffer.clear();
}

void Buffer::Append(const char *_str, int _size){
   for(int i = 0; i < _size; i ++){
        if(_str[i] != '\0')
            buffer.push_back(_str[i]);
   }
}

ssize_t Buffer::Size(){
    return buffer.size();
}

const char* Buffer::Cstr(){
    return buffer.c_str();
}

void Buffer::Clean(){
    buffer.clear();
}

std::string Buffer::getbuffer(){
	return buffer;
}
