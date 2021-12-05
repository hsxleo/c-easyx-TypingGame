#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")//��Ƶ
#define WINDOW_LONG 1000   //���ڳ�
#define WINDOW_WIDE 600    //���ڿ�
#define RADISH_LONG 60     //�ܲ�ͼƬ�ĳ�
#define RADISH_WIDE 60     //�ܲ�ͼƬ�Ŀ�
#define RADISH_MAX  10	   //һ�����µ��ܲ���������

//ȫ�ֱ���
/*************************************************************************************/

//ͼƬ����
IMAGE menubackground;     //�˵�����ͼ
IMAGE gamebackground;     //��Ϸ����ͼ
IMAGE setbackground;      //���ñ���ͼ
IMAGE overbackground;     //��Ϸ��������ͼ
IMAGE scorebackground;    //��߼�¼����ͼ
IMAGE radish[2];          //�ܲ�������ͼ�ͱ���ͼ

//�ַ��ṹ��
struct letter
{
	int x, y;   //��������
	int target; //�ַ�
};
struct letter array[RADISH_MAX];   //ÿһ�γ����ַ�����

//�ܲ����Ա���
int radish_num = 5;       //һ�����µ��ܲ���
int radish_sum = 0;       //һ�ֻ���ܲ�����

//��ʱ����
int game_second = 120;     //һ����Ϸ��ʱ��  (��)
clock_t  start, end;	   //��Ϸ��ʱ����
clock_t p_start, p_end;    //��Ϸ��ͣ
int pause_time = 0;        //��Ϸ��ͣ����ʱ��
int p_flag = 0;            //��Ϸ��ͣ������   0Ϊ��  1Ϊ��

//��߼�¼
struct high_record    
{
	int game_second;     //��Ϸ��ʱ��
	int radish_sum;      //����ܲ�����
};
struct high_record record[3] = { {120,0},{240,0},{360,0} }; //��߼�¼
/*************************************************************************************/

//��������
/*************************************************************************************/
void DrawMenu();			//���� ��Ϸ �˵�����
void DrawGameRun();			//���� ��Ϸ ���н���
void DrawGamePause();		//���� ��Ϸ ��ͣ����
void DrawGameSet();			//���� ��Ϸ ���ý���
void DrawGameOver();        //���� ��Ϸ ��������

void MenuMouse();			//��Ϸ �˵����� ��꽻�� 
void RunKeyBoard(); 		//��Ϸ ���н��� ���̽���
void PauseMouse();			//��Ϸ ��ͣ���� ��꽻��
void SetMouse();			//��Ϸ ���ý��� ��꽻��
void OverMouse();           //��Ϸ �������� ��꽻�� 
void RecordMouse();			//��Ϸ ��¼���� ��꽻�� 

void ReadHighRecordFile();	//��ȡ��߼�¼�ļ�
void SaveHighRecordFile();	//������߼�¼�ļ�


void PlayGame();		    //��Ϸ����  (��Ϸ����)
void LoadResource();                                 //������Դ
void PrintLetterRadish(int x, int y, char target);   //������ĸ���ܲ����
void InitArray(int i);                               //������ʼ���ܲ�
DWORD WINAPI PlayBombMusic(LPVOID LpParale);         //���̲߳��ű�ը��Ч
/*************************************************************************************/

int main()
{
	LoadResource();   //������Դ
	MenuMouse();      //��Ϸ�˵�
	return 0;
}

//��Ϸ���� (��Ϸ����)
void PlayGame()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	//���������ĸ�ܲ�
	for (int i = 0; i < radish_num; i++)
	{
		InitArray(i);
	}
	start = clock();   //��Ϸʱ�俪ʼ
	
	while (TRUE)
	{
		//��Ϸʱ�����
		end = clock();
		if (p_flag == 1)
		{
			pause_time += (int)(p_end - p_start) / 1000;
		}
		if ((int)(end - start) / 1000 - pause_time == game_second)    //��Ϸʱ�����
		{
			EndBatchDraw();
			OverMouse();
		}
		p_flag = 0;   //���ĸ���

		//������Ϸ����
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
			//�ܲ������±߿�
			if (array[i].y >= WINDOW_WIDE)
			{
				InitArray(i);
			}
		}
		Sleep(50);   //��ʱ50ms
		FlushBatchDraw();


		//�ж��Ƿ���ڰ���
		if (_kbhit() != 0)
		{
			EndBatchDraw();
			p_start = clock();    //��ʼ��Ϸ��ͣ
			RunKeyBoard();        //���̽�������
			p_end = clock();      //������Ϸ��ͣ
		}
	}
}





//������Ϸ�˵�����
void DrawMenu()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	//�˵�����ͼƬ
	putimage(0, 0, &menubackground);	

	//���ֲ���
	settextcolor(WHITE);                            //����������ɫ
	settextstyle(WINDOW_WIDE / 20, 0, L"������κ"); //����������ʽ
	setbkmode(TRANSPARENT);                         //���ֱ���͸��
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"��ʼ��Ϸ");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"��Ϸ����");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"��߼�¼");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"�˳���Ϸ");
}

//������Ϸ���н���
void DrawGameRun()
{
	//��Ϸ����ͼƬ
	putimage(0, 0, &gamebackground);
	settextcolor(WHITE);
	settextstyle(20, 0, L"���Ŀ���");
	setbkmode(TRANSPARENT);
	WCHAR str_second[10];
	wsprintf(str_second, L"��Ϸ����ʱ:%d��", game_second - int(end - start) / 1000 + pause_time);
	outtextxy(5 * WINDOW_LONG / 6, 1 * WINDOW_WIDE / 15, str_second);
	WCHAR str_sum[10];
	wsprintf(str_sum, L"����ܲ���:%d��", radish_sum);
	outtextxy(5 * WINDOW_LONG / 6, 2 * WINDOW_WIDE / 15, str_sum);
}

//������Ϸ��ͣ����
void DrawGamePause()
{
	//���ֲ���
	settextcolor(WHITE);                            //����������ɫ
	settextstyle(WINDOW_WIDE / 20, 0, L"������κ"); //����������ʽ
	setbkmode(TRANSPARENT);                         //���ֱ���͸��
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"������Ϸ");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"���¿�ʼ");
	outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"���ز˵�");
}

//������Ϸ���ý���
void DrawGameSet()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	//����ͼƬ
	putimage(0, 0, &setbackground);
	//������������
	settextcolor(WHITE);                            //����������ɫ
	settextstyle(WINDOW_WIDE / 20, 0, L"��������"); //����������ʽ
	setbkmode(TRANSPARENT);                         //���ֱ���͸��
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"��ѡ��һ�����µ�       ����");
	//��һ���ܲ�
	putimage(WINDOW_LONG / 7 + 225, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 15, &radish[0], SRCAND);
	putimage(WINDOW_LONG / 7 + 225, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 15, &radish[1], SRCPAINT);
	//�ܲ�����
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
	//��Ϸʱ��ѡ��
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"��ѡ����Ϸ��ʱ��");
	for (int i = 0; i < 3; i++)
	{
		WCHAR str_minute[7];
		wsprintf(str_minute, L"%d����", 2 * (i + 1));
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
	//���ز˵���ť
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
}

//������Ϸ��������
void DrawGameOver()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	putimage(0, 0, &overbackground);
	//������������
	settextcolor(WHITE);                            //����������ɫ
	settextstyle(WINDOW_WIDE / 20, 0, L"��������"); //����������ʽ
	setbkmode(TRANSPARENT);                         //���ֱ���͸��   
	WCHAR str_num[20];
	wsprintf(str_num, L"�����ռ���         ����Ϊ:%d��", radish_sum);
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, str_num);
	//��һ���ܲ�
	putimage(WINDOW_LONG / 7 + 150, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 20, &radish[0], SRCAND);
	putimage(WINDOW_LONG / 7 + 150, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 - 20, &radish[1], SRCPAINT);
	WCHAR str_second[10];
	wsprintf(str_second, L"������ʱ:%d��", game_second);
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, str_second);
	//����һ�ְ�ť
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10, L"����һ��");
	//���ز˵���ť
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
}

//������߼�¼����
void DrawHighScore()
{
	initgraph(WINDOW_LONG, WINDOW_WIDE);
	putimage(0, 0, &scorebackground);
	//������������
	settextcolor(WHITE);                            //����������ɫ
	settextstyle(WINDOW_WIDE / 20, 0, L"��������"); //����������ʽ
	setbkmode(TRANSPARENT);                         //���ֱ���͸��  
	//��ʱ
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"2����");
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"4����");
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10, L"6����");
	//��߷�
	WCHAR str_record[5];
	for (int i = 0; i < 3; i++)
	{
		wsprintf(str_record, L"%d��", record[i].radish_sum);
		outtextxy(WINDOW_LONG / 2, WINDOW_WIDE / 7 + 2 * i * WINDOW_WIDE / 10, str_record);
		putimage(WINDOW_LONG / 2 + 80, WINDOW_WIDE / 7 + 2 * i * WINDOW_WIDE / 10 - 20, &radish[0], SRCAND);
		putimage(WINDOW_LONG / 2 + 80, WINDOW_WIDE / 7 + 2 * i * WINDOW_WIDE / 10 - 20, &radish[1], SRCPAINT);
	}
	//���ز˵���ť
	outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
}




//��Ϸ�˵����� ��꽻��  ������������
void MenuMouse()
{
	DrawMenu();      //���Ʋ˵�����
	//���Ų˵���������
	mciSendString(L"open ����\\�˵���������.mp3 alias menu", nullptr, 0, nullptr);
	mciSendString(L"play menu repeat", nullptr, 0, nullptr);
	MOUSEMSG m;
	while (TRUE)
	{
		//��ȡ���λ��
		m = GetMouseMsg();

		//�ж����λ��
		//*******************************************************************
		//��ʼ��Ϸ
		settextcolor(RED);			//����������ɫ
		if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"��ʼ��Ϸ");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				//�رղ˵���������
				mciSendString(L"close menu", nullptr, 0, nullptr);
				//������Ϸ���б�������
				mciSendString(L"open ����\\��Ϸ��������.mp3 alias game", nullptr, 0, nullptr);
				mciSendString(L"play game repeat", nullptr, 0, nullptr);
				PlayGame();
			}
		}
		//******************************************************************
		//��Ϸ����
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"��Ϸ����");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				SetMouse();
			}
		}
		//******************************************************************
		//��߼�¼
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"��߼�¼");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				RecordMouse();
			}
		}
		//******************************************************************
		//�˳���Ϸ
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"�˳���Ϸ");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				exit(0);
			}
		}
		else
		{
			settextcolor(WHITE);	   //������ɫ��ԭ					
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 0 * WINDOW_WIDE / 10, L"��ʼ��Ϸ");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"��Ϸ����");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"��߼�¼");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"�˳���Ϸ");
		}
	}
	_getch();
	closegraph();
}

//��Ϸ���н��� ���̽���
void RunKeyBoard()
{
	char usekey = _getch();  //���ܰ���
	for (int i = 0; i < radish_num; i++)
	{
		if (usekey == array[i].target || usekey == array[i].target + 'a' - 'A')
		{
			InitArray(i);
			CreateThread(nullptr, 0, PlayBombMusic, nullptr, 0, nullptr);
			radish_sum++;
			break;
		}
		else if (usekey == 27)   //�û����esc��
		{
			mciSendString(L"close game", nullptr, 0, nullptr);
			MenuMouse();  //���ز˵�
		}
		else if (usekey == 32)   //�û����space��  ������ͣ����
		{
			mciSendString(L"pause game", nullptr, 0, nullptr);
			PauseMouse();
			mciSendString(L"resume game", nullptr, 0, nullptr);
			return;
		}
	}
}

//��Ϸ��ͣ���� ��꽻��
void PauseMouse()
{
	MOUSEMSG m;
	while (TRUE)
	{
		DrawGamePause();      //������ͣ����
		//��ȡ���λ��
		m = GetMouseMsg();
		settextcolor(YELLOW);	   //�����ɫ
		//������Ϸ
		if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"������Ϸ");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				p_flag = 1;  //�ı���ͣ����
				return;
			}
		}

		//���¿�ʼ
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"���¿�ʼ");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				radish_sum = 0;
				PlayGame();
			}
		}

		//���ز˵�
		else if (m.x >= WINDOW_LONG / 2 - WINDOW_LONG / 15 &&
			m.x <= WINDOW_LONG / 2 + WINDOW_LONG / 15 &&
			m.y >= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 &&
			m.y <= WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10 + WINDOW_WIDE / 20)
		{
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"���ز˵�");
			//������
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(L"close game", nullptr, 0, nullptr);
				MenuMouse();
			}
		}
		else
		{
			settextcolor(WHITE);	   //������ɫ��ԭ					
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, L"������Ϸ");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 2 * WINDOW_WIDE / 10, L"���¿�ʼ");
			outtextxy(WINDOW_LONG / 2 - WINDOW_LONG / 15, WINDOW_WIDE / 7 + 3 * WINDOW_WIDE / 10, L"���ز˵�");
		}
	}
}

//��Ϸ���ý��� ��꽻��
void SetMouse()
{
	DrawGameSet();
	MOUSEMSG m;
	while (TRUE)
	{
		//��ȡ���λ��
		m = GetMouseMsg();
		//������������
		settextstyle(WINDOW_WIDE / 20, 0, L"��������"); //����������ʽ
		setbkmode(TRANSPARENT);                         //���ֱ���͸��
		//�ܲ�������������
		for (int i = 0; i < 7; i++)
		{
			if (m.x >= WINDOW_LONG / 7 + i * 51 && m.y >= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 &&
				m.x <= WINDOW_LONG / 7 + i * 51 + 20 && m.y <= WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10 + 24)
			{
				settextcolor(YELLOW);	//����ڵ�ʱ���������ɫ
				//�����  �ı����
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					radish_num = i + 3;
				}
			}
			else if (i + 3 == radish_num)
			{
				settextcolor(YELLOW);   //�����ڵĸ���������Ϊ��ɫ
			}
			else
			{
				settextcolor(WHITE);    //��겻��ʱ�������ɫ
			}
			WCHAR str_num[2];
			wsprintf(str_num, L"%d", i + 3);
			outtextxy(WINDOW_LONG / 7 + i * 51, WINDOW_WIDE / 7 + 1 * WINDOW_WIDE / 10, str_num);
		}

		//��Ϸʱ����������
		for (int i = 0; i < 3; i++)
		{
			if (m.x >= WINDOW_LONG / 7 + i * 110 && m.y >= WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10 &&
				m.x <= WINDOW_LONG / 7 + i * 110 + 80 && m.y <= WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10 + 26)
			{
				settextcolor(YELLOW);	//����ڵ�ʱ���������ɫ
				//�����  �ı�ʱ��
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					game_second = 60 * 2 * (i + 1);
				}
			}
			else if (2 * (i + 1) == game_second / 60)
			{
				settextcolor(YELLOW);   //�����ڵ�ʱ��������Ϊ��ɫ
			}
			else
			{
				settextcolor(WHITE);    //��겻��ʱ�������ɫ
			}
			WCHAR str_minute[7];
			wsprintf(str_minute, L"%d����", 2 * (i + 1));
			outtextxy(WINDOW_LONG / 7 + i * 110, WINDOW_WIDE / 7 + 4 * WINDOW_WIDE / 10, str_minute);
		}

		//���ز˵���������
		if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//����ڵ�ʱ���������ɫ
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				MenuMouse();  //���ز˵�
			}
		}
		else
		{
			settextcolor(WHITE);    //��겻��ʱ�������ɫ
		}
		outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
	}
}

//��Ϸ�������� ��꽻��  ������������
void OverMouse()
{
	
	ReadHighRecordFile();//��ȡ��߼�¼�ļ�
	SaveHighRecordFile();//������߼�¼�ļ�

	DrawGameOver();
	MOUSEMSG m;
	while (TRUE)
	{
		//��ȡ���λ��
		m = GetMouseMsg();
		//������������
		settextstyle(WINDOW_WIDE / 20, 0, L"��������"); //����������ʽ
		setbkmode(TRANSPARENT);                         //���ֱ���͸��
		//����һ�ְ�������
		if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//����ڵ�ʱ���������ɫ
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10, L"����һ��");
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				radish_sum = 0;
				PlayGame();  //����һ��
			}

		}
		//���ز˵���������
		else if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//����ڵ�ʱ���������ɫ
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(L"close game", nullptr, 0, nullptr);
				MenuMouse();  //���ز˵�
			}
		}
		else
		{
			settextcolor(WHITE);    //��겻��ʱ�������ɫ
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 5 * WINDOW_WIDE / 10, L"����һ��");
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
		}

	}
}

//��Ϸ��¼���� ��꽻��  
void RecordMouse()
{
	//�ļ�����
	ReadHighRecordFile();
	DrawHighScore();
	MOUSEMSG m;
	while (TRUE)
	{
		//��ȡ���λ��
		m = GetMouseMsg();
		//������������
		settextstyle(WINDOW_WIDE / 20, 0, L"��������"); //����������ʽ
		setbkmode(TRANSPARENT);                         //���ֱ���͸��
		//���ز˵���������
		if (m.x >= WINDOW_LONG / 7 && m.y >= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 &&
			m.x <= WINDOW_LONG / 7 + 110 && m.y <= WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10 + 26)
		{
			settextcolor(YELLOW);	//����ڵ�ʱ���������ɫ
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(L"close game", nullptr, 0, nullptr);
				MenuMouse();  //���ز˵�
			}
		}
		else
		{
			settextcolor(WHITE);    //��겻��ʱ�������ɫ
			outtextxy(WINDOW_LONG / 7, WINDOW_WIDE / 7 + 6 * WINDOW_WIDE / 10, L"���ز˵�");
		}

	}
}





//��ȡ��߼�¼�ļ�
void ReadHighRecordFile()
{
	FILE *fp;
	if ((fopen_s(&fp, "��߼�¼.txt", "r")) != 0)
	{
		if ((fopen_s(&fp, "��߼�¼.txt", "w")) != 0)
		{
			printf("������߼�¼�ļ�ʧ��\n");
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

//������߼�¼�ļ�
void SaveHighRecordFile()
{
	FILE* fp;
	if ((fopen_s(&fp, "��߼�¼.txt", "r+")) != 0)
	{
		printf("����߼�¼�ļ�ʧ��\n");
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






//������ĸ���ܲ����
void PrintLetterRadish(int x, int y, char target)
{
	settextcolor(WHITE);
	settextstyle(18, 0, L"MS Reference Sans Serif");
	setbkmode(TRANSPARENT);
	putimage(x, y, &radish[0], SRCAND);
	putimage(x, y, &radish[1], SRCPAINT);
	outtextxy(x + RADISH_LONG / 2 - RADISH_LONG / 17, y + RADISH_WIDE / 2 + RADISH_WIDE / 10, target);
}

//������ʼ���ܲ�
void InitArray(int i)
{
	srand(unsigned int(time(nullptr)));      //�����������
	array[i].target = rand() % 26 + 'A';   //�������26��Ӣ�Ĵ�д��ĸ
	//��ĸ�ظ� ��������
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
			array[i].target = rand() % 26 + 'A';   //�������26��Ӣ�Ĵ�д��ĸ
		}
		else
		{
			break;
		}
	}
	array[i].x = rand() % 14 * WINDOW_LONG / 20 + 3 * WINDOW_LONG / 20;
	array[i].y = rand() % 50 - 100;
	//�ܲ��غ����� ��������
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

//���̲߳��ű�ը��Ч
DWORD WINAPI PlayBombMusic(LPVOID LpParale)
{
	mciSendString(L"open ����\\��ը.mp3 alias bomb", nullptr, 0, nullptr);
	mciSendString(L"play bomb wait", nullptr, 0, nullptr);
	mciSendString(L"close bomb", nullptr, 0, nullptr);
	return 0;
}

//������Դ
void LoadResource()
{
	loadimage(&menubackground, L"ͼƬ\\�˵�����ͼ.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&gamebackground, L"ͼƬ\\��Ϸ����ͼ.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&setbackground, L"ͼƬ\\���ñ���ͼ.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&overbackground, L"ͼƬ\\��Ϸ��������ͼ.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&scorebackground, L"ͼƬ\\��߼�¼����ͼ.jpg", WINDOW_LONG, WINDOW_WIDE);
	loadimage(&radish[0], L"ͼƬ\\�ܲ�����ͼ.jpg", RADISH_LONG, RADISH_WIDE);
	loadimage(&radish[1], L"ͼƬ\\�ܲ�����ͼ.jpg", RADISH_LONG, RADISH_WIDE);
}