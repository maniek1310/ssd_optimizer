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

#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS \
    CTL_CODE(IOCTL_VOLUME_BASE, 0, \
    METHOD_BUFFERED, FILE_ANY_ACCESS)

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
    QString device_adapter_property(LPCWSTR disks = L"\\\\.\\PhysicalDrive0");
    bool device_trim_property(LPCWSTR disks = L"\\\\.\\PhysicalDrive0");
    ULONGLONG device_partition_info(int type, LPCWSTR disks = L"\\\\.\\PhysicalDrive0", int nr_partitions = 0);
    int get_index_disk_for_partition(LPCWSTR partition = L"\\\\.\\C:");
};

#endif // WINDOWS_DRIVE_IDENTIFIACTION_H
