#include <windows.h>
#include <gl/gl.h>
#include "math.h"
#include "camera.h"

//каждые 3 числа x y z
float vert[] = { 10,10,0 , 10,-10,0, -10,-10,0, -10,10,0 };
float xAlfa = 20;
float zAlfa = 0;
POINTFLOAT pos = { 0,0 };// мы на карте
//рисуем дом
float pyramid[] = { 0,0,2, 1,1,0, 1,-1,0, -1,-1,0, -1,1,0, 1,1,0 };

#pragma comment(lib, "opengl32.lib")

void ShowWorld()
{
	glEnableClientState(GL_VERTEX_ARRAY);// разрешаем рисовать из массива вершин
		glVertexPointer(3, GL_FLOAT, 0, &vert);//указываемопенгл на массив
		glColor3f(0, 0.5, 0);// ставим цвет
		glDrawArrays(GL_TRIANGLE_FAN,0,4);// рисуем
	

	glVertexPointer(3, GL_FLOAT, 0, &pyramid);
	glColor3f(1, 1, 0);// ставим цвет
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);// рисуем

	glDisableClientState(GL_VERTEX_ARRAY);
}


void Player_Move()
{
	Camera_MoveDirection(GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1 : 0),
						 GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1 : 0)
				   ,0.1, GetKeyState(VK_DOWN)<0? -1: (GetKeyState(VK_UP)<0? 1: 0)
		);// последний параметр скорость
	Camera_AutoMoveByMouse(8, 8, 0.2);
}

void WndResize(int x, int y)
{
	glViewport(0, 0, x, y);
	float k = x / (float)y;
	float sz = 0.1;
	glLoadIdentity();
	glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 1000);
}

//void MoveCamera()// камера в опенгл всегда находится в одной точке для иметации движения двигаем камеру по оси координат
//{
//	if (GetKeyState(VK_UP) < 0) xAlfa = ++xAlfa > 180 ? 180 : xAlfa;
//	if (GetKeyState(VK_DOWN) < 0) xAlfa = --xAlfa < 0 ? 0 : xAlfa;
//	if (GetKeyState(VK_LEFT) < 0) zAlfa++;
//	if (GetKeyState(VK_RIGHT) < 0) zAlfa--;
//
//	float ugol = -zAlfa / 180 * M_PI;
//	float speed = 0;
//	if (GetKeyState('W') < 0) speed = 0.1;
//	if (GetKeyState('S') < 0) speed = -0.1;
//	if (GetKeyState('A') < 0){ speed = 0.1; ugol -= M_PI * 0.5;}
//	if (GetKeyState('D') < 0){ speed = 0.1; ugol += M_PI * 0.5;}
//		if (speed != 0)
//		{
//			pos.x += sin(ugol) * speed;
//			pos.y += cos(ugol) * speed;
//		}
//
//	glRotatef(-xAlfa, 1, 0, 0);//сначала поворачиваем систему координат
//	glRotatef(-zAlfa, 0, 0, 1);
//
//	glTranslatef(-pos.x, -pos.y , -3);//потом двигаемся
//}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


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
	wcex.lpszClassName = "GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		"GLSample",
		"OpenGL Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,//РАЗМЕР ОКНА
		800,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);
	//ДАЛЬНОСТЬ ПЛОСКОСТИ ОТ СЕЧЕНИЯ ЭТО ДАЛЬНОСТЬ ВИДИМОСТИ НАШЕГО ПЕРСОНАЖА(80)
	//glFrustum(-1,1, -1,1, 2,80);//перспективная проекция
	RECT rct;
	GetClientRect(hwnd, &rct);
	WndResize(rct.right, rct.bottom);
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
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			//////////////////////////////////////////////////////////////////////главный цикл
			glPushMatrix();//сохраняем текцщую систему координат
			if (GetForegroundWindow() == hwnd)
				Player_Move();
			Camera_Apply();
				//MoveCamera();// изменяем в процедуре движения камеры
				ShowWorld();//рисуем
			glPopMatrix();//сохраняем измененную систему координат
			/// //////////////////////////////////////////////////////////
			
			SwapBuffers(hDC);

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

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
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

