#include "stdafx.h"
#include <windows.h>
#include <comdef.h>
#include "Taskschd.h"
#include "TaskMaster.h"


// ctor - connect to task service
TaskMaster::TaskMaster(void)
{
    _service = NULL;
    HRESULT hr = CoCreateInstance( CLSID_TaskScheduler,
                 NULL,
                 CLSCTX_INPROC_SERVER,
                 IID_ITaskService,
                 (void**)&_service );  

    if (SUCCEEDED(hr))
        hr = _service->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

    // TODO: find better exception
    if (FAILED(hr))
        throw "COM initialization failure";
}


// create one-time time trigger on given date/time string
bool TaskMaster::AddTrigger(ITaskDefinition *pTask, wstring timeString)
{
    bool succeeded = false;

    ITriggerCollection *pTriggerCollection = NULL;

    HRESULT hr = pTask->get_Triggers( &pTriggerCollection );

    // NOTE: I vastly prefer indentation to early returns
    if (SUCCEEDED(hr))
    {
        ITrigger *pTrigger = NULL;    
        hr = pTriggerCollection->Create( TASK_TRIGGER_TIME, &pTrigger );     
        pTriggerCollection->Release();

        if (SUCCEEDED(hr))
        {
            ITimeTrigger *pTimeTrigger = NULL;
            hr = pTrigger->QueryInterface( 
                IID_ITimeTrigger, (void**) &pTimeTrigger );
            pTrigger->Release();

            if (SUCCEEDED(hr))
            {
                hr = pTimeTrigger->put_StartBoundary(ToBSTR(timeString));
                pTimeTrigger->Release();    

                succeeded = SUCCEEDED(hr);
            }
        }
    }

    return succeeded;
}

// add current user, set task as 'interactive'
bool TaskMaster::AddPrincipal(ITaskDefinition *pTask)
{
    bool succeeded = false;

    IPrincipal *pPrincipal = NULL;
    HRESULT hr = pTask->get_Principal( &pPrincipal );
    if(SUCCEEDED(hr))
    {
        hr = pPrincipal->put_LogonType( TASK_LOGON_INTERACTIVE_TOKEN );
        pPrincipal->Release();

        succeeded = SUCCEEDED(hr);
    }
    
    return succeeded;
}

// create executable action in task
bool TaskMaster::AddExecutable(ITaskDefinition *pTask, wstring path, wstring args)
{
    bool succeeded = false;

    IActionCollection *pActionCollection = NULL;

    HRESULT hr = pTask->get_Actions( &pActionCollection );
    if( SUCCEEDED(hr) )
    {
        IAction *pAction = NULL;
        hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
        pActionCollection->Release();

        if(SUCCEEDED(hr))
        {
            IExecAction *pExecAction = NULL;
            //  QI for the executable task pointer.
            hr = pAction->QueryInterface(IID_IExecAction, (void**) &pExecAction);
            pAction->Release();

            if (SUCCEEDED (hr))
            {
                pExecAction->put_Path(ToBSTR(path));

                if (!args.empty())
                    pExecAction->put_Arguments(ToBSTR(args));

                pExecAction->Release();

                succeeded = true;
            }
        }
    }
    
    return succeeded;
}

// save task to root task folder
bool TaskMaster::RegisterTask(ITaskFolder *pRoot, ITaskDefinition *pTask, wstring taskName)
{
    bool succeeded = false;

    IRegisteredTask *pRegisteredTask = NULL;
    HRESULT hr = pRoot->RegisterTaskDefinition(
            ToBSTR(taskName),
            pTask,
            TASK_CREATE_OR_UPDATE, 
            _variant_t(), 
            _variant_t(), 
            TASK_LOGON_INTERACTIVE_TOKEN,
            _variant_t(L""),
            &pRegisteredTask);

    if (SUCCEEDED(hr))
    {
        pRegisteredTask->Release();
        succeeded = true;
    }

    return succeeded;
}

bool TaskMaster::Schedule(wstring taskName, wstring path, wstring time)
{
    return Schedule(taskName, path, wstring(), time);
}

bool TaskMaster::Schedule(wstring taskName, wstring path, wstring args, wstring time)
{
    bool succeeded = false;

    ITaskFolder *pRoot = GetRoot();
    if (pRoot != NULL)
    {
        ITaskDefinition *pTask = NULL;
        HRESULT hr = _service->NewTask( 0, &pTask );

        // short circuits in case of failure
        // eliminates multiple returns or deep indentation
        succeeded = SUCCEEDED(hr)            &&
            AddTrigger(pTask, time)          && 
            AddPrincipal(pTask)              &&
            AddExecutable(pTask, path, args) && 
            RegisterTask(pRoot, pTask, taskName);
    }

    return succeeded;
}


// get root Task folder
ITaskFolder* TaskMaster::GetRoot()
{
    ITaskFolder *pRoot = NULL;    
    _service->GetFolder( _bstr_t( L"\\") , &pRoot);

    return pRoot;
}

// delete task with given name
// NOTE: There is a bug in the Task Folder security setttings
//       This will fail unless user is explicitly granted access to the folder
bool TaskMaster::Delete(wstring taskName)
{
    bool succeeded = false;

    ITaskFolder *pRoot = GetRoot();
    if (pRoot != NULL)
    {
        HRESULT hr = pRoot->DeleteTask(ToBSTR(taskName), 0);
        succeeded = SUCCEEDED(hr);
        pRoot->Release();
    }

    return succeeded;
}

_bstr_t TaskMaster::ToBSTR(wstring str)
{
     return _bstr_t(str.c_str());
}

TaskMaster::~TaskMaster(void)
{
    if (_service != NULL)
        _service->Release();
}
