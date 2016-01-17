#ifndef SSD_OPTIMIZER_H
#define SSD_OPTIMIZER_H

#include <QMainWindow>
#include <QDebug>

#include "windows_service.h"
#include "windows_registry.h"
#include "windows_console.h"
#include "windows_wmi.h"

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
    void write_gui();

private slots:
    void on_pb_win_search_clicked();

private:
    Ui::ssd_optimizer *ui;
};

#endif // SSD_OPTIMIZER_H
