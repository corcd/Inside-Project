#include "wifiDriver.h"

/*
���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
@param mode 0,������USART3_RX_STA;
						1,����USART3_RX_STA;
*/
void atk_8266_at_response(u8 mode) {
    if (USART3_RX_STA & 0X8000) {        //���յ�һ��������
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0;//���ӽ�����
        printf("response: %s", USART3_RX_BUF);    //���͵�����
        if (mode)USART3_RX_STA = 0;
    }
}

/*
ATK-ESP8266���������,�����յ���Ӧ��
@param str:�ڴ���Ӧ����
@return 0,û�еõ��ڴ���Ӧ����
				����,�ڴ�Ӧ������λ��(str��λ��)
*/
u8 *atk_8266_check_cmd(u8 *str) {
    char *strx = 0;
    if (USART3_RX_STA & 0X8000) {        //���յ�һ��������
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0;//���ӽ�����
        strx = strstr((const char *) USART3_RX_BUF, (const char *) str);
    }
    return (u8 *) strx;
}

/*
��ATK-ESP8266��������
@param cmd:���͵������ַ���
@param ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
@param waittime:�ȴ�ʱ��(��λ:10ms)
@return 0,���ͳɹ�(�õ����ڴ���Ӧ����)
        1,����ʧ��
*/
u8 atk_8266_send_cmd(u8 *cmd, u8 *ack, u16 waittime) {
    u8 res = 0;
    USART3_RX_STA = 0;
    u3_printf("%s\r\n", cmd);    //��������
    if (ack && waittime) {        //��Ҫ�ȴ�Ӧ��
        while (--waittime) {    //�ȴ�����ʱ
            delay_ms(10);
            if (USART3_RX_STA & 0X8000) {//���յ��ڴ���Ӧ����
                if (atk_8266_check_cmd(ack)) {
                    printf("ack:%s\r\n", (u8 *) ack);
                    break;//�õ���Ч����
                }
                USART3_RX_STA = 0;
            }
        }
        if (waittime == 0)res = 1;
    }
    return res;
}


/**
 * ��ATK-ESP8266����ָ������
 * @param data:���͵�����(����Ҫ���ӻس���)
 * @param dataLen ���ݳ���
 * @param ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
 * @param waittime:�ȴ�ʱ��(��λ:10ms)
 * @return 0,���ͳɹ�(�õ����ڴ���Ӧ����)
 */
u8 atk_8266_send_data(u8 *data, unsigned int dataLen,u8 *ack, u16 waittime) {
    u8 res = 0;
    USART3_RX_STA = 0;
    u3_print_data(data, dataLen);    //��������
    if (ack && waittime) {        //��Ҫ�ȴ�Ӧ��
        while (--waittime) {    //�ȴ�����ʱ
            delay_ms(10);
            if (USART3_RX_STA & 0X8000) {//���յ��ڴ���Ӧ����
                if (atk_8266_check_cmd(ack)) {
                    printf("ack:%s\r\n", (u8 *) ack);
                    break;//�õ���Ч����
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
ATK-ESP8266�˳�͸��ģʽ
@return 0,�˳��ɹ�;
       1,�˳�ʧ��
*/
u8 atk_8266_quit_trans(void) {
    while ((USART3->SR & 0X40) == 0);    //�ȴ����Ϳ�
    USART3->DR = '+';
    delay_ms(15);                    //���ڴ�����֡ʱ��(10ms)
    while ((USART3->SR & 0X40) == 0);    //�ȴ����Ϳ�
    USART3->DR = '+';
    delay_ms(15);                    //���ڴ�����֡ʱ��(10ms)
    while ((USART3->SR & 0X40) == 0);    //�ȴ����Ϳ�
    USART3->DR = '+';
    delay_ms(500);                    //�ȴ�500ms
    return atk_8266_send_cmd("AT", "OK", 20);//�˳�͸���ж�.
}


/*
��ȡATK-ESP8266ģ���AP+STA����״̬
@return 0��δ����;1,���ӳɹ�
*/
u8 atk_8266_apsta_check(void) {
    if (atk_8266_quit_trans())return 0;            //�˳�͸��
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50);    //����AT+CIPSTATUSָ��,��ѯ����״̬
    if (atk_8266_check_cmd("+CIPSTATUS:0") &&
        atk_8266_check_cmd("+CIPSTATUS:1") &&
        atk_8266_check_cmd("+CIPSTATUS:2") &&
        atk_8266_check_cmd("+CIPSTATUS:4"))
        return 0;
    else return 1;
}

/*
��ȡATK-ESP8266ģ�������״̬
@return 0,δ����;1,���ӳɹ�.
*/
u8 atk_8266_consta_check(void) {
    u8 *p;
    u8 res;
    if (atk_8266_quit_trans())return 0;            //�˳�͸��
    atk_8266_send_cmd("AT+CIPSTATUS", ":", 50);    //����AT+CIPSTATUSָ��,��ѯ����״̬
    p = atk_8266_check_cmd("+CIPSTATUS:");
    res = *p;                                    //�õ�����״̬
    return res;
}




