#ifndef WINDOWS_DRIVE_IDENTIFIACTION_H
#define WINDOWS_DRIVE_IDENTIFIACTION_H

#include <QMainWindow>
#include <QDebug>

#include <Windows.h>
#include <ntddscsi.h>
#include <memory>

#pragma comment(lib, "Kernel32.lib")

#ifndef StorageAdapterProperty
#define StorageAdapterProperty 1
#endif

#ifndef StorageDeviceTrimProperty
#define StorageDeviceTrimProperty 8
#endif

typedef enum _INFO_PARTITION_TYPE {
    IPT_BytesPerSector = 0x00,
    IPT_Cylinders,
    IPT_MediaType,
    IPT_SectorsPerTrack,
    IPT_TracksPerCylinder,
    IPT_PartitionLength,
    IPT_PartitionNumber,
    IPT_PartitionStyle,
    IPT_RewritePartition,
    IPT_StartingOffset,
} INFO_PARTITION_TYPE, *PINFO_PARTITION_TYPE;

using namespace std;

class windows_drive_identifiaction : public QObject
{
    Q_OBJECT

private:
    std::wstring lastErrorToString(DWORD err);

public:
    windows_drive_identifiaction();
    QString device_adapter_property(char partitions = 'C');
    bool device_trim_property(char partitions = 'C');
    ULONGLONG device_partition_info(int type, char partitions = 'C', int nr_partitions = 0);
};

#endif // WINDOWS_DRIVE_IDENTIFIACTION_H
