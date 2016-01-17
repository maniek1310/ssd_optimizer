#include "windows_console.h"

windows_console::windows_console()
{

}

QString windows_console::read_cmd_command(QString app, QStringList params)
{
    QString p_stdout_task;

    process.start(app, params, QIODevice::ReadOnly);
    process.waitForFinished(-1);

    p_stdout_task = process.readAllStandardOutput();

    return p_stdout_task;
}
