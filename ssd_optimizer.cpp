#include "ssd_optimizer.h"
#include "ui_ssd_optimizer.h"

ssd_optimizer::ssd_optimizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ssd_optimizer)
{
    ui->setupUi(this);

    write_gui();

    windows_console wc;

    wc.send_cmd_command("wqeqwe", QStringList ("Asdasd"));

    /*windows_service ws;

    ws.find_service("WSearch");
    ws.enable_service("WSearch", false);
    ws.start_service("WSearch", false);*/
}

ssd_optimizer::~ssd_optimizer()
{
    delete ui;
}

bool ssd_optimizer::find_str(QString txt, QString find_txt)
{
    int j = 0;
    bool find = false;

    while((j = txt.indexOf(find_txt, j)) != -1){
        find = true;
        ++j;
    }

    return find;
}

void ssd_optimizer::write_gui()
{
    QPalette p_ahci;
    windows_wmi ww;
    QString ahci = ww.sprawdz("Win32_IDEController", L"Caption", "string");

    if(find_str(ahci, "AHCI")){
        ui->radioButton->setChecked(true);
        p_ahci.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }
    else{
        ui->radioButton->setChecked(false);
        p_ahci.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }

    ui->radioButton->setDisabled(true);
    ui->radioButton->setPalette(p_ahci);
}

void ssd_optimizer::on_pb_win_search_clicked()
{
    windows_service ws;

    bool es = ws.enable_service("WSearch", false);
    bool ss = ws.start_service("WSearch", false);

    if(es == true)
        QMessageBox::information(this, "Windows Search", "Usługa Windows Search jest wyłączona");
}