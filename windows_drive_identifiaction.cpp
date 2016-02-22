#include "windows_drive_identifiaction.h"

struct handle_deleter
{
    void operator()(HANDLE h)
    {
        CloseHandle(h);
    }
};

wstring windows_drive_identifiaction::lastErrorToString(DWORD err)
{
    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
    wstring message(messageBuffer, size);
    qWarning() << "wstring message = " << QString::fromWCharArray(message.c_str());

    LocalFree(messageBuffer);

    return message;
}

windows_drive_identifiaction::windows_drive_identifiaction()
{

}

QString windows_drive_identifiaction::device_adapter_property(LPCWSTR disks)
{
    DWORD bytesReturned;
    HANDLE hDevice;
    STORAGE_ADAPTER_DESCRIPTOR sad = {0};

    QString busType;

    hDevice = CreateFile(
                disks,
                0,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

    if(hDevice != INVALID_HANDLE_VALUE)
    {
        STORAGE_PROPERTY_QUERY spqBus;
        spqBus.PropertyId = (STORAGE_PROPERTY_ID)StorageAdapterProperty;
        spqBus.QueryType = PropertyStandardQuery;

        bytesReturned = 0;
        if(::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                             &spqBus, sizeof(spqBus), &sad, sizeof(sad), &bytesReturned, NULL) && bytesReturned == sizeof(sad)){           
            switch(sad.BusType)
            {
            case BusTypeUnknown:
                busType = "Unkown";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeScsi:
                busType = "SCSI";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeAtapi:
                busType = "ATAPI";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeAta:
                busType = "ATA";
                qInfo() << "QString busType = " << busType;
                break;

            case BusType1394:
                busType = "IEEE 1394";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeSsa:
                busType = "SSA";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeFibre:
                busType = "FIBER CHANNEL";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeUsb:
                busType = "USB";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeRAID:
                busType = "RAID";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeiScsi:
                busType = "iSCSI";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeSas:
                busType = "SAS";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeSata:
                busType = "SATA";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeSd:
                busType = "SD";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeMmc:
                busType = "MMC";
                qInfo() << "QString busType = " << busType;
                break;

            case BusTypeVirtual:
                busType = "VIRTUAL";
                qInfo() << "QString busType = " << busType;
                break;
            }
        }
    }else{
        qWarning() << "HANDLE hDevice = " << INVALID_HANDLE_VALUE;
    }

    CloseHandle(hDevice);

    return busType;
}

bool windows_drive_identifiaction::device_trim_property(LPCWSTR disks)
{
    DWORD bytesReturned;
    HANDLE hDevice;

    bool trim = false;

    hDevice = CreateFile(
                disks,
                0,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

    if(hDevice != INVALID_HANDLE_VALUE)
    {
        STORAGE_PROPERTY_QUERY spqTrim;
        spqTrim.PropertyId = (STORAGE_PROPERTY_ID)StorageDeviceTrimProperty;
        spqTrim.QueryType = PropertyStandardQuery;

        bytesReturned = 0;
        DEVICE_TRIM_DESCRIPTOR dtd = {0};
        if(::DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                             &spqTrim, sizeof(spqTrim), &dtd, sizeof(dtd), &bytesReturned, NULL) && bytesReturned == sizeof(dtd)){
            trim = dtd.TrimEnabled;
        }else{
            qDebug() << "TRIM : " << "?" << " ERR : " << ::GetLastError();
        }
    }else{
        qWarning() << "HANDLE hDevice = " << INVALID_HANDLE_VALUE;
    }

    return trim;
}

ULONGLONG windows_drive_identifiaction::device_partition_info(int type, LPCWSTR disks, int nr_partitions)
{
    ULONGLONG info_return;

    HANDLE hDevice = CreateFile(disks, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, 0);
    if(hDevice != INVALID_HANDLE_VALUE)
    {
        unique_ptr<std::remove_pointer<HANDLE>::type, handle_deleter> uhDevice(hDevice);
        DWORD bytesRet;
        BOOL result;

        DISK_GEOMETRY_EX dgx;
        result = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &dgx, sizeof(dgx), &bytesRet, NULL);

        if(result)
        {
            if(type == IPT_BytesPerSector){
                info_return = dgx.Geometry.BytesPerSector;
                qInfo() << "DISK_GEOMETRY_EX dgx.Geometry.BytesPerSector = " << dgx.Geometry.BytesPerSector;
            }else if(type == IPT_Cylinders){
                info_return = dgx.Geometry.Cylinders.QuadPart;
                qInfo() << "DISK_GEOMETRY_EX dgx.Geometry.Cylinders.QuadPart = " << dgx.Geometry.Cylinders.QuadPart;
            }else if(type == IPT_MediaType){
                info_return = dgx.Geometry.MediaType;
                qInfo() << "DISK_GEOMETRY_EX dgx.Geometry.MediaType = " << dgx.Geometry.MediaType;
            }else if(type == IPT_SectorsPerTrack){
                info_return = dgx.Geometry.SectorsPerTrack;
                qInfo() << "DISK_GEOMETRY_EX dgx.Geometry.SectorsPerTrack = " << dgx.Geometry.SectorsPerTrack;
            }else if(type == IPT_TracksPerCylinder){
                info_return = dgx.Geometry.TracksPerCylinder;
                qInfo() << "DISK_GEOMETRY_EX dgx.Geometry.TracksPerCylinder = " << dgx.Geometry.TracksPerCylinder;
            }
        }else{
            DWORD err = GetLastError();
            qDebug() << "Error : " << err << ", " << lastErrorToString(err).c_str();
        }

        unique_ptr<unsigned char[]> buffer(new unsigned char[1024]);
        DRIVE_LAYOUT_INFORMATION_EX* dlix = (DRIVE_LAYOUT_INFORMATION_EX*) buffer.get();

        result = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, dlix, 1024, &bytesRet, NULL);

        if(result)
        {
            if(type == IPT_PartitionLength){
                info_return = dlix->PartitionEntry[nr_partitions].PartitionLength.QuadPart;
                qInfo() << "DRIVE_LAYOUT_INFORMATION_EX dlix->PartitionEntry[nr_partitions].PartitionLength.QuadPart = " << dlix->PartitionEntry[nr_partitions].PartitionLength.QuadPart;
            }else if(type == IPT_PartitionNumber){
                info_return = dlix->PartitionEntry[nr_partitions].PartitionNumber;
                qInfo() << "DRIVE_LAYOUT_INFORMATION_EX dlix->PartitionEntry[nr_partitions].PartitionNumber = " << dlix->PartitionEntry[nr_partitions].PartitionNumber;
            }else if(type == IPT_PartitionStyle){
                info_return = dlix->PartitionEntry[nr_partitions].PartitionStyle;
                qInfo() << "DRIVE_LAYOUT_INFORMATION_EX dlix->PartitionEntry[nr_partitions].PartitionStyle = " << dlix->PartitionEntry[nr_partitions].PartitionStyle;
            }else if(type == IPT_RewritePartition){
                info_return = dlix->PartitionEntry[nr_partitions].RewritePartition;
                qInfo() << "DRIVE_LAYOUT_INFORMATION_EX dlix->PartitionEntry[nr_partitions].RewritePartition = " << dlix->PartitionEntry[nr_partitions].RewritePartition;
            }else if(type == IPT_StartingOffset){
                info_return = dlix->PartitionEntry[nr_partitions].StartingOffset.QuadPart;
                qInfo() << "DRIVE_LAYOUT_INFORMATION_EX dlix->PartitionEntry[nr_partitions].StartingOffset.QuadPart = " << dlix->PartitionEntry[nr_partitions].StartingOffset.QuadPart;
            }
        }else{
            DWORD err = GetLastError();
            qDebug() << "Error : " << err << ", " << lastErrorToString(err).c_str() << " dysk : " << QString::fromWCharArray(disks);
        }
    }else{
        qWarning() << "HANDLE hDevice = " << INVALID_HANDLE_VALUE;
    }

    return info_return;
}

int windows_drive_identifiaction::get_index_disk_for_partition(LPCWSTR partition)
{
    HANDLE hHandle = CreateFileW(partition,
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_WRITE | FILE_SHARE_READ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    if(hHandle == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Open failed : " << partition;

        return -1;
    }

    VOLUME_DISK_EXTENTS volumeDiskExtents;
    DWORD bytes_ret = 0;
    BOOL bResult = DeviceIoControl(hHandle,
                                   IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
                                   NULL,
                                   0,
                                   &volumeDiskExtents,
                                   sizeof(volumeDiskExtents),
                                   &bytes_ret,
                                   NULL);

    CloseHandle(hHandle);

    if(!bResult)
    {
        qDebug() << "Failed to issue the control code !";

        return -1;
    }

    int index_disk = -1;

    for(DWORD n = 0; n < volumeDiskExtents.NumberOfDiskExtents; ++n)
    {
        PDISK_EXTENT pDiskExtent = &volumeDiskExtents.Extents[n];

        index_disk = pDiskExtent->DiskNumber;

        qInfo() << "int index_disk = " << index_disk;
    }

    return index_disk;
}
