#include <Windows.h>
#include "TableCell.h"


void TableCell::ChangeRect(long left, long right, long top, long bottom)
{
	this->rect.left = left;
	this->rect.right = right;
	this->rect.top = top;
	this->rect.bottom = bottom;
}
