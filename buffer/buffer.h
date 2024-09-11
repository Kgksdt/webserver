#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>
#include <cstring>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <atomic>

class Buffer {
private:
    //buffer容器的头地址
    char* BufferBeginPtr_();
    const char* BufferBeginPtr_() const;
    //扩展空间
    void MakeSpace_(std::size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;

public:
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    //剩余空间大小
    std::size_t WritableBytes() const;
    std::size_t ReadableBytes() const;
    std::size_t PrependableBytes() const;

    //返回读指针
    const char* Peek() const;
    //判断空间
    void EnsureWriteable(std::size_t len); 
    //移动写的下标到len    
    void HasWritten(std::size_t len);     
    //移动读下标到len
    void Retrieve(std::size_t len);        
    //读取到end位置
    void RetrieveUntil(const char* end); 
    //取出所有数据
    void RetrieveAll();   
    //取出剩余可读str 
    std::string RetrieveAllToStr();      
    //目前写指针的位置
    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);    
    void Append(const char* str, std::size_t len);
    void Append(const void* data,std::size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);   
};
#endif //BUFFER_H