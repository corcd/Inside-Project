#include "wifiSta.h"
#include <string.h>

//连接端口号:8086,可自行修改为其他端口.
const u8 *portnum = "1600";

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8 *wifista_ssid = "danbu";            //路由器SSID号
const u8 *wifista_password = "danbu1996";    //连接密码

const u8 *wifista_server_ip = "39.108.176.84"; // UDP 服务器

/*
检查Wifi模块是否连接
*/
void checkModuleOn(void) {
    //检查WIFI模块是否在线
    printf("Check wifi module...\r\n");
//    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
    while (atk_8266_send_cmd("AT", "OK", 20)) {
        atk_8266_quit_trans();//退出透传
        atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200);  //关闭透传模式
        printf("未检测到Wifi模块!!!\r\n");

        printf("尝试重连Wifi模块...\r\n");

//        BEEP = 1;
//        delay_ms(500);
//        BEEP = 0;
//        delay_ms(500);
//        BEEP = 1;
//        delay_ms(1000);
//        BEEP = 0;
    }
    while (atk_8266_send_cmd("ATE0", "OK", 20));//关闭回显
    printf("Wifi module OK\r\n");
}

/*
设置STA UDP 模式，然后重启模块
*/
void settingUDPMod(void) {
    // UDP 配置
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);        //设置WIFI STA模式
    atk_8266_send_cmd("AT+RST", "OK", 20);        //DHCP服务器关闭(仅AP模式有效)
    printf("Setting STA UDP mod...\r\n");
    //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
}

/*
连接路由器
@param ssid 路由器名称
@param password 口令
*/
void connectAP(const u8 *ssid, const u8 *password) {
    u8 *p;
    //p = mymalloc(SRAMIN, 32);                            //申请32字节内存
    printf("Connect to AP %s\r\n", ssid);

    //设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    sprintf((char *) p, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);//设置无线参数:ssid,密码
    while (atk_8266_send_cmd(p, "WIFI GOT IP", 300));                    //连接目标路由器,并且获得IP


    delay_ms(1000); // 延时使模块生效
    delay_ms(1000);
    delay_ms(1000);
    printf("AP connected!\r\n");

    //myfree(SRAMIN, p); // 释放内存
}

/*
获取从AP得到的IP地址
@param ipbuf 缓存变量，用于缓存IP地址字符串
@return 0 获取成功
				1 获取失败
*/
u8 getCurrentIP(u8 *ipbuf) {
    u8 *p, *p1;
    p = mymalloc(SRAMIN, 32);                            //申请32字节内存
    // 获取IP
    if (atk_8266_send_cmd("AT+CIFSR", "OK", 50)) { //获取WAN IP地址失败
        printf("Get IP address from AP failed!\r\n");
        return 1;
    }

    p = atk_8266_check_cmd("\"");
    p1 = (u8 *) strstr((const char *) (p + 1), "\"");
    *p1 = 0;
    sprintf((char *) ipbuf, "%s", p + 1);
    myfree(SRAMIN, p); // 释放内存
    return (u8) 0;
}


/*
配置UDP服务器
@param ip UDP服务器的IP
@param prot UDP服务器端口号
*/
void configUDPServer(const u8 *ip, const u8 *port) {
    u8 *p;
    p = mymalloc(SRAMIN, 32);                            //申请32字节内存

    printf("Configuration UDP server...\t\n");
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20);  //单链接模式
    delay_ms(200);
    sprintf((char *) p, "AT+CIPSTART=\"UDP\",\"%s\",%s", ip, port);    //配置目标UDP服务器
    printf("%s\r\n", p);
    while (atk_8266_send_cmd(p, "OK", 500));
    myfree(SRAMIN, p); // 释放内存
}

/*
发送UDP消息
@param msg 发送的消息内容
@param msgLen 发送消息字节数
*/
void sendUDPMsg(u8 *msg, unsigned int msgLen) {
//    unsigned int i = 0;
    u8 *p;
    p = mymalloc(SRAMIN, 32);                            //申请32字节内存

//    printf("Send msg length:%d, content:\r\n", msgLen);
//    for (i = 0; i < msgLen; ++i) {
//        if (i % 4 == 0) {
//            printf("\r\n");
//        }
//        printf("%02x  ", *(msg + i));
//    }
//    printf("\r\n");


    sprintf((char *) p, "AT+CIPSEND=%d", msgLen);
    atk_8266_send_cmd(p, "OK", 200);  //发送指定长度的数据
    delay_ms(200);
    atk_8266_send_data(msg, msgLen, "OK", 200);  //发送指定长度的数据
    myfree(SRAMIN, p); // 释放内存

}


/*
检查UDP服务器连接状态
@return 0 连接正常
				1 连接异常
*/
u8 checkUDPServerStat(void) {
    u8 res;
    //得到连接状态
    if (atk_8266_consta_check() == '+') res = 0;
    else res = 1;
    USART3_RX_STA = 0;
    return res;
}


/**
 * 获取已经接收到的消息
 * @param dataLen 有效数据长度
 */
unsigned char *getReceivedMsg(unsigned short *dataLen) {
    unsigned short dataStartIndex = 0;
    unsigned char *msgBuffer = NULL;
//    unsigned short i = 0;
    if (USART3_RX_STA & 0X8000) {    //接收到一次数据了
        *dataLen = USART3_RX_STA & 0X7FFF;    //得到本次接收到的数据长度
//        printf("data len:%d\r\n", *dataLen);
//
//        for (i = 0; i < *dataLen; i++) {
//            if(i % 4 == 0) printf("\r\n");
//            printf("%02x  ", USART3_RX_BUF[i]);
//        }
//        printf("\r\n");

        // 寻找分割有效数据的开始符号
        for (dataStartIndex = 0; dataStartIndex < *dataLen; ++dataStartIndex) {
            if (USART3_RX_BUF[dataStartIndex] == ':') {
                dataStartIndex++;
//                printf("find data index:%d\r\n", dataStartIndex);
                *dataLen = *dataLen - dataStartIndex;
                break;
            }
        }
        if (dataStartIndex == (USART3_RX_STA & 0X7FFF)) dataStartIndex = 0;
        msgBuffer = (unsigned char *) mymalloc(SRAMIN, *dataLen);
        memcpy(msgBuffer, USART3_RX_BUF + dataStartIndex, *dataLen);
    } else {
        msgBuffer = NULL;
        dataLen = 0;
    }
    USART3_RX_STA = 0; // 清除串口3缓存
    return msgBuffer;
}

void testSTA(void) {
    short t = 999;
    unsigned char *msg;
    unsigned short len, i;                          //申请32字节内存

    // -------------------- 配置模式 ---------------------
    //检查WIFI模块是否在线
    checkModuleOn();

    // UDP 配置
    settingUDPMod();

    // 连接路由器
    connectAP(wifista_ssid, wifista_password);

    // 配置UDO服务器
    configUDPServer(wifista_server_ip, portnum);
    // ------------------- 配置模式结束 -------------------

    USART3_RX_STA = 0; // 清除串口3缓存
    while (1) {

        if (t == 500) {
            sendUDPMsg("Test message from STM32F1", 25);
        }
        t++;
        if (t == 1000) {//连续10秒钟没有收到任何数据,检查连接是不是还存在.
            if (checkUDPServerStat() == 0)printf("连接状态OK\r\n");  //连接状态
            else printf("连接失败\r\n");
            t = 0;
        }
        delay_ms(10);

        // 获取收到的消息
        msg = getReceivedMsg(&len);
        if (msg != NULL && len > 0) {
            printf("dataLen: %d\r\n", len);
            for (i = 0; i < len; ++i) {
                printf("%02x ", msg[i]);
            }
            printf("\r\n");
            myfree(SRAMIN, msg);
            msg = NULL;
        }
    }
}
