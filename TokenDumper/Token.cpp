#include "TokenDumper.h"

////////////////////////////////////////////////////////////////////
// A worker function that allocates memory for various token fields
// IMPORTANT
// This function allocates memory which must be freed by the caller
// if the function succeffully gets the token data. Use LocalFree()
void GetTokenInfo(const HANDLE hToken, TOKEN_INFORMATION_CLASS tic, DWORD _Inout_* pcbSize, _Inout_ void** ppv) {
	DWORD dwSize = 0;
	*ppv = NULL;
	GetTokenInformation(hToken, tic, NULL, 0, &dwSize);
	if (dwSize > 0) {
		*ppv = LocalAlloc(LPTR, dwSize);
		if (*ppv) {
			if (!GetTokenInformation(hToken, tic, *ppv, dwSize, &dwSize)) {
				LocalFree(*ppv);
				*ppv = nullptr;
			}
			else {
				*pcbSize = dwSize;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
void DumpGroups(const HANDLE hToken) {

	TOKEN_GROUPS* ptg{};
	DWORD cbNeeded{};
	GetTokenInfo(hToken, TokenGroups, &cbNeeded, reinterpret_cast<void**>(&ptg));

	SetTextColor();
	wprintf(L"\nGROUPS {n=%d}:\n", ptg->GroupCount);
	if (ptg->GroupCount == 0)
		wprintf(L"\tNone");
	else {
		for (size_t i = 0; i < ptg->GroupCount; i++) {
			ShowSid(ptg->Groups[i].Sid, ptg->Groups[i].Attributes);
		}
	}

	if (ptg) LocalFree(ptg);
}

////////////////////////////////////////////////////////////////////
void DumpMisc(const HANDLE hToken) {
	DumpLinkedToken(hToken);
	// add others
}

////////////////////////////////////////////////////////////////////
HANDLE DumpLinkedToken(const HANDLE hToken) {
	HANDLE hLinkedToken{};
	bool fLinked = false;
	if (IsLinkedToken(hToken, &hLinkedToken)) {
		wprintf(L"\tLinked:\tYes\n");
	}
	else {
		wprintf(L"\tLinked:\tNo\n");
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
