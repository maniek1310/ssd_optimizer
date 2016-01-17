#include "windows_wmi.h"

windows_wmi::windows_wmi()
{

}

QString windows_wmi::sprawdz(string s_class, const wstring &s_name, string s_type)
{
    HRESULT hres;

    hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if(FAILED(hres))
        qDebug() << "Failed to initialize COM library. Error code = 0x" << hex << hres;

    hres = CoInitializeSecurity(
                NULL,
                -1,
                NULL,
                NULL,
                RPC_C_AUTHN_LEVEL_DEFAULT,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL,
                EOAC_NONE,
                NULL
                );

    if(FAILED(hres))
    {
        qDebug() << "Failed to initialize security. Error code = 0x" << hex << hres;
        CoUninitialize();
    }

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator, (LPVOID *) &pLoc);

    if(FAILED(hres))
    {
        qDebug() << "Failed to create IWbemLocator object. Error code = 0x" << hex << hres;
        CoUninitialize();
    }

    IWbemServices *pSvc = NULL;

    hres = pLoc->ConnectServer(
                _bstr_t(L"ROOT\\CIMV2"),
                NULL,
                NULL,
                0,
                NULL,
                0,
                0,
                &pSvc
                );

    if(FAILED(hres))
    {
        qDebug() << "Could not connect. Error code = 0x" << hex << hres;
        pLoc->Release();
        CoUninitialize();
    }

    hres = CoSetProxyBlanket(
                pSvc,
                RPC_C_AUTHN_WINNT,
                RPC_C_AUTHN_NONE,
                NULL,
                RPC_C_AUTHN_LEVEL_CALL,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL,
                EOAC_NONE
                );

    if(FAILED(hres))
    {
        qDebug() << "Could not set proxy blanket. Error code = 0x" << hex << hres;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
    }

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

    if(FAILED(hres))
    {
        qDebug() << "Query for operating system name failed. Error code = 0x" << hex << hres;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
    }

    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;

    while(pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(uReturn == 0)    break;

        QString qtString = QString::fromWCharArray(s_name.c_str());

        VARIANT vtProp;

        if(s_type == "string")
        {
            hr = pclsObj->Get(s_name.c_str(), 0, &vtProp, 0, 0);

            if(!FAILED(hr))
            {
                if((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    qDebug() << QString::fromWCharArray(s_name.c_str()) << ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY");
                else if((vtProp.vt & VT_ARRAY))
                    qDebug() << QString::fromWCharArray(s_name.c_str()) << " Array types not supported (yet)";
                else
                    qDebug() << vtProp.bstrVal;
            }

            QString name((QChar*) vtProp.bstrVal, wcslen(vtProp.bstrVal));

            return name;
        }else if(s_type == "uint")
        {
            QString name;

            hr = pclsObj->Get(s_name.c_str(), 0, &vtProp, 0, 0);

            if(!FAILED(hr))
            {
                if((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
                    name = ((vtProp.vt == VT_NULL) ? "NULL" : "EMPTY");
                else if((vtProp.vt & VT_ARRAY))
                    name = "Array types not supported (yet)";
                else
                    name = QString::number(vtProp.uintVal);
            }

            return name;
        }

        VariantClear(&vtProp);

        pclsObj->Release();
    }

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();

    if(!pclsObj)    pclsObj->Release();

    CoUninitialize();

    return "błąd";
}
