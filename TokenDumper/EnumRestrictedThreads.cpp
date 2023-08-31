#include "TokenDumper.h"

void DumpRestrictedThreadToken(DWORD pid) {
    
    THREADENTRY32 te32 = {};
    bool hasRestrictedThreads = false;

    // Take a snapshot of all running threads
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        ShowApiError(L"CreateToolhelp32Snapshot");
        return;
    }

    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread
    if (!Thread32First(hThreadSnap, &te32)) {
        ShowApiError(L"Thread32First");
        CloseHandle(hThreadSnap);
        return;
    }

    // Loop through all threads in the snapshot
    do {
        // Check if the thread belongs to the target process
        if (te32.th32OwnerProcessID == pid) {
            HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
            if (!hThread) {
                ShowApiError(L"OpenThread");
            } else {

                // get the thread token, it might not have one, in which case it's just the process token
                HANDLE hToken = NULL;
                if (!OpenThreadToken(hThread, TOKEN_QUERY, FALSE, &hToken)) {
                    if (ERROR_NO_TOKEN != GetLastError()) {
                        ShowApiError(L"OpenThreadToken");
                    }
                }

                if (IsTokenRestricted(hToken))
                    hasRestrictedThreads = TRUE;
            }
        }
    } while (Thread32Next(hThreadSnap, &te32));

    CloseHandle(hThreadSnap);

    wprintf(L"\nRESTRICTED THREADS:\n\t%s\n", hasRestrictedThreads ? L"Yes" : L"No");
}
