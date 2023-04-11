#include "TokenDumper.h"

void DumpProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        wprintf(L"CreateToolhelp32Snapshot failed with error code %d\n", GetLastError());
        return;
    }

    PROCESSENTRY32 pe = { sizeof(pe) };
    if (!Process32First(hSnapshot, &pe)) {
        wprintf(L"Process32First failed with error code %d\n", GetLastError());
        CloseHandle(hSnapshot);
        return;
    }
	
	std::vector<std::wstring> vProcessNames;

    SetTextColor();
    wprintf(L"PROCESSES:\n");
    do {
        std::wstring strProcName(std::format(L"{} [{}]", pe.szExeFile, pe.th32ProcessID));
		std::transform(strProcName.begin(), strProcName.end(), strProcName.begin(), ::tolower);
        vProcessNames.push_back(strProcName);
    } while (Process32Next(hSnapshot, &pe));
	
	std::sort(vProcessNames.begin(), vProcessNames.end());
    for (auto& s : vProcessNames) {
		wprintf(L"%s\n", s.c_str());
    }

    CloseHandle(hSnapshot);
}