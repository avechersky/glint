// TrayIcon.cpp: implementation of the CTrayIcon class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "TrayIcon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DEFAULT_ICON_TIP    "Glint Computer Activity Monitor"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrayIcon::CTrayIcon()
{
    m_hicon     = 0;
    m_uID       = 0;
    m_hwndMain  = 0;
    m_TipText[0] = 0;
    // memcpy(m_TipText, DEFAULT_ICON_TIP, strlen(DEFAULT_ICON_TIP) + 1);   // ICON_TIP_SIZE
    if (sets.m_ComputerName.GetLength() > 0 )
        sprintf(m_TipText, "System activity for computer: %s", (LPCTSTR) GetPrintableCompName(sets.m_ComputerName) );
    else
        strcpy(m_TipText, "System activity for local machine");
}

CTrayIcon::~CTrayIcon()
{
    DeleteIcon();
}

//////////////////////////////////////////////////////////////////////////////
// AddIcon - adds an icon to the taskbar status area. 
//  hwnd - handle to the window to receive callback messages. 
//  uID - identifier of the icon. 
void CTrayIcon::AddIcon(HWND hwnd, UINT uID, HICON hicon) 
{ 

    m_hicon     = hicon;
    m_uID       = uID;
    m_hwndMain  = hwnd;
    
    m_tnid.cbSize = sizeof(NOTIFYICONDATA); 
    m_tnid.hWnd     = m_hwndMain; 
    m_tnid.uID      = m_uID; 
    m_tnid.uCallbackMessage = ICON_TRAY_MSG_ID;  // actually callback Id 

    m_tnid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    m_tnid.hIcon = m_hicon; 

    memcpy(m_tnid.szTip, m_TipText, sizeof(m_tnid.szTip)); 
    m_tnid.szTip[ sizeof(m_tnid.szTip) - 1 ] = 0;

    BOOL res = Shell_NotifyIcon(NIM_ADD, &m_tnid); 
    ASSERT(res); 

    // SetVersion(0);
 } 

/////////////////////////////////////////////////////////////////
void CTrayIcon::DeleteIcon()
{
    m_tnid.cbSize = sizeof(NOTIFYICONDATA); 
    m_tnid.hWnd = m_hwndMain; 
    m_tnid.uID =    m_uID; 
    m_tnid.uFlags = NIF_ICON | NIF_TIP;     // NIF_MESSAGE
    m_tnid.uCallbackMessage = ICON_TRAY_MSG_ID;  // actually callback Id 
    m_tnid.hIcon = m_hicon; 
 
    BOOL res = Shell_NotifyIcon(NIM_DELETE, &m_tnid); 
    ASSERT(res); 
}

/////////////////////////////////////////////////////////////////
void CTrayIcon::SetVersion(UINT version)
{
#ifdef  WIN32_IE 
    m_tnid1.cbSize = sizeof(NOTIFYICONDATA1); 
    m_tnid1.hWnd        = m_hwndMain; 
    m_tnid1.uID     = m_uID; 
    m_tnid1.uCallbackMessage = ICON_TRAY_MSG_ID;  // actually callback Id 

    m_tnid1.uFlags = 0;
    m_tnid1.hIcon = 0; 
    m_tnid1.uVersion = 0;   // old behavior
    m_tnid1.dwState     = 0;
    m_tnid1.dwStateMask  = 0;
    m_tnid1.uVersion = 0;   // old behavior
    m_tnid1.dwState     = 0;
    m_tnid1.dwStateMask  = 0;

    BOOL res = Shell_NotifyIcon(NIM_SETVERSION, (PNOTIFYICONDATA)&m_tnid1); 
    ASSERT(res); 
#endif
}
