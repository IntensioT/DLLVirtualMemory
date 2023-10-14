#include "Resource.h"
#include "TextToTableStaticLinking.h"
#include "tableMash.h"
#include "TableCell.h"

#include "MemoryCheck.h"

using namespace std;

wstring userInp = L"Test123";
WCHAR UserInput[] = L"12345678912345678912";
bool isAbout = false;

TableMash* tm;
TableCell table[cellRows][N];
HINSTANCE hInst;


LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:

	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			isAbout = !isAbout;
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	break;
	case WM_CREATE:
	{
		tm = new TableMash();
		if (!(tm->Init(hWnd)))
		{
			delete tm;
			return 0;
		}
		tm->CreateTextFactory(hWnd,userInp.c_str(), userInp.size());
		return 0;
	}
	case WM_SIZE:
		tm->UpdateMesh(hWnd, table);
		return 0;
	case WM_PAINT:
		//Render
		tm->BeginDraw();
		tm->ClearScreen(1, 1, 1);
		if (isAbout)
		{
			tm->DrawCircle(hWnd, 0, 0, 0, 1);
		}
		else 
		{
			tm->DrawAllRect(table, userInp.c_str(), userInp.size());
		}
		tm->EndDraw();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			// Process the UP ARROW key. 
			changeWord(GetCurrentProcess(), (char*)"Made in BSUIR");

			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t ClassName[] = L"TableWindow";
	WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
 
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = ClassName;
	wc.lpszMenuName = MAKEINTRESOURCEW(IDC_TEXTTOTABLESTATICLINKING);


	RegisterClass(&wc);

	hInst = hInstance;

	HWND hWnd = CreateWindowEx(0, ClassName, L"TextToTable", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);


	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		tm->UpdateMesh(hWnd, table);
		DispatchMessage(&msg);
	}

	return 0;
}