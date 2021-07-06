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

    rfilter[0].can_id =0x11;                        //定义接收规则，只接收表示符等于 0x11 的报文
    rfilter[0].can_mask = CAN_SFF_MASK;

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)); //设置过滤规则
    while(1)                                       //canbus消息
    {
    nbytes = write(s, &frame, sizeof(frame));       
        if(nbytes > 0)
         {
        printf("ID=0x%X DLC=%d data[0]=0x%X\n", frame.can_id,frame.can_dlc, frame.data[0]);
         }
    }
    close(s);
    return 0;
}