#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void DumpUsernameAndSid(const HANDLE hToken) {
    DWORD dwUser = 0;
    TOKEN_USER* ptu{};
    
    GetTokenInfo(hToken, TokenUser, &dwUser, reinterpret_cast<void**>(&ptu));
    if (dwUser > 0) {
        ShowSid(ptu->User.Sid, ptu->User.Attributes);
    }
    else {
        ShowApiError(L"GetTokenInformation");
    }

    if (&ptu)
        LocalFree(&ptu);

    // Restricted

}

////////////////////////////////////////////////////////////////////
void DumpGroups(const HANDLE hToken) {

    TOKEN_GROUPS* ptg{};
    DWORD cbNeeded = 2000;
	GetTokenInfo(hToken, TokenGroups, &cbNeeded, reinterpret_cast<void**>(&ptg));

    wprintf(L"Number of groups: %d\n", ptg->GroupCount);
    if (ptg->GroupCount == 0)
        wprintf(L"None");
    else {
        for (size_t i = 0; i < ptg->GroupCount; i++) {
            ShowSid(ptg->Groups[i].Sid, ptg->Groups[i].Attributes);
        }
    }

	if (ptg) free(ptg);
}

////////////////////////////////////////////////////////////////////
void DumpToken(const HANDLE hToken) {
    DumpUsernameAndSid(hToken);
    //DumpGroups(hToken);
    DumpPrivs(hToken);
}

////////////////////////////////////////////////////////////////////
void ShowApiError(__in_z const wchar_t* wszError) {
    wprintf(L"%ls failed. Error = %d", wszError, GetLastError());
}

void Usage() {
    wprintf(L"TokenDumper V0.1 - Michael Howard.\nCopyright (c) Microsoft Corp. All Rights Reserved.\n");
    wprintf(L"Usage: TokenDumper [-p <pid>] [-l] [-h]\n");
    wprintf(L"\t-p <pid>\tProcess ID to dump token for\n");
    wprintf(L"\t-l\t\tList all processes\n");
    wprintf(L"\t-h\t\tThis help\n");
}

////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[]) {
	
    int pid = 0;
	
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (wcscmp(argv[i], L"-p") == 0) {
                if (++i < argc) {
                    pid = _wtoi(argv[i]);
                    if (pid == 0) {
                        Usage();
                        return 1;
                    }
                } else {
                    Usage();
                    return 1;
                }
            } else if (wcscmp(argv[i], L"-l") == 0) {
                DumpProcesses();
                return 0;
            } else if (wcscmp(argv[i], L"-h") == 0) {
                Usage();
                return 0;
            }
        }
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid == 0 ? GetCurrentProcessId(): pid);
    if (hProcess == NULL) {
        ShowApiError(L"OpenProcess");
        return -1;
    }

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
        ShowApiError(L"OpenProcessToken");
        return -1;
    }

    DumpToken(hToken);
    HANDLE hLinkedToken = HandleLinkedToken(hToken);
    //if (hLinkedToken)             // we only care about the main token
    //    DumpToken(hLinkedToken);

    if (hToken)
        CloseHandle(hToken);

    if (hProcess)
        CloseHandle(hProcess);

    return 0;
}
