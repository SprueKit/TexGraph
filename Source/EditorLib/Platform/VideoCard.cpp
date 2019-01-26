#include "VideoCard.h"

#ifdef WIN32
#include "comutil.h"
#include "Wbemcli.h"
#include "Wbemidl.h"
#endif



VideoCard::VideoCard()
{
#ifdef WIN32
    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    IWbemLocator *pIWbemLocator = NULL;
    hr = CoCreateInstance(__uuidof(WbemLocator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (LPVOID *)&pIWbemLocator);

    BSTR bstrServer = SysAllocString(L"\\\\.\\root\\cimv2");
    IWbemServices *pIWbemServices;
    hr = pIWbemLocator->ConnectServer(bstrServer, NULL, NULL, 0L, 0L, NULL, NULL, &pIWbemServices);

    hr = CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_DEFAULT);

    BSTR bstrWQL = SysAllocString(L"WQL");
    BSTR bstrPath = SysAllocString(L"select * from Win32_VideoController");
    IEnumWbemClassObject* pEnum;
    hr = pIWbemServices->ExecQuery(bstrWQL, bstrPath, WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);

    IWbemClassObject* pObj = NULL;
    ULONG uReturned;
    VARIANT var;
    hr = pEnum->Next(WBEM_INFINITE, 1, &pObj, &uReturned);

    if (uReturned)
    {
        hr = pObj->Get(L"AdapterRAM", 0, &var, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            char str[MAX_PATH];
            sprintf(str, "Video Memory %ld", (var.lVal / (1024 * 1024)));

            videoMemory = str;
        }

        hr = pObj->Get(L"ConfigManagerErrorCode", 0, &var, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            char str[MAX_PATH];
            sprintf(str, "ConfigManagerErrorCode %ld", var.lVal);
            switch (var.lVal)
            {
            case 0:
                errorStatus = "This device is working properly";
                break;
            case 1:
                errorStatus = "This device is not configured correctly";
                break;
            case 2:
                errorStatus = "Windows cannot load the driver for this device.";
                break;
            case 3:
                errorStatus = "The driver for this device might be corrupted, or your system may be running low on memory or other resources.";
                break;
            case 4:
                errorStatus = "This device is not working properly. One of its drivers or your registry might be corrupted.";
                break;
            case 5:
                errorStatus = "The driver for this device needs a resource that Windows cannot manage.";
                break;
            case 6:
                errorStatus = "The boot configuration for this device conflicts with other devices.";
                break;
            case 7:
                errorStatus = "Cannot filter.";
                break;
            case 8:
                errorStatus = "The driver loader for the device is missing";
                break;
            case 11:
                errorStatus = "This device failed.";
                break;
            case 12:
                errorStatus = "This device cannot find enough free resources that it can use.";
                break;
            case 14:
                errorStatus = "This device cannot work properly until you restart your computer.";
                break;
            case 18:
                errorStatus = "Reinstall the drivers for this device.";
                break;
            case 20:
                errorStatus = "Your registry might be corrupted.";
                break;
            }
        }

        hr = pObj->Get(L"Name", 0, &var, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            char sString[256];
            WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sString, sizeof(sString), NULL, NULL);
            cardName = sString;
        }

        hr = pObj->Get(L"DeviceID", 0, &var, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            char sString[256];
            WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sString, sizeof(sString), NULL, NULL);
            deviceID = sString;
        }

        hr = pObj->Get(L"DriverVersion", 0, &var, NULL, NULL);
        if (SUCCEEDED(hr))
        {
            char sString[256];
            WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sString, sizeof(sString), NULL, NULL);
            deviceVersion = sString;
        }
    }

    pEnum->Release();
    SysFreeString(bstrPath);
    SysFreeString(bstrWQL);
    pIWbemServices->Release();
    SysFreeString(bstrServer);
    CoUninitialize();
#endif
}