#include<windows.h>
#include<strsafe.h>

#define INTERVAL 15        //棋盘间隔

static char stone[INTERVAL][INTERVAL] = { 0 };                           //棋盘数据
static bool flag = true;                                                           //黑白轮循
static char outcome;    
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);                           //窗口回调

void CreateBoard(HDC hdc, int cxClient, int cyClient);     //绘制棋盘
int Pieces_x(int cxClient, int cyClient, int x);         //获取棋子x坐标函数
int Pieces_y(int cxClient, int cyClient, int y);      //获取棋子y坐标函数
void RawPieces_white(HDC hdc, int chess_x, int chess_y, int cxClient, int cyClient);//绘制白色棋子
void RawPieces_black(HDC hdc, int chess_x, int chess_y, int cxClient, int cyClient);//绘制黑色棋子
char Outcome(void);                                     //成五判断胜负
void Aichess(int m, int n);                                                                                                                        //AI下棋

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	static TCHAR szAppName[] = TEXT("MyGame");                //定义类名称
	MSG msg;                    //消息句柄
	HWND hwnd;                      //句柄
	WNDCLASS wndclass;               //定义窗口类对象

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	//注册并检查结果
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("注册失败！"), TEXT("错误"), MB_OK);
	}

	//创建实例化窗口类
	hwnd = CreateWindow(szAppName, TEXT("欧晨曦"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	//显示窗口
	ShowWindow(hwnd, nCmdShow);
	//更新窗口
	UpdateWindow(hwnd);

	//消息循环
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);                        //转换翻译消息
		DispatchMessage(&msg);                        //调度消息
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;                                                      
	PAINTSTRUCT ps;

	static TCHAR szBuffer[10];
	size_t lenght;

	static int cxClient;                                //宽度
	static int cyClient;                                //高度

	static int m;
	static int n;

	switch (message)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);                //宽度
		cyClient = HIWORD(lParam);                //高度

		return 0;

	case WM_LBUTTONDOWN:
		hdc = GetDC(hwnd);

		m = Pieces_x(cxClient, cyClient, LOWORD(lParam));
		n = Pieces_y(cxClient, cyClient, HIWORD(lParam));

		if ((stone[m][n] == 0) && flag == true && outcome != 2)
		{
			stone[m][n] = 1;
			flag = false;
			Aichess(m, n);
		}

		if (outcome != 2)
			outcome = Outcome();

		InvalidateRect(hwnd, NULL, TRUE);
		ReleaseDC(hwnd, hdc);
		return 0;

	case WM_RBUTTONDOWN:

		if (2 == outcome)
		{
			for (int i = 0; i < INTERVAL; i++)
			{
				for (int j = 0; j < INTERVAL; j++)
					stone[i][j] = 0;
			}
			outcome = 0;
			flag = true;
		}

		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);

		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		//点击坐标显示
		StringCchPrintf(szBuffer, 10, TEXT("%d  %d"), m, n);
		StringCchLength(szBuffer, 10, &lenght);
		TextOut(hdc, 100, 100, szBuffer, lenght);

		CreateBoard(hdc, cxClient, cyClient);
		for (m = 0; m <INTERVAL; m++)
		{
			for (n = 0; n <INTERVAL; n++)
			{
				if (stone[m][n] == 1)
				{
					RawPieces_white(hdc, m, n, cxClient, cyClient);
				}
				else if (stone[m][n] == -1)
				{
					RawPieces_black(hdc, m, n, cxClient, cyClient);
				}
			}
		}

		if (1 == outcome)
		{
			MessageBox(hwnd, TEXT("白棋获胜"), TEXT("结果"), MB_OK);
			outcome = 2;
		}
		else if (-1 == outcome)
		{
			MessageBox(hwnd, TEXT("黑棋获胜"), TEXT("结果"), MB_OK);
			outcome = 2;
		}

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);

		return 0;

	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Aichess(int m, int n)
{
	char count_one = 1;                                //横方向分数
	char count_two = 1;                                //竖方向分数
	char count_three = 1;                        //左上到右下分数
	char count_four = 1;                        //左下到右上分数

	if (stone[m][n] == 1)
	{
		for (int i = 1; i < 3; i++)        //横方向
		{
			if (stone[m - i][n] == 1 || stone[m + i][n] == 1)
				count_one++;
		}
		for (int i = 1; i < 3; i++)        //竖方向
		{
			if (stone[m][n - i] == 1 || stone[m][n + i] == 1)
				count_two++;
		}
		for (int i = 1; i < 3; i++)        //左上到右下方向
		{
			if (stone[m - i][n - i] == 1 || stone[m + i][n + i] == 1)
				count_three++;
		}
		for (int i = 1; i < 3; i++)        //左下到右上方向
		{
			if (stone[m - i][n + i] == 1 || stone[m + i][n - i] == 1)
				count_four++;
		}
	}
	if (count_one >= count_two && count_one >= count_three && count_one >= count_four)
	{
		for (int i = 1; i < 4; i++)        //横方向下棋
		{
			if (stone[m - i][n] == 0) { stone[m - i][n] = -1; break; }
			else if (stone[m + i][n] == 0) { stone[m + i][n] = -1; break; }
		}
	}
	else if (count_two >= count_one && count_two >= count_three && count_two >= count_four)
	{
		for (int i = 1; i < 4; i++)        //竖方向下棋
		{
			if (stone[m][n - i] == 0) { stone[m][n - i] = -1; break; }
			else if (stone[m][n + i] == 0) { stone[m][n + i] = -1; break; }
		}
	}
	else if (count_three >= count_one && count_three >= count_two && count_three >= count_four)
	{
		for (int i = 1; i < 4; i++)        //左上到右下下棋
		{
			if (stone[m - i][n - i] == 0) { stone[m - i][n - i] = -1; break; }
			else if (stone[m + i][n + i] == 0) { stone[m + i][n + i] = -1; break; }
		}
	}
	else if (count_four >= count_one && count_four >= count_two && count_four >= count_three)
	{
		for (int i = 1; i < 4; i++)        //左下到右上下棋
		{
			if (stone[m - i][n + i] == 0) { stone[m - i][n + i] = -1; break; }
			else if (stone[m + i][n - i] == 0) { stone[m + i][n - i] = -1; break; }
		}
	}
	flag = true;
}

char Outcome(void)
{
	char white_count = 0;                                        //临时变量，用以记取分数，当分数达到5分时，则判断一方输赢。
	char black_count = 0;                                        //临时变量，用以记取分数，当分数达到5分时，则判断一方输赢。

	char result = 0;                                //返回的结果是1，白棋赢，返回的结构是-1，则黑棋赢

	for (int m = 0; m < INTERVAL; m++)
	{
		for (int n = 0; n < INTERVAL; n++)
		{
			if (stone[m][n] == 1)
			{
				white_count = 1;
				for (int i = 1; i <= 4; i++)                                //向右检测
				{
					if (stone[m + i][n] == 1 && m + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m + i][n] == -1 || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //向左检测
				{
					if (stone[m - i][n] == 1 && m - i >= 0)
					{
						white_count++;
					}
					else if (stone[m - i][n] == -1 || m - i < 0)
						break;
				}

				if (white_count != 5)                                                //没有等于5则初始化
					white_count = 1;
				else if (white_count == 5)
					return 1;

				for (int i = 1; i <= 4; i++)                                //判断向下方向
				{
					if (stone[m][n + i] == 1 && n + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m][n + i] == -1 || n + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //判断向上方向
				{
					if (stone[m][n - i] == 1 && n - i >= 0)
					{
						white_count++;
					}
					else if (stone[m][n - i] == -1 || n - i < 0)
						break;
				}

				if (white_count != 5)                                                //没有等于5则初始化
					white_count = 1;
				else if (white_count == 5)
					return 1;

				for (int i = 1; i <= 4; i++)                                //左上到右下
				{
					if (stone[m + i][n + i] == 1 && n + i < INTERVAL && m + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m + i][n + i] == -1 || n + i >= INTERVAL || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //左上到右下
				{
					if (stone[m - i][n - i] == 1 && n - i >= 0 && m - i >= 0)
					{
						white_count++;
					}
					else if (stone[m - i][n - i] == -1 || n - i < 0 || m - i < 0)
						break;
				}

				if (white_count != 5)                                                //没有等于5则初始化
					white_count = 1;
				else if (white_count == 5)
					return 1;

				for (int i = 1; i <= 4; i++)                                //左下到右上
				{
					if (stone[m - i][n + i] == 1 && n + i < INTERVAL && m - i > 0)
					{
						white_count++;
					}
					else if (stone[m - i][n + i] == -1 || n + i >= INTERVAL || m - i < 0)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //左下到右上
				{
					if (stone[m + i][n - i] == 1 && n - i >= 0 && m + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m + i][n - i] == -1 || n - i < 0 || m + i >= INTERVAL)
						break;
				}

				if (white_count != 5)                                                //没有等于5则初始化
					white_count = 0;
				else if (white_count == 5)
					return 1;
			}
			else if (stone[m][n] == -1)
			{
				black_count = 1;
				for (int i = 1; i <= 4; i++)                                //向右检测
				{
					if (stone[m + i][n] == -1 && m + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m + i][n] == 1 || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //向左检测
				{
					if (stone[m - i][n] == -1 && m - i >= 0)
					{
						black_count++;
					}
					else if (stone[m - i][n] == 1 || m - i < 0)
						break;
				}

				if (black_count != 5)                                                //没有等于5则初始化
					black_count = 1;
				else if (black_count == 5)
					return -1;

				for (int i = 1; i <= 4; i++)                                //判断向下方向
				{
					if (stone[m][n + i] == -1 && n + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m][n + i] == 1 || n + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //判断向上方向
				{
					if (stone[m][n - i] == -1 && n - i >= 0)
					{
						black_count++;
					}
					else if (stone[m][n - i] == 1 || n - i < 0)
						break;
				}

				if (black_count != 5)                                                //没有等于5则初始化
					black_count = 1;
				else if (black_count == 5)
					return -1;

				for (int i = 1; i <= 4; i++)                                //左上到右下
				{
					if (stone[m + i][n + i] == -1 && n + i < INTERVAL && m + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m + i][n + i] == 1 || n + i >= INTERVAL || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //左上到右下
				{
					if (stone[m - i][n - i] == -1 && n - i >= 0 && m - i >= 0)
					{
						black_count++;
					}
					else if (stone[m - i][n - i] == 1 || n - i < 0 || m - i < 0)
						break;
				}

				if (black_count != 5)                                                //没有等于5则初始化
					black_count = 1;
				else if (black_count == 5)
					return -1;

				for (int i = 1; i <= 4; i++)                                //左下到右上
				{
					if (stone[m - i][n + i] == -1 && n + i < INTERVAL && m - i > 0)
					{
						black_count++;
					}
					else if (stone[m - i][n + i] == 1 || n + i >= INTERVAL || m - i < 0)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //左下到右上
				{
					if (stone[m + i][n - i] == -1 && n - i >= 0 && m + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m + i][n - i] == 1 || n - i < 0 || m + i >= INTERVAL)
						break;
				}

				if (black_count != 5)                                                //没有等于5则初始化
					black_count = 0;
				else if (black_count == 5)
					return -1;
			}
		}
	}

	if (white_count == 5)
	{
		result = 1;
	}
	else if (black_count == 5)
	{
		result = -1;
	}

	return result;                                        //返回结果
}

int Pieces_x(int cxClient, int cyClient, int x)
{
	int count = 0;                                                                                //临时变量，记取坐标

	if (cxClient < cyClient)
	{
		for (int i = 0; i < INTERVAL; i++)
		{
			if (x >(i + 1) * cxClient / 16 + cxClient / 16 / 2)
				count++;
		}
	}
	else
	{
		for (int i = 0; i < INTERVAL; i++)
		{
			if (x >((cxClient - 14 * cyClient / 16) / 2 + i * cyClient / 16) + cyClient / 16 / 2)
				count++;
		}
	}
	if (count >= 15)
		count = count - 1;

	return count;
}

int Pieces_y(int cxClient, int cyClient, int y)
{
	int count = 0;                                                                        //临时变量，记取坐标                                        

	if (cxClient < cyClient)
	{
		for (int i = 0; i < INTERVAL; i++)
		{
			if (y >((cyClient - 14 * cxClient / 16) / 2 + i * cxClient / 16) + cxClient / 16 / 2)
				count++;
		}
	}
	else
	{
		for (int i = 0; i < INTERVAL; i++)
		{
			if (y >(i + 1) * cyClient / 16 + cyClient / 16 / 2)
				count++;
		}
	}
	if (count >= 15)
		count = count - 1;

	return count;
}

void RawPieces_white(HDC hdc, int chess_x, int chess_y, int cxClient, int cyClient)
{
	HGDIOBJ hPen, hOldPen;                                        //画刷句柄

	hPen = GetStockObject(WHITE_BRUSH);                        //获取白色色画刷
	hOldPen = SelectObject(hdc, hPen);                                        //设置默认画刷为黑色画刷

	if (cxClient < cyClient)
	{
		Ellipse(hdc, (chess_x + 1) * cxClient / 16 - 4 * cxClient / 16 / 9, (cyClient - 14 * cxClient / 16) / 2 + chess_y * cxClient / 16 - 4 * cxClient / 16 / 9,
			(chess_x + 1) * cxClient / 16 + 4 * cxClient / 16 / 9, (cyClient - 14 * cxClient / 16) / 2 + chess_y * cxClient / 16 + 4 * cxClient / 16 / 9);
	}
	else
	{
		Ellipse(hdc, (cxClient - 14 * cyClient / 16) / 2 + chess_x * cyClient / 16 - 4 * cyClient / 16 / 9, (chess_y + 1) * cyClient / 16 - 4 * cyClient / 16 / 9,
			(cxClient - 14 * cyClient / 16) / 2 + chess_x * cyClient / 16 + 4 * cyClient / 16 / 9, (chess_y + 1) * cyClient / 16 + 4 * cyClient / 16 / 9);
	}

	SelectObject(hdc, hOldPen);
}

void RawPieces_black(HDC hdc, int chess_x, int chess_y, int cxClient, int cyClient)
{
	HGDIOBJ hPen, hOldPen;                                        //画刷句柄

	hPen = GetStockObject(BLACK_BRUSH);                        //获取系统黑色画刷
	hOldPen = SelectObject(hdc, hPen);                                        //设置默认画刷为黑色画刷

																			  //Sleep(300);
	if (cxClient < cyClient)
	{
		Ellipse(hdc, (chess_x + 1) * cxClient / 16 - 4 * cxClient / 16 / 9, (cyClient - 14 * cxClient / 16) / 2 + chess_y * cxClient / 16 - 4 * cxClient / 16 / 9,
			(chess_x + 1) * cxClient / 16 + 4 * cxClient / 16 / 9, (cyClient - 14 * cxClient / 16) / 2 + chess_y * cxClient / 16 + 4 * cxClient / 16 / 9);
	}
	else
	{
		Ellipse(hdc, (cxClient - 14 * cyClient / 16) / 2 + chess_x * cyClient / 16 - 4 * cyClient / 16 / 9, (chess_y + 1) * cyClient / 16 - 4 * cyClient / 16 / 9,
			(cxClient - 14 * cyClient / 16) / 2 + chess_x * cyClient / 16 + 4 * cyClient / 16 / 9, (chess_y + 1) * cyClient / 16 + 4 * cyClient / 16 / 9);
	}

	SelectObject(hdc, hOldPen);
}

void CreateBoard(HDC hdc, int cxClient, int cyClient)
{
	HGDIOBJ hPen, hOldPen;                                        //画刷句柄

																  //屏幕自适应，如果在客户区宽度比高度低，则以客户区宽度参数为准，如果客户区高度比宽度低，则以客户区高度参数为准；
	if (cxClient < cyClient)
	{
		for (int i = 0; i < INTERVAL; i++)
		{
			MoveToEx(hdc, cxClient / 16, (cyClient - 14 * cxClient / 16) / 2 + i * cxClient / 16, NULL);
			LineTo(hdc, 15 * cxClient / 16, (cyClient - 14 * cxClient / 16) / 2 + i * cxClient / 16);
			MoveToEx(hdc, (i + 1) * cxClient / 16, (cyClient - 14 * cxClient / 16) / 2, NULL);
			LineTo(hdc, (i + 1) * cxClient / 16, (cyClient - 14 * cxClient / 16) / 2 + 14 * cxClient / 16);
		}

		hPen = GetStockObject(BLACK_BRUSH);
		hOldPen = SelectObject(hdc, hPen);

		//[3][3]坐标加粗
		Ellipse(hdc, (3 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 - 3,
			(3 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 + 3);

		//[3][11]坐标加粗
		Ellipse(hdc, (3 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 - 3,
			(3 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 + 3);

		//[11][3]坐标加粗
		Ellipse(hdc, (11 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 - 3,
			(11 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 + 3);

		//[11][11]坐标加粗
		Ellipse(hdc, (11 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 - 3,
			(11 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 + 3);

		//[7][7]坐标加粗
		Ellipse(hdc, (7 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 7 * cxClient / 16 - 3,
			(7 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 7 * cxClient / 16 + 3);

		SelectObject(hdc, hOldPen);
	}
	else
	{
		int base_x = (cxClient - 14 * cyClient / 16) / 2;

		for (int i = 0; i < INTERVAL; i++)
		{
			MoveToEx(hdc, base_x, (i + 1) * cyClient / 16, NULL);
			LineTo(hdc, base_x + 14 * cyClient / 16, (i + 1) * cyClient / 16);
			MoveToEx(hdc, base_x + i * cyClient / 16, cyClient / 16, NULL);
			LineTo(hdc, base_x + i * cyClient / 16, 15 * cyClient / 16);
		}

		hPen = GetStockObject(BLACK_BRUSH);
		hOldPen = SelectObject(hdc, hPen);

		//[3][3]坐标加粗
		Ellipse(hdc, base_x + 3 * cyClient / 16 - 3, (3 + 1) * cyClient / 16 - 3,
			base_x + 3 * cyClient / 16 + 3, (3 + 1) * cyClient / 16 + 3);

		//[3][11]坐标加粗
		Ellipse(hdc, base_x + 3 * cyClient / 16 - 3, (11 + 1) * cyClient / 16 - 3,
			base_x + 3 * cyClient / 16 + 3, (11 + 1) * cyClient / 16 + 3);

		//[11][3]坐标加粗
		Ellipse(hdc, base_x + 11 * cyClient / 16 - 3, (3 + 1) * cyClient / 16 - 3,
			base_x + 11 * cyClient / 16 + 3, (3 + 1) * cyClient / 16 + 3);

		//[11][11]坐标加粗
		Ellipse(hdc, base_x + 11 * cyClient / 16 - 3, (11 + 1) * cyClient / 16 - 3,
			base_x + 11 * cyClient / 16 + 3, (11 + 1) * cyClient / 16 + 3);

		//[7][7]坐标加粗
		Ellipse(hdc, base_x + 7 * cyClient / 16 - 3, (7 + 1) * cyClient / 16 - 3,
			base_x + 7 * cyClient / 16 + 3, (7 + 1) * cyClient / 16 + 3);

		SelectObject(hdc, hOldPen);

	}
}