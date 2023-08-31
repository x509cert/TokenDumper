#pragma once

#define VERSION	L"(v2.0)"

#pragma strict_gs_check (  on )

#ifndef _WIN32_WINNT		              
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif						

#include <stdio.h>
#include <tchar.h>
#include "sal.h"
#include "windows.h"
#include "sddl.h"
#include <tlhelp32.h>
#include <psapi.h>

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <format>
#include <iostream>

void    ShowApiError(_In_z_ const wchar_t* wszError);
bool	IsLinkedToken(const HANDLE hToken, _Inout_ HANDLE* pLinkedToken);
void    DumpUsernameAndSid(const HANDLE hToken);
void    DumpGroups(const HANDLE hToken);
void    DumpPrivs(const HANDLE hToken);
void	DumpMisc(const HANDLE hToken);
HANDLE	DumpLinkedToken(const HANDLE hToken);
void    ShowSid(_In_ PSID psid, const DWORD attr);
void	DumpProcesses();
void	GetTokenInfo(const HANDLE hToken, TOKEN_INFORMATION_CLASS tic, DWORD _Inout_* pcbSize, _Inout_ void** ppv);
void	SetTextColor(WORD dwColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
bool	IsDangerousPriv(LPWSTR szPrivName);
void	Usage();
void	Banner();
void	DumpProcessName(DWORD pid);
void	DumpRestrictedThreadToken(DWORD pid);
