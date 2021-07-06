//this is a Incomplete code 

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    S_CanFrame sendframe, recvframe;
    byte *psendframe = (byte *)&sendframe;
    byte *precvframe = (byte *)&recvframe;
    u_canframe_data_t *psend_data = (u_canframe_data_t *)sendframe.data;
    const int can_frame_len = sizeof(S_CanFrame); 

    pid_t pid = -1;
    int   status;

    int  ret = 0;
    char buf[128] = {0};
    bool carry_bit = false;// 进位标志

    int segment_id;//id for shared memo


    if (parse_options(argc, argv))
    {
        usage();    return  0;
    }

    if (!find_can(port))
    {
        sprintf(buf, "\n\t错误：CAN%d设备不存在\n\n", port + 1);
        panic(buf);
        return  -1;
    }

    close_can(port);// 必须先关闭CAN，才能成功设置CAN波特率
    set_bitrate(port, bitrate);// 操作CAN之前，先要设置波特率
    open_can(port, bitrate);

    send_socket_fd = socket_connect(port);
    recv_socket_fd = socket_connect(port);
    //printf("send_socket_fd = %d, recv_socket_fd = %d\n", send_socket_fd, recv_socket_fd);
    if (send_socket_fd < 0 || send_socket_fd < 0)
    {
        disconnect(&send_socket_fd);
        disconnect(&recv_socket_fd);
        panic("\n\t打开socket can错误\n\n");
        return  -1;
    }
    set_can_filter();
    set_can_loopback(send_socket_fd, lp);

    printf_head();

    memset(&sendframe, 0x00, sizeof(sendframe));
    memset(&recvframe, 0x00, sizeof(recvframe));

    if (extended_frame) // 指定发送帧类型：扩展帧或标准帧
    {
        sendframe.can_id = (send_frame_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    } 
    else
    {
        sendframe.can_id = (send_frame_id & CAN_SFF_MASK);
    }
    sendframe.can_dlc = dlc;
    memcpy(sendframe.data, send_frame_data, dlc);

    
    segment_id = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);// allocate memo
    pframeno = (int *)shmat(segment_id, NULL, 0);// attach the memo
    if (pframeno == NULL)
    {
        panic("\n\t创建共享内存失败\n\n");
        return  -1;
    }
    *pframeno = 1;

    run = true;

    pid = fork();
    if(pid == -1) 
    { 
        panic("\n\t创建进程失败\n\n");
        return  -1;
    }
    else if(pid == 0) // 子进程，用于发送CAN帧
    {
        while (run && (send_frame_times > 0))
        {
            ret = send_frame(send_socket_fd, (char *)&sendframe, sizeof(sendframe));
            printf_frame(sendframe.can_id & CAN_EFF_MASK, sendframe.data, sendframe.can_dlc, 
                ((sendframe.can_id & CAN_EFF_FLAG) ? true : false),
                ret > 0 ? true : false, 
                true);
            delay_ms(send_frame_freq_ms);

            if (send_frame_id_inc_en)
            {
                sendframe.can_id++;
                if (extended_frame)
                {
                    sendframe.can_id = (sendframe.can_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
                } 
                else
                {
                    sendframe.can_id = (sendframe.can_id & CAN_SFF_MASK);
                }
            }

            if (send_frame_data_inc_en && dlc > 0)
            {
                if (dlc > 4 && psend_data->s.dl == ((__u32)0xFFFFFFFF))
                {
                    carry_bit = true;// 发生进位
                }
                psend_data->s.dl++;

                if (dlc <= 4)
                {
                    if (psend_data->s.dl >= (1 << (dlc * 8)))
                    {
                        psend_data->s.dl = 0;
                    }
                }
                else if (dlc <= 8)
                {
                    if (carry_bit)
                    {
                        psend_data->s.dh++;
                        if (psend_data->s.dh >= (1 << ((dlc - 4) * 8)))
                        {
                            psend_data->s.dh = 0;
                        }

                        carry_bit = false;
                    }
                }
            }

            send_frame_times--;
        }

        exit(0);
    }
    else // 父进程，接收CAN帧
    {
        install_sig();

        while (run)
        {
            memset(precvframe, 0x00, can_frame_len);
            ret = recv_frame(recv_socket_fd, precvframe, can_frame_len, 5 * 1000);
            if (ret > 0)
            {
                printf_frame(recvframe.can_id & CAN_EFF_MASK, recvframe.data, recvframe.can_dlc, 
                    ((recvframe.can_id & CAN_EFF_FLAG) ? true : false),
                    true, 
                    false);
            }
        }

        while(((pid = wait(&status)) == -1) && (errno == EINTR))
        {
            delay_ms(10);
        }
    }

    disconnect(&send_socket_fd);
    disconnect(&recv_socket_fd);

    shmdt(pframeno);// detach memo
    shmctl(segment_id, IPC_RMID, NULL);// remove

    return  0;
}
