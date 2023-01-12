#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
HRESULT IsLinkedToken(const HANDLE hToken, __inout HANDLE* pLinkedToken, bool* fIsLinked) {
    HRESULT hr = S_OK;
    DWORD cbData = 0;
    TOKEN_LINKED_TOKEN LinkedToken = { 0 };

    *fIsLinked = FALSE;

    if (GetTokenInformation(hToken,
        TokenLinkedToken,
        &LinkedToken,
        sizeof(LinkedToken),
        &cbData)) {
        *fIsLinked = true;
        *pLinkedToken = LinkedToken.LinkedToken;
    }
    else {
        DWORD err = GetLastError();
        if (ERROR_NO_SUCH_LOGON_SESSION != err)
            hr = HRESULT_FROM_WIN32(err);
    }

    return hr;
}

////////////////////////////////////////////////////////////////////
void DumpHighLevelStuff(const HANDLE hToken) {
    // Username and SID
    DWORD dwUser = 0;
    TOKEN_USER* ptu = NULL;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &dwUser);
    if (dwUser > 0) {
        std::unique_ptr<TOKEN_USER> ptu(new TOKEN_USER);
        //ptu = (TOKEN_USER*)malloc(dwUser);
        if (ptu) {
            if (GetTokenInformation(hToken, TokenUser, ptu.get(), dwUser, &dwUser)) {
                ShowSid(ptu->User.Sid, ptu->User.Attributes);
            }
        }
    }
    else {
        ShowApiError(L"GetTokenInformation");
    }

    // Restricted

}

////////////////////////////////////////////////////////////////////
void DumpGroups(const HANDLE hToken, const bool fVerbose) {
    void* p = malloc(2000);
    if (!p) {
        ShowApiError(L"malloc");
        return;
    }

    TOKEN_GROUPS* ptg = (TOKEN_GROUPS*)p;
    DWORD cbNeeded = 20000;
    if (!GetTokenInformation(hToken, TokenGroups, ptg, 2000, &cbNeeded)) {
        ShowApiError(L"GetTokenInformation(...,TokenGroups,...");
        return;
    }

    if (!fVerbose) {
        wprintf(L"Number of groups: %d\n", ptg->GroupCount);
    }
    else {
        if (ptg->GroupCount == 0)
            wprintf(L"None");
        else {
            for (size_t i = 0; i < ptg->GroupCount; i++) {
                ShowSid(ptg->Groups[i].Sid, ptg->Groups[i].Attributes);
            }
        }
    }

    if (p) free(p);
}



////////////////////////////////////////////////////////////////////
void DumpToken(const HANDLE hToken) {
    //DumpHighLevelStuff(hToken);
    //DumpGroups(hToken);
    DumpPrivs(hToken);
}

////////////////////////////////////////////////////////////////////
HANDLE HandleLinkedToken(const HANDLE hToken) {
    HANDLE hLinkedToken = NULL;
    bool fLinked = false;
    if (IsLinkedToken(hToken, &hLinkedToken, &fLinked) == S_OK && fLinked) {
        wprintf(L"Linked:\tYes\n");
    }
    else {
        wprintf(L"Linked:\nNo\n");
    }

    return hLinkedToken;
}

void ShowSid(__in PSID pSid, const DWORD attr) {
    wchar_t user[MAX_PATH], domain[MAX_PATH];
    DWORD cbUser, cbDomain;
    SID_NAME_USE snu;

    cbUser = _countof(user);
    cbDomain = _countof(domain);
    if (!LookupAccountSidW(NULL, pSid, user, &cbUser, domain, &cbDomain, &snu)) {
        //DWORD rc = gle;

        //if ( IsLogonSid( pSid ) )
        //	wprintf( L"(interactive logon session SID)" );
        //else
        //	wprintf( L"[LAS(): gle = %lu]", rc );
        return;
    }

    const wchar_t* t;
    switch (snu) {
    case SidTypeUser:           t = L"user";             break;
    case SidTypeGroup:          t = L"group";            break;
    case SidTypeDomain:         t = L"domain";           break;
    case SidTypeAlias:          t = L"alias";            break;
    case SidTypeWellKnownGroup: t = L"well-known group"; break;
    case SidTypeDeletedAccount: t = L"deleted";          break;
    case SidTypeInvalid:        t = L"invalid";          break;
    case SidTypeUnknown:        t = L"unknown";          break;
    case SidTypeComputer:       t = L"computer";         break;
    case SidTypeLabel:          t = L"integrity label";  break;
    default:                    t = L"unknown";          break;
    }

    const wchar_t* sep = L"\\";
    if (domain[0] == L'\0' || user[0] == L'\0')
        sep = L"";

    wchar_t wszAttr[128];
    wcscpy_s(wszAttr, _countof(wszAttr), L"");
    if (attr & SE_GROUP_MANDATORY)          wcscat_s(wszAttr, _countof(wszAttr), L"[Mandatory]");
    if (attr & SE_GROUP_ENABLED_BY_DEFAULT) wcscat_s(wszAttr, _countof(wszAttr), L"[Enabled by default]");
    if (attr & SE_GROUP_ENABLED)            wcscat_s(wszAttr, _countof(wszAttr), L"[Enabled]");
    if (attr & SE_GROUP_OWNER)              wcscat_s(wszAttr, _countof(wszAttr), L"[Owner]");
    if (attr & SE_GROUP_USE_FOR_DENY_ONLY)  wcscat_s(wszAttr, _countof(wszAttr), L"[Deny]");
    if (attr & SE_GROUP_INTEGRITY)          wcscat_s(wszAttr, _countof(wszAttr), L"[Integrity]");
    if (attr & SE_GROUP_INTEGRITY_ENABLED)  wcscat_s(wszAttr, _countof(wszAttr), L"[Integrity Enabled]");
    if (attr & SE_GROUP_LOGON_ID)           wcscat_s(wszAttr, _countof(wszAttr), L"[Logon ID]");
    if (attr & SE_GROUP_RESOURCE)           wcscat_s(wszAttr, _countof(wszAttr), L"[Resource]");

    wprintf(L"%ls%ls%ls (%ls) %ls%ls ",
        domain,
        sep,
        user,
        t,
        wcslen(wszAttr) ? L"" : L", ",
        wszAttr);

    wchar_t* pwszSid = NULL;
    ConvertSidToStringSidW(pSid, &pwszSid);
    wprintf(L"[%ls]\n", pwszSid);
    if (pwszSid) LocalFree(pwszSid);
}

////////////////////////////////////////////////////////////////////
void ShowApiError(__in_z const wchar_t* wszError) {
    wprintf(L"%ls failed. Error = %d", wszError, GetLastError());
}

////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[]) {
	
    int pid = 0;

    for (int i = 1; i < argc; i++) {
		if (wcscmp(argv[i], L"-p") == 0) {
			pid = _wtoi(argv[++i]);
		}
        else if (wcscmp(argv[i], L"-l") == 0) {
			DumpProcesses();
			return 0;
		}
		else if (wcscmp(argv[i], L"-h") == 0) {
			wprintf(L"Usage: %ls [-p <pid>] [-l] [-h]\n", argv[0]);
			wprintf(L"\t-p <pid>\tProcess ID to dump token for\n");
			wprintf(L"\t-l\t\tList all processes\n");
			wprintf(L"\t-h\t\tThis help\n");
			return 0;
        }
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
    if (hProcess == NULL) {
        ShowApiError(L"OpenProcess");
        return -1;
    }

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
        ShowApiError(L"OpenProcessToken");
    }

    DumpToken(hToken);
    HANDLE hLinkedToken = HandleLinkedToken(hToken);
    if (hLinkedToken)
        DumpToken(hLinkedToken);

    if (hToken)
        CloseHandle(hToken);

    if (hProcess)
        CloseHandle(hProcess);

    return 0;
}
