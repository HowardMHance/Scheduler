// Scheduler.cpp : Defines the entry point for the console application.
// A simple utility for scheduling tasks via Task Scheduler
//

#include "stdafx.h"
#include "TaskMaster.h"
#include "ArgParser.h"

// do the actual work of the app
int RunSchedule(int argc, _TCHAR* argv[])
 {
    int returnCode = 1;

    ArgParser parser = ArgParser();

    if (parser.Parse(argc, argv))
    {
        TaskMaster master = TaskMaster();

        if (parser.IsDelete())
        {
            if (master.Delete(parser.Name()))
                returnCode = 0;
        } 
        else
        {
            if (master.Schedule(parser.Name(),
                                parser.Command(),
                                parser.Arguments(),
                                parser.Time()))
                returnCode = 0;
        }
    }
    else // parsing failed
    {
        printf("Invalid command line\n");
    }
    
    return returnCode;
 }

// COM initialization & help
int _tmain(int argc, _TCHAR* argv[])
{
    int returnCode = 1;

    if (argc <= 1)
    {
        printf("\nUSAGE:\n");
        printf("Scheduler name time command [arguments]\n");
        printf("> Scheduler MyTask 2015-05-02T08:00:00 Notepad.exe Myfile.txt\n\n");

        printf("Scheduler /delete name\n");
        printf("> Scheduler /delete MyTask\n\n");

    }
    else if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
    {
        //  Set general COM security levels.
        HRESULT hr = CoInitializeSecurity(
            NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
            RPC_C_IMP_LEVEL_DELEGATE,
            NULL,
            0,
            NULL);

        if (SUCCEEDED(hr))
        {
            try
            {
                returnCode = RunSchedule(argc, argv);
            }
            catch(...)
            {
                printf("An exception was thrown");
            }
        }

        CoUninitialize();
    }
    else 
    {
        printf("Failed to initialize COM");
    }

    if (returnCode == 0)        printf("SUCCEEDED\n");    else        printf("FAILED\n");
	return returnCode;
}

 