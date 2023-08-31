#include "TokenDumper.h"
#include <cstring>
#include <windows.h>

static BOOL SidToName(PSID pSid, LPWSTR& lpName, LPWSTR& lpDomain) {

    if (!pSid || !lpName || !lpDomain)
        return false;

    WCHAR szName[128] = { 0 };
    DWORD dwNameSize = _countof(szName);

    WCHAR szDomain[128] = { 0 };
    DWORD dwDomainSize = _countof(szDomain);

    SID_NAME_USE sidType;

    if (LookupAccountSidW(NULL, pSid, szName, &dwNameSize, szDomain, &dwDomainSize, &sidType)) {
        lpName = _wcsdup(szName);
        lpDomain = _wcsdup(szDomain);
        return TRUE;
    }

    return FALSE;
}

static PSID GetProcessSid(DWORD dwProcessId) {
    HANDLE hProcess = NULL;
    HANDLE hToken = NULL;
    PTOKEN_USER pTokenUser = NULL;
    DWORD dwLength = 0;
    PSID pSid = NULL;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
    if (hProcess == NULL) {
        return NULL;
    }

    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
        CloseHandle(hProcess);
        return NULL;
    }

    if (!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLength) &&
        GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return NULL;
    }

    pTokenUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
    if (pTokenUser == NULL) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return NULL;
    }

    if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwLength, &dwLength)) {
        HeapFree(GetProcessHeap(), 0, pTokenUser);
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return NULL;
    }

    DWORD sidSize = GetLengthSid(pTokenUser->User.Sid);
    pSid = (PSID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sidSize);
    if (pSid) {
        CopySid(sidSize, pSid, pTokenUser->User.Sid);
    }

    HeapFree(GetProcessHeap(), 0, pTokenUser);
    CloseHandle(hToken);
    CloseHandle(hProcess);

    return pSid;
}

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

    size_t unknownSids = 0;

    do {
        wchar_t wszProcessId[80];
        PSID pSid{};

        if ((pSid = GetProcessSid(pe.th32ProcessID)) != NULL) {
            LPWSTR lpName = nullptr;
            LPWSTR lpDomain = nullptr;

            if (SidToName(pSid, lpName, lpDomain)) {
                std::wstring strFullname(std::format(L"{}\\{}", lpDomain, lpName));
                wcscpy_s(wszProcessId, _countof(wszProcessId), strFullname.c_str());

                free(lpName);
                free(lpDomain);
            }
        } else {
            wcscpy_s(wszProcessId,_countof(wszProcessId),L"SID??");
            unknownSids++;
        }

        std::wstring strProcName(std::format(L"{}\t[{}] [{}]", pe.th32ProcessID, pe.szExeFile, wszProcessId));
		//std::transform(strProcName.begin(), strProcName.end(), strProcName.begin(), ::tolower);
        vProcessNames.push_back(strProcName);

    } while (Process32Next(hSnapshot, &pe));
	
	std::sort(vProcessNames.begin(), vProcessNames.end());
    for (auto& s : vProcessNames) {
		wprintf(L"%s\n", s.c_str());
    }

    if (unknownSids)
        wprintf(L"\nFound some unknown SIDs, they are probably Windows Protected Processes.\n");

    CloseHandle(hSnapshot);
}