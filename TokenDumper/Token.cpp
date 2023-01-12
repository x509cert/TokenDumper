#include "TokenDumper.h"

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
	HANDLE hLinkedToken = NULL;
	bool fLinked = false;
	if (IsLinkedToken(hToken, &hLinkedToken, &fLinked) == S_OK && fLinked) {
		wprintf(L"Linked:\tYes\n");
	}
	else {
		wprintf(L"Linked:\tNo\n");
	}

	return hLinkedToken;
}

////////////////////////////////////////////////////////////////////
HRESULT IsLinkedToken(const HANDLE hToken, _Inout_ HANDLE* pLinkedToken, _Inout_ bool* fIsLinked) {
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
