#ifndef WINDOWS_WMI_H
#define WINDOWS_WMI_H

#include <QMainWindow>
#include <iostream>
#include <comdef.h>
#include <wbemidl.h>
#include <string>
#include <sstream>
#include <QDebug>

using namespace std;

#pragma comment(lib, "wbemuuid.lib")

class windows_wmi
{
public:
    windows_wmi();
    QString sprawdz(string s_class, const wstring &s_name, string s_type);
};

#endif // WINDOWS_WMI_H
