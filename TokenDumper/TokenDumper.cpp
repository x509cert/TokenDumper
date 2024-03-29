#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void DumpToken(const HANDLE hToken, DWORD pid) {
    DumpProcessName(pid);
    DumpUsernameAndSid(hToken);
    DumpGroups(hToken);
    DumpPrivs(hToken);
    DumpRestrictedThreadToken(pid);
}

////////////////////////////////////////////////////////////////////
int wmain(int argc, wchar_t* argv[]) {

    Banner();
	
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


    // if no args, dump current process token, output should be similar to whoami /all
    if (!pid) {
        pid = GetCurrentProcessId();
    }
    else {
        // user must have debug priv to get process token of a another process
        // technically, only for a process not running as the user, but we'll keep it simple for a moment!
        if (!SetDebugPrivilege(TRUE))
            return -1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        ShowApiError(L"OpenProcess");
        return -1;
    }

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
        ShowApiError(L"OpenProcessToken");
        return -1;
    }

    DumpToken(hToken, pid);

    //HANDLE hLinkedToken = HandleLinkedToken(hToken);
    //if (hLinkedToken)             // we only care about the main token
    //    DumpToken(hLinkedToken);

    if (hToken)
        CloseHandle(hToken);

    if (hProcess)
        CloseHandle(hProcess);

    return 0;
}
