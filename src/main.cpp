#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

int PrintModules( DWORD processID )
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Print the process identifier.

    printf( "\nProcess ID: %u\n", processID );

    // Get a handle to the process.

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            FALSE, processID );
    if (NULL == hProcess)
        return 1;

   // Get a list of all the modules in this process.

    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(TCHAR)))
            {
                // Print the module name and handle value.

                _tprintf( TEXT("\t%s (0x%p)\n"), szModName, hMods[i] );
            }
        }
    }
    
    // Release the handle to the process.

    CloseHandle( hProcess );

    return 0;
}

DWORD findProcessByName(const TCHAR* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    DWORD th32ProcessID = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_tcsicmp (entry.szExeFile, processName) == 0)
            {
                th32ProcessID = entry.th32ProcessID;
                break;
            }
        }
    }

    CloseHandle(snapshot);
    return th32ProcessID;
}

int _tmain( int argc, TCHAR *argv[] )
{
    DWORD processID = findProcessByName(argv[1]);
    if (processID == 0) {
        printf("Process not found\n");
        return 1;
    }

    PrintModules(processID);
    return 0;
}