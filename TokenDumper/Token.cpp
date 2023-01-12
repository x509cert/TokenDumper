#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
void DumpGroups(const HANDLE hToken) {

	TOKEN_GROUPS* ptg{};
	DWORD cbNeeded{};
	GetTokenInfo(hToken, TokenGroups, &cbNeeded, reinterpret_cast<void**>(&ptg));

	wprintf(L"\nGROUPS {n=%d}:\n", ptg->GroupCount);
	if (ptg->GroupCount == 0)
		wprintf(L"\tNone");
	else {
		for (size_t i = 0; i < ptg->GroupCount; i++) {
			ShowSid(ptg->Groups[i].Sid, ptg->Groups[i].Attributes);
		}
	}

	if (ptg) free(ptg);
}

////////////////////////////////////////////////////////////////////
// A Worker function that allocates memory for various token fields
void GetTokenInfo(const HANDLE hToken, TOKEN_INFORMATION_CLASS tic, DWORD _Inout_ *pcbSize, _Inout_ void** ppv) {
	DWORD dwSize = 0;
	*ppv = NULL;
	GetTokenInformation(hToken, tic, NULL, 0, &dwSize);
	if (dwSize > 0) {
		*ppv = malloc(dwSize);
		if (*ppv) {
			if (!GetTokenInformation(hToken, tic, *ppv, dwSize, &dwSize)) {
				free(*ppv);
				*ppv = NULL;
			}
			else {
				*pcbSize = dwSize;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
HANDLE HandleLinkedToken(const HANDLE hToken) {
	HANDLE hLinkedToken{};
	bool fLinked = false;
	if (IsLinkedToken(hToken, &hLinkedToken)) {
		wprintf(L"Linked:\tYes\n");
	}
	else {
		wprintf(L"Linked:\tNo\n");
	}

	return hLinkedToken;
}

////////////////////////////////////////////////////////////////////
bool IsLinkedToken(const HANDLE hToken, _Inout_ HANDLE* pLinkedToken) {
	bool fIsLinked = false;
	DWORD cbData = 0;
	TOKEN_LINKED_TOKEN LinkedToken = { 0 };

	if (GetTokenInformation(hToken, TokenLinkedToken, &LinkedToken, sizeof(LinkedToken),&cbData)) {
		fIsLinked = true;
		*pLinkedToken = LinkedToken.LinkedToken;
	}
	
	return fIsLinked;
}
