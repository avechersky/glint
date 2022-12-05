#if !defined(AFX_PROCESSWINDOW_H__66925B45_FA53_4962_BD5D_6404870CD4F8__INCLUDED_)
#define AFX_PROCESSWINDOW_H__66925B45_FA53_4962_BD5D_6404870CD4F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessWindow.h : header file
// Author: Alexander Vechersky, 2010 

#include "IndicatorToolTip.h"

class CGadgets;

/////////////////////////////////////////////////////////////////////////////
// CProcessWindow window

class CProcessWindow : public CStatic
{
// Construction
public:
    CProcessWindow();

// Attributes
protected:

    CRect       m_ClientRect;
    int         m_WindowsXSize;
    CRect       m_WindowRect;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProcessWindow)
    //}}AFX_VIRTUAL

// Implementation
public:
    void InitProcWin(CWnd* parent, CRect r, CGadgets* gadgets);
    virtual ~CProcessWindow();
    CRect&   Rect() { return m_WindowRect; }
    
    void            UpdateToolTip(); 

// DATA
    CWnd *          m_parent;
    bool            m_MouseHover;
    CIndicatorToolTip   m_ToolTip;

    // Generated message map functions
protected:
    //{{AFX_MSG(CProcessWindow)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSWINDOW_H__66925B45_FA53_4962_BD5D_6404870CD4F8__INCLUDED_)
