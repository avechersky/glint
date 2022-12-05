#if !defined(IndicatorTip_h_INCLUDED_)
#define IndicatorTip_h_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Indicator.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// CIndicatorToolTip window

#define INDIC_TIP_SIZE 300

class CIndicatorToolTip : public CToolTipCtrl
{
public:
    CIndicatorToolTip();
    virtual ~CIndicatorToolTip();
    void    Init(CWnd* parent, UINT id);

// DATA
    char        m_TipText[INDIC_TIP_SIZE+2];
    TOOLINFO    m_ToolInfo;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CIndicatorToolTip)
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CIndicatorToolTip)
    // NOTE - the ClassWizard will add and remove member functions here.
    afx_msg void OnTtnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(IndicatorTip_h_INCLUDED_)
