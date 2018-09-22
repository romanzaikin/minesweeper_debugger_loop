#pragma once

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>

#define DEBUG_PROC ("c:\\Users\\roman\\Desktop\\winmine.exe")

typedef struct _DEBUG_BREAKPOINT {
	BYTE cInstruction;
	INT index;
	DWORD dwAddress;
	struct _DEBUG_BREAKPOINT *next;
	struct _DEBUG_BREAKPOINT *prev;

}DEBUG_BREAKPOINT, *PDEBUG_BREAKPOINT;

extern HANDLE g_hProcess;
extern HANDLE g_hThread;
extern DWORD  g_dwStartAddress;

extern PDEBUG_BREAKPOINT g_debugger_curr;
extern PDEBUG_BREAKPOINT g_debugger_head;
extern PDEBUG_BREAKPOINT g_debugger_tail;

DWORD OnCreateProcessDebugEvent(const LPDEBUG_EVENT);
DWORD OnExitThreadDebugEvent(const LPDEBUG_EVENT);
DWORD OnExitProcessDebugEvent(const LPDEBUG_EVENT);
DWORD OnLoadDllDebugEvent(const LPDEBUG_EVENT);

DWORD WINAPI KeyListener(LPVOID);

/***********************************************************************************
*	Function Name	:	SetBreakPoint
*	Description		:	Return if there is a breakpoint on the address	1 - there is, 0 - there is not.
*
*	Parameters		:	address		[IN]	the address where breakpoint set
*	Return			:	DWORD
************************************************************************************/
DWORD SetBreakPoint(_In_ DWORD);

/***********************************************************************************
*	Function Name	:	GetBreakPoint
*	Description		:	Remove the breakpoint from the address by breakpoint number, 0 - success, 1 - failure
*
*	Parameters		:	dwAddress	[IN]	the address where breakpoint set
*	Return			:	DWORD
************************************************************************************/
DWORD GetBreakPoint(_In_ DWORD);

/***********************************************************************************
*	Function Name	:	FindBreakPointByAddress
*	Description		:	Check if there is a breakpoint on the address and return it's index
*
*	Parameters		:	address		[IN]	the address where breakpoint set
*	Return			:	DWORD
************************************************************************************/
DWORD FindBreakPointByAddress(_In_ DWORD);