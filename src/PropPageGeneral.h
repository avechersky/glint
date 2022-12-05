#if !defined(AFX_PROPPAGEGENERAL_H__011E244F_1AD4_4C0E_BF88_8557C3A5D1CA__INCLUDED_)
#define AFX_PROPPAGEGENERAL_H__011E244F_1AD4_4C0E_BF88_8557C3A5D1CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPageGeneral.h : header file
// Author: Alexander Vechersky, 2010 

/////////////////////////////////////////////////////////////////////////////
// CPropPageGeneral dialog

class CPropPageGeneral : public CPropertyPage
{
    DECLARE_DYNCREATE(CPropPageGeneral)

// Construction
public:
    CPropPageGeneral();
    ~CPropPageGeneral();

    void DeleteScheme(CString &scheme_name);
    void FillSchemeList();
    void LoadSchema(const char * schemaName, bool to_finish = true);
    void FinishLoadingSchema();
    void UpdateSettings();
    void SetSchemaNameText();
    CString GetSelectedSchema();
    void UpdateControlsData(bool edits_only = false);
    void Modify();
    void FillViewModeList();

 // Dialog Data
    //{{AFX_DATA(CPropPageGeneral)
    enum { IDD = IDD_DIALOG_GENERAL_SETS };
    BOOL    m_FullView;
    BOOL    m_MiniBar;
    BOOL    m_ShowProcesses;
    BOOL    m_ShowValues;
    BOOL    m_ShowLabels;
    BOOL    m_AlwaysOnTop;
    BOOL    m_SystemTaskActivity;
    int     m_IndicsInterval;
    int     m_IndicsInRow;
    int     m_ProcessesInterval;
    int     m_ProcessTrackTime;
    int     m_ValueShowInterval;
    //double  m_BrigtnessCoeff;
    BOOL    m_ShowToolTips;
    BOOL    m_LargeView;
    EDisplayMode    m_DispMode;
    //}}AFX_DATA

    CStringArray    m_schemes;

    bool        m_Modified;
    bool        m_IsInitialized;

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPropPageGeneral)
public:
    virtual BOOL OnApply();
    virtual BOOL OnKillActive();
    virtual BOOL OnSetActive();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
public:
    // Generated message map functions
    //{{AFX_MSG(CPropPageGeneral)
    afx_msg void OnCheckMiniBar();
    afx_msg void OnCheckFullView();
    afx_msg void OnCheckShowValues();
    afx_msg void OnCheckShowProcesses();
    afx_msg void OnCheckAlwaysOnTop();
    afx_msg void OnCheckShowLabels();
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonSchemeLoad();
    afx_msg void OnButtonSchemeSave();
    afx_msg void OnButtonResetDefaults();
    afx_msg void OnCheckShowTips();
    afx_msg void OnKillfocusEditIndicatorsInterval();
    afx_msg void OnKillfocusEditValuesInterval();
    afx_msg void OnKillfocusEditProcessesInterval2();
    afx_msg void OnKillfocusEditProcessesTrackInterval();
    afx_msg void OnKillfocusEditIndicsInRow();
    afx_msg void OnKillfocusEDITBrightness();
    afx_msg void OnChangeEDITBrightness();
    afx_msg void OnChangeEditIndicatorsInterval();
    afx_msg void OnChangeEditIndicsInRow();
    afx_msg void OnChangeEditProcessesInterval2();
    afx_msg void OnChangeEditProcessTrackTime();
    afx_msg void OnChangeEditValuesInterval();
    afx_msg void OnCheckLargeView();
    afx_msg void OnCheckAllowOnlyOneInstance();
    afx_msg void OnSchemeDelete();
    afx_msg void OnDblclkListSchemes();
    afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnBnClickedCheckShowSysTasksActivity();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEGENERAL_H__011E244F_1AD4_4C0E_BF88_8557C3A5D1CA__INCLUDED_)
