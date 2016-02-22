#ifndef SSD_OPTIMIZER_H
#define SSD_OPTIMIZER_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTreeView>
#include <QDebug>

#include <QString>
#include <QDateTime>
#include <iostream>
#include <QFile>
#include <QTextStream>

#include "windows_service.h"
#include "windows_registry.h"
#include "windows_console.h"
#include "windows_wmi.h"
#include "windows_drive_identifiaction.h"

//#include "organization_project.h"

void verboseMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

namespace Ui {
class ssd_optimizer;
}

class ssd_optimizer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ssd_optimizer(QWidget *parent = 0);
    ~ssd_optimizer();
    bool find_str(QString txt, QString find_txt);
    QString calc_size(ULONGLONG size);
    void write_gui();
    QStandardItemModel *createModel(QObject* parent = 0);

private slots:
    void on_pb_win_search_clicked();

    void on_pb_trim_clicked();

    void on_pb_defrag_clicked();

    void on_pb_przesuw_pocz_clicked();

    void on_pb_dziennik_clicked();

    void on_pb_prefetch_clicked();

private:
    Ui::ssd_optimizer *ui;
    windows_wmi *ww;
    QStringList* partition;
    int size;
};

#endif // SSD_OPTIMIZER_H
