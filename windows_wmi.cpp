#include "windows_wmi.h"

HRESULT hres;
IWbemLocator *pLoc = NULL;
IWbemServices *pSvc = NULL;

void windows_wmi::polacz_wmi()
{
    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x"
            << hex << hres << endl;
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres =  CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities
        NULL                         // Reserved
        );


    if (hres != S_OK && hres != RPC_E_TOO_LATE)
    {
        cout << "Failed to initialize security. Error code = 0x"
            << hex << hres << endl;
        CoUninitialize();
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------



    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres << endl;
        CoUninitialize();
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method



    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object
         &pSvc                    // pointer to IWbemServices proxy
         );

    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x"
             << hex << hres << endl;
        pLoc->Release();
        CoUninitialize();
    }

    //cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities
    );

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x"
            << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
    }
}

void windows_wmi::rozlacz_wmi()
{
    pSvc->Release();
    pLoc->Release();

    CoUninitialize();
}

QString windows_wmi::sprawdz(string s_class, const wstring &s_name)
{
    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system

    std::ostringstream win32_class (std::ostringstream::ate);

    win32_class.str("");
    win32_class << "SELECT * FROM ";
    win32_class << s_class;

    IEnumWbemClassObject* pEnumerator = NULL;

    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(win32_class.str().c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query for operating system name failed." << " Error code = 0x" << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    QString returns;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(0 == uReturn || FAILED(hr))
            break;

        VARIANT vtProp;

        hr = pclsObj->Get(s_name.c_str(), 0, &vtProp, 0, 0);

        if(!FAILED(hr))
        {
            if((vtProp.vt==VT_NULL) || (vtProp.vt==VT_EMPTY)){                          // empty or null
                wcout << " " << ((vtProp.vt==VT_NULL) ? "NULL" : "EMPTY") << endl;
            }else if(vtProp.vt == VT_BSTR){                                             // string
                returns = QString::fromStdWString(vtProp.bstrVal);
            }else if(vtProp.vt == VT_I4){                                               // 4-byte signed integer
                returns = QString::number(vtProp.uintVal);
            }else if(vtProp.vt == VT_BOOL){                                             // bool
                returns = QString::number(vtProp.boolVal);
                if(returns == "-1" || returns == "1")
                    returns = "True";
                else
                    returns = "False";
            }
        }
        //qDebug() << returns << "    " << vtProp.vt << endl;
        VariantClear(&vtProp);

        return returns;
    }

    //QString system_name, system_architecture, system_version, system_service_pack, system_name_computer,
    //        system_work_group, system_organization, system_instalation, system_serial_number, system_language;

    // Cleanup
    // ========

    pclsObj->Release();
    pEnumerator->Release();
    if(!pclsObj) pclsObj->Release();

    return returns;
}

QStringList windows_wmi::sprawdz_array(string s_class, const wstring &s_name, string s_set)
{
    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system

    std::ostringstream win32_class (std::ostringstream::ate);

    win32_class.str("");
    win32_class << "SELECT * FROM ";
    win32_class << s_class;

    if(s_set != "NULL")
    {
        win32_class << " WHERE ";
        win32_class << s_set;
    }

    IEnumWbemClassObject* pEnumerator = NULL;

    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(win32_class.str().c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        //qDebug() << ("Query for operating system name failed. Error code = 0x" + hres);
        cout << "Query for operating system name failed." << " Error code = 0x" << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();

        return QStringList("FAILED hres");
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    QStringList returns_list;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(0 == uReturn || FAILED(hr))
            break;

        VARIANT vtProp;

        hr = pclsObj->Get(s_name.c_str(), 0, &vtProp, 0, 0);

        if(!FAILED(hr))
        {
            if((vtProp.vt==VT_NULL) || (vtProp.vt==VT_EMPTY)){                          // empty or null
                returns_list << "";
            }else if(vtProp.vt == VT_BSTR){                                             // string
                returns_list << QString::fromStdWString(vtProp.bstrVal);
            }else if(vtProp.vt == VT_I4){                                               // 4-byte signed integer
               returns_list << QString::number(vtProp.uintVal);
            }else if(vtProp.vt == VT_BOOL){                                             // bool
                if(QString::number(vtProp.boolVal) == "-1" || QString::number(vtProp.boolVal) == "1")
                    returns_list << "True";
                else
                    returns_list << "False";
            }else if(vtProp.vt & VT_ARRAY){                                             // tablica string
                SAFEARRAY *psa = vtProp.parray;

                if(psa)
                {
                    LONG uBound = -1, lBound = 0;
                    SafeArrayGetUBound(psa, 1, &uBound);
                    SafeArrayGetLBound(psa, 1, &lBound);

                    //int nCount = uBound - lBound + 1;                                 // do pętli for

                    std::wstring ws1(((BSTR*)(psa->pvData))[0], SysStringLen(((BSTR*)(psa->pvData))[0]));
                    QString tessa1 = QString::fromStdWString(ws1);
                    QString asdd = QString::fromStdWString(s_name);

                    if(asdd == "DefaultIPGateway"){
                        returns_list << tessa1;
                    }else{
                        std::wstring ws2(((BSTR*)(psa->pvData))[1], SysStringLen(((BSTR*)(psa->pvData))[1]));
                        QString tessa2 = QString::fromStdWString(ws2);

                       returns_list << tessa1 + " / " + tessa2;
                    }
                }
            }
        }

        //qDebug() << returns[i] << "    " << vtProp.vt << "     fadsdf     " << i << endl;
        VariantClear(&vtProp);
    }

    //QString system_name, system_architecture, system_version, system_service_pack, system_name_computer,
    //        system_work_group, system_organization, system_instalation, system_serial_number, system_language;

    // Cleanup
    // ========

    pclsObj->Release();
    pEnumerator->Release();
    if(!pclsObj) pclsObj->Release();

    return returns_list;
}
