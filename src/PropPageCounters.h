#if !defined(AFX_PROPPAGECOUNTERS_H__29742EBA_E937_442F_A4C4_DC147A5E5394__INCLUDED_)
#define AFX_PROPPAGECOUNTERS_H__29742EBA_E937_442F_A4C4_DC147A5E5394__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPageCounters.h : header file
// Author: Alexander Vechersky, 2010 

#include "CountersListBox.h"

class CPerfDataBase;
class CIndicatorSettings;

/////////////////////////////////////////////////////////////////////////////
// CPropPageCounters dialog

class CPropPageCounters : public CPropertyPage
{
    DECLARE_DYNCREATE(CPropPageCounters)

// Construction
public:
    CPropPageCounters();
    ~CPropPageCounters();

    void ResetCountersTree();
    void FillCounterGroups();
    void FillComputerName();
    void FillCountersTree();
    void AddCounterFromTree();
    void RemoveSelectedCounter();
    void FillCountersList();
    void EnablingAddButton(bool isFillInstances);       // Enable Add button if necessary. 

    void Modify();
    void SetModifyState();

// Dialog Data
    //{{AFX_DATA(CPropPageCounters)
    enum { IDD = IDD_DIALOG_SELECT_COUNTER };
    //}}AFX_DATA

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPropPageCounters)
    public:
    virtual BOOL OnApply();
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPropPageCounters)
    virtual BOOL OnInitDialog();
    afx_msg LRESULT OnQuerySiblings( WPARAM wParam, LPARAM lParam );
    afx_msg void OnButtonAdd();
    afx_msg void OnButtonRemove();
    afx_msg void OnSelchangeListSelectedCounters();
    afx_msg void OnSelchangedTreeSystemCounters(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSetfocusListSelectedCounters();
    afx_msg void OnSetfocusTreeSystemCounters(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelchangeCounterGroup();
    afx_msg void OnButtonConnect();
    afx_msg void OnLbnSelchangeListInstances();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
    bool    m_Modified;

protected:
// DATA

    CButton* m_ButtonAdd;
    CButton* m_ButtonRemove;
    CCountersListBox  m_indicList;

#define MAX_HELP_TEXT_SIZE  2048
    char m_txtHelp[MAX_HELP_TEXT_SIZE + 2];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGECOUNTERS_H__29742EBA_E937_442F_A4C4_DC147A5E5394__INCLUDED_)
