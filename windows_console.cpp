#include "windows_console.h"

windows_console::windows_console()
{

}

void windows_console::send_cmd_command(QString app, QStringList params)
{

    int status = 0;

    QString run = "fsutil.exe";
    QStringList para;
    para << "behavior" << "query" << "DisableDeleteNotify";

    connect(&process, SIGNAL(readyRead()), this, SLOT(readData()));
    //connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readError()));

    //process.open(QIODevice::ReadOnly);

    status = process.execute(run, para);

    process.waitForFinished();
}

void windows_console::readData()
{
    QString output(process.readAllStandardOutput());

    qDebug() << "output : " << output;

    QMessageBox::information(NULL, "test", output, QMessageBox::Yes);
}

void windows_console::readError()
{
    QString outerr(process.readAllStandardError());

    qDebug() << "error : " << outerr;
}
