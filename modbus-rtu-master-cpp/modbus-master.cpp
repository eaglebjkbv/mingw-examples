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

int main(int argc, char **argv){  
    char portNr[20]="\\\\.\\";
    char portNrPart[5];
    bool portOK=false;
    int baudrate=9600;
    int byteSize=8;
    int stopBits=1;
    int parity=NOPARITY;

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"-n")==0){
            strcat(portNr,argv[i+1]);
            portOK=true;
        }
        if(strcmp(argv[i],"-b")==0){ 
            if(strcmp(argv[i+1],"4800")==0){
                baudrate=CBR_4800;
            }else if(strcmp(argv[i+1],"9600")==0){
                baudrate=CBR_9600;
            }else if(strcmp(argv[i+1],"19200")==0){
                baudrate=CBR_19200;
            }else if(strcmp(argv[i+1],"38400")==0){
                baudrate=CBR_38400;
            }
        }
        if(strcmp(argv[i],"-d")==0){ 
            if(strcmp(argv[i+1],"7")==0){
                byteSize=7;    
            }else if(strcmp(argv[i+1],"8")==0){
                byteSize=8;    
            }  
        }
        if(strcmp(argv[i],"-s")==0){ 
            if(strcmp(argv[i+1],"1")==0){
                stopBits=1;    
            }else if(strcmp(argv[i+1],"2")==0){
                stopBits=2;    
            }  
        }
        if(strcmp(argv[i],"-p")==0){ 
            if(strcmp(argv[i+1],"none")==0){
                parity=NOPARITY;    
            }else if(strcmp(argv[i+1],"even")==0){
                parity=EVENPARITY;    
            }else if(strcmp(argv[i+1],"odd")==0){
                parity=ODDPARITY;    
            }  
        }

    }

    if(portOK==false){
        strcat(portNr,"COM4");
    }
    std::cout<<"Usage Example: modbus-master -n COM4 -b 9600 -d 8 -s 1 -p even"<<std::endl;
    std::cout<<"Modbus Master v1.1 CPP made by BV 2020";
    std::cout<<" for using Modbus RTU commands on command prompt."<<std::endl;
    std::cout<<"_______Settings_______\n";
    
    memcpy(portNrPart,portNr+4,5);
    std::cout<<"Port Name :"<<portNrPart<<std::endl;
    std::cout<<"Baudrate  :"<<baudrate<<std::endl;
    std::cout<<"Byte Size :"<<byteSize<<std::endl;
    std::cout<<"Stop Bits :"<<stopBits<<std::endl;
    std::cout<<"Parity    :";
    if(parity==NOPARITY){
        std::cout<<"NONE"<<std::endl;
    }else if(parity==EVENPARITY){
        std::cout<<"EVEN"<<std::endl;
    }else if(parity==ODDPARITY){
        std::cout<<"ODD"<<std::endl;
    }
    

    std::cout<<"______________________"<<std::endl;
    
    
    serialHandle = CreateFile(portNr, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(serialHandle==INVALID_HANDLE_VALUE){
        std::cout<<"Connection Failed !!!"<<std::endl;
        return 1;
    }
    DCB serialParams={ 0 };
    serialParams.DCBlength=sizeof(serialParams);

    GetCommState(serialHandle,&serialParams);

    serialParams.BaudRate=baudrate;
    serialParams.ByteSize=byteSize;
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
    printf("\nEnter RTU Command (without CRC Ex: 0106010000f0 )(0 to Exit) :");
    while(strcmp((char*)sendData,"0")!=0){
        
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
            WriteFile(serialHandle,sendDataChar,strlen((char*)sendDataChar),NULL,NULL);
        }else{
            std::cout<<"Bye ..."<<std::endl;
        }
        
    }
    stop=true;
    readPort.join();
    CloseHandle(serialHandle);
}