#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

void CreateExplorerExeProcess()
{
	TCHAR lpCommandLine[0x200] = _T("C:\\Windows\\explorer.exe");

	STARTUPINFO si = { 0, };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_NORMAL;

	PROCESS_INFORMATION pi = { 0, };

	if (CreateProcess(NULL, lpCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void CheckProcessNameAndID(DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		GetProcessImageFileName(hProcess, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
	}

	// Print the process name and identifier.

	if (_tcsstr(szProcessName, _T("\\Windows\\explorer.exe")))
	{
		if (hProcess)
			CloseHandle(hProcess);

		hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
		::TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
		hProcess = NULL;
	}
	//_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

	// Release the handle to the process.

	if (hProcess)
		CloseHandle(hProcess);
}

int main(void)
{
	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}


	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			CheckProcessNameAndID(aProcesses[i]);
		}
	}

	return 0;
}