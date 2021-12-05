#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")//音频
#define WINDOW_LONG 1000   //窗口长
#define WINDOW_WIDE 600    //窗口宽
#define RADISH_LONG 60     //萝卜图片的长
#define RADISH_WIDE 60     //萝卜图片的宽
#define RADISH_MAX  10	   //一次落下的萝卜最大的数量

//全局变量
/*************************************************************************************/

//图片变量
IMAGE menubackground;     //菜单背景图
IMAGE gamebackground;     //游戏背景图
IMAGE setbackground;      //设置背景图
IMAGE overbackground;     //游戏结束背景图
IMAGE scorebackground;    //最高记录背景图
IMAGE radish[2];          //萝卜的掩码图和背景图

//字符结构体
struct letter
{
	int x, y;   //横纵坐标
	int target; //字符
};
struct letter array[RADISH_MAX];   //每一次出现字符数组

//萝卜属性变量
int radish_num = 5;       //一次落下的萝卜数
int radish_sum = 0;       //一局获得萝卜个数

//计时变量
int game_second = 120;     //一局游戏的时间  (秒)
clock_t  start, end;	   //游戏计时变量
clock_t p_start, p_end;    //游戏暂停
int pause_time = 0;        //游戏暂停的总时间
int p_flag = 0;            //游戏暂停的旗帜   0为假  1为真

//最高记录
struct high_record    
{
	int game_second;     //游戏的时间
	int radish_sum;      //获得萝卜个数
};
struct high_record record[3] = { {120,0},{240,0},{360,0} }; //最高纪录
/*************************************************************************************/

//函数声明
/*************************************************************************************/
void DrawMenu();			//绘制 游戏 菜单界面
void DrawGameRun();			//绘制 游戏 进行界面
void DrawGamePause();		//绘制 游戏 暂停界面
void DrawGameSet();			//绘制 游戏 设置界面
void DrawGameOver();        //绘制 游戏 结束界面

void MenuMouse();			//游戏 菜单界面 鼠标交互 
void RunKeyBoard(); 		//游戏 进行界面 键盘交互
void PauseMouse();			//游戏 暂停界面 鼠标交互
void SetMouse();			//游戏 设置界面 鼠标交互
void OverMouse();           //游戏 结束界面 鼠标交互 
void RecordMouse();			//游戏 纪录界面 鼠标交互 

void ReadHighRecordFile();	//读取最高纪录文件
void SaveHighRecordFile();	//保存最高纪录文件


void PlayGame();		    //游戏进行  (游戏核心)
void LoadResource();                                 //加载资源
void PrintLetterRadish(int x, int y, char target);   //绘制字母和萝卜结合
void InitArray(int i);                               //单个初始化萝卜
DWORD WINAPI PlayBombMusic(LPVOID LpParale);         //多线程播放爆炸音效
/*************************************************************************************/

int main()
{
	LoadResource();   //加载资源
	MenuMouse();      //游戏菜单
	return 0;
}

//游戏进行 (游戏核心)
void PlayGame()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	//生成随机字母萝卜
	for (int i = 0; i < radish_num; i++)
	{
		InitArray(i);
	}
	start = clock();   //游戏时间开始
	
	while (TRUE)
	{
		//游戏时间控制
		end = clock();
		if (p_flag == 1)
		{
			pause_time += (int)(p_end - p_start) / 1000;
		}
		if ((int)(end - start) / 1000 - pause_time == game_second)    //游戏时间结束
		{
			EndBatchDraw();
			OverMouse();
		}
		p_flag = 0;   //旗帜更新

		//绘制游戏界面
		BeginBatchDraw();
		DrawGameRun();
		for (int i = 0; i < radish_num; i++)
		{
			array[i].y += WINDOW_WIDE / 100;
		}
		for (int i = 0; i < radish_num; i++)
		{
			PrintLetterRadish(array[i].x, array[i].y, array[i].target);
		}
		for (int i = 0; i < radish_num; i++)
		{
			//萝卜超出下边框
			if (array[i].y >= WINDOW_WIDE)
			{
				InitArray(i);
			}
		}
		Sleep(50);   //延时50ms
		FlushBatchDraw();


		//判断是否存在按键
		if (_kbhit() != 0)
		{
			EndBatchDraw();
			p_start = clock();    //开始游戏暂停
			RunKeyBoard();        //键盘交互函数
			p_end = clock();      //结束游戏暂停
		}
	}
}





//绘制游戏菜单界面
void DrawMenu()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	//菜单背景图片
	putimage(0, 0, &menubackground);	

	//文字布局
	settextcolor(WHITE);                            //设置字体颜色
	settextstyle(WINDOW_WIDE / 20, 0, L"华文新魏"); //设置文字样式
	setbkmode(TRANSPARENT);                         //文字背景透明
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"开始游戏");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"游戏设置");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"最高纪录");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"退出游戏");
}

//绘制游戏进行界面
void DrawGameRun()
{
	//游戏背景图片
	putimage(0, 0, &gamebackground);
	settextcolor(WHITE);
	settextstyle(20, 0, L"华文楷体");
	setbkmode(TRANSPARENT);
	WCHAR str_second[10];
	wsprintf(str_second, L"游戏倒计时:%d秒", game_second - int(end - start) / 1000 + pause_time);
	outtextxy(5 * WINDOW_LONG / 6, 1 * WINDOW_WIDE / 15, str_second);
	WCHAR str_sum[10];
	wsprintf(str_sum, L"获得萝卜数:%d个", radish_sum);
	outtextxy(5 * WINDOW_LONG / 6, 2 * WINDOW_WIDE / 15, str_sum);
}

//绘制游戏暂停界面
void DrawGamePause()
{
	//文字布局
	settextcolor(WHITE);                            //设置字体颜色
	settextstyle(WINDOW_WIDE / 20, 0, L"华文新魏"); //设置文字样式
	setbkmode(TRANSPARENT);                         //文字背景透明
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"继续游戏");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"重新开始");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"返回菜单");
}

//绘制游戏设置界面
void DrawGameSet()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	//背景图片
	putimage(0, 0, &setbackground);
	//文字属性设置
	settextcolor(WHITE);                            //设置字体颜色
	settextstyle(WINDOW_WIDE / 20, 0, L"华文琥珀"); //设置文字样式
	setbkmode(TRANSPARENT);                         //文字背景透明
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"请选择一次落下的       个数");
	//贴一个萝卜
	putimage(WINDOW_LONG / 7 + 225, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 15, &radish[0], SRCAND);
	putimage(WINDOW_LONG / 7 + 225, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 15, &radish[1], SRCPAINT);
	//萝卜个数
	for (int i = 0; i < 7; i++)
	{
		WCHAR str_num[2];
		wsprintf(str_num, L"%d", i + 3);
		if (i + 3 == radish_num)
		{
			settextcolor(YELLOW);
		}
		else
		{
			settextcolor(WHITE);
		}
		outtextxy(WINDOW_LONG / 7 + i * 51, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, str_num);
	}
	//游戏时间选择
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"请选择游戏的时长");
	for (int i = 0; i < 3; i++)
	{
		WCHAR str_minute[7];
		wsprintf(str_minute, L"%d分钟", 2 * (i + 1));
		if (2 * (i + 1) == game_second / 60)
		{
			settextcolor(YELLOW);
		}
		else
		{
			settextcolor(WHITE);
		}
		outtextxy(WINDOW_LONG / 7 + i * 110, WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10, str_minute);
	}
	//返回菜单按钮
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
}

//绘制游戏结束界面
void DrawGameOver()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	putimage(0, 0, &overbackground);
	//文字属性设置
	settextcolor(WHITE);                            //设置字体颜色
	settextstyle(WINDOW_WIDE / 20, 0, L"华文琥珀"); //设置文字样式
	setbkmode(TRANSPARENT);                         //文字背景透明   
	WCHAR str_num[20];
	wsprintf(str_num, L"本局收集的         个数为:%d个", radish_sum);
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, str_num);
	//贴一个萝卜
	putimage(WINDOW_LONG / 7 + 150, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 20, &radish[0], SRCAND);
	putimage(WINDOW_LONG / 7 + 150, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 20, &radish[1], SRCPAINT);
	WCHAR str_second[10];
	wsprintf(str_second, L"本局用时:%d秒", game_second);
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, str_second);
	//再来一局按钮
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10, L"再来一局");
	//返回菜单按钮
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
}

//绘制最高记录界面
void DrawHighScore()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	putimage(0, 0, &scorebackground);
	//文字属性设置
	settextcolor(WHITE);                            //设置字体颜色
	settextstyle(WINDOW_WIDE / 20, 0, L"华文琥珀"); //设置文字样式
	setbkmode(TRANSPARENT);                         //文字背景透明  
	//用时
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"2分钟");
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"4分钟");
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10, L"6分钟");
	//最高分
	WCHAR str_record[5];
	for (int i = 0; i < 3; i++)
	{
		wsprintf(str_record, L"%d个", record[i].radish_sum);
		outtextxy(WINDOW_LONG / 2, WINDOW_WIDE / 7 + 2 * i * WINDOW_WIDE / 10, str_record);
		putimage(WINDOW_LONG / 2 + 80, WINDOW_WIDE / 7 + 2 * i * WINDOW_WIDE / 10 - 20, &radish[0], SRCAND);
		putimage(WINDOW_LONG / 2 + 80, WINDOW_WIDE / 7 + 2 * i * WINDOW_WIDE / 10 - 20, &radish[1], SRCPAINT);
	}
	//返回菜单按钮
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
}




//游戏菜单界面 鼠标交互  背景音乐设置
void MenuMouse()
{
	DrawMenu();      //绘制菜单界面
	//播放菜单背景音乐
	mciSendString(L"open 音乐\\菜单背景音乐.mp3 alias menu", nullptr, 0, nullptr);
	mciSendString(L"play menu repeat", nullptr, 0, nullptr);
	MOUSEMSG m;
	while (TRUE)
	{
		//获取鼠标位置
		m = GetMouseMsg();

		//判断鼠标位置
		//*******************************************************************
		//开始游戏
		settextcolor(RED);			//设置字体颜色
		if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"开始游戏");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				//关闭菜单背景音乐
				mciSendString(L"close menu", nullptr, 0, nullptr);
				//播放游戏进行背景音乐
				mciSendString(L"open 音乐\\游戏背景音乐.mp3 alias game", nullptr, 0, nullptr);
				mciSendString(L"play game repeat", nullptr, 0, nullptr);
				PlayGame();
			}
		}
		//******************************************************************
		//游戏设置
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"游戏设置");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				SetMouse();
			}
		}
		//******************************************************************
		//最高记录
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"最高纪录");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				RecordMouse();
			}
		}
		//******************************************************************
		//退出游戏
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"退出游戏");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				exit(0);
			}
		}
		else
		{
			settextcolor(WHITE);	   //字体颜色还原					
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"开始游戏");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"游戏设置");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"最高纪录");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"退出游戏");
		}
	}
	_getch();
	closegraph();
}

//游戏进行界面 键盘交互
void RunKeyBoard()
{
	char usekey = _getch();  //接受按键
	for (int i = 0; i < radish_num; i++)
	{
		if (usekey == array[i].target || usekey == array[i].target + 'a' - 'A')
		{
			InitArray(i);
			CreateThread(nullptr, 0, PlayBombMusic, nullptr, 0, nullptr);
			radish_sum++;
			break;
		}
		else if (usekey == 27)   //用户点击esc键
		{
			mciSendString(L"close game", nullptr, 0, nullptr);
			MenuMouse();  //返回菜单
		}
		else if (usekey == 32)   //用户点击space键  弹出暂停界面
		{
			mciSendString(L"pause game", nullptr, 0, nullptr);
			PauseMouse();
			mciSendString(L"resume game", nullptr, 0, nullptr);
			return;
		}
	}
}

//游戏暂停界面 鼠标交互
void PauseMouse()
{
	MOUSEMSG m;
	while (TRUE)
	{
		DrawGamePause();      //绘制暂停界面
		//获取鼠标位置
		m = GetMouseMsg();
		settextcolor(YELLOW);	   //字体变色
		//继续游戏
		if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"继续游戏");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				p_flag = 1;  //改变暂停旗帜
				return;
			}
		}

		//重新开始
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"重新开始");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				radish_sum = 0;
				PlayGame();
			}
		}

		//返回菜单
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"返回菜单");
			//鼠标左击
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(L"close game", nullptr, 0, nullptr);
				MenuMouse();
			}
		}
		else
		{
			settextcolor(WHITE);	   //字体颜色还原					
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"继续游戏");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"重新开始");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"返回菜单");
		}
	}
}

//游戏设置界面 鼠标交互
void SetMouse()
{
	DrawGameSet();
	MOUSEMSG m;
	while (TRUE)
	{
		//获取鼠标位置
		m = GetMouseMsg();
		//文字属性设置
		settextstyle(WINDOW_WIDE / 20, 0, L"华文琥珀"); //设置文字样式
		setbkmode(TRANSPARENT);                         //文字背景透明
		//萝卜个数按键设置
		for (int i = 0; i < 7; i++)
		{
			if (m.x >= WINDOW_LONG / 7 + i * 51 && m.y >= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 &&
				m.x <= WINDOW_LONG / 7 + i * 51 + 20 && m.y <= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 + 24)
			{
				settextcolor(YELLOW);	//鼠标在的时候字体的颜色
				//鼠标点击  改变个数
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					radish_num = i + 3;
				}
			}
			else if (i + 3 == radish_num)
			{
				settextcolor(YELLOW);   //把现在的个数字体设为黄色
			}
			else
			{
				settextcolor(WHITE);    //鼠标不在时字体的颜色
			}
			WCHAR str_num[2];
			wsprintf(str_num, L"%d", i + 3);
			outtextxy(WINDOW_LONG / 7 + i * 51, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, str_num);
		}

		//游戏时长按键设置
		for (int i = 0; i < 3; i++)
		{
			if (m.x >= WINDOW_LONG / 7 + i * 110 && m.y >= WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10 &&
				m.x <= WINDOW_LONG / 7 + i * 110 + 80 && m.y <= WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10 + 26)
			{
				settextcolor(YELLOW);	//鼠标在的时候字体的颜色
				//鼠标点击  改变时长
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					game_second = 60 * 2 * (i + 1);
				}
			}
			else if (2 * (i + 1) == game_second / 60)
			{
				settextcolor(YELLOW);   //把现在的时长字体设为黄色
			}
			else
			{
				settextcolor(WHITE);    //鼠标不在时字体的颜色
			}
			WCHAR str_minute[7];
			wsprintf(str_minute, L"%d分钟", 2 * (i + 1));
			outtextxy(WINDOW_LONG / 7 + i * 110, WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10, str_minute);
		}

		//返回菜单按键设置
		if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//鼠标在的时候字体的颜色
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				MenuMouse();  //返回菜单
			}
		}
		else
		{
			settextcolor(WHITE);    //鼠标不在时字体的颜色
		}
		outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
	}
}

//游戏结束界面 鼠标交互  背景音乐设置
void OverMouse()
{
	
	ReadHighRecordFile();//读取最高纪录文件
	SaveHighRecordFile();//保存最高纪录文件

	DrawGameOver();
	MOUSEMSG m;
	while (TRUE)
	{
		//获取鼠标位置
		m = GetMouseMsg();
		//文字属性设置
		settextstyle(WINDOW_WIDE / 20, 0, L"华文琥珀"); //设置文字样式
		setbkmode(TRANSPARENT);                         //文字背景透明
		//再来一局按键设置
		if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//鼠标在的时候字体的颜色
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10, L"再来一局");
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				radish_sum = 0;
				PlayGame();  //再来一局
			}

		}
		//返回菜单按键设置
		else if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//鼠标在的时候字体的颜色
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(L"close game", nullptr, 0, nullptr);
				MenuMouse();  //返回菜单
			}
		}
		else
		{
			settextcolor(WHITE);    //鼠标不在时字体的颜色
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10, L"再来一局");
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
		}

	}
}

//游戏纪录界面 鼠标交互  
void RecordMouse()
{
	//文件操作
	ReadHighRecordFile();
	DrawHighScore();
	MOUSEMSG m;
	while (TRUE)
	{
		//获取鼠标位置
		m = GetMouseMsg();
		//文字属性设置
		settextstyle(WINDOW_WIDE / 20, 0, L"华文琥珀"); //设置文字样式
		setbkmode(TRANSPARENT);                         //文字背景透明
		//返回菜单按键设置
		if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//鼠标在的时候字体的颜色
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(L"close game", nullptr, 0, nullptr);
				MenuMouse();  //返回菜单
			}
		}
		else
		{
			settextcolor(WHITE);    //鼠标不在时字体的颜色
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"返回菜单");
		}

	}
}





//读取最高纪录文件
void ReadHighRecordFile()
{
	FILE *fp;
	if ((fopen_s(&fp, "最高纪录.txt", "r")) != 0)
	{
		if ((fopen_s(&fp, "最高纪录.txt", "w")) != 0)
		{
			printf("创建最高纪录文件失败\n");
			exit(-1);
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			fread(&record[i], sizeof(struct high_record), 1, fp);
		}
	}
	fclose(fp);
}

//保存最高纪录文件
void SaveHighRecordFile()
{
	FILE* fp;
	if ((fopen_s(&fp, "最高纪录.txt", "r+")) != 0)
	{
		printf("打开最高纪录文件失败\n");
		exit(-1);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (game_second == record[i].game_second && radish_sum > record[i].radish_sum)
			{
				record[i].radish_sum = radish_sum;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			fwrite(&record[i], sizeof(struct high_record), 1, fp);
		}
	}
	fclose(fp);
}






//绘制字母和萝卜结合
void PrintLetterRadish(int x, int y, char target)
{
	settextcolor(WHITE);
	settextstyle(18, 0, L"MS Reference Sans Serif");
	setbkmode(TRANSPARENT);
	putimage(x, y, &radish[0], SRCAND);
	putimage(x, y, &radish[1], SRCPAINT);
	outtextxy(x + RADISH_LONG / 2 - RADISH_LONG / 17, y + RADISH_WIDE / 2 + RADISH_WIDE / 10, target);
}

//单个初始化萝卜
void InitArray(int i)
{
	srand(unsigned int(time(nullptr)));      //生成随机种子
	array[i].target = rand() % 26 + 'A';   //随机生成26个英文大写字母
	//字母重复 重新生成
	while (TRUE)
	{
		int flag = 0;
		int j = 1;
		while (j < radish_num)
		{
			if (array[i].target == array[(i + j) % radish_num].target)
			{
				flag = 1;
				break;
			}
			j++;
		}
		if (flag == 1)
		{
			array[i].target = rand() % 26 + 'A';   //随机生成26个英文大写字母
		}
		else
		{
			break;
		}
	}
	array[i].x = rand() % 14 * WINDOW_LONG / 20 + 3 * WINDOW_LONG / 20;
	array[i].y = rand() % 50 - 100;
	//萝卜重合严重 重新生成
	while (TRUE)
	{
		int flag = 0;
		int j = 1;
		if (radish_num <= 6)
		{
			while (j < radish_num)
			{
				if (array[i].x >= array[(i + j) % radish_num].x - RADISH_LONG && array[i].x <= array[(i + j) % radish_num].x + RADISH_LONG &&
					array[i].y >= array[(i + j) % radish_num].y - RADISH_WIDE && array[i].y <= array[(i + j) % radish_num].y + RADISH_WIDE)
				{
					flag = 1;
				}
				j++;
			}
		}
		else
		{
			while (j < radish_num)
			{
				if (array[i].x >= array[(i + j) % radish_num].x - RADISH_LONG / 1.3 && array[i].x <= array[(i + j) % radish_num].x + RADISH_LONG / 1.3 &&
					array[i].y >= array[(i + j) % radish_num].y - RADISH_WIDE / 1.3 && array[i].y <= array[(i + j) % radish_num].y + RADISH_WIDE / 1.3)
				{
					flag = 1;
				}
				j++;
			}
		}
		if (flag == 1)
		{
			array[i].x = rand() % 14 * WINDOW_LONG / 20 + 3 * WINDOW_LONG / 20;
			array[i].y = rand() % 50 - 100;
		}
		else
		{
			break;
		}
	}
}

//多线程播放爆炸音效
DWORD WINAPI PlayBombMusic(LPVOID LpParale)
{
	mciSendString(L"open 音乐\\爆炸.mp3 alias bomb", nullptr, 0, nullptr);
	mciSendString(L"play bomb wait", nullptr, 0, nullptr);
	mciSendString(L"close bomb", nullptr, 0, nullptr);
	return 0;
}

//加载资源
void LoadResource()
{
	loadimage(&menubackground, L"图片\\菜单背景图.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&gamebackground, L"图片\\游戏背景图.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&setbackground, L"图片\\设置背景图.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&overbackground, L"图片\\游戏结束背景图.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&scorebackground, L"图片\\最高记录背景图.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&radish[0], L"图片\\萝卜掩码图.jpg", RADISH_LONG, RADISH_WIDE);
	loadimage(&radish[1], L"图片\\萝卜背景图.jpg", RADISH_LONG, RADISH_WIDE);
}