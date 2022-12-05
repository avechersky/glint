#if !defined(AFX_SCHEMASAVE_H__AA6A02E9_B8C1_411A_8B66_CFF78E742575__INCLUDED_)
#define AFX_SCHEMASAVE_H__AA6A02E9_B8C1_411A_8B66_CFF78E742575__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SchemaSave.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// CSchemaSave dialog

class CSchemaSave : public CDialog
{
// Construction
public:
    CSchemaSave(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CSchemaSave)
    enum { IDD = IDD_DIALOG_SAVE_SCHEMA };
    CString m_SchemaName;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSchemaSave)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CSchemaSave)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEMASAVE_H__AA6A02E9_B8C1_411A_8B66_CFF78E742575__INCLUDED_)
