#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>


class memory {
public:
	memory(const char* processname);
	~memory();

	//un wrapper pentru a usura citirea din memorie
	template <typename T>
	T Read(uintptr_t addr) {
		T value{};

		ReadProcessMemory(processH, (LPCVOID)addr, &value, sizeof(T), nullptr);

		return value;
	}
	std::string Read(uintptr_t addr, int bytes = 16) {
		std::string value;

		ReadProcessMemory(processH, (LPCVOID)addr, &value, bytes, nullptr);

		return value;
	}

	bool ProcessOpen() { return bprocessfound; }

	bool SetProcIDandHandle(const char* name);

	uintptr_t GetModuleAddress(const char* modulename);

	void GetCS2HWND();

private:
	DWORD procid = 0;

	HANDLE processH = nullptr;

	bool bprocessfound = false;
};