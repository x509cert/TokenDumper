#include "TokenDumper.h"

void DumpProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        _tprintf(_T("CreateToolhelp32Snapshot failed with error code %d\n"), GetLastError());
        return;
    }

    PROCESSENTRY32 pe = { sizeof(pe) };
    if (!Process32First(hSnapshot, &pe)) {
        _tprintf(_T("Process32First failed with error code %d\n"), GetLastError());
        CloseHandle(hSnapshot);
        return;
    }

    _tprintf(_T("Processes:\n"));
    do {
        _tprintf(_T("  %s (%d)\n"), pe.szExeFile, pe.th32ProcessID);
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);
}