#include<windows.h>
#include<strsafe.h>

#define INTERVAL 15        //���̼��

static char stone[INTERVAL][INTERVAL] = { 0 };                           //��������
static bool flag = true;                                                           //�ڰ���ѭ
static char outcome;    
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);                           //���ڻص�

void CreateBoard(HDC hdc, int cxClient, int cyClient);     //��������
int Pieces_x(int cxClient, int cyClient, int x);         //��ȡ����x���꺯��
int Pieces_y(int cxClient, int cyClient, int y);      //��ȡ����y���꺯��
void RawPieces_white(HDC hdc, int chess_x, int chess_y, int cxClient, int cyClient);//���ư�ɫ����
void RawPieces_black(HDC hdc, int chess_x, int chess_y, int cxClient, int cyClient);//���ƺ�ɫ����
char Outcome(void);                                     //�����ж�ʤ��
void Aichess(int m, int n);                                                                                                                        //AI����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	static TCHAR szAppName[] = TEXT("MyGame");                //����������
	MSG msg;                    //��Ϣ���
	HWND hwnd;                      //���
	WNDCLASS wndclass;               //���崰�������

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

	//ע�Ტ�����
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("ע��ʧ�ܣ�"), TEXT("����"), MB_OK);
	}

	//����ʵ����������
	hwnd = CreateWindow(szAppName, TEXT("ŷ����"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	//��ʾ����
	ShowWindow(hwnd, nCmdShow);
	//���´���
	UpdateWindow(hwnd);

	//��Ϣѭ��
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);                        //ת��������Ϣ
		DispatchMessage(&msg);                        //������Ϣ
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;                                                      
	PAINTSTRUCT ps;

	static TCHAR szBuffer[10];
	size_t lenght;

	static int cxClient;                                //���
	static int cyClient;                                //�߶�

	static int m;
	static int n;

	switch (message)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);                //���
		cyClient = HIWORD(lParam);                //�߶�

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

		//���������ʾ
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
			MessageBox(hwnd, TEXT("�����ʤ"), TEXT("���"), MB_OK);
			outcome = 2;
		}
		else if (-1 == outcome)
		{
			MessageBox(hwnd, TEXT("�����ʤ"), TEXT("���"), MB_OK);
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
	char count_one = 1;                                //�᷽�����
	char count_two = 1;                                //���������
	char count_three = 1;                        //���ϵ����·���
	char count_four = 1;                        //���µ����Ϸ���

	if (stone[m][n] == 1)
	{
		for (int i = 1; i < 3; i++)        //�᷽��
		{
			if (stone[m - i][n] == 1 || stone[m + i][n] == 1)
				count_one++;
		}
		for (int i = 1; i < 3; i++)        //������
		{
			if (stone[m][n - i] == 1 || stone[m][n + i] == 1)
				count_two++;
		}
		for (int i = 1; i < 3; i++)        //���ϵ����·���
		{
			if (stone[m - i][n - i] == 1 || stone[m + i][n + i] == 1)
				count_three++;
		}
		for (int i = 1; i < 3; i++)        //���µ����Ϸ���
		{
			if (stone[m - i][n + i] == 1 || stone[m + i][n - i] == 1)
				count_four++;
		}
	}
	if (count_one >= count_two && count_one >= count_three && count_one >= count_four)
	{
		for (int i = 1; i < 4; i++)        //�᷽������
		{
			if (stone[m - i][n] == 0) { stone[m - i][n] = -1; break; }
			else if (stone[m + i][n] == 0) { stone[m + i][n] = -1; break; }
		}
	}
	else if (count_two >= count_one && count_two >= count_three && count_two >= count_four)
	{
		for (int i = 1; i < 4; i++)        //����������
		{
			if (stone[m][n - i] == 0) { stone[m][n - i] = -1; break; }
			else if (stone[m][n + i] == 0) { stone[m][n + i] = -1; break; }
		}
	}
	else if (count_three >= count_one && count_three >= count_two && count_three >= count_four)
	{
		for (int i = 1; i < 4; i++)        //���ϵ���������
		{
			if (stone[m - i][n - i] == 0) { stone[m - i][n - i] = -1; break; }
			else if (stone[m + i][n + i] == 0) { stone[m + i][n + i] = -1; break; }
		}
	}
	else if (count_four >= count_one && count_four >= count_two && count_four >= count_three)
	{
		for (int i = 1; i < 4; i++)        //���µ���������
		{
			if (stone[m - i][n + i] == 0) { stone[m - i][n + i] = -1; break; }
			else if (stone[m + i][n - i] == 0) { stone[m + i][n - i] = -1; break; }
		}
	}
	flag = true;
}

char Outcome(void)
{
	char white_count = 0;                                        //��ʱ���������Լ�ȡ�������������ﵽ5��ʱ�����ж�һ����Ӯ��
	char black_count = 0;                                        //��ʱ���������Լ�ȡ�������������ﵽ5��ʱ�����ж�һ����Ӯ��

	char result = 0;                                //���صĽ����1������Ӯ�����صĽṹ��-1�������Ӯ

	for (int m = 0; m < INTERVAL; m++)
	{
		for (int n = 0; n < INTERVAL; n++)
		{
			if (stone[m][n] == 1)
			{
				white_count = 1;
				for (int i = 1; i <= 4; i++)                                //���Ҽ��
				{
					if (stone[m + i][n] == 1 && m + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m + i][n] == -1 || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //������
				{
					if (stone[m - i][n] == 1 && m - i >= 0)
					{
						white_count++;
					}
					else if (stone[m - i][n] == -1 || m - i < 0)
						break;
				}

				if (white_count != 5)                                                //û�е���5���ʼ��
					white_count = 1;
				else if (white_count == 5)
					return 1;

				for (int i = 1; i <= 4; i++)                                //�ж����·���
				{
					if (stone[m][n + i] == 1 && n + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m][n + i] == -1 || n + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //�ж����Ϸ���
				{
					if (stone[m][n - i] == 1 && n - i >= 0)
					{
						white_count++;
					}
					else if (stone[m][n - i] == -1 || n - i < 0)
						break;
				}

				if (white_count != 5)                                                //û�е���5���ʼ��
					white_count = 1;
				else if (white_count == 5)
					return 1;

				for (int i = 1; i <= 4; i++)                                //���ϵ�����
				{
					if (stone[m + i][n + i] == 1 && n + i < INTERVAL && m + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m + i][n + i] == -1 || n + i >= INTERVAL || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //���ϵ�����
				{
					if (stone[m - i][n - i] == 1 && n - i >= 0 && m - i >= 0)
					{
						white_count++;
					}
					else if (stone[m - i][n - i] == -1 || n - i < 0 || m - i < 0)
						break;
				}

				if (white_count != 5)                                                //û�е���5���ʼ��
					white_count = 1;
				else if (white_count == 5)
					return 1;

				for (int i = 1; i <= 4; i++)                                //���µ�����
				{
					if (stone[m - i][n + i] == 1 && n + i < INTERVAL && m - i > 0)
					{
						white_count++;
					}
					else if (stone[m - i][n + i] == -1 || n + i >= INTERVAL || m - i < 0)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //���µ�����
				{
					if (stone[m + i][n - i] == 1 && n - i >= 0 && m + i < INTERVAL)
					{
						white_count++;
					}
					else if (stone[m + i][n - i] == -1 || n - i < 0 || m + i >= INTERVAL)
						break;
				}

				if (white_count != 5)                                                //û�е���5���ʼ��
					white_count = 0;
				else if (white_count == 5)
					return 1;
			}
			else if (stone[m][n] == -1)
			{
				black_count = 1;
				for (int i = 1; i <= 4; i++)                                //���Ҽ��
				{
					if (stone[m + i][n] == -1 && m + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m + i][n] == 1 || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //������
				{
					if (stone[m - i][n] == -1 && m - i >= 0)
					{
						black_count++;
					}
					else if (stone[m - i][n] == 1 || m - i < 0)
						break;
				}

				if (black_count != 5)                                                //û�е���5���ʼ��
					black_count = 1;
				else if (black_count == 5)
					return -1;

				for (int i = 1; i <= 4; i++)                                //�ж����·���
				{
					if (stone[m][n + i] == -1 && n + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m][n + i] == 1 || n + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //�ж����Ϸ���
				{
					if (stone[m][n - i] == -1 && n - i >= 0)
					{
						black_count++;
					}
					else if (stone[m][n - i] == 1 || n - i < 0)
						break;
				}

				if (black_count != 5)                                                //û�е���5���ʼ��
					black_count = 1;
				else if (black_count == 5)
					return -1;

				for (int i = 1; i <= 4; i++)                                //���ϵ�����
				{
					if (stone[m + i][n + i] == -1 && n + i < INTERVAL && m + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m + i][n + i] == 1 || n + i >= INTERVAL || m + i >= INTERVAL)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //���ϵ�����
				{
					if (stone[m - i][n - i] == -1 && n - i >= 0 && m - i >= 0)
					{
						black_count++;
					}
					else if (stone[m - i][n - i] == 1 || n - i < 0 || m - i < 0)
						break;
				}

				if (black_count != 5)                                                //û�е���5���ʼ��
					black_count = 1;
				else if (black_count == 5)
					return -1;

				for (int i = 1; i <= 4; i++)                                //���µ�����
				{
					if (stone[m - i][n + i] == -1 && n + i < INTERVAL && m - i > 0)
					{
						black_count++;
					}
					else if (stone[m - i][n + i] == 1 || n + i >= INTERVAL || m - i < 0)
						break;
				}
				for (int i = 1; i <= 4; i++)                                //���µ�����
				{
					if (stone[m + i][n - i] == -1 && n - i >= 0 && m + i < INTERVAL)
					{
						black_count++;
					}
					else if (stone[m + i][n - i] == 1 || n - i < 0 || m + i >= INTERVAL)
						break;
				}

				if (black_count != 5)                                                //û�е���5���ʼ��
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

	return result;                                        //���ؽ��
}

int Pieces_x(int cxClient, int cyClient, int x)
{
	int count = 0;                                                                                //��ʱ��������ȡ����

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
	int count = 0;                                                                        //��ʱ��������ȡ����                                        

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
	HGDIOBJ hPen, hOldPen;                                        //��ˢ���

	hPen = GetStockObject(WHITE_BRUSH);                        //��ȡ��ɫɫ��ˢ
	hOldPen = SelectObject(hdc, hPen);                                        //����Ĭ�ϻ�ˢΪ��ɫ��ˢ

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
	HGDIOBJ hPen, hOldPen;                                        //��ˢ���

	hPen = GetStockObject(BLACK_BRUSH);                        //��ȡϵͳ��ɫ��ˢ
	hOldPen = SelectObject(hdc, hPen);                                        //����Ĭ�ϻ�ˢΪ��ɫ��ˢ

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
	HGDIOBJ hPen, hOldPen;                                        //��ˢ���

																  //��Ļ����Ӧ������ڿͻ�����ȱȸ߶ȵͣ����Կͻ�����Ȳ���Ϊ׼������ͻ����߶ȱȿ�ȵͣ����Կͻ����߶Ȳ���Ϊ׼��
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

		//[3][3]����Ӵ�
		Ellipse(hdc, (3 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 - 3,
			(3 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 + 3);

		//[3][11]����Ӵ�
		Ellipse(hdc, (3 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 - 3,
			(3 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 + 3);

		//[11][3]����Ӵ�
		Ellipse(hdc, (11 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 - 3,
			(11 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 3 * cxClient / 16 + 3);

		//[11][11]����Ӵ�
		Ellipse(hdc, (11 + 1) * cxClient / 16 - 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 - 3,
			(11 + 1) * cxClient / 16 + 3, (cyClient - 14 * cxClient / 16) / 2 + 11 * cxClient / 16 + 3);

		//[7][7]����Ӵ�
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

		//[3][3]����Ӵ�
		Ellipse(hdc, base_x + 3 * cyClient / 16 - 3, (3 + 1) * cyClient / 16 - 3,
			base_x + 3 * cyClient / 16 + 3, (3 + 1) * cyClient / 16 + 3);

		//[3][11]����Ӵ�
		Ellipse(hdc, base_x + 3 * cyClient / 16 - 3, (11 + 1) * cyClient / 16 - 3,
			base_x + 3 * cyClient / 16 + 3, (11 + 1) * cyClient / 16 + 3);

		//[11][3]����Ӵ�
		Ellipse(hdc, base_x + 11 * cyClient / 16 - 3, (3 + 1) * cyClient / 16 - 3,
			base_x + 11 * cyClient / 16 + 3, (3 + 1) * cyClient / 16 + 3);

		//[11][11]����Ӵ�
		Ellipse(hdc, base_x + 11 * cyClient / 16 - 3, (11 + 1) * cyClient / 16 - 3,
			base_x + 11 * cyClient / 16 + 3, (11 + 1) * cyClient / 16 + 3);

		//[7][7]����Ӵ�
		Ellipse(hdc, base_x + 7 * cyClient / 16 - 3, (7 + 1) * cyClient / 16 - 3,
			base_x + 7 * cyClient / 16 + 3, (7 + 1) * cyClient / 16 + 3);

		SelectObject(hdc, hOldPen);

	}
}