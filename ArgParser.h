#pragma once

#include<string>

using namespace std;

// command line parameter parser specific to Scheduler
class ArgParser
{
public:
    ArgParser(void);
    ~ArgParser(void);

    bool Parse(int argc, _TCHAR* argv[]);    

    // accessors - (MSVC now puportedly supports properties)
    bool IsDelete()
    { return _isDelete; }

    wstring Name()
    { return _name; }

    wstring Command()
    { return _command; }

    wstring Arguments()
    { return _arguments; }

    wstring Time()
    { return _time; }

private:
    bool _isDelete;
    wstring _name;
    wstring _command;
    wstring _arguments;
    wstring _time;

    void WToUpper(wstring& str);
};

