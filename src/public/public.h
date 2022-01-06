#ifndef PUBLIC_H
#define PUBLIC_H

#include <src/logger/logger.h>


struct Settings
{
    string basePath = "conf/";
    string shaderPath = "shader/";
};

extern Logger logger;

extern Settings settings;

#endif // PUBLIC_H
