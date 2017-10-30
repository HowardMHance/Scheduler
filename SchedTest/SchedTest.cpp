// SchedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tester.h"

void ShowResults(bool pass)
{
    if (pass)
        printf("SUCCESS\n");
    else
        printf("FAILED\n");
}

void RunTest()
{
    Tester tester = Tester();

    if (tester.SchedulerPresent())
    {
        printf("\nRunning Tests\n");

        printf("\n === Bad Command Line === \n");
        bool passed = tester.BadCommandLine();
        printf(" === Bad Command Line: ");
        ShowResults(passed);

        printf("\n\n === Good Command Line === \n");
        passed = tester.GoodCommandLine();
        printf(" === Good Command Line: ");
        ShowResults(passed);

        printf("\n\n === Is Task Running? === \n");
        printf("waiting...");
        passed = tester.TaskRunning();
        printf("\n === Is Task Running?: ");
        ShowResults(passed);

        printf("\n\n === Delete Task === \n");
        passed = tester.DeleteTask();
        printf("\n === Delete Task: ");
        ShowResults(passed);
    }
    else
    {
        printf("\nScheduler.exe not found: must be in same directory as SchedTest.exe\n");
    }
}


int _tmain(int argc, _TCHAR* argv[])
{
    RunTest();
    printf("\nFinished\n");

	return 0;
}

