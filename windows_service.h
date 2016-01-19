#ifndef WINDOWS_SERVICE_H
#define WINDOWS_SERVICE_H

#include <QMainWindow>
#include <QDebug>

#include <windows.h>
#include <iostream>

#pragma comment(lib, "Advapi32.lib")

class windows_service
{
public:
    windows_service();
    bool find_service(QString name);
    bool start_service(QString name, bool status);
    bool enable_service(QString name, bool status);
    bool state_service(QString name);
};

#endif // WINDOWS_SERVICE_H
