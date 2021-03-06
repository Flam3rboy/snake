// GT_HelloWorldWin32.cpp  
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c  

#include "stdafx.h"
#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>
#include <math.h>
#include <time.h>
#include "Snake.h"
#include <string.h>
#include <winsock2.h>
#include <Windowsx.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 10000  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

using namespace std;
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("SnakeV3 Server");
HINSTANCE hInst;
HANDLE hTimerQueue = NULL;
HANDLE hTimer = NULL;
SOCKET s;
struct sockaddr_in server, si_other;
int slen, recv_len;
char buf[BUFLEN];
WSADATA wsa;


int Theme = 1;
//SNAKE
int BoardX = GetBoardSizeX();
int BoardY = GetBoardSizeY();
//SNAKE

// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CALLBACK socketServer(HWND lpParam) {
	char tmpbuf[BUFLEN];
	memset(tmpbuf, '\0', BUFLEN);

	while (1)
	{
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{

		}

		tmpbuf[1] = tmpbuf[0];
		tmpbuf[0] = buf[0];

		if (strcmp((char *)&tmpbuf[1], (char *)&buf[0]) != 0) {
			switch (buf[0]) {
			case 'A':
				WndProc(lpParam, WM_KEYDOWN, VK_LEFT, 0);
				break;
			case 'D':
				WndProc(lpParam, WM_KEYDOWN, VK_RIGHT, 0);
				break;
			case 'W':
				WndProc(lpParam, WM_KEYDOWN, VK_UP, 0);
				break;
			case 'S':
				WndProc(lpParam, WM_KEYDOWN, VK_DOWN, 0);
				break;
			case 'B':

				for (int y = 0; y < 100; y++) {
					for (int x = 0; x < 100; x++) {
						SetBoard(y, x, buf[y + x + 1]);
					}
				}

				break;
			default:
				WndProc(lpParam, WM_KEYDOWN, buf[0], 0);
				break;
			}
		}
		//print details of the client/peer and the data received

		//now reply the client with the same data
		/*if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		{
		exit(EXIT_FAILURE);
		}*/
	}

	closesocket(s);
	WSACleanup();
}

VOID CALLBACK TimerRoutine(HWND lpParam, BOOLEAN TimerOrWaitFired)
{

	BoardEditing();
	InvalidateRect(lpParam, NULL, TRUE);

	UpdateWindow(lpParam); //Only for force to Update immediately
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{

	srand(time(NULL));

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);
	}
	reset();
	hInst = hInstance; // Store instance handle in our global variable  

					   // The parameters to CreateWindow explained:  
					   // szWindowClass: the name of the application  
					   // szTitle: the text that appears in the title bar  
					   // WS_OVERLAPPEDWINDOW: the type of window to create  
					   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
					   // 500, 100: initial size (width, length)  
					   // NULL: the parent of this window  
					   // NULL: this application does not have a menu bar  
					   // hInstance: the first parameter from WinMain  
					   // NULL: not used in this application  

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		BoardX * 30.5, BoardY * 31.5,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	/*HWND hwndButton = CreateWindow(
	L"BUTTON",  // Predefined class; Unicode assumed
	L"OK",      // Button text
	WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
	0,         // x position
	0,         // y position
	50,        // Button width
	50,        // Button height
	hWnd,     // Parent window
	NULL,       // No menu.
	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
	NULL);      // Pointer not needed.*/



	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Create the timer queue.
	hTimerQueue = CreateTimerQueue();
	if (NULL == hTimerQueue)
	{
	}

	// Set a timer to call the timer routine in  seconds.
	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)TimerRoutine, hWnd, 0, ((BoardX*BoardY) / (BoardX + BoardY / 2)) * 6.6, 0))
	{
	}

	slen = sizeof(si_other);
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		exit(EXIT_FAILURE);
	}

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
	}

	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)socketServer, hWnd, 0, 0, 0))
	{
	}

	reset();

	// Main message loop:  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int WindowHeight;
	int WindowWidth;

	POINT ptPrevious = {0 , 0};
	POINT ptNext = {0, 0};

	RECT rect;
	if (GetWindowRect(hWnd, &rect))
	{
		WindowWidth = rect.right - rect.left;
		WindowHeight = rect.bottom - rect.top;
	}

	BoardX = ((WindowWidth - (WindowWidth % 30)) / 30);
	BoardY = ((WindowHeight - (WindowHeight % 31)) / 31);

	PAINTSTRUCT ps;
	HDC hdc = NULL;
	TCHAR ScoreText[] = _T("Ich bin Samuel");
	TCHAR gameover[] = _T("GameOver");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (Theme == 1) {
			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkColor(hdc, RGB(0, 0, 0));
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(0, 0, 0)));
		}
		else if (Theme == 0) {
			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkColor(hdc, RGB(255, 255, 255));
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(255, 255, 255)));
		}

		_itow_s(GetScore() + 1, (TCHAR *)ScoreText, sizeof(ScoreText) / sizeof(TCHAR), 10);
		TextOut(hdc, 0, 0, ScoreText, _tcslen(ScoreText));

		if (GetGameOver() == 1) {
			SetInput('x');
			TextOut(hdc, 100, 100, _T("GameOver"), _tcslen(_T("GameOver")));
		}
		else {
			HBRUSH brushgruen = CreateSolidBrush(RGB(0, 255, 0));
			HBRUSH brushrot = CreateSolidBrush(RGB(255, 144, 0));
			if (GetFlag() == 1) {
				brushrot = CreateSolidBrush(RGB(0, 100, 255));
			}

			for (int y = 0; y < GetBoardSizeY(); y++) {
				for (int x = 0; x < GetBoardSizeX(); x++) {
					RECT rect = { x * 30,y * 30,x * 30 + 29,y * 30 + 29 };
					int Status = GetStatusBoard(y, x);
					if (Status == 1) {
						FillRect(hdc, &rect, brushrot);
					}
					else if (Status == 2) {
						FillRect(hdc, &rect, brushgruen);
					}
				}
			}

			DeleteObject(brushgruen);
			DeleteObject(brushrot);

		}

		EndPaint(hWnd, &ps);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		//ResizeBoard(BoardX, BoardY);
		SetBoardSize(BoardX, BoardY);
		break;
	case WM_LBUTTONDOWN:

		ptPrevious.x = LOWORD(lParam);
		ptPrevious.y = HIWORD(lParam);
		break;
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON) {
			ptNext.x = LOWORD(lParam);
			ptNext.y = HIWORD(lParam);

			if (fabs(ptPrevious.x - ptNext.x) > fabs(ptPrevious.y - ptNext.y)) {

				if (ptPrevious.x > ptNext.x) {
					WndProc(hWnd, WM_KEYDOWN, VK_LEFT, 0);
				}
				else if (ptPrevious.x < ptNext.x) {
					WndProc(hWnd, WM_KEYDOWN, VK_RIGHT, 0);
				}
			}
			else {
				if (ptPrevious.y > ptNext.y) {
					WndProc(hWnd, WM_KEYDOWN, VK_UP, 0);
				}
				else if (ptPrevious.y < ptNext.y) {
					WndProc(hWnd, WM_KEYDOWN, VK_DOWN, 0);
				}
			}
		}
		break;
	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_LEFT:
			SetInput('a');
			// Process the LEFT ARROW key. 
			if (GetStatusSnakeTail() == 1) {
				SetInput('d');
			}

			break;
		case VK_RIGHT:
			SetInput('d');
			// Process the RIGHT ARROW key.
			if (GetStatusSnakeTail() == 1) {
				SetInput('a');
			}

			break;
		case VK_UP:
			SetInput('w');
			// Process the UP ARROW key. 
			if (GetStatusSnakeTail() == 1) {
				SetInput('s');
			}

			break;

		case VK_DOWN:
			SetInput('s');
			// Process the DOWN ARROW key. 
			if (GetStatusSnakeTail() == 1) {
				SetInput('w');
			}

			break;
		case 'X':
			SetInput('x');
			break;
		case VK_ESCAPE:
			SetInput('x');
			break;
		case 'O':
			Theme = !Theme;
			break;
		case 'R':
			SetInput('q');
			break;
		default:
			break;
		}
		break;
	case WM_ACTIVATE:
		///SetInput('x');
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
