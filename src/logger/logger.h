#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

using namespace std;

class Logger
{
public:

    Logger();
    ~Logger();
    void error(string msg, string description = "");
    void info(string msg, string description = "");

private:
    ofstream stream;
    string getLogMessage(string type, string msg, string description, string file, int line);
};

#endif // LOGGER_H
