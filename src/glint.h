// glint.h : main header file for the GLINT application
// Author: Alexander Vechersky, 2010 

#if !defined(AFX_GLINT_H__B22EEF31_DBD5_4762_A3B8_62F2CA074957__INCLUDED_)
#define AFX_GLINT_H__B22EEF31_DBD5_4762_A3B8_62F2CA074957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "AutoPtr.h"
#include "cdefs.h"

class   CPanel;

/////////////////////////////////////////////////////////////////////////////
// CGlintApp:
// See glint.cpp for the implementation of this class
//

class CGlintApp : public CWinApp
{
public:
    BOOL CreatePanel();
    CGlintApp();
    ~CGlintApp();

    const char* ProcessArgKeys(const char* cmdLine);   //  Read keys from command line
    void SetRegistryKey(LPCSTR key) { CWinApp::SetRegistryKey(key); }

public:   // DATA
    AutoPtr<CPanel> m_panel;
    CString m_MyClass;
    bool    m_RestartPanel;
    bool    m_PortableMode;     // key: -r  <-- use Registry instead of INI-file for settings
    
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGlintApp)
    public:
    virtual BOOL InitInstance();
    virtual int Run();
    //}}AFX_VIRTUAL

    void OnHelp();

// Implementation

    //{{AFX_MSG(CGlintApp)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    afx_msg void OnExit();
    virtual void OnFinalRelease();
};


extern CGlintApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLINT_H__B22EEF31_DBD5_4762_A3B8_62F2CA074957__INCLUDED_)
