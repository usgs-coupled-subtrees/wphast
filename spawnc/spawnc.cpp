// spawnc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	// Make sure that we've been passed the right number of arguments
	if (argc < 3)
	{
		_tprintf(_TEXT("Usage: %s (InheritableEventHandle) (CommandLineToSpawn)\n"), argv[0]);
		return -1;
	}

	// Construct the full command line
	TCHAR szCmdLine[MAX_PATH] = { 0 };
	for (int x = 2; x < argc; x++)
	{
		_tcscat_s(szCmdLine, MAX_PATH, argv[x]);
		_tcscat_s(szCmdLine, MAX_PATH, __TEXT(" "));
	}

	STARTUPINFO         si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	DWORD dwExitCode = 0;

	HANDLE h[2];
	h[0] = (HANDLE) _ttoi(argv[1]);  // The inherited event handle

	// Spawn the other processes as part of this Process Group
	BOOL f = CreateProcess(
		NULL,
		szCmdLine,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi
		);

	if (f)
	{
		CloseHandle(pi.hThread);
		h[1] = pi.hProcess;

		// Wait for the spawned-process to die or for the event
		// indicating that the processes should be forcibly killed.
		switch (WaitForMultipleObjects(2, h, FALSE, INFINITE))
		{
		 case WAIT_OBJECT_0 + 0:  // Force termination
			 GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0);
			 WaitForSingleObject(pi.hProcess, INFINITE);
			 break;

		 case WAIT_OBJECT_0 + 1:  // App terminated normally
			 // Make it's exit code our exit code
			 GetExitCodeProcess(pi.hProcess, &dwExitCode);
			 break;
		}
		CloseHandle(pi.hProcess);
	}
	CloseHandle(h[0]);   // Close the inherited event handle
	return dwExitCode;
}
