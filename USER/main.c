//
//  Created by 梦想家泽 on 2018/6/12.
//  Copyright ? 2018年 whzcorcd. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
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

int PutChess(u8 map[20][20], u8 side, u8 x, u8 y);
void DrawChess(u8 side, u8 mode, u8 ch_s);
void Judge(u8 map[20][20]);
void Result(u8 flag);
void DrawSide(u8 side);

u8 enable = 1;		  //主菜单响应变量，默认为1
u8 map[20][20] = {0}; //棋谱 Map
u8 side = 0;		  //当前执子的颜色
u8 mode = -1;		  //游戏模式
u16 ch_s = 0;		  //开局选择的颜色
u16 x = -1;			  //预定义坐标x
u16 y = -1;			  //预定义坐标y
u8 dx = 0;			  //x,y增量
u8 dy = 0;
s16 temp = 0; //起点终点互换中间值
u8 inc = 0;
int i = 0; //棋盘初始坐标
int j = 0;

void app_init()
{
	u8 key;
	u8 i = 0;

	LCD_Clear(BLACK);
	enable = 1;
	memset(map,0,sizeof(u8)*20*20);
	side = 0;
	mode = -1;
	x = -1;
	y = -1;

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

//*************************************************
//	                 棋盘界面函数
//*************************************************
void Disp_Line(u8 x0, u8 y0, u8 x1, u8 y1, u8 Color) //(x0,y0)和(x1,y1)分别为起始点
{
	//u8 dx, dy;
	//s16 temp;
	//u8 inc;

	if (x1 >= x0)
	{
		dx = x1 - x0; // X轴方向上的增量
	}
	else
	{
		dx = x0 - x1;
	}
	if (y1 >= y0)
	{
		dy = y1 - y0; // Y轴方向上的增量
	}
	else
	{
		dy = y0 - y1;
	}

	if (dx >= dy) // 靠近x轴,以x轴递增画线
	{
		if (x0 > x1)
		{
			temp = x1; // 起点大于终点 交换数据
			x1 = x0;
			x0 = temp;

			temp = y1;
			y1 = y0;
			y0 = temp;
		}

		if (y0 > y1)
		{
			inc = 0;
		}
		else
		{
			inc = 1;
		}
	}
	else // 靠近y轴,以y轴递增画线
	{
		if (y0 > y1)
		{
			temp = x1; // 起点大于终点 交换数据
			x1 = x0;
			x0 = temp;

			temp = y1;
			y1 = y0;
			y0 = temp;
		}
		if (x0 > x1)
		{
			inc = 0;
		}
		else
		{
			inc = 1;
		}
	}

	if (dx == 0) // X轴上没有增量 画垂直线
	{
		do
		{
			LCD_Draw_Point(x0, y0, Color); // 逐点显示 描垂直线
			y0++;
		} while (y1 >= y0);
		return;
	}
	if (dy == 0) // Y轴上没有增量 画水平直线
	{
		do
		{
			LCD_Draw_Point(x0, y0, Color); // 逐点显示 描水平线
			x0++;
		} while (x1 >= x0);
		return;
	}
	// Bresenham算法画线
	if (dx >= dy) // 靠近X轴
	{
		temp = 2 * dy - dx; // 计算下个点的位置
		while (x0 != x1)
		{
			LCD_Draw_Point(x0, y0, Color); // 画起点
			x0++;						   // X轴上加1
			if (temp > 0)				   // 判断下下个点的位置
			{
				if (inc == 1) // 为右上相邻点，即（x0+1,y0+1）
				{
					y0++;
				}
				else
				{
					y0--;
				}
				temp += 2 * dy - 2 * dx;
			}
			else
			{
				temp += 2 * dy; // 判断下下个点的位置
			}
		}
		LCD_Draw_Point(x0, y0, Color);
	}
	else
	{
		temp = 2 * dx - dy; // 靠近Y轴
		while (y0 != y1)
		{
			LCD_Draw_Point(x0, y0, Color);
			y0++;
			if (temp > 0)
			{
				if (inc == 1) // 为右上相邻点，即（x0+1,y0+1）
				{
					x0++;
				}
				else
				{
					x0--;
				}
				temp += 2 * dx - 2 * dy;
			}
			else
			{
				temp += 2 * dx;
			}
		}
		LCD_Draw_Point(x0, y0, Color);
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
//	                   游戏模块
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
		LCD_Draw_Point(xi, y0); //水平线填充
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
			LCD_Draw_Point(xi, -c_y + y0);
			LCD_Draw_Point(xi, c_y + y0); //扫描线填充
		}
		for (xi = -c_y + x0; xi <= c_y + x0; xi++)
		{
			LCD_Draw_Point(xi, -c_x + y0);
			LCD_Draw_Point(xi, c_x + y0); //扫描线填充其量
		}
	}
}

//********************** Core *********************
void DrawSide(u8 side)
{
	//画下棋方颜色
	if (side == 1) // 黑
	{
		FillCircle(30, 300, 5, BLACK);
	}
	else if (side == 2) //白
	{
		FillCircle(30, 300, 5, WHITE);
	}
}

void DrawChess(u8 side, u8 mode, u8 ch_s) //ch_s默认0，黑棋1，白棋2
{
	switch (mode)
	{
	case 1:
		while (1)
		{
			delay_ms(20);
			tp_dev.scan(0);
			if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
			{
				if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
				{
					if (1) //限制条件
					{
						x = tp_dev.x[0];
						y = tp_dev.y[0];
						//画棋子
						if (side == 1) // 黑
						{
							FillCircle(x, y, 2.5, BLACK);
						}
						else if (side == 2) //白
						{
							FillCircle(x, y, 2.5, WHITE);
						}

						PutChess(map, side, x, y);
					}
				}
			}
			delay_ms(20);
		}
		break;
	case 2:
		while (1)
		{
			if (side == ch_s)
			{

				tp_dev.scan(0);
				if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
				{
					if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
					{
						if (tp_dev.x[0] > 50 && tp_dev.y[0] > 150 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 190)
						{
							//画棋子
							if (side == 1) // 黑
							{
								FillCircle(tp_dev.x[0], tp_dev.y[0], 2.5, BLACK);
							}
							else if (side == 2) //白
							{
								FillCircle(tp_dev.x[0], tp_dev.y[0], 2.5, WHITE);
							}
							PutChess(map, side, x, y);
						}
					}
				}
			}
			//上传数据

			else
			{
				//等待数据传入

				if (side == 1) // 黑
				{
					//FillCircle(get_x, get_y, 2.5, BLACK);
				}
				else if (side == 2) //白
				{
					//FillCircle(get_x, get_y, 2.5, WHITE);
				}
				//PutChess(map, side, get_x, get_y);
			}
		}
		break;
	}
}

int PutChess(u8 map[20][20], u8 side, u8 x, u8 y) //side = 1为黑棋，=2为白棋
{
	char ch;
	u8 temp_x = 0, temp_y = 0;
	while (1)
	{
		while (side == 1)
		{
			if (!(x > 0 && x < 20) || !(y > 0 || y < 20) || map[x][y] != 0)
			{
				//pru8f("输入非法！重新输入！\n");
				continue;
			}
			else
			{
				map[x][y] = '*';
				//fflush(stdin);

				Judge(map);
				//DrawChess(side, x, y);
				side = 2;
				DrawChess(side, mode, ch_s);
				//DrawMap(map, who);
				break;
			}
		}
		while (side == 2)
		{

			if (!(x > 0 && x < 20) || !(y > 0 || y < 20) || map[x][y] != 0)
			{
				//pru8f("输入非法！重新输入！\n");
				continue;
			}
			else
			{
				map[x][y] = 'O';
				//fflush(stdin);

				Judge(map);
				//DrawChess(side, x, y);
				side = 1;
				DrawChess(side, mode, ch_s);
				//DrawMap(map, who);
				break;
			}
		}
	}
}
void Judge(u8 map[20][20])
{
	u8 i = 0, j = 0;
	u8 x = 0, y = 0;
	u8 flag = 0;
	for (i = 1; i < 20; i++)
	{
		for (j = 1; j < 20; j++)
		{
			if (map[i][j] == '*')
			{
				for (x = i + 1, y = j + 1; x < i + 5, y < j + 5; x++, y++)
				{
					if (map[x][j] == '*')
					{
						flag = 1;
					}
					else if (map[i][y] == '*')
					{
						flag = 1;
					}
					else if (map[x][y] == '*')
					{
						flag = 1;
					}
					else
					{
						flag = 0;
					}
				}
				for (x = i - 1, y = j - 1; x > i - 5, y > j - 5; x--, y--)
				{
					if (map[x][j] == '*')
					{
						flag = 1;
					}
					else if (map[i][y] == '*')
					{
						flag = 1;
					}
					else if (map[x][y] == '*')
					{
						flag = 1;
					}
					else
					{
						flag = 0;
					}
				}
				Result(flag);
			}
			else if (map[i][j] == '#')
			{
				flag = 0;
				for (x = i + 1, y = j + 1; x < i + 5, y < j + 5; x++, y++)
				{
					if (map[x][j] == '#')
					{
						flag = 2;
					}
					else if (map[i][y] == '#')
					{
						flag = 2;
					}
					else if (map[x][y] == '#')
					{
						flag = 2;
					}
				}
				for (x = i + 4, y = j + 4; x >= i, y >= j; x--, y--)
				{
					if (map[x][j] == '#')
					{
						flag = 2;
					}
					else if (map[i][y] == '#')
					{
						flag = 2;
					}
					else if (map[x][y] == '#')
					{
						flag = 2;
					}
				}
				Result(flag);
			}
		}
	}
}
void Result(u8 flag)
{
	if (flag == 1)
	{
		LCD_Clear(WHITE);
		//黑棋获取

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
		//白棋获取

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
	else
	{
		exit(0);
	}
}
//************************************************

void game_single()
{
	mode = 0;
	//单人默认先手黑方开局
	ch_s = side = 1;
	//DrawChess(side, mode, ch_s);

	LCD_Clear(WHITE);

	for (i = 16; i <= 226; i = i + 15)
	{
		Disp_Line(i, 50, i, 500, BLACK);
	}
	for (j = 50; j <= 260; j = j + 15)
	{
		Disp_Line(0, j, 240, j, BLACK);
	}

	while (1)
	{
		DrawChess(side, mode, ch_s);
	}
}

void game_net()
{
	mode = 1;
	LCD_Clear(WHITE);
	//选择颜色
	while (1)
	{
		DrawChess(side, mode, ch_s);
	}
}

void game_help()
{
	LCD_Clear(WHITE);
	while (1)
	{
		//显示帮助
		fWriteHz24(GB_24[11].Msk, 65, 100, 1.5, RED);
		fWriteHz24(GB_24[12].Msk, 95, 100, 1.5, RED);
		fWriteHz24(GB_24[13].Msk, 125, 100, 1.5, RED);
		fWriteHz24(GB_24[14].Msk, 155, 100, 1.5, RED);

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

void game_settings()
{
	LCD_Clear(WHITE);
	while (1)
	{
		//显示设置
		app_init();
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

	app_init();
}
