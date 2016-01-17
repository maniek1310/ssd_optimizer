#ifndef WINDOWS_REGISTRY_H
#define WINDOWS_REGISTRY_H

#include <QMainWindow>
#include <QSettings>
#include <QStringList>
#include <QDebug>

class windows_registry
{
public:
    windows_registry();
    QString read_value_key(QString tree, QString key);
};

#endif // WINDOWS_REGISTRY_H
