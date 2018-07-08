#include "wifiDriver.h"

/*
将收到的AT指令应答数据返回给电脑串口
@param mode 0,不清零USART3_RX_STA;
						1,清零USART3_RX_STA;
*/
void atk_8266_at_response(u8 mode) {
    if (USART3_RX_STA & 0X8000) {        //接收到一次数据了
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0;//添加结束符
        printf("response: %s", USART3_RX_BUF);    //发送到串口
        if (mode)USART3_RX_STA = 0;
    }
}

/*
ATK-ESP8266发送命令后,检测接收到的应答
@param str:期待的应答结果
@return 0,没有得到期待的应答结果
				其他,期待应答结果的位置(str的位置)
*/
u8 *atk_8266_check_cmd(u8 *str) {
    char *strx = 0;
    if (USART3_RX_STA & 0X8000) {        //接收到一次数据了
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0;//添加结束符
        strx = strstr((const char *) USART3_RX_BUF, (const char *) str);
    }
    return (u8 *) strx;
}

/*
向ATK-ESP8266发送命令
@param cmd:发送的命令字符串
@param ack:期待的应答结果,如果为空,则表示不需要等待应答
@param waittime:等待时间(单位:10ms)
@return 0,发送成功(得到了期待的应答结果)
        1,发送失败
*/
u8 atk_8266_send_cmd(u8 *cmd, u8 *ack, u16 waittime) {
    u8 res = 0;
    USART3_RX_STA = 0;
    u3_printf("%s\r\n", cmd);    //发送命令
    if (ack && waittime) {        //需要等待应答
        while (--waittime) {    //等待倒计时
            delay_ms(10);
            if (USART3_RX_STA & 0X8000) {//接收到期待的应答结果
                if (atk_8266_check_cmd(ack)) {
                    printf("ack:%s\r\n", (u8 *) ack);
                    break;//得到有效数据
                }
                USART3_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}


/**
 * 向ATK-ESP8266发送指定数据
 * @param data:发送的数据(不需要添加回车了)
 * @param dataLen 数据长度
 * @param ack:期待的应答结果,如果为空,则表示不需要等待应答
 * @param waittime:等待时间(单位:10ms)
 * @return 0,发送成功(得到了期待的应答结果)
 */
u8 atk_8266_send_data(u8 *data, unsigned int dataLen,u8 *ack, u16 waittime) {
    u8 res = 0;
    USART3_RX_STA = 0;
    u3_print_data(data, dataLen);    //发送命令
    if (ack && waittime) {        //需要等待应答
        while (--waittime) {    //等待倒计时
            delay_ms(10);
            if (USART3_RX_STA & 0X8000) {//接收到期待的应答结果
                if (atk_8266_check_cmd(ack)) {
                    printf("ack:%s\r\n", (u8 *) ack);
                    break;//得到有效数据
                }
            }
        }
        if (waittime == 0) {
            res = 1;
        }
    }
    return res;
}

/*
ATK-ESP8266退出透传模式
@return 0,退出成功;
       1,退出失败
*/
u8 atk_8266_quit_trans(void) {
    while ((USART3->SR & 0X40) == 0);    //等待发送空
    USART3->DR = '+';
    delay_ms(15);                    //大于串口组帧时间(10ms)
    while ((USART3->SR & 0X40) == 0);    //等待发送空
    USART3->DR = '+';
    delay_ms(15);                    //大于串口组帧时间(10ms)
    while ((USART3->SR & 0X40) == 0);    //等待发送空
    USART3->DR = '+';
    delay_ms(500);                    //等待500ms
    return atk_8266_send_cmd("AT", "OK", 20);//退出透传判断.
}


/*
获取ATK-ESP8266模块的AP+STA连接状态
@return 0，未连接;1,连接成功
*/
u8 atk_8266_apsta_check(void) {
    if (atk_8266_quit_trans())return 0;            //退出透传
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50);    //发送AT+CIPSTATUS指令,查询连接状态
    if (atk_8266_check_cmd("+CIPSTATUS:0") &&
        atk_8266_check_cmd("+CIPSTATUS:1") &&
        atk_8266_check_cmd("+CIPSTATUS:2") &&
        atk_8266_check_cmd("+CIPSTATUS:4"))
        return 0;
    else return 1;
}

/*
获取ATK-ESP8266模块的连接状态
@return 0,未连接;1,连接成功.
*/
u8 atk_8266_consta_check(void) {
    u8 *p;
    u8 res;
    if (atk_8266_quit_trans())return 0;            //退出透传
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50);    //发送AT+CIPSTATUS指令,查询连接状态
    p = atk_8266_check_cmd("+CIPSTATUS:");
    res = *p;                                    //得到连接状态
    return res;
}





