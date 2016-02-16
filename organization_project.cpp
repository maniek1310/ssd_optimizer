#include "organization_project.h"

organization_project::organization_project()
{

}

void organization_project::init_log()
{
    //qInstallMessageHandler(*verboseMessageHandler);
}

void organization_project::verboseMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static const char* typeStr[] = {"[   Debug]", "[ Warning]", "[Critical]", "[   Fatal]", "[    Info]" };

    if(type <= QtInfoMsg)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        QString contextString(QStringLiteral("(%1, %2, %3)")
                              .arg(context.file)
                              .arg(context.function)
                              .arg(context.line));

        QString timeStr(QDateTime::currentDateTime().toString("dd-MM-yy HH:mm:ss:zzz"));

        std::cerr << timeStr.toLocal8Bit().constData() << " - "
                  << typeStr[type] << " "
                  << contextString.toLocal8Bit().constData() << " "
                  << localMsg.constData() << std::endl;

        if(type == QtFatalMsg)
        {
            abort();
        }
    }
}

void organization_project::create_log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

}
