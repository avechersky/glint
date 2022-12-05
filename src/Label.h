#if !defined(AFX_LABEL_H__AFC079CD_B38B_4F1C_9CBC_54F3B92B14AF__INCLUDED_)
#define AFX_LABEL_H__AFC079CD_B38B_4F1C_9CBC_54F3B92B14AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Label.h : header file
// Author: Alexander Vechersky, 2010 

class CIndicator;

/////////////////////////////////////////////////////////////////////////////
// Label window

class Label : public CStatic
{
// Construction
public:
    Label(CIndicator* ind);

// Attributes
public:

protected:
    CString     m_Text;
    int  m_TextLen;
    CIndicator* m_Indicator;
    //RECT m_ClientRect;
    CRect  m_ClientRect;
    CRect  m_WindowRect;


// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(Label)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~Label();
    void    InitLabel(CWnd* parent, CRect r, LPCSTR txt, UINT id );
    CRect&   Rect() { return m_WindowRect; }

    // Generated message map functions
protected:
    //{{AFX_MSG(Label)
    afx_msg void OnPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__AFC079CD_B38B_4F1C_9CBC_54F3B92B14AF__INCLUDED_)
