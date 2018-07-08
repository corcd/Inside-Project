#ifndef __ESP8266_H__
#define __ESP8266_H__


typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

extern volatile unsigned long sys_tick;

#define RECV_BUF_SIZE  96//定义接收的缓存，尽可能的大，防止溢出
#define TIME_OUT 100

#define AI_LINK 0
#define ESP_TOUCH 1
#define AIR_LINK 2

#define STATUS_GETIP 2 //获取到IP
#define STATUS_GETLINK 3 //建立连接
#define STATUS_LOSTLINK 4 //失去连接
#define STATUS_LOSTIP 5 //未获取到IP

//function
void AutoLink(void);
int WifiInit(const char *addr, uint32_t port);
void timer1msINT(void);
unsigned long millis(void);
void delay(unsigned int ms);
int SetBaud(uint32_t baud);
int restart(void);
int setOprToStationSoftAP(void);
int smartLink(uint8_t  type,char *link_msg);
int stopSmartLink(void);
int getSystemStatus(void);
int disableMUX(void);
int createTCP(const char *addr, uint32_t port);
int send(const uint8_t *buffer, uint32_t len);
int recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout);
void rx_empty(void);

#endif