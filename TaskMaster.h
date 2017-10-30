#pragma once
#include "Taskschd.h"
#include "comutil.h"
#include <string>

using namespace std;

// simple wrapper class for Task Scheduler interfaces
// supports : creation of a time trigger class
//          : deletion of tasks
class TaskMaster
{
public:
    TaskMaster(void);
    ~TaskMaster(void);

    // create interactive for logged on user task with one-time time trigger
    bool Schedule(wstring taskName, wstring path, wstring time);
    bool Schedule(wstring taskName, wstring path, wstring args, wstring time);

    // delete given task
    bool Delete(wstring taskName);

private:
    ITaskService* _service;    

    _bstr_t ToBSTR(wstring str);
    ITaskFolder* GetRoot();

    bool AddTrigger(ITaskDefinition *pTask, wstring time);
    bool AddPrincipal(ITaskDefinition *pTask);
    bool AddExecutable(ITaskDefinition *pTask, wstring path, wstring args);
    bool RegisterTask(ITaskFolder *pRoot, ITaskDefinition *pTask, wstring taskName);
};

