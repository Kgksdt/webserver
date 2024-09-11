#include "buffer.h"

Buffer::Buffer(int initBuffsize) :
buffer_(initBuffsize), readPos_(0), writePos_(0) {}

//剩余可写空间大小
std::size_t Buffer::WritableBytes() const {
    return buffer_.size() - writePos_;
}

//剩余可读空间大小
std::size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;
}

//剩余预留空间大小
std::size_t Buffer::PrependableBytes() const {
    return readPos_ + WritableBytes();
}

//返回读指针
const char* Buffer::Peek() const {
    return &buffer_[readPos_];
}

//判断空间
void Buffer::EnsureWriteable(std::size_t len) {
    MakeSpace_(len);
    assert(len <= WritableBytes());
}

//移动写的下标到len    
void Buffer::HasWritten(std::size_t len) {
    writePos_ += len;
}

//移动读下标到len
void Buffer::Retrieve(std::size_t len) {
    readPos_ += len;
}

//读取到end位置
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() == end);
    Retrieve(end - Peek());
}

//取出所有数据 格式化
void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size()); // 覆盖原本数据
    readPos_ = writePos_ = 0;
}

//取出剩余可读str 
std::string Buffer::RetrieveAllToStr() {
    std::string str = (Peek(), BeginWriteConst());
    RetrieveAll();
    return str;
}

//目前写指针的位置
const char* Buffer::BeginWriteConst() const {
    return &buffer_[writePos_];
}
char* Buffer::BeginWrite() {
    return &buffer_[writePos_];
}

// 添加str到缓冲区
void Buffer::Append(const char* str, std::size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const std::string& str) {
    Append(str.c_str(), str.size());
}

void Buffer::Append(const void* data, std::size_t len) {
    Append(static_cast<const char*>(data), len);
}

// 将buffer中的读下标的地方放到该buffer中的写下标位置
void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

// 将fd的内容读到缓冲区，即writable的位置
ssize_t Buffer::ReadFd(int fd, int* Errno) {
    //定义一个缓冲区
    char buff[65535];
    std::size_t writeable = WritableBytes();
    //初始化iov
    struct iovec iov[2];
    iov[0].iov_base = BeginWrite();
    iov[0].iov_len = buffer_.size() - writePos_;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        *Errno = errno;
    }
    else if (static_cast<std::size_t>(len) <= WritableBytes()) {
        //空间够
        HasWritten(writePos_ + len);
    }
    else if (static_cast<std::size_t>(len) > WritableBytes()) {
        writePos_ = buffer_.size();
        Append(buff, static_cast<std::size_t>(len - writeable));
    }
    return len;
}

// 将buffer中可读的区域写入fd中
ssize_t Buffer::WriteFd(int fd, int* Errno) {
    ssize_t len = write(fd, Peek(), ReadableBytes());
    if(len < 0) {
        *Errno = errno;
        return len;
    } 
    Retrieve(len);
    return len;
}

char* Buffer::BufferBeginPtr_() {
    return &buffer_[0];
}

const char* Buffer::BufferBeginPtr_() const{
    return &buffer_[0];
}

// 扩展空间
void Buffer::MakeSpace_(std::size_t len) {
   if(PrependableBytes() < len) {
        buffer_.resize(writePos_ + len +1);
   }
   else {
        std::size_t tmp = ReadableBytes();
        std::copy(BufferBeginPtr_() + readPos_, BufferBeginPtr_() + writePos_, BufferBeginPtr_());
        writePos_ = tmp;
        readPos_ = 0;
        assert(tmp == ReadableBytes());
   }
}
int main()
{

}