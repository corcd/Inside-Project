//
//  Created by 梦想家泽 on 2018/6/12.
//  Copyright ? 2018年 whzcorcd. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hanzi.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"

void app_init();
void game_help();
void game_single();
void game_net();
void game_settings();

void Disp_Line(u8 x0, u8 y0, u8 x1, u8 y1, u8 Color);
void LCD_Draw_Point(u16 x, u16 y, u16 color);
void FillCircle(u16 x0, u16 y0, u16 r, u16 Color);
void lcd_line(u16 x1, u16 y1, u16 x2, u16 y2, u16 Color);

void PutChess(u16 side, u16 m,u16 n);
void DrawChess(u16 side, u8 mode, u8 ch_s);
void Judge(u8 map[15][15]);
int win();
int IfWin(u16 x, u16 y, u16 side);
void Result(u8 flag);
void DrawSide(u16 side);
void DrawChessboard();
void DrawWin();
void DrawFailed();
void test(u16 x, u16 y, u8 test);

u8 enable = 1;		  //主菜单响应变量，默认为1
u8 enable_test =0;  //测试选项，默认为0
u8 map[15][15] = {0}; //棋谱 Map
u16 side = 0;		  //当前执子的颜色
u8 mode = -1;		  //游戏模式
u16 ch_s = 0;		  //开局选择的颜色
u16 x = -1;			  //预定义坐标x
u16 y = -1;			  //预定义坐标y
u8 dx = 0;			  //x,y增量
u8 dy = 0;
u16 temp = 0; //起点终点互换中间值
u8 inc = 0;
int i = 0; //棋盘初始坐标
int j = 0;
int t = 0;
int left_x = 0;
int right_x = 0;
int up_y = 0;
int down_y = 0;

struct coordinate
{
	u16 chess_x;
  u16 chess_y;
	u16 m;//横坐标所在行
	u16 n;//纵坐标所在行
};


struct coordinate a={0,0,0,0};

void app_init()
{
	u8 key;
	u8 i = 0;

	LCD_Clear(BLACK);
	enable = 1;
	side = 0;
	mode = -1;
	x = -1;
	y = -1;
	
	memset(map,0,sizeof(u8)*15*15);


	//
	fWriteHz24(GB_24[0].Msk, 50, 30, 2, RED);
	fWriteHz24(GB_24[1].Msk, 100, 30, 2, RED);
	fWriteHz24(GB_24[2].Msk, 150, 30, 2, RED);
	//
	fWriteHz24(GB_24[3].Msk, 65, 120, 1, GBLUE);
	fWriteHz24(GB_24[4].Msk, 95, 120, 1, GBLUE);
	fWriteHz24(GB_24[5].Msk, 125, 120, 1, GBLUE);
	fWriteHz24(GB_24[6].Msk, 155, 120, 1, GBLUE);
	//
	fWriteHz24(GB_24[7].Msk, 65, 160, 1, GBLUE);
	fWriteHz24(GB_24[8].Msk, 95, 160, 1, GBLUE);
	fWriteHz24(GB_24[9].Msk, 125, 160, 1, GBLUE);
	fWriteHz24(GB_24[10].Msk, 155, 160, 1, GBLUE);
	//
	fWriteHz24(GB_24[11].Msk, 65, 200, 1, GBLUE);
	fWriteHz24(GB_24[12].Msk, 95, 200, 1, GBLUE);
	fWriteHz24(GB_24[13].Msk, 125, 200, 1, GBLUE);
	fWriteHz24(GB_24[14].Msk, 155, 200, 1, GBLUE);
	//
	fWriteHz24(GB_24[15].Msk, 65, 240, 1, GBLUE);
	fWriteHz24(GB_24[16].Msk, 95, 240, 1, GBLUE);
	fWriteHz24(GB_24[17].Msk, 125, 240, 1, GBLUE);
	fWriteHz24(GB_24[18].Msk, 155, 240, 1, GBLUE);

	while (1)
	{
		if (enable)
		{
			delay_ms(50);
			key = KEY_Scan(0);
			tp_dev.scan(0);

			if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
			{

				if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
				{
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 120 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 160)
					{
						game_single(); //练习模式
						enable = 0;
					}
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 160 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 200)
					{
						game_net(); //对战模式
						enable = 0;
					}
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 200 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 240)
					{
						game_help(); //游戏帮助
						enable = 0;
					}
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 240 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 280)
					{
						game_settings(); //游戏设置
						enable = 0;
					}
				}
			}
			delay_ms(50);
		}
	}
}

void LCD_Draw_Point(u16 x, u16 y, u16 color)
{
		u16 type;
		type = POINT_COLOR;
		POINT_COLOR = color;
		LCD_DrawPoint(x, y);
		POINT_COLOR = temp;
}

//*************************************************
//	                 基本棋盘绘制函数
//*************************************************
void DrawChessboard()
{
	//LCD_Clear(WHITE);

	i = 15;
	j = 20;
	
	while(i < lcddev.width)
	{
		LCD_DrawLine(i, 20, i, 20 + lcddev.width);
		i += 15;
	}

	while(j < lcddev.width + 30)
	{
		LCD_DrawLine(0, j, lcddev.width, j);
		j+=15;
	}
		
	//得分固定标记（得分：）
	fWriteHz24(GB_24[35].Msk, 20,280, 1, WHITE);
  fWriteHz24(GB_24[36].Msk, 40,280, 1, WHITE);
	fWriteHz24(GB_24[37].Msk, 60,280, 1, WHITE);
	FillCircle(100, 290, 10, BLACK);
	
	fWriteHz24(GB_24[26].Msk, 180,280, 1, WHITE);//认输
	fWriteHz24(GB_24[27].Msk, 200,280, 1, WHITE);
	
	
}

//*************************************************
//	                   棋子绘制
//*************************************************
void FillCircle(u16 x0, u16 y0, u16 r, u16 Color)
{
	int c_x, c_y;
	int deltax, deltay;
	int d;
	int xi;
	c_x = 0;
	c_y = r;
	deltax = 3;
	deltay = 2 - r - r;
	d = 1 - r;

	LCD_Draw_Point(c_x + x0, c_y + y0, Color);
	LCD_Draw_Point(c_x + x0, -c_y + y0, Color);
	for (xi = -r + x0; xi <= r + x0; xi++)
		LCD_Draw_Point(xi, y0 , Color); //水平线填充
	while (c_x < c_y)
	{
		if (d < 0)
		{
			d += deltax;
			deltax += 2;
			c_x++;
		}
		else
		{
			d += (deltax + deltay);
			deltax += 2;
			deltay += 2;
			c_x++;
			c_y--;
		}
		for (xi = -c_x + x0; xi <= c_x + x0; xi++)
		{
			LCD_Draw_Point(xi, -c_y + y0  , Color);
			LCD_Draw_Point(xi, c_y + y0 , Color); //扫描线填充
		}
		for (xi = -c_y + x0; xi <= c_y + x0; xi++)
		{
			LCD_Draw_Point(xi, -c_x + y0 , Color);
			LCD_Draw_Point(xi, c_x + y0 , Color); //扫描线填充其量
		}
	}
}

//*************************************************
//	                   定位函数
//*************************************************
int CheckAd(u8 x,u8 y)
{
	u16 temp_x=0;
	u16 temp_y=0;
	
	u16 a1=0;
	u16 b1=0;

	for(temp_y=20+15;temp_y<(lcddev.height-60);temp_y+=15)
	{
		for(temp_x=0+15;temp_x<lcddev.width;temp_x+=15)
		{
			a1=temp_x/15-1;
			b1=(temp_y-20)/15-1;
			if(y>20 && y<lcddev.height-60 && x > temp_x-7.5 && x < temp_x+7.5 && y > temp_y-7.5 && y < temp_y+7.5 && map[a1][b1]==0)
			{			
					a.chess_x = temp_x;
					a.chess_y = temp_y;
					a.m=a1;
					a.n=b1;
					return 1;
			}
		}
	}
	return 0;
}

void test(u16 x, u16 y, u8 test)
{
	if(test)
	{
		LCD_DrawLine(0,y,lcddev.width,y);
		LCD_DrawLine(x,0,x,lcddev.height);
	}
}

//********************** Core *********************
void DrawSide(u16 side)
{
	//画下棋方颜色
	if (side == 1) // 黑
	{
		FillCircle(30, 300, 10, WHITE);
	}
	else if (side == 2) //白
	{
		FillCircle(30, 300, 10, BLACK);
	}
}

void DrawChess(u16 side, u8 mode, u8 ch_s) //ch_s默认0，黑棋1，白棋2
{
	short t = 999;
	char *pStr;
	char *gStr;
	
	while(1){
		delay_ms(50);
			tp_dev.scan(0);
			if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
			{
				if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
				{
					if (tp_dev.x[0] > 180 && tp_dev.y[0] > 280 && tp_dev.x[0] < (lcddev.width - 15) && tp_dev.y[0] < 300)
					{
						if(side ==1)
						{
							Result(2);
						}
						else if(side ==2)
						{
							Result(1);
						}
						app_init();
						exit(0);
					}
					test(tp_dev.x[0],tp_dev.y[0],enable_test);
	        switch (mode)
					{
						case 0:
			
							if (CheckAd(tp_dev.x[0], tp_dev.y[0])) //限制条件
						{
							//画棋子
							if (side == 1) // 黑
							{
								FillCircle(a.chess_x, a.chess_y, 5, BLACK);
								FillCircle(100, 290, 10, RED);
							}
							else if (side == 2) //白
							{
								FillCircle(a.chess_x, a.chess_y, 5, RED);
								FillCircle(100, 290, 10, BLACK);
							}
							PutChess(side, a.m, a.n);
						}
						break;
					case 1:
						//configUDPServer("39.108.176.84", "1600");
					
						if (side == ch_s)
						{
							if (CheckAd(tp_dev.x[0], tp_dev.y[0]))
							{
								//画棋子
								if (side == 1) // 黑
								{
									FillCircle(a.chess_x, a.chess_y, 5, BLACK);
									FillCircle(100, 290, 10, RED);
								}
								else if (side == 2) //白
								{
									FillCircle(a.chess_x, a.chess_y, 5, RED);
									FillCircle(100, 290, 10, BLACK);
								}
								PutChess(side, a.m, a.n);
							}
							
							//上传数据
							//sendUDPMsg(*pStr,1024);
						}
					else
					{
						//等待数据传入
						//*gStr=getReceivedMsg(1024);
						
						
						if (side == 1) // 黑
								{
									FillCircle(a.chess_x, a.chess_y, 5, BLACK);
									FillCircle(100, 290, 10, RED);
								}
						else if (side == 2) //白
								{
									FillCircle(a.chess_x, a.chess_y, 5, RED);
									FillCircle(100, 290, 10, BLACK);
								}
						   PutChess(side, a.m, a.n);
						}
					break;
				}	
			}
		}
		delay_ms(50);
	}
}

void PutChess(u16 side, u16 m,u16 n)
{
	map[m][n]=side;
	Result(win());
	if(side == 1)
	{
		side =2;
	}
	else if(side ==2)
	{
		side =1;
	}
	DrawChess(side, mode, ch_s);
}

int win()
{
    int x = 0, y = 0, z, nz, x_1, y_1, t;
    for (x = 0; x < 15; x++)
    {
        for (y = 0; y < 15; y++)
        {
            z = map[x][y];
            if (z == 0)
            {
            }
            else
            {
                //建立子检索点 对该点全面检索
                x_1 = x;
                y_1 = y;
                for (t = 1; t < 5; t++)
                { 
                    nz = map[x_1 + t][y_1];
                    if (nz != z)
                        break;
                }
                if (t == 5) //获胜
                    return z;
                for (t = 1; t < 5; t++)
                { 
                    nz = map[x_1][y_1 + t];
                    if (nz != z)
                        break;
                }
                if (t == 5) //获胜
                    return z;
                for (t = 1; t < 5; t++)
                { 
                    nz = map[x_1 + t][y_1 + t];
                    if (nz != z)
                        break;
                }
                if (t == 5) //获胜
                    return z;
                for (t = 1; t < 5; t++)
                { 
                    nz = map[x_1 - t][y_1 + t];
                    if (nz != z)
                        break;
                }
                if (t == 5) //获胜
                    return z;
            }
        }
    }
    return 0;
}


int IfWin(u16 x, u16 y, u16 color)
{
    int a = 0, b = 0;
    for (a = x - 4; a <= x + 4; a++) //判断横
        if (map[a][y] == color && map[a + 1][y] == color && map[a + 2][y] == color && map[a + 3][y] == color && map[a + 4][y] == color)
        {
            return 1;
        }
    for (b = y - 4; b <= y + 4; b++) //判断竖
        if (map[x][b] == color && map[x][b + 1] == color && map[x][b + 2] == color && map[x][b + 3] == color && map[x][b + 4] == color)
        {
            return 1;
        }
    for (a = x - 4, b = y - 4; a <= x + 4; a++, b++) //判断右斜
        if (map[a][b] == color && map[a + 1][b + 1] == color && map[a + 2][b + 2] == color && map[a + 3][b + 3] == color && map[a + 4][b + 4] == color)
        {
            return 1;
        }
    for (a = x - 4, b = y + 4; a <= x + 4; a++, b--) //判断左斜
        if (map[a][b] == color && map[a + 1][b - 1] == color && map[a + 2][b - 2] == color && map[a + 3][b - 3] == color && map[a + 4][b - 4] == color)
        {
            return 1;
        }
    return 0;
}


void Result(u8 flag)
{
	if (flag == 1)
	{
		LCD_Clear(BLACK);
		//黑棋获胜
		POINT_COLOR = WHITE;
    LCD_DrawLine(0, 100, 240, 100);
	  LCD_DrawLine(0, 200, 240, 200);
		fWriteHz24(GB_24[28].Msk, 70,140, 1, RED);
    fWriteHz24(GB_24[29].Msk, 100,140, 1, RED);
	  fWriteHz24(GB_24[22].Msk, 130,140, 1, RED);
    fWriteHz24(GB_24[23].Msk, 160,140, 1, RED);

		
		while (1)
		{
			//显示信息

			delay_ms(50);

			tp_dev.scan(0);
			if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
			{
				if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
				{
					app_init();
				}
			}
			delay_ms(50);
		}

		app_init();
	}
	else if (flag == 2)
	{
		LCD_Clear(WHITE);
		//白棋获胜
    
    LCD_DrawLine(0, 100, 240, 100);
	  LCD_DrawLine(0, 200, 240, 200);
		fWriteHz24(GB_24[30].Msk, 70,140, 1, RED);
    fWriteHz24(GB_24[29].Msk, 100,140, 1, RED);
	  fWriteHz24(GB_24[22].Msk, 130,140, 1, RED);
    fWriteHz24(GB_24[23].Msk, 160,140, 1, RED);
		while (1)
		{
			//显示信息

			delay_ms(50);

			tp_dev.scan(0);
			if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
			{
				if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
				{
					app_init();
				}
			}
			delay_ms(50);
		}
	}
}
//************************************************

void game_single()
{
	mode = 0;
	ch_s = 1;
	side = 1;
	//单人默认先手黑方开局
	LCD_Clear(WHITE);
	DrawChessboard();
	//printf("hello");
		DrawChess(side, mode, ch_s);	
}

void game_net()
{
	mode = 1;
	LCD_Clear(WHITE);
	//选择颜色
	
	while (1)
	{
		delay_ms(50);

		tp_dev.scan(0);
		if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
		{
			if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
			{
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 160 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 200)
					{
						 //黑棋
						side = ch_s =1;
						break;
					}
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 200 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 240)
					{
						 //白棋
						side =1;
						ch_s =2;
						break;
					}
			}
		}
		delay_ms(50);
	}
	
	DrawChessboard();
	DrawChess(side, mode, ch_s);	
}

void game_help()
{
	LCD_Clear(BLACK);
	while (1)
	{
		//显示帮助
		fWriteHz24(GB_24[11].Msk, 65, 100, 1.5, RED);
		fWriteHz24(GB_24[12].Msk, 95, 100, 1.5, RED);
		fWriteHz24(GB_24[13].Msk, 125, 100, 1.5, RED);
		fWriteHz24(GB_24[14].Msk, 155, 100, 1.5, RED);

		delayvvvvvv_ms(50);

		tp_dev.scan(0);
		if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
		{
			if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
			{
				app_init();
			}
		}
		delay_ms(50);
	}
}

void game_settings()
{ed
	LCD_Clear(BLACK);
	//显示设置
//		fWriteHz24(GB_24[11].Msk, 65, 100, 1.5, RED);
//		fWriteHz24(GB_24[12].Msk, 95, 100, 1.5, RED);
//		fWriteHz24(GB_24[13].Msk, 125, 100, 1.5, RED);
//		fWriteHz24(GB_24[14].Msk, 155, 100, 1.5, RED);
	
	fWriteHz24(GB_24[33].Msk, 65, 160, 1, GBLUE);
	fWriteHz24(GB_24[34].Msk, 95, 160, 1, GBLUE);
	fWriteHz24(GB_24[5].Msk, 125, 160, 1, GBLUE);
	fWriteHz24(GB_24[6].Msk, 155, 160, 1, GBLUE);
	
	fWriteHz24(GB_24[31].Msk, 65, 200, 1, GBLUE);
	fWriteHz24(GB_24[32].Msk, 95, 200, 1, GBLUE);
	fWriteHz24(GB_24[5].Msk, 125, 200, 1, GBLUE);
	fWriteHz24(GB_24[6].Msk, 155, 200, 1, GBLUE);
	
	fWriteHz24(GB_24[38].Msk, 65, 240, 1, GBLUE);
	fWriteHz24(GB_24[39].Msk, 95, 240, 1, GBLUE);
	fWriteHz24(GB_24[40].Msk, 125, 240, 1, GBLUE);
	fWriteHz24(GB_24[41].Msk, 155, 240, 1, GBLUE);
	
	while (1)
	{
		delay_ms(50);

		tp_dev.scan(0);
		if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
		{
			if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
			{
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 160 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 200)
					{
						if(enable_test)
							enable_test=0;
						else
							enable_test=1;
						//测试模式
					}
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 200 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 240)
					{
						 //静音模式
					}
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 240 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 280)
					{
						app_init();  //返回主界面
					}
			}
		}
		delay_ms(50);
	}

}

//main 函数主体
int main()
{
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LCD_Init();
	tp_dev.init();
	uart_init(115200);
	LED_Init();
	KEY_Init();
	POINT_COLOR = RED;
	
	//checkModuleOn();
	
	//settingUDPMod();
	
	//connectAP("HZNU's ruojin never give up","19970502");

	//Touch_Adjust();
	app_init();
}
