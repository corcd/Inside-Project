#ifndef __WIFI_STA__
#define __WIFI_STA__


#include "wifiDriver.h"
#include "malloc.h"	  
//#include "beep.h"
#include "usart.h"	 


/*
检查Wifi模块是否连接
*/
void checkModuleOn(void);

/*
设置STA UDP 模式，然后重启模块
*/
void settingUDPMod(void);
/*
连接路由器
@param ssid 路由器名称
@param password 口令
*/
void connectAP(const u8 * ssid,const u8 * password);

/*
获取从AP得到的IP地址
@param ipbuf 缓存变量，用于缓存IP地址字符串
@return 0 获取成功
				1 获取失败
*/
u8 getCurrentIP(u8* ipbuf);

/*
配置UDP服务器
@param ip UDP服务器的IP
@param prot UDP服务器端口号
*/
void configUDPServer(const u8 * ip, const u8 * port);

/*
发送UDP消息
@param msg 发送的消息内容
@param msgLen 发送消息字节数
*/
void sendUDPMsg(u8 * msg,unsigned int msgLen);


/*
检查UDP服务器连接状态
@return 0 连接正常
				1 连接异常
*/
u8 checkUDPServerStat(void);

/**
 * 获取已经接收到的消息
 * @param dataLen 有效数据长度
 */
unsigned char *getReceivedMsg(unsigned short *dataLen);


/*
STA UDP连接测试
*/
void testSTA(void);

#endif
