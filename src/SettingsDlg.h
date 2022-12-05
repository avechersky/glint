#if !defined(AFX_SETTINGSDLG_H__9DAD4FAD_B2B5_4E7B_AC87_584F46147829__INCLUDED_)
#define AFX_SETTINGSDLG_H__9DAD4FAD_B2B5_4E7B_AC87_584F46147829__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDlg.h : header file
// Author: Alexander Vechersky, 2010 

#include "PropPageGeneral.h"
#include "PropPageCounters.h"
#include "PropPageIndicators.h"
#include "AutoPtr.h"

#define  CHECK_STATE_UNDEFINED  2 

class   CPerfDataBase;
extern  AutoPtr<CPerfDataBase>  g_DlgPfDataDB;

extern  bool  g_SchemaWasModified;
extern  bool  g_LastLadedSchemaChanged;     // true: when schema was changhed in current dialog since last load/save

//////////////////////////////////////////////////////////////////////////////
int   LastSelIndicsCount();
void  ResetDlgPerfDatabase();               // Re-Initialize counters database for new machine.


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg

class CSettingsDlg : public CPropertySheet
{
    DECLARE_DYNAMIC(CSettingsDlg)

// Construction
public:
    CSettingsDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CSettingsDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    
    bool    IsModified();           // is anything was mofified
    bool    IsSchemaModified();     // is schema was mofified
    bool    WasAnythingModified();  // was anything mofified

// Attributes
public:

// Operations
public:

    CPropPageGeneral        m_PageGeneral;
    CPropPageCounters       m_PageCounters;
    CPropPageIndicators     m_PageIndicators;


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSettingsDlg)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CSettingsDlg();

    // Generated message map functions
protected:
    //{{AFX_MSG(CSettingsDlg)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__9DAD4FAD_B2B5_4E7B_AC87_584F46147829__INCLUDED_)
