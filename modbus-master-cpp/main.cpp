// Derelemek için : g++ main.c -o main
#include <windows.h>
#include <iostream>
#include <thread>


HANDLE serialHandle;
bool stop=false;
char szBuff[100]={0};
DWORD dwBytesRead=0;
unsigned char sendData[100];
unsigned char sendDataChar[100];
int sendDataVal[100];
void run(){  
    while(!stop){
        ReadFile(serialHandle,szBuff,sizeof(szBuff)-1,&dwBytesRead,NULL);
        if(dwBytesRead>0){
            printf("\nResponse message :");
            for(int i=0;i<dwBytesRead;i++){
                    printf("%02x",(unsigned int)(unsigned char)szBuff[i]);
            }
            printf("\nEnter RTU Command (without CRC Ex: 0106010000f0 )(0 to Exit) :");  
        }   
    }   
}

unsigned int calculate_crc(unsigned char* data, unsigned char length){
    int j;
    unsigned int reg_crc=0xFFFF;
    while(length--){ //  data uzunluğu kadar döngü
        reg_crc ^= *data++; // reg_crc ile data exor yap:
        for(j=0;j<8;j++){
            if(reg_crc & 0x01){ /* LSB(b0)=1 sıfırıncı bit 1 ise */
            reg_crc=(reg_crc>>1) ^ 0xA001; // bir kaydır ve 0xA001 ile exor yap
        }else{ // değil se
            reg_crc=reg_crc >>1; // sadece 1 bit kaydır 
        }
    }
}
return reg_crc;
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
    readPort.joinable();
    while(strcmp((char*)sendData,"0")!=0){
        printf("\nEnter RTU Command (without CRC Ex: 0106010000f0 )(0 to Exit) :");
        std::cin>>sendData;
        if(strcmp((char*)sendData,"0")!=0){
            int commandLength=strlen((char*)sendData)/2;
            for(int i=0;i<commandLength;i++){
                unsigned char temp[3];
                temp[0]=sendData[i*2];
                temp[1]=sendData[i*2+1];
                temp[2]=0;
                sendDataVal[i]=strtol((char*)temp,NULL,16);  
                sendDataChar[i]=sendDataVal[i];          
            }
            int crc=calculate_crc(sendDataChar ,commandLength);
            sendDataChar[commandLength]=(unsigned char)(crc & 0X00ff);
            sendDataChar[commandLength+1]=(unsigned char)(crc >>8);
            sendDataChar[commandLength+2]='\0';
            printf("Calculated CRC is : 0x%02x%02x\n",(unsigned char)(crc & 0X00ff),(unsigned char)(crc >>8));
            WriteFile(serialHandle,sendDataChar,100,NULL,NULL);
        }else{
            std::cout<<"Bye ..."<<std::endl;
        }
        
    }
    stop=true;
    readPort.join();
    CloseHandle(serialHandle);
}