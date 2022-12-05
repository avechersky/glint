#if !defined(AFX_PANEL_H__46F8EB16_60DF_42D5_8259_97C15427A913__INCLUDED_)
#define AFX_PANEL_H__46F8EB16_60DF_42D5_8259_97C15427A913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Panel.h : header file
// Author: Alexander Vechersky, 2010 

#include <Afxtempl.h>
#include "AutoPtr.h"

extern  HICON icon;

class   Label;
class   ValueText;
class   ValueMaxText;
class   CGadgets;
class   CTrayIcon;
class   CProcessWindow;

/////////////////////////////////////////////////////////////////////////////
// CPanel window

class CPanel : public CWnd
{
// Construction
public:
    CPanel();

    bool    IsReady() { return m_isReady; }

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPanel)
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
public:
    void    Initialize();
    void    InitPanel();
    virtual ~CPanel();

    bool    m_isReady;          // false    : all draing is stopped
    bool    m_isSkipDraw;       // true : Pause, i.e. refreshing is stopped 
    DWORD   m_OldMouseTrackFlags;   // store mouse tracking mode, to avoid repeated calls.
    AutoPtr<CProcessWindow> m_ProcessesWin;
    AutoPtr<CTrayIcon>      m_TrayIcon;

protected:

    bool    m_sizingStarted;
    bool    m_LeftDown;             // true left button is down
    bool    m_TempNormalView;       // true, if full view temp opened from a system tray
    bool    m_SettingsDlgOpen;      // true: if Settings Dialog is active
    bool    m_isShowValuesText;     // true: Values Text is to be refreshed
    int     m_recent_err_count;     // # of recent error when reading counters

    int     m_BorderSize;

    CTypedPtrList<CObList, Label*>          m_labels;
    CTypedPtrList<CObList, ValueText*>      m_valueTexts;
    CTypedPtrList<CObList, ValueMaxText*>   m_valueMaxTexts;

    // last positions, when moving mouse
    int     m_LastMousePosX;
    int     m_LastMousePosY;

    int     m_xProcesses;
    int     m_yProcesses;

    UINT    m_TaskbarCreatedMsg;   // message code from Shell to reinstall tray icon

public:
    void UpdateIndicatorsRects();
    void HandlePfReadException();
    void RefreshIndicators();
    void RefreshProcess();
    void RedrawIndicators();

    void ClearToolTips();
    void Reinit();
    void PositionWindow(DWORD flags = 0);
    void HandleTrayEvent(UINT message, WPARAM wParam, LPARAM lParam);
    void SetMouseTracking(DWORD flags);
    
    void ResetPanel();
    void ResetIndicatorsTimer();
    void RemoveLists();
    void CleanPanelWindow();
    void DrawBorders(bool   invalidate = true);

    bool CheckComputerConnection();

    // Generated message map functions
protected:
    //{{AFX_MSG(CPanel)
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnExit();
    afx_msg void OnTopToggle();
    afx_msg void OnPauseToggle();
    afx_msg void OnSmallToggle();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnFullToggle();
    afx_msg void OnShowProcessesToggle();
    afx_msg void OnResetCmd();
    afx_msg void OnDestroy();
    afx_msg void OnLabelsToggle();
    afx_msg void OnAbout();
    afx_msg void OnPaint();
    afx_msg void OnShowValuesToggle();
    afx_msg void OnPopupSettings();
    afx_msg void OnPopupToolTipsToggle();
    afx_msg void OnLargeViewToggle();
    afx_msg void OnShowGraphsToggle();
    afx_msg void OnPopup1ColumnsToggle();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnPopupGlintlights();
    afx_msg void OnPopup1Changecomputer();
    afx_msg void OnClose();
    afx_msg void OnEndSession(BOOL bEnding);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANEL_H__46F8EB16_60DF_42D5_8259_97C15427A913__INCLUDED_)
