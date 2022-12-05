#if !defined(AFX_HELPDLG_H__9A386DF8_5809_475A_93DC_58BF9EB3989F__INCLUDED_)
#define AFX_HELPDLG_H__9A386DF8_5809_475A_93DC_58BF9EB3989F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpDlg.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg dialog

class CHelpDlg : public CDialog
{
// Construction
public:
    CHelpDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CHelpDlg)
    enum { IDD = IDD_HELP_DLG };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CHelpDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CHelpDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPDLG_H__9A386DF8_5809_475A_93DC_58BF9EB3989F__INCLUDED_)
