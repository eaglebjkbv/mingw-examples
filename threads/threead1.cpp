#include <iostream>
#include <thread>
bool stop=false;
void work(){
    while(!stop){
        std::cout<<"Thread den ";
    }
}


int main()
{
    std::thread worker(work);
    std::cout<<"Durdurmak icin bir tusa basiniz";
    std::cin.get();
    stop=true;
    worker.join(); // main thread ile birleştiriliyor.
    return 0;
}