#if !defined(AFX_VALUEMAXTEXT_H__EB339705_49E7_48CC_82A1_7DBEA4E6FEAB__INCLUDED_)
#define AFX_VALUEMAXTEXT_H__EB339705_49E7_48CC_82A1_7DBEA4E6FEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ValueMaxText.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// ValueMaxText window

class ValueMaxText : public CStatic
{
// Construction
public:
    ValueMaxText(CIndicator* ind);
    void InitValueMaxText(CWnd* parent, CRect r, LPCSTR txt, UINT id);

// Attributes
public:

protected:
    CIndicator* m_Indicator;
    CRect       m_ClientRect;
    CRect       m_WindowRect;

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ValueMaxText)
    //}}AFX_VIRTUAL

// Implementation
public:
    void Update();
    virtual ~ValueMaxText();

    CRect&  Rect() { return m_WindowRect; }

    // Generated message map functions
protected:
    //{{AFX_MSG(ValueMaxText)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALUEMAXTEXT_H__EB339705_49E7_48CC_82A1_7DBEA4E6FEAB__INCLUDED_)
