#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void ShowApiError(__in_z const wchar_t* wszError) {
    wprintf(L"ERR: %ls failed. Error = %d\n", wszError, GetLastError());
}

////////////////////////////////////////////////////////////////////
void Banner() {
	wprintf(L"TokenDumper " VERSION L" - Michael Howard.\nCopyright (c) Microsoft Corp. All Rights Reserved.\n");
}

////////////////////////////////////////////////////////////////////
void Usage() {
    wprintf(L"Usage: TokenDumper [-p <pid>] [-l] [-h]\n");
    wprintf(L"\t-p <pid>\tProcess ID to dump token for\n");
    wprintf(L"\t-l\t\tList all processes\n");
    wprintf(L"\t-h\t\tThis help\n");
}

////////////////////////////////////////////////////////////////////
void SetTextColor(WORD dwColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, dwColor);
}

////////////////////////////////////////////////////////////////////
void GetProcessNameFromPid(DWORD pid, _Out_writes_(chProcessName) wchar_t *wszProcessName, DWORD chProcessName) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid ? pid : GetCurrentProcessId());
	if (hProcess == NULL) {
		ShowApiError(L"OpenProcess");
		return;
	}
	
	if (!GetModuleFileNameEx(hProcess, NULL, wszProcessName, chProcessName))
		ShowApiError(L"GetModuleFileNameEx");
	
	if (hProcess) CloseHandle(hProcess);
}

////////////////////////////////////////////////////////////////////
void DumpProcessName(DWORD pid) {
	wchar_t wszProcessName[MAX_PATH];
	DWORD chProcessName = _countof(wszProcessName);
	GetProcessNameFromPid(pid, wszProcessName, chProcessName);
	SetTextColor();
	wprintf(L"PROCESS:\n");
	wprintf(L"\t%s\n", wszProcessName);
}