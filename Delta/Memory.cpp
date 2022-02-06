#include "Memory.h"


HANDLE Memory::hProc = 0;

uint64_t Memory::get_module_base(DWORD ProcID, const wchar_t* moduleName)
{
    uint64_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcID);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, moduleName))
                {
                    modBaseAddr = (uint64_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

void Memory::writef(uint64_t address, float value)
{
    if (!hProc)
        hProc = OpenProcess(PROCESS_ALL_ACCESS, false, get_proc_id(L"RustClient.exe"));

    size_t bytesRead;
    WriteProcessMemory(hProc, (BYTE*)address, &value, sizeof(value), &bytesRead);
}

uint64_t Memory::scan_for_class(const char* name)
{
    auto base = Memory::get_module_base(get_proc_id(L"RustClient.exe"), L"GameAssembly.dll");
    auto dos_header = Memory::read<IMAGE_DOS_HEADER>(base);
    auto data_header = Memory::read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
    auto next_section = Memory::read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
    auto data_size = next_section.VirtualAddress - data_header.VirtualAddress;

    if (strcmp((char*)data_header.Name, ".data")) {
        printf("[!] Section order changed \n");
    }

    for (uint64_t offset = data_size; offset > 0; offset -= 8) {
        char klass_name[0x100] = { 0 };
        auto klass = Memory::read<uint64_t>(base + data_header.VirtualAddress + offset);
        if (klass == 0) { continue; }
        auto name_pointer = Memory::read<uint64_t>(klass + 0x10);
        if (name_pointer == 0) { continue; }
        Memory::read(name_pointer, klass_name, sizeof(klass_name));
        if (!strcmp(klass_name, name)) {
            printf("[*] 0x%I64x -> %s\n", data_header.VirtualAddress + offset, name);
            return klass;
        }
    }

    printf("[!] Unable to find %s in scan\n", name);
    exit(0);
}

static uint64_t read_buffer(uint64_t address, size_t size)
{
    uint64_t returnAddress = reinterpret_cast<uint64_t>(malloc(size));
    Memory::read(address, &returnAddress, size);
}

BOOL Memory::read(uint64_t address, LPVOID variable, size_t size)
{
    if (hProc == 0)
        hProc = OpenProcess(PROCESS_ALL_ACCESS, false, get_proc_id(L"RustClient.exe"));
    if (hProc)
    {
        size_t bytesRead;
        if (NtReadVirtualMemory(hProc, (BYTE*)address, variable, (ULONG)size, (PULONG)&bytesRead))
        {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

uint64_t Memory::read(uint64_t address, std::vector<unsigned int> offsets)
{
    if (hProc == 0)
        hProc = OpenProcess(PROCESS_ALL_ACCESS, false, get_proc_id(L"RustClient.exe"));
    uint64_t ptr = address;
    for (unsigned int i = 0; i < offsets.size(); i++)
    {
        NtReadVirtualMemory(hProc, (BYTE*)ptr, &ptr, sizeof(ptr), 0);
        ptr += offsets[i];
    }
    NtReadVirtualMemory(hProc, (BYTE*)ptr, &ptr, sizeof(ptr), 0);
    return ptr;
}

DWORD Memory::get_proc_id(const wchar_t* processName)
{
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    Process32First(processesSnapshot, &processInfo);
    if (!_wcsicmp(processInfo.szExeFile, processName))
    {
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo))
    {
        if (!_wcsicmp(processInfo.szExeFile, processName))
        {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }

    CloseHandle(processesSnapshot);
    return 0;
}
