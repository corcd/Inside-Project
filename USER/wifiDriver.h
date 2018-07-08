#ifndef __WIFI_ESP8266_DIRVER__
#define __WIFI_ESP8266_DIRVER__
#include "usart3.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

/*
该头文件定义一系列与ESP8266 wifi模块有关的函数，用于驱动wifi模块工作
*/



/*
将收到的AT指令应答数据返回给电脑串口
@param mode 0,不清零USART3_RX_STA;
						1,清零USART3_RX_STA;
*/
void atk_8266_at_response(u8 mode);

/*
ATK-ESP8266发送命令后,检测接收到的应答
@param str:期待的应答结果
@return 0,没有得到期待的应答结果
				其他,期待应答结果的位置(str的位置)
*/
u8* atk_8266_check_cmd(u8 *str);



/*
向ATK-ESP8266发送命令
@param cmd:发送的命令字符串
@param ack:期待的应答结果,如果为空,则表示不需要等待应答
@param waittime:等待时间(单位:10ms)
@return 0,发送成功(得到了期待的应答结果)
        1,发送失败
*/
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

/*
向ATK-ESP8266发送指定数据
@param data:发送的数据(不需要添加回车了)
@param ack:期待的应答结果,如果为空,则表示不需要等待应答
@param waittime:等待时间(单位:10ms)
@return 0,发送成功(得到了期待的应答结果)
*/
u8 atk_8266_send_data(u8 *data, unsigned int dataLen,u8 *ack, u16 waittime);


/*
ATK-ESP8266退出透传模式
@return 0,退出成功;
       1,退出失败
*/
u8 atk_8266_quit_trans(void);

/*
获取ATK-ESP8266模块的AP+STA连接状态
@return 0，未连接;1,连接成功
*/
u8 atk_8266_apsta_check(void);
/*
获取ATK-ESP8266模块的连接状态
@return 0,未连接;1,连接成功.
*/
u8 atk_8266_consta_check(void);

#endif
