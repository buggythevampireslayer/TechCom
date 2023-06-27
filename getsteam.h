#include <Windows.h>
#include <tchar.h>
#include "files.h"

struct libraryfolder {
    std::string path;
    std::vector<int> apps;
};

std::wstring get_installpath()
{
    HKEY m_key;
    DWORD dw_Return;
    dw_Return = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\WOW6432Node\\Valve\\Steam"),
        NULL,
        KEY_QUERY_VALUE,
        &m_key);

    if (dw_Return != ERROR_SUCCESS)
    {
        dw_Return = RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            _T("SOFTWARE\\Valve\\Steam"),
            NULL,
            KEY_QUERY_VALUE,
            &m_key);

        if (dw_Return != ERROR_SUCCESS)
            throw "Steam install path not found.";
    }

    const DWORD SIZE = 1024;
    TCHAR sz_Value[SIZE] = _T("");
    DWORD dw_Value = SIZE;
    DWORD dw_Type = 0;
    dw_Return = RegQueryValueEx(
        m_key,
        _T("InstallPath"),
        NULL,
        &dw_Type,
        (LPBYTE)&sz_Value,
        &dw_Value);
    if (dw_Return != ERROR_SUCCESS)
    {
       std::clog << "Something fucked up in RegQueryValueEx" << dw_Return << GetLastError() << "\n";
       throw "Steam path couldn't be located.";
    }
    if (dw_Type != REG_SZ)
    {
        std::clog << "Value isn't a string.\n";
        throw "Steam path couldn't be located.";
    }
    return (std::wstring)sz_Value;
}

std::string get_tf2path(std::wstring steaminstallpath)
{
    std::string path( steaminstallpath.begin(), steaminstallpath.end() );
    path += "\\steamapps\\libraryfolders.vdf";
    std::ifstream m_lfstream(path);
    if (!m_lfstream.is_open())
        m_lfstream.open(path);

    std::vector<libraryfolder> v_libfolders;
    std::string item;
    while (m_lfstream >> item)
    {
        if (item[1] >= '0' || item[1] <= '9'){
            m_lfstream >> item >> item >> item;
            libraryfolder lf;
            lf.path = item;
            while (m_lfstream >> item){
                if (item == "\"apps\""){
                    while (item != "}"){
                        m_lfstream >> item;
                        lf.apps.push_back(stoi(item.substr(1, item.length() - 2)));
                    }
                    break;
                }
                if (item == "}")
                    break;
            }
            v_libfolders.push_back(lf);
        }
    }
    for (libraryfolder lf : v_libfolders)
    {
        for (int app : lf.apps){
            if (app == 440){
                return lf.path;
            }
        }
    }
}