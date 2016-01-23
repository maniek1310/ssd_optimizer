#include "ssd_optimizer.h"
#include "ui_ssd_optimizer.h"

ssd_optimizer::ssd_optimizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ssd_optimizer)
{
    ui->setupUi(this);

    ww->polacz_wmi();

    write_gui();

    //qDebug() << ww->sprawdz("Win32_DiskPartition", L"StartingOffset", "string", "");
    QStringList disk_offset = ww->sprawdz_array("Win32_DiskPartition", L"StartingOffset");

    for(int i = 0; i < disk_offset.size(); i++)
    {
        int dzielna = QString(disk_offset.at(i)).toInt() % 4096;
        qDebug() << "dzielna z " << disk_offset.at(i) << " wynosi : " << dzielna;
    }

    ww->rozlacz_wmi();
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

QString ssd_optimizer::calc_size(ULONGLONG size)
{
    QString size_return;

    if(size > ((1024 * 1024 * 1024) - 1))
        size_return = QString::number(((size / 1024) / 1024) / 1024) + " GB";
    else if(size > ((1024 * 1024)) - 1)
        size_return = QString::number((size / 1024) / 1024) + " MB";
    else if(size > (1024 - 1))
        size_return = QString::number(size / 1024) + " KB";

    return size_return;
}

void ssd_optimizer::write_gui()
{
    QPalette p_ahci;
    QString ahci = ww->sprawdz("Win32_IDEController", L"Caption");

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

    QPalette p_defrag;
    bool state_defrag = ws.state_service("defragsvc");

    QStringList service_defrag_state = ww->sprawdz_array("Win32_Service", L"StartMode", "Name = \"defragsvc\"");

    if(state_defrag == true || service_defrag_state[0] == "Manual" || service_defrag_state[0] == "Auto"){
        ui->rb_defrag->setChecked(false);
        ui->pb_defrag->setEnabled(true);
        p_defrag.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }else if(state_defrag == false){
        ui->rb_defrag->setChecked(true);
        ui->pb_defrag->setEnabled(false);
        p_defrag.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }

    ui->rb_defrag->setDisabled(true);
    ui->rb_defrag->setPalette(p_defrag);

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
        ui->pb_trim->setEnabled(true);
        p_trim.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }

    ui->rb_trim->setDisabled(true);
    ui->rb_trim->setPalette(p_trim);

    // Przesunięcie względem początku dysku

    QPalette p_przesuw_pocz;
    QStringList partition_name = ww->sprawdz_array("Win32_DiskPartition", L"Name");
    QStringList partition_size = ww->sprawdz_array("Win32_DiskPartition", L"Size");
    QStringList partition_offset = ww->sprawdz_array("Win32_DiskPartition", L"StartingOffset");
    partition = new QStringList[partition_name.size()];
    size = partition_name.size();

    bool b_offset = true;

    for(int i = 0; i < partition_name.size(); i++)
    {
        int offset = QString(partition_offset.at(i)).toInt() % 4096;

        if(b_offset == true){
            if(offset != 0)
                b_offset = false;
        }

        partition[i] << partition_name[i] << partition_size[i] << QString::number(offset);

        qDebug() << i << " : " << partition[i] << " zmienna b_offset to : " << b_offset;
    }

    if(b_offset){
        ui->rb_przesuw_pocz->setChecked(true);
        ui->pb_przesuw_pocz->setEnabled(false);
        p_przesuw_pocz.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }else{
        ui->rb_przesuw_pocz->setChecked(false);
        ui->pb_przesuw_pocz->setEnabled(true);
        p_przesuw_pocz.setColor(QPalette::WindowText, QColor(170, 0, 0));
    }

    ui->rb_przesuw_pocz->setDisabled(true);
    ui->rb_przesuw_pocz->setPalette(p_przesuw_pocz);
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

void ssd_optimizer::on_pb_defrag_clicked()
{
    QPalette p_defrag;
    windows_service ws;

    ws.enable_service("defragsvc", false);
    ws.start_service("defragsvc", false);

    bool state_defrag = ws.state_service("defragsvc");

    if(state_defrag == true){
        QMessageBox::information(this, "SSD Optimizer", "Nie udało się wyłączyć usługi Optymalizacji Dysków", QMessageBox::Ok);
    }else if(state_defrag == false){
        ui->rb_defrag->setChecked(true);
        ui->pb_defrag->setEnabled(false);
        p_defrag.setColor(QPalette::WindowText, QColor(0, 170, 0));
    }

    ui->rb_defrag->setDisabled(true);
    ui->rb_defrag->setPalette(p_defrag);
}

void ssd_optimizer::on_pb_przesuw_pocz_clicked()
{
    QString text = "Któraś z partycji nie jest prawidłowo sformatowana.\n"
                   "Problem dotyczy partycji, której offset jest większy niż 0.\n\n";

    for(int i = 0; i < size; i++)
    {
        QString size_partition = calc_size(QString(partition[i].at(1)).toULongLong());

        text += "Nazwa partycji : ";
        text += partition[i].at(0);
        text += " Rozmiar partycji : ";
        text += size_partition;
        text += " Offset : ";
        text += partition[i].at(2);
        text += "\n\n";
    }

    QMessageBox::information(this, "SSD Optimizer", text, QMessageBox::Ok);
}
