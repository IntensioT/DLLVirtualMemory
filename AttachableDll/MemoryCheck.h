#pragma once
#include <Windows.h>

#ifdef MEMLIBRARY_EXPORTS
#define MEMLIBRARY_API __declspec(dllexport)
#else
#define MEMLIBRARY_API __declspec(dllimport)
#endif

//TODO: Explore what extern "C" is and declspec
// https://learn.microsoft.com/ru-ru/cpp/cpp/dllexport-dllimport?view=msvc-170
extern "C" MEMLIBRARY_API void changeWord(HANDLE hProcess, char* searchWord);
extern "C" MEMLIBRARY_API void changeWordNoPar();
extern "C" MEMLIBRARY_API void writeWord(LPVOID address, char* value, int changebleSize);
extern "C" MEMLIBRARY_API int compareSubstr(char* search, int searchSize, char* buffer, int bufferSize);

