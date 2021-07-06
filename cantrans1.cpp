#include <iostream>

#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

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

int main(int argc, char **argv)
{
    int s, nbytes;                                //socket套接字和发送字节数目
    struct sockaddr_can addr;                     //设置结构体
    struct ifreq ifr;                             //指定can设备结构体   
    struct can_frame frame;                       //send-buffer
    struct can_filter rfilter[1];                 //过滤结构体
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);        //创建套接字

    strcpy(ifr.ifr_name, "vcan0" );                
    ioctl(s, SIOCGIFINDEX, &ifr);                 //指定 vcan0
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr)); //将套接字与 vcan0 绑定
     
    /*
    struct can_filter rfilter[2];
    rfilter[0].can_id   = 0x123;
    rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[1].can_id   = 0x200;
    rfilter[1].can_mask = 0x700;
    */
    rfilter[0].can_id =0x03;                        //ID
    rfilter[0].can_mask = CAN_SFF_MASK;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)); //设置过滤规则
    int flag=0;
        for(int i=0;i<8;i++)
        {
            frame.data[i]=1;
        };
    while(1)                                       //canbus
    {
        cin >> flag;
        if(flag!=0)
        {
            //write data in  frame.data
            nbytes = write(s, &frame, sizeof(frame));       //sendcan
            if(nbytes > 0)
            {
            printf("ID=0x-%X DLC=%d\n", frame.can_id,frame.can_dlc);
            }
            flag=0;
        }
        
    }
    close(s);
    return 0;
}