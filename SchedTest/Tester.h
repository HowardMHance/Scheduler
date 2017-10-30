#pragma once

#include <string>

using namespace std;

class Tester
{
public:
    Tester(void);
    ~Tester(void);

    bool SchedulerPresent();
    bool BadCommandLine();
    bool GoodCommandLine();
    bool TaskRunning();
    bool DeleteTask();

private:
    wstring _taskName;
    wstring _homeDir;
    wstring _testFilePath;
    wstring _schedulerPath;

    wstring ExePath(); 
    int     RunScheduler(wstring commandLine);
    void    CreateTestFile();
    
};

