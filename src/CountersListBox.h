#if !defined(AFX_COUNTERSLISTBOX_H__7A621C04_DA8B_4933_A1E6_9972B150A598__INCLUDED_)
#define AFX_COUNTERSLISTBOX_H__7A621C04_DA8B_4933_A1E6_9972B150A598__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CountersListBox.h : header file
// Author: Alexander Vechersky, 2010 

class CIndicatorSettings;

/////////////////////////////////////////////////////////////////////////////
// CCountersListBox window

class CCountersListBox : public CListBox
{
// Construction
public:
    CCountersListBox();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCountersListBox)
    public:
    virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCountersListBox();
    bool    CheckForSameInstance(CIndicatorSettings* pindsets);

    // Generated message map functions
protected:
    //{{AFX_MSG(CCountersListBox)
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COUNTERSLISTBOX_H__7A621C04_DA8B_4933_A1E6_9972B150A598__INCLUDED_)
