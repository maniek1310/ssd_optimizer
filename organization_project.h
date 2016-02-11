#ifndef ORGANIZATION_PROJECT_H
#define ORGANIZATION_PROJECT_H

#include <QDebug>
#include <QString>
#include <QDateTime>
#include <iostream>

class organization_project
{
public:
    organization_project();
    void init_log();
    void verboseMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // ORGANIZATION_PROJECT_H
