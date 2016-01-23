#ifndef WINDOWS_WMI_H
#define WINDOWS_WMI_H

#include <QCoreApplication>
#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <string>
#include <sstream>
#include <QDebug>

using namespace std;

#pragma comment(lib, "wbemuuid.lib")

class windows_wmi
{
public:
    windows_wmi();
    void polacz_wmi();
    void rozlacz_wmi();
    QString sprawdz(string s_class, const wstring &s_name);
    QStringList sprawdz_array(string s_class, const wstring &s_name, string s_set = "NULL");
};

#endif // WINDOWS_WMI_H

