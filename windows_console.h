#ifndef WINDOWS_CONSOLE_H
#define WINDOWS_CONSOLE_H

#include <QMainWindow>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QObject>
#include <QMessageBox>

class windows_console : public QObject
{
    Q_OBJECT
public:
    windows_console();
    void send_cmd_command(QString app, QStringList params);

public slots:
    void readData();
    void readError();

private:
    QProcess process;
};

#endif // WINDOWS_CONSOLE_H
