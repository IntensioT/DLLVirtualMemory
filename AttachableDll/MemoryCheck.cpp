#include "pch.h"
#include "MemoryCheck.h"

int compareSubstr(char* search, int searchSize, char* buffer, int bufferSize)
{
	int offset = -1;

	for (int i = 0; i < (bufferSize - searchSize + 1); i++)
	{
		int j = 0;
		while ((buffer[i + j] == search[j]) && (j < searchSize))
		{
			j++;
		}
		if (j == searchSize)
		{
			offset = i;
			break;
		}
	}

	return offset;
}

void writeWord(LPVOID address, char* value, int size)
{
	HWND hWnd;
	DWORD dwID;
	HANDLE hProcess;

	hWnd = FindWindow(NULL , L"TextToTable");

	GetWindowThreadProcessId(hWnd, &dwID);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, dwID); //

	LPCVOID tst = (LPCVOID)value;
	int tstInt = sizeof(value);
	BOOL res = WriteProcessMemory(hProcess, address, (LPCVOID)value, size, NULL);
	DWORD err = GetLastError();
}


void changeWord(HANDLE hProcess, char* searchWord)
{

	char* buffer{ nullptr };
	char* addr{ nullptr };
	char* match{ nullptr };
	char* aboba;
	aboba = (char*)"aboba12";
	SIZE_T bytesRead;

	MEMORY_BASIC_INFORMATION mbi{ 0 };

	while (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS /*&& (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_WRITECOPY || mbi.Protect == 128)*/)
		{
			delete[] buffer;
			buffer = new char[mbi.RegionSize];

			BOOL res = ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead);
			int err = GetLastError();
			long long mbiAddr = (long long)mbi.BaseAddress;

			int offset = compareSubstr(searchWord, strlen(searchWord), buffer, bytesRead);

			if (offset != -1)
			{
				writeWord((LPVOID)(mbiAddr + offset), aboba, sizeof(aboba));
				err = GetLastError();
			}
		}
		addr += mbi.RegionSize;
	}
}

void changeWordNoPar()
{
	MessageBox(
		NULL,
		L"Current Proc: Made in BSUIR change",
		L"=^..^=",
		MB_OK
	);

	HANDLE hProcess = GetCurrentProcess();
	char* searchWord = (char*)"Made in BSUIR";

	char* buffer{ nullptr };
	char* addr{ nullptr };
	char* match{ nullptr };
	char* aboba;
	aboba = (char*)"aboba12";
	SIZE_T bytesRead;

	MEMORY_BASIC_INFORMATION mbi{ 0 };

	while (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS /*&& (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_WRITECOPY || mbi.Protect == 128)*/)
		{
			delete[] buffer;
			buffer = new char[mbi.RegionSize];

			BOOL res = ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead);
			int err = GetLastError();
			long long mbiAddr = (long long)mbi.BaseAddress;

			int offset = compareSubstr(searchWord, strlen(searchWord), buffer, bytesRead);

			if (offset != -1)
			{
				writeWord((LPVOID)(mbiAddr + offset), aboba, sizeof(aboba));
				err = GetLastError();
			}
		}
		addr += mbi.RegionSize;
	}
}
