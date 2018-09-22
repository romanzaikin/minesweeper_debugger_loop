#include "main.h"

/***********************************************************************************
*	GLOBAL VARAIBLES
************************************************************************************/

HANDLE g_hProcess;
HANDLE g_hThread;

PDEBUG_BREAKPOINT g_debugger_curr = 0;
PDEBUG_BREAKPOINT g_debugger_head = 0;
PDEBUG_BREAKPOINT g_debugger_tail = 0;

/***********************************************************************************
*	Function Name	:	FindBreakPointByAddress
*	Description		:	Check if there is a breakpoint on the address and return it's index
*
*	Parameters		:	address		[IN]	the address where breakpoint set
*	Return			:	DWORD
************************************************************************************/
DWORD FindBreakPointByAddress(
	_In_ DWORD address
)
{
	if (g_debugger_curr)
	{
		int i = g_debugger_curr->index;
		for (i = 0; i <= i; i++) 
{
			if (g_debugger_curr->dwAddress == address)
			{
				return g_debugger_curr;
			}
			g_debugger_curr = g_debugger_curr->next;
		}
	}

	return 0;
}

/***********************************************************************************
*	Function Name	:	GetBreakPoint
*	Description		:	Remove the breakpoint from the address by breakpoint number, 0 - success, 1 - failure
*
*	Parameters		:	dwAddress	[IN]	the address where breakpoint set
*	Return			:	DWORD
************************************************************************************/
DWORD GetBreakPoint(
	_In_ DWORD dwAddress
)
{
	DWORD dwWriteSize = 0;
	PDEBUG_BREAKPOINT temp = FindBreakPointByAddress(dwAddress);
	printf("%p\n", temp);
	if (NULL == temp)
	{
		goto exit;
	}

	// after we procced with INT 3 EIP moved one instruction down, we need to move it back to change the opcode.
	CONTEXT lcContext;
	lcContext.ContextFlags = CONTEXT_ALL;
	GetThreadContext(g_hThread, &lcContext);

	// Move back
	lcContext.Eip--;

	SetThreadContext(g_hThread, &lcContext);
	WriteProcessMemory(g_hProcess, (void*)dwAddress, &temp->cInstruction, 1, &dwWriteSize);
	FlushInstructionCache(g_hProcess, (void*)dwAddress, 1);

	// Cleanup
	if (g_debugger_curr->prev->next)
	{
		g_debugger_curr->prev->next = g_debugger_curr->next;
	}
	else
	{
		g_debugger_curr->prev->next = NULL;
	}

	if (g_debugger_curr->next->prev)
	{
		g_debugger_curr->next->prev = g_debugger_curr->prev;
	}
	else
	{
		g_debugger_curr->prev->next = NULL;
	}

	free(g_debugger_curr);
	g_debugger_curr = NULL;

	return 0;
exit:
	return 1;
}


/***********************************************************************************
*	Function Name	:	SetBreakPoint
*	Description		:	Fill in the debugger struct
*
*	Parameters		:	address		[IN]	the address where breakpoint set
*	Return			:	DWORD
************************************************************************************/
DWORD SetBreakPoint(
	_In_ DWORD dwAddress
)
{
	DWORD dwReadBytes = 0;
	int number = 0;
	BYTE cInstruction;

	// Check If there is BreakPoint on this address before set new one
	if (FindBreakPointByAddress(dwAddress))
	{
		goto exit;
	}

	// Read the first instruction    
	ReadProcessMemory(g_hProcess, (void*)dwAddress, &cInstruction, 1, &dwReadBytes);

	// Print the opcode
	printf("opcode %X:\n", cInstruction);

	if (g_debugger_curr)
	{
		number = g_debugger_curr->index;
		number++;
	}
	else
	{
		number = 1;
	}

	g_debugger_curr = (PDEBUG_BREAKPOINT)malloc(sizeof(DEBUG_BREAKPOINT));
	g_debugger_curr->cInstruction = cInstruction;
	g_debugger_curr->index = number;
	g_debugger_curr->next = 0;
	g_debugger_curr->dwAddress = dwAddress;
	g_debugger_curr->prev = g_debugger_head;
	g_debugger_head = g_debugger_curr;

	if (g_debugger_curr->prev)
	{
		g_debugger_curr->prev->next = g_debugger_curr;
	}
	else
	{
		g_debugger_tail = g_debugger_curr;
	}

	g_debugger_curr->next = g_debugger_tail;
	g_debugger_tail->prev = g_debugger_curr;

	// Replace it with Breakpoint
	cInstruction = 0xCC;

	WriteProcessMemory(g_hProcess, (void*)dwAddress, &cInstruction, 1, &dwReadBytes);
	FlushInstructionCache(g_hProcess, (void*)dwAddress, 1);

	return 0;
exit:
	return 1;
}