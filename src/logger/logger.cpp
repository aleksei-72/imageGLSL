#include <src/logger/logger.h>

#include <iostream>

const string TAB = "    ";

Logger::Logger()
{
    stream.open("file.log");
};

Logger::~Logger()
{
    stream.close();
};

void Logger::error(string msg, string description)
{
    string s = getLogMessage("error", msg, description, __FILE__, __LINE__);
    cerr << s;
    stream << s;
}

void Logger::info(string msg, string description)
{
    string s = getLogMessage("info", msg, description, __FILE__, __LINE__);
    cout << s;
    stream << s;
}


string Logger::getLogMessage(string type, string msg, string description, string file, int line)
{
    string str = type + ": " + msg + '\n';

    if (description.length())
    {
        str += TAB + description + '\n';
    }

    //str += TAB + "file: " + file + '\n' + "line: " + to_string(line) + '\n';

    return str + '\n';
}
