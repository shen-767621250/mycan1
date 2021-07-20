#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bitset>
//this code can recev vcan0's massge
///usr/src/linux-hwe-5.4-headers-5.4.0-42/include/linux/can
#include <linux/can.h>
#include <linux/can/raw.h>

using namespace std;

char * strcpy(char * strDest,const char * strSrc)
   
{
if ((NULL==strDest) || (NULL==strSrc)) 
//[1]
throw "Invalid argument(s)"; 
//[2]
char * strDestCopy = strDest; 
//[3]
while ((*strDest++=*strSrc++)!='\0'); 
//[4]
return strDestCopy;
}
//转二进制
void transformT2(int bits, int buff[7]) { 

    if(bits==0)for(int i=0;i<8;i++){buff[i]=0;}
	buff[0] = (bits & 0x01) == 0x01 ? 1 : 0;
	buff[1] = (bits & 0x02) == 0x02 ? 1 : 0;
	buff[2] = (bits & 0x04) == 0x04 ? 1 : 0;
	buff[3] = (bits & 0x08) == 0x08 ? 1 : 0;
	buff[4] = (bits & 0x10) == 0x10 ? 1 : 0;
	buff[5] = (bits & 0x20) == 0x20 ? 1 : 0;
	buff[6] = (bits & 0x40) == 0x40 ? 1 : 0;
	buff[7] = (bits & 0x80) == 0x80 ? 1 : 0;

}
int main(int argc, char **argv)
{
   // int flag;
   int buffdata[7];
    int i=0;
    int byte;

    int s, nbytes;                                //socket套接字和发送字节数目
    struct sockaddr_can addr;                     //设置用结构体
    struct ifreq ifr;                             //指定can设备用结构体   
    struct can_frame frame;                       //接收数据的buffer
    struct can_filter rfilter[1];                 //过滤用的结构体
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);        //创建套接字
    strcpy(ifr.ifr_name, "vcan0" );                
    ioctl(s, SIOCGIFINDEX, &ifr);                 //指定 vcan0 设备
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr)); //将套接字与 vcan0 绑定

    rfilter[0].can_id =0x02;                        //ID shezhi
    rfilter[0].can_mask = CAN_SFF_MASK;

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)); //设置过滤规则
    while(1)                                       //while读取canbus消息
    {
        nbytes = read(s, &frame, sizeof(frame));       //接收显示报文
        if(nbytes > 0)
         {
        
        printf("ID=0x-%X DLC=%d\n", frame.can_id,frame.can_dlc);

        
        for(i=0;i<8;i++)
            {
                byte=frame.data[i];
                transformT2(byte, buffdata);
                for (int i = 0; i < 8; i++) 
                {
                     cout << buffdata[7-i] << endl;
                };
            }
        
        }

    }
    close(s);
    return 0;
}

