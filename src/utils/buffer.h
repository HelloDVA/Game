
#pragma once
#include<string>

class Buffer
{
    private:
        std::string buffer;
        
    public:
        Buffer();
        ~Buffer();
        void Append(const char *_str, int _size);
        ssize_t Size();
        const char *Cstr();
		std::string getbuffer();
        void Clean();

};
