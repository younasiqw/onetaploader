#pragma once
#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <WinBase.h>
#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "Advapi32.lib")
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <WinInet.h>
#include <iomanip>
#include <cstdio>  
#include <TlHelp32.h>
 inline std::string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
 inline HMODULE GetModuleHandleExtern(char *szModuleName, DWORD dwProcessId) // GetMoguleHandle recode for external processes
{
	if (!szModuleName || !dwProcessId) { return NULL; } // invalid input
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (hSnap == INVALID_HANDLE_VALUE) { return NULL; }
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnap, &me)) // we go now
	{
		while (Module32Next(hSnap, &me)) // through all modules in the target process
		{
			if (!strcmp(me.szModule, szModuleName)) // is this the model we are looking for?
			{
				CloseHandle(hSnap);
				return me.hModule; // this is our module, return it.
			}
		}
	}
	CloseHandle(hSnap);
	return NULL; // counldn't find module
}

typedef HMODULE(WINAPI *pLoadLibraryA)(LPCSTR);
typedef FARPROC(WINAPI *pGetProcAddress)(HMODULE, LPCSTR);

typedef BOOL(WINAPI *PDLL_MAIN)(HMODULE, DWORD, PVOID);

typedef struct _MANUAL_INJECT
{
	PVOID ImageBase;
	PIMAGE_NT_HEADERS NtHeaders;
	PIMAGE_BASE_RELOCATION BaseRelocation;
	PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;
	pLoadLibraryA fnLoadLibraryA;
	pGetProcAddress fnGetProcAddress;
}MANUAL_INJECT, *PMANUAL_INJECT;


class manualmap
{
public:
	int manualmapmain(const char* proccessname, const char* dllname, int argc);
private:
	int map(unsigned int pid, LPCSTR dllname, LPCSTR exename);
	unsigned int GetProcessPID(const char* process_name);
	bool LoadProcess(const char* procName);
private:
	HANDLE hProcess = NULL;
	DWORD dwProcessId = NULL;
	bool processOk = false;
	unsigned int PID;
};

extern manualmap* manual_map;