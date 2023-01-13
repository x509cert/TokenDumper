#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void DumpPrivs(const HANDLE hToken) {
    DWORD bufferSize = 0;
    PVOID ppv{};

	GetTokenInfo(hToken, TokenPrivileges, &bufferSize, &ppv);
    if (!ppv) return; //todo
	
    PTOKEN_PRIVILEGES privileges = reinterpret_cast<PTOKEN_PRIVILEGES>(ppv);

    SetTextColor();
    wprintf(L"\nPRIVILEGES {n=%d}:\n", privileges->PrivilegeCount);
    for (DWORD i = 0; i < privileges->PrivilegeCount; ++i) {
        DWORD nameSize = 0;
        
        LookupPrivilegeName(NULL, &privileges->Privileges[i].Luid, NULL, &nameSize);
        LPWSTR wszPrivilegeName = reinterpret_cast<LPWSTR>(LocalAlloc(LPTR, 1 + (nameSize * sizeof(wchar_t))));
        
        LookupPrivilegeName(NULL, &privileges->Privileges[i].Luid, wszPrivilegeName, &nameSize);
		if (IsDangerousPriv(wszPrivilegeName))
            SetTextColor(FOREGROUND_RED);
		
        wprintf(L"\t%s\n", wszPrivilegeName);
		
        SetTextColor();
        
        if (wszPrivilegeName) {
            LocalFree(wszPrivilegeName);
            wszPrivilegeName = nullptr;
        }
    }

    if (ppv) LocalFree(ppv);
}

bool IsDangerousPriv(LPWSTR szPrivName) {
	const wchar_t *szDanergousPrivs[] = { SE_BACKUP_NAME, SE_RESTORE_NAME, SE_TCB_NAME, SE_TAKE_OWNERSHIP_NAME,
                                           SE_DEBUG_NAME, SE_IMPERSONATE_NAME,  SE_CREATE_GLOBAL_NAME, SE_CREATE_TOKEN_NAME,
                                           SE_SECURITY_NAME, SE_RELABEL_NAME, SE_LOAD_DRIVER_NAME, SE_SYSTEMTIME_NAME, SE_RELABEL_NAME };
    for (int i = 0; i < sizeof(szDanergousPrivs) / sizeof(szDanergousPrivs[0]); i++) {
		if (wcscmp(szPrivName, szDanergousPrivs[i]) == 0) return true;
    }

    return false;
}