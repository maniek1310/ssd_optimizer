#include "windows_registry.h"

windows_registry::windows_registry()
{

}

QString windows_registry::read_value_key(QString tree, QString key)
{
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion", QSettings::NativeFormat);

    QStringList ak = settings.childGroups();

    settings.beginGroup("CurrentVersion");

    QString path = settings.value("ProductId").toString().trimmed();

    settings.endGroup();

    return "test";
}
