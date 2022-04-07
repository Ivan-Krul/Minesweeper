#include <windows.h>
#include <gl/GL.h>
#include <iostream>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")

#define WINDOW_X 900
#define WINDOW_Y 900
#define MAP_X 10
#define MAP_Y 10
#define MINES 5

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

bool is_pause = false;

void ScreenToGL(HWND hwnd, int x, int y, float* ox, float* oy) {
	RECT rct;
	GetClientRect(hwnd, &rct);
	*ox = x / float(rct.right) * MAP_X;
	*oy = MAP_Y - y / float(rct.bottom) * MAP_Y;
}

struct Area {
	bool is_flag = false;
	bool is_bomb = false;
	bool is_open = false;
	int neighbour = 0;
};

Area Field[MAP_X][MAP_Y];
int checked_mines = 0;
bool is_work = true;

bool IsInMap(int x, int y) {
	return (x >= 0) && (x < MAP_X) && (y >= 0) && (y < MAP_Y);
}

void ClearField() {
	for (int i = 0;i < MAP_X;i++) {
		for (int j = 0;j < MAP_Y;j++) {
			Field[i][j].is_bomb = false;
			Field[i][j].is_flag = false;
			Field[i][j].is_open = false;
			Field[i][j].neighbour = 0;
		}
	}
}

void PlantMine() {
	for (int m = 0;m < MINES;m++){
		int x = rand() % MAP_X;
		int y = rand() % MAP_Y;

		if (Field[x][y].is_bomb) m--;
		else{
			Field[x][y].is_bomb = true;

			for (int i = -1;i < 2;i++)
				for (int j = -1;j < 2;j++)
					if (IsInMap(x + i, y + j))
						Field[x + i][y + j].neighbour++;
		}
	}
}

void OpenFields(int x,int y) {
	if (!IsInMap(x, y) || Field[x][y].is_open) return;
	if (Field[x][y].neighbour == 0) {
		Field[x][y].is_open = true;
		for (int i = -1;i < 2;i++)
			for (int j = -1;j < 2;j++)
				if (IsInMap(x + i, y + j))
					OpenFields(x + i, y + j);
	}
	else {
		Field[x][y].is_open = true;
	}
}

void IsTouchedMine(int x,int y) {
	if (Field[x][y].is_bomb && is_work) {
		for (int i = 0;i < MAP_X;i++) {
			for (int j = 0;j < MAP_Y;j++) {
				if (Field[i][j].is_bomb && !Field[i][j].is_flag) {
					Field[i][j].is_open = true;
				}
					
			}
		}
		is_work = false;
		Beep(300, 250);
		Beep(200, 250);
	}
}

bool IsChecked() {
	if (checked_mines == MINES) {
		is_work = false;
		Beep(400, 125);
		Beep(450, 125);
		Beep(500, 125);
		Beep(600, 375);
		return true;
	}
	else return false;
}

void line(double x1, double y1, double x2, double y2) {
	glVertex2d(x1, y1);	glVertex2d(x2, y2);
}

void ShowNumber(int num) {
	glLineWidth(4);
	glBegin(GL_LINES);
	glColor3b(79 / 2, 48 / 2, 184 / 2);
	if (num == 0 || num == 4 || num == 5 || num == 6 || num == 8 || num == 9) line(0.15, 0.85, 0.15, 0.5);
	if (num == 0 || num == 2 || num == 6 || num == 8) line(0.15, 0.5, 0.15, 0.15);
	if (num == 0 || num == 2 || num == 3 || num == 5 || num == 6 || num == 7 || num == 8 || num == 9) line(0.15, 0.85, 0.85, 0.85);
	if (num == 2 || num == 3 || num == 4 || num == 5 || num == 6 || num == 8 || num == 9) line(0.15, 0.5, 0.85, 0.5);
	if (num == 0 || num == 2 || num == 3 || num == 5 || num == 6 || num == 8 || num == 9) line(0.15, 0.15, 0.85, 0.15);
	if (num == 0 || num == 1 || num == 3 || num == 2 || num == 4 || num == 7 || num == 8 || num == 9) line(0.85, 0.5, 0.85, 0.85);
	if (num == 0 || num == 1 || num == 3 || num == 4 || num == 5 || num == 6 || num == 7 || num == 8 || num == 9) line(0.85, 0.5, 0.85, 0.15);
	glEnd();
}

void ShowField() {
	glBegin(GL_TRIANGLE_STRIP);
	glColor3b(205 / 2, 245 / 2, 132 / 2); glVertex2d(0.0, 0.0);
	glColor3b(215 / 2, 255 / 2, 142 / 2); glVertex2d(1.0, 0.0);
	glColor3b(195 / 2, 235 / 2, 122 / 2); glVertex2d(0.0, 1.0);
	glColor3b(205 / 2, 245 / 2, 132 / 2); glVertex2d(1.0, 1.0);
	glEnd();
}

void ShowClosedField() {
	glBegin(GL_TRIANGLE_STRIP);//71, 153, 35
	glColor3b(71 / 2, 153 / 2, 35 / 2); glVertex2d(0.0, 0.0);
	glColor3b(81 / 2, 163 / 2, 45 / 2); glVertex2d(1.0, 0.0);
	glColor3b(61 / 2, 143 / 2, 25 / 2); glVertex2d(0.0, 1.0);
	glColor3b(71 / 2, 153 / 2, 35 / 2); glVertex2d(1.0, 1.0);
	glEnd();
}

void ShowMine() {
	glBegin(GL_TRIANGLE_STRIP);
	glColor3b(17 / 2, 17 / 2, 17 / 2);
	glVertex2d(0.2, 0.2);
	glVertex2d(0.8, 0.2);
	glVertex2d(0.2, 0.8);
	glVertex2d(0.8, 0.8);
	glEnd();
}

void ShowFlag() {
	glBegin(GL_TRIANGLE_STRIP);
	glColor3b(0,0,0);
	glVertex2d(0.1, 0.1);
	glVertex2d(0.1, 0.2);
	glVertex2d(0.9, 0.1);
	glVertex2d(0.9, 0.2);

	glVertex2d(0.45, 0.2);
	glVertex2d(0.55, 0.2);
	glVertex2d(0.45, 0.9);
	glVertex2d(0.55, 0.9);

	glColor3b(242 / 2, 12 / 2, 12 / 2);
	glVertex2d(0.5, 0.9);
	glVertex2d(0.5, 0.45);
	glVertex2d(0.9, 0.675);
	glEnd();
}

void Paint() {
	glLoadIdentity();
	glTranslated(-1, -1, 0);
	glScaled(2.0/MAP_X, 2.0 / MAP_Y, 0);

	for (int i = 0;i < MAP_X;i++) {
		for (int j = 0;j < MAP_Y;j++) {
			glPushMatrix();
			glTranslated(i, j, 0);

			if (Field[i][j].is_open) {
				ShowField();
				if (Field[i][j].is_bomb)
					ShowMine();
				if (!Field[i][j].is_bomb && Field[i][j].neighbour > 0)
					ShowNumber(Field[i][j].neighbour);
			}
			else { 
				ShowClosedField();
				if (Field[i][j].is_flag)
					ShowFlag();
			}
			glPopMatrix();
		}
	}
}

void GameBegin() {
	srand(time(NULL));
	is_work = true;
	checked_mines = 0;
	Beep(800, 500);
	ClearField();
	PlantMine();
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	float theta = 0.0f;

	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		L"GLSample",
		L"MinesweeperGL",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_X,
		WINDOW_Y,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);

	GameBegin();

	/* program main loop */
	while (!bQuit)
	{

		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{

			/* OpenGL animation code goes here */
			if (!is_pause) {

				glClearColor(0, 0, 0, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				Paint();

				SwapBuffers(hDC);

				theta += 1.0f;
			}
			Sleep(1);
		}
	}

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		return 0;

	case WM_LBUTTONDOWN:
	{
		if (is_work) {
			POINTFLOAT pf;
			ScreenToGL(hwnd, LOWORD(lParam), HIWORD(lParam), &pf.x, &pf.y);
			Beep(500, 120);
			int xx = int(pf.x);
			int yy = int(pf.y);
			if (IsInMap(xx, yy) && !Field[xx][yy].is_flag)
				OpenFields(xx, yy);
			IsTouchedMine(xx, yy);
			IsChecked();
		}
		else GameBegin();
	}
		break;
	case WM_RBUTTONDOWN:
	{
		POINTFLOAT pf;
		ScreenToGL(hwnd, LOWORD(lParam), HIWORD(lParam), &pf.x, &pf.y);
		Beep(400, 120);
		int xx = int(pf.x);
		int yy = int(pf.y);
		if (IsInMap(xx, yy)) {
			Field[xx][yy].is_flag = !Field[xx][yy].is_flag;
			if (Field[xx][yy].is_bomb && Field[xx][yy].is_flag) checked_mines++;
		}
	}
	break;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case ' ':
			is_pause = !is_pause;
			break;
		case VK_F4:
			GameBegin();
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}
