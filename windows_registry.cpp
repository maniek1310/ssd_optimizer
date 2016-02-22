#include "windows_registry.h"

windows_registry::windows_registry()
{

}

QString windows_registry::read_value_key(QString tree, QString key)
{
    QSettings settings(tree, QSettings::NativeFormat);

    QString value = settings.value(key, "NULL").toString();

    return value;
}

bool windows_registry::set_value_key(QString tree, QString key, int value)
{
    QSettings settings(tree, QSettings::NativeFormat);

    settings.setValue(key, value);

    QString check = settings.value(key, "NULL").toString();

    if(QString::number(value) == check)
        return true;

    return false;
}

bool windows_registry::find_key_registry(QString tree, QString key)
{
    QSettings settings(tree, QSettings::NativeFormat);

    QStringList all_keys = settings.allKeys();

    foreach(QString v, all_keys){
        if(v == key)
            return true;
    }

    return false;
}
