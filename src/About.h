#if !defined(AFX_ABOUT_H__40F23F83_3C6B_4F5B_8D64_6991C3B29D9F__INCLUDED_)
#define AFX_ABOUT_H__40F23F83_3C6B_4F5B_8D64_6991C3B29D9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// About.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

class CAbout : public CDialog
{
// Construction
public:
    CAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CAbout)
    enum { IDD = IDD_ABOUT_DIALOG1 };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAbout)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CAbout)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnStnClickedTextAuthor();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUT_H__40F23F83_3C6B_4F5B_8D64_6991C3B29D9F__INCLUDED_)
