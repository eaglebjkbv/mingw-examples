#include <stdio.h>

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


int main()
{   // Ornek 1: 0000 nolu registerdan itibaren 3 adet registerdaki dataları okuyan komut 
    unsigned char arr3[]={0x01,0x03,0x00,0x00,0x00,0003};
    // ornek 2: 0000 nolu registera 0xffff bilgisini yazdıran komut
    unsigned char arr6[]={0x01,0x06,0x00,0x00,0xFF,0xFF};
    // ornek 3: 0x0001 nolu registeran itibaren 2 adet (4 byte) registera sırasıyla 0x000A,0x002 datalarını yaz.
    unsigned char arr16[]={0x01,0x10,0x00,0x01,0x00,0x02,0x04,0x00,0x0A,0x00,0x02};

    int crc3=calculate_crc(arr3,6);
    int crc6=calculate_crc(arr6,6);
    int crc16=calculate_crc(arr16,11);
    printf("Result of Command Number 3 example with CRC :");
    for(int i=0;i<6;i++){
        printf("%02x-",arr3[i]);
    }
    printf("%x\n",crc3);
    printf("Result of Command Number 6 example with CRC :");
    for(int i=0;i<6;i++){
        printf("%02x-",arr6[i]);
    }
    printf("%x\n",crc6);

    printf("Result of Command Number 16 example with CRC :");
    for(int i=0;i<11;i++){
        printf("%02x-",arr16[i]);
    }
    printf("%x\n",crc16);
    return 0;
}