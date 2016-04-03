// Graphic-lab2.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Graphic-lab2.h"

#include <cmath>
#include <algorithm>

#define MAX_LOADSTRING 100
#define PI 3.14159265

#define MIN_ZOOM 4
#define MAX_ZOOM 20
#define ZOOM_STEP 1
#define ANGLE_STEP 5

struct Point
{
	double X;
	double Y;

	Point(double x = 0, double y = 0)
	{
		X = x;
		Y = y;
	}

	Point GetRotatedPoint(double angle)
	{
		return Point(X * cos(angle) - Y * sin(angle), X * sin(angle) + Y * cos(angle));
	}
};

struct LineProperties
{
	Point		Start;
	Point		End;
	COLORREF	Color;

	LineProperties(Point start, Point end, COLORREF color)
	{
		Start = start;
		End = end;
		Color = color;
	}
};

struct CircleProperties
{
	Point Centre;
	int Radius;
	COLORREF Color;

	CircleProperties(Point centre, int radius, COLORREF color)
	{
		Centre = centre;
		Radius = radius;
		Color = color;
	}
};

struct StarProperties
{
	Point		RotatingCentre;

	Point		CircleCentre;
	int			Radius;

	double		Angle;
	double		Zoom;
	COLORREF	Color;

	StarProperties(Point rotatingCentre, Point circleCentre, int radius, double angle, double zoom, COLORREF color)
	{
		RotatingCentre = rotatingCentre;
		CircleCentre = circleCentre;
		Radius = radius;
		Angle = angle;
		Zoom = zoom;
		Color = color;
	}
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

StarProperties star(Point(0, 0), Point(12, 12), 4, 0, 8, RGB(100, 100, 150));

int zoomDirection = 1;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DrawLine(HDC target, LineProperties properties);
void DrawCircle(HDC target, CircleProperties properties);
void DrawStar(HDC target, StarProperties properties);
void DrawAll(HWND hWnd, HDC hdc, HINSTANCE hInst, StarProperties properties);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GRAPHICLAB2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICLAB2));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICLAB2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_GRAPHICLAB2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		DrawAll(hWnd, hdc, hInst, star);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONUP:
	{
		hdc = GetDC(hWnd);

		star.Angle += ANGLE_STEP;
		DrawAll(hWnd, hdc, hInst, star);

		ReleaseDC(hWnd, hdc);
		return(0);
	}
	break;
	case WM_RBUTTONUP:
	{
		hdc = GetDC(hWnd);

		if (star.Zoom + ZOOM_STEP * zoomDirection > MAX_ZOOM || star.Zoom + ZOOM_STEP * zoomDirection < MIN_ZOOM)
			zoomDirection *= -1;

		star.Zoom += ZOOM_STEP * zoomDirection;

		DrawAll(hWnd, hdc, hInst, star);

		ReleaseDC(hWnd, hdc);
		return(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void DrawLine(HDC target, LineProperties properties)
{
	double startX = properties.Start.X, endX = properties.End.X, startY = properties.Start.Y, endY = properties.End.Y;

	bool steep = (fabs(endY - startY) > fabs(endX - startX));

	if (steep)
	{
		std::swap(startX, startY);
		std::swap(endX, endY);
	}

	if (startX > endX)
	{
		std::swap(startX, endX);
		std::swap(startY, endY);
	}

	double dx = endX - startX;
	double dy = fabs(endY - startY);

	double error = dx / 2.0;
	int step = (startY < endY) ? 1 : -1;

	for (int x = (int)startX, y = (int)startY; x < (int)endX; x++)
	{
		SetPixel(target, steep ? y : x, steep ? x : y, properties.Color);

		error -= dy;

		if (error < 0)
		{
			y += step;
			error += dx;
		}
	}
}

void DrawCircle(HDC target, CircleProperties properties)
{
	int x = 0, y = properties.Radius;
	int delta = 1 - 2 * properties.Radius;

	while (y >= 0)
	{
		SetPixel(target, properties.Centre.X + x, properties.Centre.Y + y, properties.Color);
		SetPixel(target, properties.Centre.X + x, properties.Centre.Y - y, properties.Color);
		SetPixel(target, properties.Centre.X - x, properties.Centre.Y + y, properties.Color);
		SetPixel(target, properties.Centre.X - x, properties.Centre.Y - y, properties.Color);

		if (delta < 0 && (2 * (delta + y) - 1) <= 0)
		{
			++x;
			delta += 2 * x + 1;

			continue;
		}

		if (delta > 0 && (2 * (delta - x) - 1) > 0)
		{
			--y;
			delta += 1 - 2 * y;

			continue;
		}

		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void DrawStar(HDC target, StarProperties properties)
{
	const int numberOfPoints = 10;
	Point points[numberOfPoints] = {
		Point(12, 24),
		Point(15, 16),
		Point(24, 16),
		Point(17, 11),
		Point(20, 2),
		Point(12, 7),
		Point(4, 2),
		Point(7, 11),
		Point(0, 16),
		Point(9, 16) 
	};

	// Масштабирование.
	for (int i = 0; i < numberOfPoints; i++)
	{
		points[i].X *= properties.Zoom;
		points[i].Y *= properties.Zoom;
	}

	// Поворот.
	if (properties.Angle)
	{
		for (int i = 0; i < numberOfPoints; i++)
		{
			points[i] = points[i].GetRotatedPoint(properties.Angle * PI / 180);
		}
	}

	// Сдвиг относительно центра.
	for (int i = 0; i < numberOfPoints; i++)
	{
		points[i].X += properties.RotatingCentre.X;
		points[i].Y += properties.RotatingCentre.Y;
	}

	// Прорисовка.
	DrawLine(target, LineProperties(points[0], points[1], properties.Color));
	DrawLine(target, LineProperties(points[1], points[2], properties.Color));
	DrawLine(target, LineProperties(points[2], points[3], properties.Color));
	DrawLine(target, LineProperties(points[3], points[4], properties.Color));
	DrawLine(target, LineProperties(points[4], points[5], properties.Color));
	DrawLine(target, LineProperties(points[5], points[6], properties.Color));
	DrawLine(target, LineProperties(points[6], points[7], properties.Color));
	DrawLine(target, LineProperties(points[7], points[8], properties.Color));
	DrawLine(target, LineProperties(points[8], points[9], properties.Color));
	DrawLine(target, LineProperties(points[9], points[0], properties.Color));

	// Масштабирование окружности.
	properties.Radius *= properties.Zoom;
	properties.CircleCentre.X *= properties.Zoom;
	properties.CircleCentre.Y *= properties.Zoom;

	// Поворот окружности.
	properties.CircleCentre = properties.CircleCentre.GetRotatedPoint(properties.Angle * PI / 180);

	// Сдвиг окружности относительно центра.
	properties.CircleCentre.X += properties.RotatingCentre.X;
	properties.CircleCentre.Y += properties.RotatingCentre.Y;

	// Прорисовка окружности.
	DrawCircle(target, CircleProperties(properties.CircleCentre, properties.Radius, properties.Color));
}

void DrawAll(HWND hWnd, HDC hdc, HINSTANCE hInst, StarProperties properties)
{
	RECT clientRactangle;
	GetClientRect(hWnd, &clientRactangle);

	HDC deviceContextHandleBuffer = CreateCompatibleDC(hdc);
	HBITMAP bitmapHandleBuffer = CreateCompatibleBitmap(hdc, clientRactangle.right, clientRactangle.bottom);
	HBITMAP oldBitmapHandle = (HBITMAP)SelectObject(deviceContextHandleBuffer, bitmapHandleBuffer);

	// Получение центра экрана.
	properties.RotatingCentre.X = (clientRactangle.right - clientRactangle.left) / 2;
	properties.RotatingCentre.Y = (clientRactangle.bottom - clientRactangle.top) / 2;

	// Прорисовка в буфере.
	DrawStar(deviceContextHandleBuffer, properties);

	BitBlt(hdc, 0, 0, clientRactangle.right, clientRactangle.bottom, deviceContextHandleBuffer, 0, 0, SRCCOPY);

	SelectObject(deviceContextHandleBuffer, oldBitmapHandle);
	DeleteDC(deviceContextHandleBuffer);
	DeleteObject(bitmapHandleBuffer);
}
