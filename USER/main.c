//
//  Created by 梦想家泽 on 2018/6/12.
//  Copyright © 2018年 whzcorcd. All rights reserved.
//
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "hanzi.h"

u8 map[20][20] = {0};
u8 side = 0;
u8 mode = -1;
u8 ch_s = 0;
u8 x = -1;
u8 y = -1;

void app_init()
{
	u8 key;
	u8 i = 0;

	LCD_Clear(BLACK);
	map = {0};
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
		key = KEY_Scan(0);
		tp_dev.scan(0);
		if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
		{
			if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
			{
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 150 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 190)
					game_single(); //
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 190 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 230)
					game_net(); //
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 230 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 270)
					game_help(); //
				if (tp_dev.x[0] > 50 && tp_dev.y[0] > 270 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 310)
					game_settings(); //
			}
		}
	}
}

//*************************************************
//	                   游戏模块
//*************************************************
void DrawSide(u8 side)
{
	//画下棋方颜色
}

void DrawChess(u8 side, u8 mode, u8 ch_s) //choosed_side 默认0，黑棋1，白棋2
{
	switch (mode)
	{
	case 1:
		while (1)
		{
			key = KEY_Scan(0);
			tp_dev.scan(0);
			if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
			{
				if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
				{
					if (tp_dev.x[0] > 50 && tp_dev.y[0] > 150 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 190)
					{
						x = ;
						y = ;
						//画棋子
						if (side == 1) // 黑
						{
						}
						else if (side == 2) //白
						{
						}
						else
						{
							continue;
						}

						PutChess(map, side, x, y);
					}
				}
			}
		}
		break;
	case 2:
		while (1)
		{
			if (side == ch_s)
			{

				key = KEY_Scan(0);
				tp_dev.scan(0);
				if (tp_dev.sta & TP_PRES_DOWN) //判断触摸屏是否被按下
				{
					if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
					{
						if (tp_dev.x[0] > 50 && tp_dev.y[0] > 150 && tp_dev.x[0] < (lcddev.width - 50) && tp_dev.y[0] < 190)
						{
							x = ;
							y = ;
							//画棋子

							PutChess(map, side, x, y);
						}
					}
				}
			}
			//上传数据

			else
			{
				//等待数据传入

				PutChess(map, side, x, y);
			}
		}
		break;
	}
}

//********************** Core *********************
u8 PutChess(u8 map[20][20], u8 side, u8 x, u8 y) //side = 1为黑棋，=2为白棋
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
				fflush(stdin);

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
				fflush(stdin);

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
			else if (map == '#')
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
u8 Result(u8 flag)
{
	if (flag == 1)
	{
		printf("黑方获胜！\n");

		app_init();
		return 0;
	}
	else if (flag == 2)
	{
		printf("白方获胜！\n");

		app_init();
		return 0;
	}
	else
	{
		return 0;
	}
}

void game_single()
{
	mode = 0;
	LCD_Clear(WHITE);
	//单人默认先手黑方开局
	ch_s = side = 1;
	DrawChess(side, mode, ch_s);
}

void game_net()
{
	mode = 1;
	LCD_Clear(WHITE);
	//选择颜色
	while (1)
	{
		if ()
			ch_s = side = 1;
		DrawChess(side, mode, ch_s);
		else if ()
			ch_s = side = 2;
		DrawChess(side, mode, ch_s);
		else continue;
	}
}

void game_help()
{
	LCD_Clear(WHITE);
	while(1){
		//显示帮助
		app_init();
	}
}

void game_settings()
{
	LCD_Clear(WHITE);
	while(1){
		//显示设置
		app_init();
	}
}

//
u8 main(void)
{
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	LED_Init();
	LCD_Init();
	KEY_Init();
	tp_dev.init();

	POint_COLOR = RED;
	app_init();
}
