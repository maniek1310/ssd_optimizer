#ifndef WINDOWS_REGISTRY_H
#define WINDOWS_REGISTRY_H

#include <QMainWindow>
#include <QSettings>
#include <QStringList>
#include <QDebug>
#include <Windows.h>

class windows_registry
{
public:
    windows_registry();
    QString read_value_key(QString tree, QString key);
    bool set_value_key(QString tree, QString key, int value);
    bool find_key_registry(QString tree, QString key);
};

#endif // WINDOWS_REGISTRY_H
