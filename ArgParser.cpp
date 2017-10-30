#include "stdafx.h"
#include <string>
#include "ArgParser.h"

using namespace std;


ArgParser::ArgParser(void)
{
}


ArgParser::~ArgParser(void)
{
}

void ArgParser::WToUpper(wstring& str)
{
    for (unsigned i = 0; i < str.length(); i++)
        str[i] = toupper(str[i]);
}

bool ArgParser::Parse(int argc, _TCHAR* argv[])
{
    bool succeeded = false;


    if ((argc >= 2) && (argc <= 5))
    {
        wstring first(argv[0]);
        // upcase for easy comparison
        WToUpper(first);
        
        // name of program won't be first arg if launched via CreateProcess
        int firstArg = 1;
        if  (first.find(L"SCHEDULER.EXE") == -1)
            firstArg = 0;

        wstring next = argv[firstArg];
        WToUpper(next);

        _isDelete = next.find(L"/DELETE") != -1;
        if  (_isDelete)
        {
            _name = argv[firstArg +1];
            succeeded = true;
        }
        else if (argc >= 3 + firstArg) 
        {
            _name = argv[firstArg];
            _time = argv[firstArg +1];
            _command = argv[firstArg +2];
            if (argc == 4 + firstArg)
                _arguments = argv[firstArg +3];

            // TODO: Validate Date/Time format (true pain)
            succeeded = true;
        }
    }

    return succeeded;
}


