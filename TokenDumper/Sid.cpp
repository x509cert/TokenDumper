#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void DumpUsernameAndSid(const HANDLE hToken) {
    DWORD dwUser = 0;
    TOKEN_USER* ptu{};

    GetTokenInfo(hToken, TokenUser, &dwUser, reinterpret_cast<void**>(&ptu));
    if (dwUser > 0) {
        wprintf(L"OWNER:\n");
        ShowSid(ptu->User.Sid, ptu->User.Attributes);
    } else 
        ShowApiError(L"GetTokenInformation");

    if (&ptu) LocalFree(&ptu);
}

bool IsDangerousGroupOrLabel(_In_ PSID pSid, const DWORD attr) {
    const wchar_t* wszDangerousGroups[] = { 
        L"S-1-5-18",     // SYSTEM
        L"S-1-5-32-544", // BUILTIN Administrators
        L"S-1-5-32-547"  // Power Users(should never see this)
    };

	// ignore deny-only SIDs
    if (attr & SE_GROUP_USE_FOR_DENY_ONLY)
        return false;
    
	// check for integrity level above Medium
	if (attr & SE_GROUP_INTEGRITY) {
		DWORD dwIntegrityLevel = *GetSidSubAuthority(pSid, 0);
		if (dwIntegrityLevel > SECURITY_MANDATORY_MEDIUM_RID)
			return true;
	}

    bool fIsDangerous = false;
    for (int i = 0; i < _countof(wszDangerousGroups); i++) {
        PSID pSid2{};
        if (ConvertStringSidToSid(wszDangerousGroups[i], &pSid2)) {
            if (EqualSid(pSid, pSid2))
                fIsDangerous = true;

            LocalFree(pSid2);
        }
    }
		
    return fIsDangerous;
}

void ShowSid(_In_ PSID pSid, const DWORD attr) {
    wchar_t user[MAX_PATH], domain[MAX_PATH];
    SID_NAME_USE sidType;

    DWORD cbUser = _countof(user);
    DWORD cbDomain = _countof(domain);
    if (!LookupAccountSidW(NULL, pSid, user, &cbUser, domain, &cbDomain, &sidType)) {
        // ???		
    }
    
    const wchar_t* wszType{};
    switch (sidType) {
        case SidTypeUser:           wszType = L"user";             break;
        case SidTypeGroup:          wszType = L"group";            break;
        case SidTypeDomain:         wszType = L"domain";           break;
        case SidTypeAlias:          wszType = L"alias";            break;
        case SidTypeWellKnownGroup: wszType = L"well-known group"; break;
        case SidTypeDeletedAccount: wszType = L"deleted";          break;
        case SidTypeInvalid:        wszType = L"invalid";          break;
        case SidTypeUnknown:        wszType = L"unknown";          break;
        case SidTypeComputer:       wszType = L"computer";         break;
        case SidTypeLabel:          wszType = L"integrity label";  break;
        default:                    wszType = L"unknown";          break;
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

    if (IsDangerousGroupOrLabel(pSid, attr))
        SetTextColor(FOREGROUND_RED);
    else
        SetTextColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

    wprintf(L"\t%ls%ls%ls (%ls)%ls%ls",
        domain,
        sep,
        user,
        wszType,
		wcslen(wszAttr) ? L"" : L" ",
        wszAttr);

    wchar_t* pwszSid = NULL;
    ConvertSidToStringSidW(pSid, &pwszSid);
    wprintf(L"[%ls]\n", pwszSid);
    if (pwszSid) LocalFree(pwszSid);
}