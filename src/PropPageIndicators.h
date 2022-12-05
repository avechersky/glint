#if !defined(AFX_PROPPAGEINDICATORS_H__37D26B1B_DD6A_4536_A3CE_9A177D3CC376__INCLUDED_)
#define AFX_PROPPAGEINDICATORS_H__37D26B1B_DD6A_4536_A3CE_9A177D3CC376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PropPageIndicators.h : header file
// Author: Alexander Vechersky, 2010 

#include "CountersListBox.h"

class CPerfDataBase;
class CIndicatorSettings;

/////////////////////////////////////////////////////////////////////////////
// CPropPageIndicators dialog

class CPropPageIndicators : public CPropertyPage
{
    DECLARE_DYNCREATE(CPropPageIndicators)

// Construction
public:
    CPropPageIndicators();
    ~CPropPageIndicators();

// Dialog Data
    //{{AFX_DATA(CPropPageIndicators)
    enum { IDD = IDD_INDICATORS_PROPS };
    CString m_SmallTitle;
    BOOL    m_Visible;
    BOOL    m_ShowInMinibar;
    BOOL    m_ShowLocalVariation;
    BOOL    m_IsAdjustMaximum;
    BOOL    m_OnlyInFullMode;
    //}}AFX_DATA

    double  m_MaxValue;         // handled separately

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPropPageIndicators)
    public:
    virtual BOOL OnKillActive();
    virtual BOOL OnApply();
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

public:
    void SetMoveButtons();
    void SetIndicatorsSelection();
    void GetIndicatorListSelection();

    void MoveIndicator(int direction);

    void Modify();
    void MarkCurrentChanged();

    COLORREF m_color;
    bool    m_Modified;             // indicator(s) modified
    bool    m_CurrentIndicChanged;  // current indicator options modified
    bool    m_OnActivation;     // true: fields are activated and no modifications are made yet

protected:
    
    void EnableAllControls(BOOL  enable);
    void PaintMaxValue(); 
    void PaintColorButton(); 
    void FillIndicatorsList();   // param: where to point selection
    void UpdateIndicSets();
    void ModifyCheck3State(int id, BOOL& var);   // make checkbox selection definite for one specific inidicator 

    CCountersListBox  m_indicList;
    CButton*    m_ButtonUp;
    CButton*    m_ButtonDown;

    CButton*    m_CheckAdjustMax;
    CButton*    m_CheckLocalVariations;


// Implementation
protected:
    
    // Generated message map functions
    //{{AFX_MSG(CPropPageIndicators)
    afx_msg void OnButtonColor();
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeListIndicCounters();
    afx_msg void OnChangeEditMaxValue();
    afx_msg void OnChangeEditSmallTitle();
    afx_msg void OnCheckVisible();
    afx_msg void OnCheckFullModeOnly();
    afx_msg void OnCheckShowInMinibar();
    afx_msg void OnCheckAdjustMax();
    afx_msg void OnCheckLocalVariations();
    afx_msg LRESULT OnQuerySiblings( WPARAM wParam, LPARAM lParam );
    afx_msg void OnButtonIndicatorUp();
    afx_msg void OnButtonIndicatorDown();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGEINDICATORS_H__37D26B1B_DD6A_4536_A3CE_9A177D3CC376__INCLUDED_)
