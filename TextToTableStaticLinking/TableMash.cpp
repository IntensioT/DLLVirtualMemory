#include "tableMash.h"
#include <dwrite.h>
#include <cmath>
#include <iostream>

# define PI           3.14159265358979323846 

using namespace std;

IDWriteFactory* pDWriteFactory;
IDWriteTextLayout* pTextLayout;
IDWriteTextLayout* pEmblemLayout;
IDWriteTextFormat* pTextFormat;


TableMash::TableMash()
{
	pFactory = NULL;
	pRenderTarget = NULL;
	pBlackBrush = NULL;
	pTextLayout = NULL;
	pDWriteFactory = NULL;
	pTextFormat = NULL;
	brush = NULL;

}

TableMash::~TableMash()
{
	if (pFactory) pFactory->Release(); //Com interfaces should be releasesd
	if (pRenderTarget) pRenderTarget->Release();
	if (pBlackBrush) pBlackBrush->Release();
	if (pTextLayout) pTextLayout->Release();
	if (pDWriteFactory) pDWriteFactory->Release();
	if (pTextFormat) pTextFormat->Release();
	if (brush) brush->Release();
}

bool TableMash::Init(HWND windowHandle)
{
	res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
	if (res != S_OK)
	{
		return false;
	}

	RECT rect;
	GetClientRect(windowHandle, &rect);
	

	res = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
		&pRenderTarget);

	if (res != S_OK)
	{
		return false;
	}

	res = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlackBrush);

	if (res != S_OK)
	{
		return false;
	}

	res = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);

	if (res != S_OK)
	{
		return false;
	}

	return true;
}

void TableMash::ClearScreen(float r, float g, float b)
{
	pRenderTarget->Clear(D2D1::ColorF(r, g, b));
}

void TableMash::UpdateMesh(HWND windowHandle, TableCell(&table)[cellRows][N])
{
	RECT rect;
	GetClientRect(windowHandle, &rect);
	pRenderTarget->Resize(D2D1::SizeU(rect.right, rect.bottom));
	long cellWidth = (rect.right - rect.left) / N;
	long cellHeight = (rect.bottom - rect.top) / cellRows;
	long curLeft = rect.left, curTop = rect.top;

	for (int i = 0; i < cellRows; i++)
	{
		for (int j = 0; j < N; j++)
		{
			table[i][j].ChangeRect(curLeft, curLeft+cellWidth, curTop, curTop+cellHeight);
			curLeft += cellWidth;

		}
		curLeft = rect.left;
		curTop += cellHeight;
	}
}


void TableMash::DrawAllRect(TableCell table[cellRows][N],const WCHAR text1[], int textSize)
{

	for (int i = 0; i < cellRows; i++)
	{
		for (int j = 0; j < N; j++)
		{
			D2D1_RECT_F rectangle = D2D1::RectF(table[i][j].GetLeft(), table[i][j].GetTop(), table[i][j].GetRight(), table[i][j].GetBottom());

			// Draw the outline of a rectangle.
			pRenderTarget->DrawRectangle(&rectangle, pBlackBrush);


			DWRITE_TEXT_RANGE textRange = { 0, textSize };

			pTextLayout->SetFontSize(sqrt((((table[i][j].GetRight() - table[i][j].GetLeft()))^2) + ((table[i][j].GetBottom() - table[i][j].GetTop()))^2), textRange);
			pTextLayout->SetMaxWidth(table[i][j].GetRight() - table[i][j].GetLeft());
			pTextLayout->SetMaxHeight(table[i][j].GetBottom() - table[i][j].GetTop());
			pRenderTarget->DrawTextLayout(D2D1::Point2F(table[i][j].GetLeft(), table[i][j].GetTop()),pTextLayout,pBlackBrush);
		}
	}
		
	
}

HRESULT TableMash::CreateTextFactory(HWND hWnd, const WCHAR text1[], int textSize)
{
	//static const WCHAR msc_fontName[] = L"Verdana";
	//static const WCHAR msc_fontName[] = L"Microsoft Sans Serif";
	//static const WCHAR msc_fontName[] = L"Comic Sans MS";
	static const WCHAR msc_fontName[] = L"Courier New";
	//static const WCHAR msc_fontName[] = L"Death Note";
	static const FLOAT msc_fontSize = 30;
	
	if (SUCCEEDED(res))
	{
		// Create a DirectWrite factory.
		res = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);
	}
	if (SUCCEEDED(res))
	{
		// Create a DirectWrite text format object.
		res = pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&pTextFormat
		);
	}


	if (SUCCEEDED(res))
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		float width = rect.right / cellRows;
		float height = rect.bottom / N;

		res = pDWriteFactory->CreateTextLayout(
			text1,      // The string to be laid out and formatted.
			textSize,  // The length of the string.
			pTextFormat,  // The text format to apply to the string (contains font information, etc).
			width,         // The width of the layout box.
			height,        // The height of the layout box.
			&pTextLayout  // The IDWriteTextLayout interface pointer.
		);
	}

		if (SUCCEEDED(res))
	{
		// Center the text horizontally and vertically.
		pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	
	return res;
}

void TableMash::DrawCircle(HWND hWnd, float r, float g, float b, float a)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	static char madein[14] = "Made in BSUIR";
	int inpSize = std::strlen(madein);

	float x = rect.right / 2;
	float y = rect.bottom / 2;
	float radiusX = (rect.right / 5);
	float width = radiusX / inpSize;
	float radiusY = (rect.bottom / 5);
	float height = radiusY / inpSize;
	
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f, D2D1::Point2F(x, y - radiusY)));

	brush->SetColor(D2D1::ColorF(0, 1, 1, 1));
	pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2(x, y), radiusX, radiusY), brush);


	float rot = 0;
	float angleStep = 360.0f / inpSize;
	float letterX = x + radiusX;
	float letterY = y;

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &madein[0], strlen(madein), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &madein[0], strlen(madein), &wstrTo[0], size_needed);

	for (int i = 0; i < inpSize; i++)
	{
		if (SUCCEEDED(res))
		{
			const WCHAR* tempInp = &wstrTo[i];
			res = pDWriteFactory->CreateTextLayout(
				tempInp,      // The string to be laid out and formatted.
				1,  // The length of the string.
				pTextFormat,  // The text format to apply to the string (contains font information, etc).
				width,         // The width of the layout box.
				height,        // The height of the layout box.
				&pEmblemLayout  // The IDWriteTextLayout interface pointer.
			);
		}
		
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(rot + 90, D2D1::Point2F(letterX, letterY)));
		pRenderTarget->DrawTextLayout(D2D1::Point2F(letterX, letterY), pEmblemLayout, pBlackBrush);

		
		rot += angleStep;
		letterX = x + radiusX * cos(rot * PI / 180); // converting degrees to radians
		letterY = y + radiusY * sin(rot * PI / 180);
	}
	

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f, D2D1::Point2F(x, y - radiusY)));

	brush->SetColor(D2D1::ColorF(r, g, b, a));
	pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2(x, y), radiusX, radiusY), brush, 3.0f);

}

