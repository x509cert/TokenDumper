#pragma once

#pragma strict_gs_check (  on )

#ifndef _WIN32_WINNT		              
#define _WIN32_WINNT _WIN32_WINNT_WIN8	
#endif						

#include <stdio.h>
#include <tchar.h>
#include "sal.h"
#include "windows.h"
#include "sddl.h"
#include <tlhelp32.h>

#include <memory>
#include <vector>

void    ShowApiError(__in_z const wchar_t* wszError);
HRESULT IsLinkedToken(const HANDLE hToken, __inout HANDLE* pLinkedToken, bool* fIsLinked);
void    DumpHighLevelStuff(const HANDLE hToken);
void    DumpGroups(const HANDLE hToken);
void    DumpPrivs(const HANDLE hToken);
void    DumpToken(const HANDLE hToken);
HANDLE  HandleLinkedToken(const HANDLE hToken);
void    ShowSid(__in PSID psid, const DWORD attr);
void    ShowPrivilege(LUID_AND_ATTRIBUTES& pPriv);
void	DumpProcesses();