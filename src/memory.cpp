#include "memory.h"

//BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam);

memory::memory(const char* processname)
{
	SetProcIDandHandle(processname);
}
memory::~memory()
{
	if (processH) {
		CloseHandle(processH);
	}
}

bool memory::SetProcIDandHandle(const char* name) {
	//entry este structura care contine date despre un proces
	PROCESSENTRY32 entry{};
	 

	entry.dwSize = sizeof(PROCESSENTRY32);

	//creaza o "poza" a tuturor proceselor ce ruleaza
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//trecem prin toate procesele pana gasim targetul cs2 si 
	while (Process32Next(snapshot, &entry)) {
		if (!strcmp(name, entry.szExeFile)) {// if we find the process with the correct name
			//we get it's processID and open a handle to it
			bprocessfound = true;
			procid = entry.th32ProcessID;
			OutputDebugStringA("PROCESS ID: " + procid);
			processH = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION,  FALSE, procid);
			if (!processH) {
				OutputDebugStringA("Could not open handle!\n");
			}
			return true;
			break;
		}
	}
	
	if (snapshot)

		CloseHandle(snapshot);
	return false;
}

uintptr_t memory::GetModuleAddress(const char* modulename) {

	MODULEENTRY32 entry{};

	entry.dwSize = sizeof(MODULEENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->procid);

	uintptr_t result = 0;

	while (Module32Next(snapshot, &entry)) {
		if (!strcmp(modulename, entry.szModule)) {
			result = (uintptr_t)entry.modBaseAddr;
			break;
		}
	}

	if (snapshot)
		CloseHandle(snapshot);

	return result;
}

//BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
//{
//	extern HWND cs2window;
//	DWORD lpdwProcessId;
//	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
//	if (lpdwProcessId == lParam)
//	{
//		cs2window = hwnd;
//		return FALSE;
//	}
//	return TRUE;
//}
//you need to have a global cs2window hwnd in the file you call this from
void memory::GetCS2HWND() {
	//EnumWindows(EnumWindowsProcMy, procid);
}