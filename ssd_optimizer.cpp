#include "ssd_optimizer.h"
#include "ui_ssd_optimizer.h"

ssd_optimizer::ssd_optimizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ssd_optimizer)
{
    ui->setupUi(this);

    write_gui();
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
        ui->rb_ahci->setChecked(true);
        p_ahci.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }else{
        ui->rb_ahci->setChecked(false);
        p_ahci.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }

    ui->rb_ahci->setDisabled(true);
    ui->rb_ahci->setPalette(p_ahci);

    QPalette p_win_search;
    windows_service ws;

    bool state_win_search = ws.state_service("WSearch");

    if(state_win_search == true){
        ui->rb_win_search->setChecked(false);
        ui->pb_win_search->setEnabled(true);
        p_win_search.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }else if(state_win_search == false){
        ui->rb_win_search->setChecked(true);
        ui->pb_win_search->setEnabled(false);
        p_win_search.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }

    ui->rb_win_search->setDisabled(true);
    ui->rb_win_search->setPalette(p_win_search);

    QPalette p_trim;
    windows_console wc;

    QStringList trim;
    trim << "behavior" << "query" << "DisableDeleteNotify";

    QString s_trim = wc.read_cmd_command("fsutil.exe", trim);

    if(find_str(s_trim, "DisableDeleteNotify = 0")){
        ui->rb_trim->setChecked(true);
        ui->pb_trim->setEnabled(false);
        p_trim.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }else{
        ui->rb_trim->setChecked(false);
        p_trim.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }

    ui->rb_trim->setDisabled(true);
    ui->rb_trim->setPalette(p_trim);
}

void ssd_optimizer::on_pb_win_search_clicked()
{
    QPalette p_win_search;
    windows_service ws;

    ws.enable_service("Wsearch", false);
    ws.start_service("WSearch", false);

    bool state_win_search = ws.state_service("WSearch");

    if(state_win_search == true){
        QMessageBox::information(this, "SSD Optimizer", "Nie udało się wyłączyć usługi Windows Search", QMessageBox::Ok);
    }else if(state_win_search == false){
        ui->rb_win_search->setChecked(true);
        ui->pb_win_search->setEnabled(false);
        p_win_search.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }

    ui->rb_win_search->setDisabled(true);
    ui->rb_win_search->setPalette(p_win_search);
}

void ssd_optimizer::on_pb_trim_clicked()
{
    QPalette p_trim;
    windows_console wc;

    QStringList trim;
    trim << "behavior" << "set" << "DisableDeleteNotify" << "0";

    QString s_trim = wc.read_cmd_command("fsutil.exe", trim);

    if(find_str(s_trim, "DisableDeleteNotify = 0")){
        ui->rb_trim->setChecked(true);
        ui->pb_trim->setEnabled(false);
        p_trim.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }else{
        QMessageBox::information(this, "SSD Optimizer", "Włączenie funkcji TRIM nie powiodło się.", QMessageBox::Ok);
        ui->rb_trim->setChecked(false);
        p_trim.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }

    ui->rb_trim->setDisabled(true);
    ui->rb_trim->setPalette(p_trim);
}
