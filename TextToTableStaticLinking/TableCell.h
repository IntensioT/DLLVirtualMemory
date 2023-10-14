#pragma once

#include <Windows.h>

class TableCell
{
	RECT rect;
	wchar_t content;

public:
	int GetContent() { return content; };
	long GetLeft() { return rect.left; };
	long GetRight() { return rect.right; };
	long GetTop() { return rect.top; };
	long GetBottom() { return rect.bottom; };
	void SetContent(wchar_t cont) { this->content = cont; };
	void ChangeRect(long left,long right,long top,long bottom);
};