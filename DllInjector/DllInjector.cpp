#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

typedef void (*ChangeWordProc)(HANDLE, char*);

DWORD get_PID(CHAR* PrName);
char* getCharWCHAR(WCHAR* str);
HANDLE inject_DLL(const char* file_name, int PID);

int main()
{
	DWORD PID;
	HANDLE hProcess;
	DWORD BaseAddress;
	char* PrName = (char*)"TextToTable.exe";


	if (!(PID = get_PID(PrName)))
	{
		printf("Process does not exist\n");
		return 0;
	}

	printf("Process founded!\n");
	printf("Process name: %s\n", PrName);
	printf("PID: %d\n\n", PID);

	//if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID)))
	//{
	//	printf("OpenProcess error\n");
	//	return 0;
	//}

	//printf("OpenProcess is ok\n");
	//printf("Now we have handle of process %s\n", PrName);
	//printf("Handle: %d\n\n", hProcess);

	inject_DLL("..\\x64\\Debug\\AttachableDll.dll", PID);

	return 0;
}

DWORD get_PID(CHAR* PrName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			char* tempChar = getCharWCHAR(entry.szExeFile);
			//printf("%s\n", entry.szExeFile);
			if (strcmp(tempChar, PrName) == 0)
			{
				return entry.th32ProcessID;
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

char* getCharWCHAR(WCHAR* str)
{
	// Convert the wchar_t string to a char* string. Record
	// the length of the original string and add 1 to it to
	// account for the terminating null character.
	size_t origsize = wcslen(str) + 1;
	size_t convertedChars = 0;

	// Allocate two bytes in the multibyte output string for every wide
	// character in the input string (including a wide character
	// null). Because a multibyte character can be one or two bytes,
	// you should allot two bytes for each character. Having extra
	// space for the new string isn't an error, but having
	// insufficient space is a potential security problem.
	const size_t newsize = origsize * 2;
	// The new string will contain a converted copy of the original
	// string plus the type of string appended to it.
	char* nstring = new char[newsize];

	// Put a copy of the converted string into nstring
	wcstombs_s(&convertedChars, nstring, newsize, str, _TRUNCATE);
	// append the type of string to the new string.

	return nstring;
}

HANDLE inject_DLL(const char* file_name, int PID)
{
	HANDLE h_process, h_rThread;
	char fullDLLPath[_MAX_PATH];
	LPVOID DLLPath_addr, LoadLib_addr;
	DWORD exit_code;

	/* Извлечение handle-идентификатора целевого процесса */
	h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	/* Получение полного пути к DLL-файлу */
	if (!(GetFullPathNameA(file_name, _MAX_PATH, fullDLLPath, NULL))) { return nullptr; }

	/* Выделение памяти в целевом процессе */
	if ((DLLPath_addr = VirtualAllocEx(h_process, NULL, _MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) == 0)
	{
		return nullptr;
	}

	/* Запись пути к DLL-файлу в недавно созданный блок памяти */
	BOOL res = WriteProcessMemory(h_process, DLLPath_addr, fullDLLPath, strlen(fullDLLPath), NULL);
	int err = GetLastError();

	/* Получение адреса LoadLibraryA (такой же для всех процессов), чтобы начать его запуск */
	LoadLib_addr = GetProcAddress(GetModuleHandleA((LPCSTR)"Kernel32.dll"), "LoadLibraryA");

	/* Запуск удалённого потока в LoadLibraryA, и проброс пути к DLL в качестве аргумента */
	h_rThread = CreateRemoteThread(h_process, NULL, 0,
		(LPTHREAD_START_ROUTINE)LoadLib_addr, DLLPath_addr, 0, NULL);

	//HMODULE hModule = LoadLibraryA(file_name);
	//ChangeWordProc pFunction = (ChangeWordProc)GetProcAddress(hModule, "changeWord");

	//pFunction(h_process, (char*)"Made in BSUIR");

	/* Ожидание его завершения */
	WaitForSingleObject(h_rThread, INFINITE);

	/* Получение кода завершения (то есть, значение handle, возвращённый вызовом LoadLibraryA */
	GetExitCodeThread(h_rThread, &exit_code);

	/* Освобождение носителя внедрённого потока. */
	CloseHandle(h_rThread);
	/* А также память, выделенная для пути к DLL */
	VirtualFreeEx(h_process, DLLPath_addr, 0, MEM_RELEASE);
	/* А также handle-иднетификатор целевого процесса */
	CloseHandle(h_process);

	return (HANDLE)exit_code;
}

