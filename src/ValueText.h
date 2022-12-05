#if !defined(AFX_VALUETEXT_H__236F7F55_2D0B_4E7C_A2B8_59F91A2B7573__INCLUDED_)
#define AFX_VALUETEXT_H__236F7F55_2D0B_4E7C_A2B8_59F91A2B7573__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ValueText.h : header file
// Author: Alexander Vechersky, 2010 

class CIndicator;

/////////////////////////////////////////////////////////////////////////////
// ValueText window

class ValueText : public CStatic
{
// Construction
public:

    ValueText(CIndicator* ind);
    void InitValueText(CWnd* parent, CRect r, LPCSTR txt, UINT id);

// Attributes
public:

protected:
    CString     m_Text;
    CIndicator* m_Indicator;
    CRect       m_ClientRect;
    CRect       m_WindowRect;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ValueText)
    //}}AFX_VIRTUAL

// Implementation
public:
    void Update();
    virtual ~ValueText();
    CRect&  Rect() { return m_WindowRect; }

    // Generated message map functions
protected:
    //{{AFX_MSG(ValueText)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALUETEXT_H__236F7F55_2D0B_4E7C_A2B8_59F91A2B7573__INCLUDED_)
