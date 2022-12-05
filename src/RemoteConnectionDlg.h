#if !defined(AFX_REMOTECONNECTIONDLG_H__CCEB8E8F_BFC5_4A27_B3D1_7CBA46ACC6B8__INCLUDED_)
#define AFX_REMOTECONNECTIONDLG_H__CCEB8E8F_BFC5_4A27_B3D1_7CBA46ACC6B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoteConnectionDlg.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// CRemoteConnectionDlg dialog

class CRemoteConnectionDlg : public CDialog
{
// Construction
public:
    CRemoteConnectionDlg(CWnd* pParent, CString m_current_machine);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CRemoteConnectionDlg)
    enum { IDD = IDD_DIALOG_ENTER_REMOTE_COMPUTER };
    CString m_machine;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRemoteConnectionDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    void FillComputerNames();

    // Generated message map functions
    //{{AFX_MSG(CRemoteConnectionDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTECONNECTIONDLG_H__CCEB8E8F_BFC5_4A27_B3D1_7CBA46ACC6B8__INCLUDED_)
