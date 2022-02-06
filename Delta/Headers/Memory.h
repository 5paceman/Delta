#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <vector>

#pragma comment (lib, "ntdll.lib")

EXTERN_C NTSTATUS NTAPI NtReadVirtualMemory(HANDLE, PVOID, PVOID, ULONG, PULONG);

class Memory
{
private:
    static HANDLE hProc;

public:
    static uint64_t get_module_base(DWORD ProcID, const wchar_t* moduleName);
    static DWORD get_proc_id(const wchar_t* processName);
    static uint64_t scan_for_class(const char* name);
    static BOOL read(uint64_t address, LPVOID string, size_t size);
    static void writef(uint64_t address, float value);
    static uint64_t read(uint64_t address, std::vector<unsigned int>offsets);

    template <class T>
    static void write(uint64_t address, void* object)
    {
        if (!hProc)
            hProc = OpenProcess(PROCESS_VM_READ, false, get_proc_id(L"RustClient.exe"));

        if (hProc)
        {
            size_t bytesRead;
            WriteProcessMemory(hProc, (LPVOID)address, &object, sizeof(object), &bytesRead);
        }
    }

    template <class T>
    static void write(uint64_t address, T value)
    {
        if (!hProc)
            hProc = OpenProcess(PROCESS_VM_READ, false, get_proc_id(L"RustClient.exe"));

        if (hProc)
        {
            size_t bytesRead;
            WriteProcessMemory(hProc, (LPVOID)address, &value, sizeof(value), &bytesRead);
        }
    }

    template <class T>
    static T read(uint64_t address)
    {
        if (!hProc)
            hProc = OpenProcess(PROCESS_VM_READ, false, get_proc_id(L"RustClient.exe"));
        T returnType;
        if (hProc)
        {
            size_t bytesRead;
            
            if (ReadProcessMemory(hProc, (LPCVOID)address, &returnType, sizeof(returnType), &bytesRead))
            {
                return returnType;
            }
            else {
                return returnType;
            }
        }
        return returnType;
    }
};
