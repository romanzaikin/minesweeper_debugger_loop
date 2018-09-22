#include "main.h"


DWORD EnterDebugLoop(const LPDEBUG_EVENT DebugEv)
{
	DWORD dwContinueStatus = DBG_CONTINUE;			// exception continuation 
	BOOL m_bBreakpointOnceHit = FALSE;

	CONTEXT lcContext = { 0 };					   // thread context
	lcContext.ContextFlags = CONTEXT_ALL;

	for (;;)
	{
		WaitForDebugEvent(DebugEv, INFINITE);

		switch (DebugEv->dwDebugEventCode)
		{
		case EXCEPTION_DEBUG_EVENT:
			switch (DebugEv->u.Exception.ExceptionRecord.ExceptionCode)
			{
			case EXCEPTION_BREAKPOINT:
				// pass NTDLL Break Point
				if (m_bBreakpointOnceHit)
				{
					GetBreakPoint((DWORD)0x01003488);
					GetThreadContext(g_hThread, &lcContext);

					printf("ESI = %08X\n", lcContext.Esi);
					lcContext.Esi = 0x1;							 // Win :)
					SetThreadContext(g_hThread, &lcContext);
				}
				else
				{
					m_bBreakpointOnceHit = TRUE;
				}

				break;

			default:
				// Handle other exceptions. 
				break;
			}

			break;

		case CREATE_PROCESS_DEBUG_EVENT:
			dwContinueStatus = OnCreateProcessDebugEvent(DebugEv);
			break;

		case EXIT_THREAD_DEBUG_EVENT:
			// Display the thread's exit code. 

			dwContinueStatus = OnExitThreadDebugEvent(DebugEv);
			break;

		case EXIT_PROCESS_DEBUG_EVENT:
			dwContinueStatus = OnExitProcessDebugEvent(DebugEv);
			goto exit_loop;
			break;

		case LOAD_DLL_DEBUG_EVENT:
			dwContinueStatus = OnLoadDllDebugEvent(DebugEv);
			break;
		}

		ContinueDebugEvent(DebugEv->dwProcessId,
			DebugEv->dwThreadId,
			dwContinueStatus);

	}
exit_loop:
	return 0;
}

int
main
(void)
{
	DWORD dwReturn = 0;
	DEBUG_EVENT de = { 0 };
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	si.cb = sizeof(si);

	dwReturn = CreateProcessA(DEBUG_PROC, NULL, NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi);
	if (0 == dwReturn)
	{
		printf("Error %d\n", GetLastError());
		goto lblcleanup;
	}
	else
	{
		EnterDebugLoop(&de);
	}

	dwReturn = 1;
lblcleanup:
	getchar();
	return dwReturn;
}

DWORD
OnCreateProcessDebugEvent
(
	const LPDEBUG_EVENT DebugEv
)
{
	printf("process id: %d\n", DebugEv->dwProcessId);
	printf("EntryPoint: %p\n", DebugEv->u.CreateProcessInfo.lpStartAddress);

	g_hProcess = DebugEv->u.CreateProcessInfo.hProcess;
	g_hThread = DebugEv->u.CreateProcessInfo.hThread;

	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	hThread = CreateThread(NULL, NULL, &KeyListener, NULL, NULL, &dwThreadId);

	if (NULL == hThread)
	{
		printf("CreateThread Error %d\n", GetLastError());
	}

	return DBG_CONTINUE;
}


DWORD OnExitThreadDebugEvent(const LPDEBUG_EVENT DebugEv)
{
	UNREFERENCED_PARAMETER(DebugEv);
	puts("OnExitThreadDebugEvent");

	return DBG_CONTINUE;
}

DWORD OnExitProcessDebugEvent(const LPDEBUG_EVENT DebugEv)
{
	UNREFERENCED_PARAMETER(DebugEv);
	puts("OnExitProcessDebugEvent");

	return DBG_CONTINUE;
}

DWORD OnLoadDllDebugEvent(const LPDEBUG_EVENT DebugEv)
{
	UNREFERENCED_PARAMETER(DebugEv);
	puts("OnLoadDllDebugEvent");

	return DBG_CONTINUE;
}

DWORD WINAPI KeyListener(LPVOID lpvoid)
{
	UNREFERENCED_PARAMETER(lpvoid);

	while (TRUE)
	{
		if (GetAsyncKeyState(VK_F12) & 1) SetBreakPoint((DWORD)0x01003488);
		Sleep(100);
	}

	return 0;
}