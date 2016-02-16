#include "ssd_optimizer.h"
#include "ui_ssd_optimizer.h"

#include <QtMsgHandler>
#include <QMessageLogContext>

ssd_optimizer::ssd_optimizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ssd_optimizer)
{
    ui->setupUi(this);

    if(QFile::exists("log.txt"))
        QFile::remove("log.txt");

    qInstallMessageHandler(verboseMessageHandler);

    ww->polacz_wmi();

    write_gui();

    ww->rozlacz_wmi();
}

void verboseMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile log_file("log.txt");

    if(log_file.open(QIODevice::Append | QIODevice::Text))
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

            QTextStream out(&log_file);

            out << timeStr.toLocal8Bit().constData() << " - "
                << typeStr[type] << " "
                << contextString.toLocal8Bit().constData() << " "
                << localMsg.constData() << "\n";

            if(type == QtFatalMsg)
            {
                abort();
            }
        }
    }

    log_file.flush();
    log_file.close();
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
    QStandardItemModel* model = createModel(this);

    ui->treeView->setModel(model);

    QPalette p_ahci;
    QString ahci = ww->sprawdz("Win32_IDEController", L"Caption");

    qInfo() << "QString ahci = " << ahci;

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

    qInfo() << "bool state_win_search = " << state_win_search;

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

    qInfo() << "bool state_defrag = " << state_defrag;

    QStringList service_defrag_state = ww->sprawdz_array("Win32_Service", L"StartMode", "Name = \"defragsvc\"");

    qInfo() << "QStringList service_defrag_state = " << service_defrag_state;

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

    qInfo() << "QString s_trim = " << s_trim;

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
    qInfo() << "QStringList partition_name = " << partition_name;

    QStringList partition_size = ww->sprawdz_array("Win32_DiskPartition", L"Size");
    qInfo() << "QStringList partition_size = " << partition_size;

    QStringList partition_offset = ww->sprawdz_array("Win32_DiskPartition", L"StartingOffset");
    qInfo() << "QStringList partition_offset = " << partition_offset;

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

        qInfo() << "QStringList partition = " << partition[i];
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

QStandardItemModel *ssd_optimizer::createModel(QObject *parent)
{
    QStandardItemModel* model = new QStandardItemModel(parent);

    QStringList disks_name = ww->sprawdz_array("Win32_DiskDrive", L"Model");
    qInfo() << "QStringList disks_name = " << disks_name;

    QStringList disks_device_id = ww->sprawdz_array("Win32_DiskDrive", L"DeviceID");
    qInfo() << "QStringList disks_device_id = " << disks_device_id;

    QStringList disks_size = ww->sprawdz_array("Win32_DiskDrive", L"Size");
    qInfo() << "QStringList disks_size = " << disks_size;

    QStringList disks_serial_number = ww->sprawdz_array("Win32_DiskDrive", L"SerialNumber");
    qInfo() << "QStringList disks_serial_number = " << disks_serial_number;

    QStringList disks_firmware_revision = ww->sprawdz_array("Win32_DiskDrive", L"FirmwareRevision");
    qInfo() << "QStringList disks_firmware_revision = " << disks_firmware_revision;

    QStringList disks_total_cylinders = ww->sprawdz_array("Win32_DiskDrive", L"TotalCylinders");
    qInfo() << "QStringList disks_total_cylinders = " << disks_total_cylinders;

    QStringList disks_total_sectors = ww->sprawdz_array("Win32_DiskDrive", L"TotalSectors");
    qInfo() << "QStringList disks_total_sectors = " << disks_total_sectors;

    QStringList disks_total_tracks = ww->sprawdz_array("Win32_DiskDrive", L"TotalTracks");
    qInfo() << "QStringList disks_total_tracks = " << disks_total_tracks;

    QStringList disks_tracks_per_cylinder = ww->sprawdz_array("Win32_DiskDrive", L"TotalSectors");
    qInfo() << "QStringList disks_tracks_per_cylinder = " << disks_tracks_per_cylinder;

    QStringList disks_scsi_bus = ww->sprawdz_array("Win32_DiskDrive", L"SCSIBus");
    qInfo() << "QStringList disks_scsi_bus = " << disks_scsi_bus;

    QStringList disks_index = ww->sprawdz_array("Win32_DiskDrive", L"Index");
    qInfo() << "QStringList disks_index = " << disks_index;


    QStringList partitions_caption = ww->sprawdz_array("Win32_Volume", L"Caption", "FileSystem = \"FAT32\" OR FileSystem = \"exFAT\" OR FileSystem = \"NTFS\"");
    qInfo() << "QStringList partitions_caption = " << partitions_caption;

    QStringList partitions_drive_letter = ww->sprawdz_array("Win32_Volume", L"DriveLetter", "FileSystem = \"FAT32\" OR FileSystem = \"exFAT\" OR FileSystem = \"NTFS\"");
    qInfo() << "QStringList partitions_drive_letter = " << partitions_drive_letter;

    QStringList partitions_file_system = ww->sprawdz_array("Win32_Volume", L"FileSystem", "FileSystem = \"FAT32\" OR FileSystem = \"exFAT\" OR FileSystem = \"NTFS\"");
    qInfo() << "QStringList partitions_file_system = " << partitions_file_system;

    QStringList partitions_free_space = ww->sprawdz_array("Win32_Volume", L"FreeSpace", "FileSystem = \"FAT32\" OR FileSystem = \"exFAT\" OR FileSystem = \"NTFS\"");
    qInfo() << "QStringList partitions_free_space = " << partitions_free_space;

    QStringList partitions_total_space = ww->sprawdz_array("Win32_Volume", L"Capacity", "FileSystem = \"FAT32\" OR FileSystem = \"exFAT\" OR FileSystem = \"NTFS\"");
    qInfo() << "QStringList partitions_total_space = " << partitions_total_space;

    QStringList partitions_label = ww->sprawdz_array("Win32_Volume", L"Label", "FileSystem = \"FAT32\" OR FileSystem = \"exFAT\" OR FileSystem = \"NTFS\"");
    qInfo() << "QStringList partitions_label = " << partitions_label;

    QStringList partitions_drive_index;

    windows_drive_identifiaction wdi;

    for(int i = 0; i < partitions_caption.size(); i++)
    {
        QString remove_slash = partitions_caption.at(i);
        remove_slash.replace("\\\\?\\", "\\");
        remove_slash.replace("\\", "");

        QString locate_partition;

        if(find_str(remove_slash, "Volume") == true)
            locate_partition = "\\\\?\\" + remove_slash;
        else
            locate_partition = "\\\\.\\" + remove_slash;

        qInfo() << "QString locate_partition = " << locate_partition;

        partitions_drive_index << QString::number(wdi.get_index_disk_for_partition((const wchar_t*) locate_partition.utf16()));
        qInfo() << "QStringList partitions_drive_index = " << partitions_drive_index[i];
    }

    int k = 0;

    for(int i = 0; i < disks_device_id.size(); i++)
    {
        bool is_trim = wdi.device_trim_property((const wchar_t*) QString(disks_device_id.at(i)).utf16());
        qInfo() << "bool is_trim = " << is_trim << " is disks_device_id = " << disks_device_id.at(i);

        if(is_trim == true)
        {
            ULONGLONG partition_style = wdi.device_partition_info(IPT_PartitionStyle, (const wchar_t*) QString(disks_device_id.at(i)).utf16());
            qInfo() << "ULONGLONG partition_style = " << partition_style << " is disks_device_id = " << disks_device_id.at(i);

            QString s_partition_style;

            if(partition_style == 0)
                s_partition_style = "MBR";
            else if(partition_style = 1)
                s_partition_style = "GPT";
            else if(partition_style = 2)
                s_partition_style = "RAW";

            QStandardItem* disk_name = new QStandardItem(disks_name.at(i));
            QStandardItem* disk_size = new QStandardItem(QString("Rozmiar dysku : " + calc_size(QString(disks_size.at(i)).toULongLong())));
            QStandardItem* disk_id   = new QStandardItem(QString("Identyfikator : " + disks_device_id.at(i)));
            QStandardItem* disk_serial_number = new QStandardItem(QString("Numer seryjny : " + disks_serial_number.at(i)));
            QStandardItem* disk_firmware_revision = new QStandardItem(QString("Wersja Firmware : " + disks_firmware_revision.at(i)));
            QStandardItem* disk_style = new QStandardItem(QString("Rodzaj partycjonowania : " + s_partition_style));
            QStandardItem* disk_bus_type = new QStandardItem(QString("Typ złącza : " + wdi.device_adapter_property((const wchar_t*) QString(disks_device_id.at(i)).utf16())));
            QStandardItem* disk_total_cylinders = new QStandardItem(QString("Całkowita liczba cylindrów : " + disks_total_cylinders.at(i)));
            QStandardItem* disk_total_sectors = new QStandardItem(QString("Całkowita liczba sektorów : " + disks_total_sectors.at(i)));
            QStandardItem* disk_total_tracks = new QStandardItem(QString("Całkowita liczba ścieżek : " + disks_total_tracks.at(i)));
            QStandardItem* disk_tracks_per_cylinder = new QStandardItem(QString("Ścieżki / cylinder : " + disks_tracks_per_cylinder.at(i)));
            QStandardItem* disk_scsi_bus = new QStandardItem(QString("Port złącza : " + disks_scsi_bus.at(i)));
            QStandardItem* disk_partitions = new QStandardItem("Partycje");

            QStandardItem* disk_clear_line = new QStandardItem("");

            disk_name->appendRow(disk_size);
            disk_name->appendRow(disk_id);
            disk_name->appendRow(disk_serial_number);
            disk_name->appendRow(disk_firmware_revision);
            disk_name->appendRow(disk_style);
            disk_name->appendRow(disk_bus_type);
            disk_name->appendRow(disk_scsi_bus);
            disk_name->appendRow(disk_total_cylinders);
            disk_name->appendRow(disk_total_sectors);
            disk_name->appendRow(disk_total_tracks);
            disk_name->appendRow(disk_tracks_per_cylinder);

            disk_name->appendRow(disk_clear_line);

            disk_name->appendRow(disk_partitions);

            for(int j = 0; j < partitions_caption.size(); j++)
            {
                if(QString(partitions_drive_index.at(j)).toInt() == QString(disks_index.at(i)).toInt())
                {
                    QStandardItem* partition_caption = new QStandardItem("Identyfikator partycji : " + partitions_caption.at(j));
                    QStandardItem* partition_drive_letter = new QStandardItem("Litera dysku : " + partitions_drive_letter.at(j));
                    QStandardItem* partition_file_system = new QStandardItem("System plików : " + partitions_file_system.at(j));
                    QStandardItem* partition_free_space = new QStandardItem("Wolne miejsce : " + calc_size(QString(partitions_free_space.at(j)).toULongLong()));
                    QStandardItem* partition_total_space = new QStandardItem("Całkowita powierzchnia : " + calc_size(QString(partitions_total_space.at(j)).toULongLong()));
                    QStandardItem* partition_label = new QStandardItem("Nazwa woluminu : " + partitions_label.at(j));

                    QStandardItem* partition_clear_line = new QStandardItem("");

                    disk_partitions->appendRow(partition_caption);
                    disk_partitions->appendRow(partition_drive_letter);
                    disk_partitions->appendRow(partition_file_system);
                    disk_partitions->appendRow(partition_free_space);
                    disk_partitions->appendRow(partition_total_space);
                    disk_partitions->appendRow(partition_label);

                    disk_partitions->appendRow(partition_clear_line);
                }
            }

            model->setItem(k, 0, disk_name);

            k += 2;
        }
    }

    model->setHorizontalHeaderItem(0, new QStandardItem("Nazwa"));

    return model;
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
