// Compile Command : g++ -g -Wall -std=c++0x -pthread serialport.cpp -o serialport

#include <windows.h>
#include <iostream>
#include <thread>


HANDLE serialHandle;
bool stop=false;
char szBuff[9]={0};
DWORD dwBytesRead=0;

void run(){
    
    while(!stop){
        
        ReadFile(serialHandle,szBuff,sizeof(szBuff)-1,&dwBytesRead,NULL);
        if(dwBytesRead>0){
            printf("Gelen Mesaj :");
            for(int i=0;i<(int)sizeof(szBuff)-1;i++){
                if(i<(int)sizeof(szBuff)-2){
                    printf("%02X:",(unsigned int)(unsigned char)szBuff[i]);
                }else
                {
                    printf("%02X",(unsigned int)(unsigned char)szBuff[i]);
                }
                

            }
            printf("\nPress any key for exit...\n");
        }
        
    }
    
}

int main(){
    

    
    serialHandle = CreateFile("\\\\.\\COM4", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    DCB serialParams={ 0 };
    serialParams.DCBlength=sizeof(serialParams);

    GetCommState(serialHandle,&serialParams);

    serialParams.BaudRate=CBR_9600;
    serialParams.ByteSize=8;
    serialParams.StopBits=ONESTOPBIT;
    serialParams.Parity=NOPARITY;
    SetCommState(serialHandle,&serialParams);

    COMMTIMEOUTS timeout={ 0 };
    timeout.ReadIntervalTimeout=1;
    timeout.ReadTotalTimeoutConstant=1;
    timeout.ReadTotalTimeoutMultiplier = 1;
    timeout.WriteTotalTimeoutConstant = 1;
    timeout.WriteTotalTimeoutMultiplier = 1;

    SetCommTimeouts(serialHandle, &timeout);
    
    std::thread readPort(run);
    readPort.detach();
    printf("\nPress any key for exit...\n");
    std::cin.get();
    stop=true;
    
    CloseHandle(serialHandle);

}