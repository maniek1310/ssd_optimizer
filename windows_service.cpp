#include "windows_service.h"

windows_service::windows_service()
{

}

bool windows_service::find_service(QString name)
{
    SC_HANDLE hServiceControlManager;
    SC_HANDLE hService;

    bool find = false;

    hServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if(NULL != hServiceControlManager)
    {
        hService = OpenServiceA(hServiceControlManager, name.toStdString().c_str(), SC_MANAGER_ALL_ACCESS);

        if(NULL != hService){
            find = true;
            qDebug() << "Usługa istnieje";

        }else{
            find = false;
            qDebug() << "Błąd : " << GetLastError();
        }

        CloseServiceHandle(hService);
    }

    CloseServiceHandle(hServiceControlManager);

    return find;
}

bool windows_service::start_service(QString name, bool status)
{
    bool bResult = FALSE;

    SC_HANDLE hServiceControlManager;
    SC_HANDLE hService;

    SERVICE_STATUS_PROCESS ssp;
    DWORD bytesNeeded;

    hServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if(NULL != hServiceControlManager)
    {
        hService = OpenServiceA(hServiceControlManager, name.toStdString().c_str(), SC_MANAGER_ALL_ACCESS);

        if(hService != NULL)
        {
            QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded);

            if(ssp.dwCurrentState == SERVICE_RUNNING && status == false){ // Zatrzymanie usługi
                bResult = ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp);

                if(bResult)
                    qDebug() << "Usługa zatrzymana";
                else
                    qDebug() << "Nie można zatrzymać usługi";
            }else if(ssp.dwCurrentState == SERVICE_STOPPED && status == true){ // Uruchomienie usługi
                bResult = StartService(hService, NULL, NULL);

                if(bResult)
                    qDebug() << "Usługa uruchomiona";
                else
                    qDebug() << "Nie można uruchomić usługi";
            }


            CloseServiceHandle(hService);
        }

        CloseServiceHandle(hServiceControlManager);
    }

    return bResult;
}

bool windows_service::enable_service(QString name, bool status)
{
    bool bResult = FALSE;

    SC_HANDLE hServiceControlManager;
    SC_HANDLE hService;

    hServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if(NULL != hServiceControlManager)
    {
        hService = OpenService(hServiceControlManager, (const wchar_t*) name.utf16(), SC_MANAGER_ALL_ACCESS);

        if(hService != NULL)
        {
            bResult = ChangeServiceConfig(hService, SERVICE_WIN32_OWN_PROCESS,
                                          status ? SERVICE_AUTO_START : SERVICE_DISABLED,
                                          SERVICE_NO_CHANGE,
                                          NULL, NULL, NULL, NULL, NULL, NULL, NULL);

            CloseServiceHandle(hService);
        }

        CloseServiceHandle(hServiceControlManager);
    }

    return bResult;
}
