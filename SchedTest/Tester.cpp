#include "stdafx.h"
#include "windows.h"
#include "Tester.h"
#include "Shlwapi.h"
#include <strsafe.h>

#include <iostream>
#include <fstream>  


#define FileName L"\\MyFile.txt"
#define SchedulerName L"\\Scheduler.exe"
#define TaskName L"TestTask"

Tester::Tester(void)
{
    _homeDir = ExePath();   

    _testFilePath  = _homeDir;
    _testFilePath.append(FileName);
    
    _schedulerPath = _homeDir;
    _schedulerPath.append(SchedulerName);

    // generate unique task name
    UUID newId;
    UuidCreate( &newId );
    WCHAR* buffer = NULL;
    UuidToStringW(&newId, (RPC_WSTR*)&buffer);

    _taskName = TaskName;
    _taskName.append(buffer);


    if (!PathFileExists(_testFilePath.c_str()))
        CreateTestFile();

}


BOOL CALLBACK FindTaskWindow(HWND hwnd, LPARAM lParam)
{
    BOOL keepTrying = TRUE;

    WCHAR title[1024];
	GetWindowText(hwnd,title,sizeof(title));

    wstring titleTest(L"MyFile.txt - Notepad");
    wstring titleTestNoExt(L"MyFile - Notepad");

    if ((titleTest.compare(title) == 0) ||
        (titleTestNoExt.compare(title) == 0))
    {
        *(HWND*)lParam = hwnd;
        keepTrying = FALSE;
    }

	return keepTrying;
}

// returns true if task has been deleted
bool Tester::DeleteTask()
{
     wstring command(L"/delete ");
     command.append(_taskName);

     return (RunScheduler(command) == 0);
}

// returns true if task is running
bool Tester::TaskRunning()
{
    // we scheduled it <= a minute in the future...
    Sleep(60 * 1000);

    HWND foundWindow = 0;
    EnumWindows(FindTaskWindow, (LPARAM)&foundWindow);

    if (foundWindow != NULL)
        SendMessage(foundWindow, WM_CLOSE, 0, 0);

    return (foundWindow != NULL);
}

// sends bad command line,
// success means scheduling failed
bool Tester::BadCommandLine()                                        
{
    return (RunScheduler(L"foobar") != 0);
}

// sends good command line,
// success means scheduling succeed
bool Tester::GoodCommandLine()                                        
{
    wstring commandLine = _taskName;

    SYSTEMTIME time;
    GetSystemTime(&time);
    time.wMinute += 1;

    WCHAR buffer[25];
    wsprintf(buffer,L" %4d-%02d-%02dT%02d:%02d:00Z ",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute);
    commandLine.append(buffer);
    commandLine.append(L" Notepad.exe ");
    commandLine.append(_testFilePath);

    return (RunScheduler(commandLine) == 0);
}

wstring Tester::ExePath() 
{
    WCHAR buffer[MAX_PATH];

    GetCurrentDirectory(MAX_PATH, buffer);

    return wstring( buffer );
}

void Tester::CreateTestFile()
{
    std::ofstream outfile (_testFilePath.c_str());

    outfile << "Scheduler Test File!" << std::endl;

    outfile.close();

}

// runs scheduler, returns return code
int Tester::RunScheduler(wstring commandLine)
{
    STARTUPINFO info={sizeof(info)};
    PROCESS_INFORMATION processInfo;

    DWORD exitCode = -1;

    wstring fullCmd = _schedulerPath;
    fullCmd.append(L" ");
    fullCmd.append(commandLine);


    // wstring is const buffer - CreateProcess doesn't like it
    WCHAR argLine[1024];
    wsprintf(argLine, fullCmd.c_str());

    if (CreateProcess(NULL, argLine, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
    {
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        GetExitCodeProcess(processInfo.hProcess, &exitCode);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    else
    {
        // debug scaffolding
        wchar_t buf[256];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);

    }

    return (int)exitCode;
}

bool Tester::SchedulerPresent()
{
    return (PathFileExists(_schedulerPath.c_str()) != 0);
}

Tester::~Tester(void)
{
}
