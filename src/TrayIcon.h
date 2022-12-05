// TrayIcon.h: interface for the CTrayIcon class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAYICON_H__DB0D391C_7BCE_4629_A31A_EB010DC34A2A__INCLUDED_)
#define AFX_TRAYICON_H__DB0D391C_7BCE_4629_A31A_EB010DC34A2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define ICON_TRAY_ID        0x002501
#define ICON_TRAY_MSG_ID    0x002502
#define ICON_TIP_SIZE       100

///////////////////////////////////////////////////////////////////
#undef  WIN32_IE
//#define   WIN32_IE  0x0500

typedef struct _NOTIFYICONDATA1 {
        DWORD cbSize;
        HWND hWnd;
        UINT uID;
        UINT uFlags;
        UINT uCallbackMessage;
        HICON hIcon;
    #if (WIN32_IE < 0x0500)
        CHAR   szTip[64];
    #else
        CHAR   szTip[128];
    #endif

    #if (WIN32_IE >= 0x0500)
        DWORD dwState;
        DWORD dwStateMask;
        CHAR   szInfo[256];
        union {
            UINT  uTimeout;
            UINT  uVersion;
        } DUMMYUNIONNAME;
        CHAR   szInfoTitle[64];
        DWORD dwInfoFlags;
    #endif

    #if (WIN32_IE >= 0x600)
        GUID guidItem;
    #endif
} NOTIFYICONDATA1, *PNOTIFYICONDATA1;
 

WINSHELLAPI BOOL WINAPI Shell_NotifyIconA1(DWORD dwMessage, PNOTIFYICONDATA1 lpData);

// for (_WIN32_IE >= 0x0500)
#define NIM_ADD         0x00000000
#define NIM_MODIFY      0x00000001
#define NIM_DELETE      0x00000002
#define NIM_SETFOCUS    0x00000003
#define NIM_SETVERSION  0x00000004
#define NOTIFYICON_VERSION 3        


#define NIF_MESSAGE     0x00000001
#define NIF_ICON        0x00000002
#define NIF_TIP         0x00000004
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010
#define NIF_GUID        0x00000020

#define NIS_HIDDEN      0x00000001
#define NIS_SHAREDICON  0x00000002    // says this is the source of a shared icon

// Notify Icon Infotip flags
#define NIIF_NONE       0x00000000 

#define NIIF_INFO       0x00000001
#define NIIF_WARNING    0x00000002
#define NIIF_ERROR      0x00000003
#define NIIF_ICON_MASK  0x0000000F
#define NIIF_NOSOUND    0x00000010    // (_WIN32_IE >= 0x0600)


///////////////////////////////////////////////////////////////////
class CTrayIcon  
{
public:
    void SetVersion(UINT version);
    CTrayIcon();
    virtual ~CTrayIcon();

    void AddIcon(HWND hwnd, UINT uID, HICON hicon); 
    void DeleteIcon();

protected:
    // DATA
    NOTIFYICONDATA  m_tnid; 

    #ifdef  WIN32_IE 
    NOTIFYICONDATA1 m_tnid1; 
    #endif

    HICON   m_hicon;
    UINT    m_uID;
    HWND    m_hwndMain;
    char    m_TipText[ICON_TIP_SIZE + 2];
    
};

#endif // !defined(AFX_TRAYICON_H__DB0D391C_7BCE_4629_A31A_EB010DC34A2A__INCLUDED_)
