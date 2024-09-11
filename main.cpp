#include <iostream>
#include <thread>
int num = 0;
class Num()
{
    int num;
}

void threadtest() 
{
    for(int i = 0; i < 500000; i++)
        num++;
}

int main()
{
    std::thread t1(threadtest);
    std::thread t2(threadtest);
    t1.join();
    t2.join();
    std::cout << num << std::endl;
    return 0;
}