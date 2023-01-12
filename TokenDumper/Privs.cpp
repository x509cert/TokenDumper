#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void DumpPrivs(const HANDLE hToken) {
    DWORD bufferSize = 0;
    GetTokenInformation(hToken, TokenPrivileges, nullptr, 0, &bufferSize);

    std::vector<BYTE> buffer(bufferSize);
    PTOKEN_PRIVILEGES privileges = reinterpret_cast<PTOKEN_PRIVILEGES>(buffer.data());

    if (!GetTokenInformation(hToken, TokenPrivileges, privileges, bufferSize, &bufferSize)) {
        _tprintf(_T("GetTokenInformation failed with error code %u\n"), GetLastError());
        return;
    }

    _tprintf(_T("Token privileges:\n"));
    for (DWORD i = 0; i < privileges->PrivilegeCount; ++i) {
        LPTSTR privilegeName;
        DWORD nameSize = 0;
        LookupPrivilegeName(NULL, &privileges->Privileges[i].Luid, NULL, &nameSize);
        privilegeName = new TCHAR[nameSize];
        LookupPrivilegeName(NULL, &privileges->Privileges[i].Luid, privilegeName, &nameSize);
        _tprintf(_T("  %s\n"), privilegeName);
        delete[] privilegeName;
    }
}