#if !defined(AFX_INDICATOR_H__A2A817B5_7946_4AFA_A284_880F9F691A68__INCLUDED_)
#define AFX_INDICATOR_H__A2A817B5_7946_4AFA_A284_880F9F691A68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Indicator.h : header file
// Author: Alexander Vechersky, 2010 

#include "GraphValues.h"
#include "IndicatorToolTip.h"

class   CPerfCount;
class   CIndicatorSettings;
class   Label;
class   ValueText;
class ValueMaxText;

/////////////////////////////////////////////////////////////////////////////
// CIndicator window

class CIndicator : public CStatic
{

public:
// Construction
    CIndicator(CPerfCount*  counter, CIndicatorSettings* settings );

// Attributes
public:

    CIndicatorToolTip   m_ToolTip;
    int             m_TicksTextRefresh;    // # of ticks to keep avg value
    Label*          m_Label;
    ValueText*      m_ValueText;
    ValueMaxText*   m_ValueMaxText;
    AutoPtr<CGraphValues>   m_graph;

    // Doc data
    CIndicatorSettings*     m_indicSettings;    
    AutoPtr<CPerfCount>     m_counter;

    // View data
    CRect     m_Rect;   // position in parent
    CRect     m_ClientRect; // Indicator rect
    double    m_rval;       // Relative indic value 0-1.0
    double    m_AbsValue;   // Abs indic value

    bool      m_MouseHover;
    bool      m_UpdateDisplayedValues;      // true: related values have to be redrdawn
    bool      m_ForceDrawToolTip;           // true: tool tip needs to be drawn
    bool      m_UpdateToolTip;              // true: tool tip text has changed and needs to be redrawn

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CIndicator)
    //}}AFX_VIRTUAL

// Implementation
public:
    void RedrawLabels();
    double  GetLastAverageValue() { return m_LastAvgVal; } ;
    void    RefreshData();             // puts 'true' into m_UpdateDisplayedValues if value text should be updated
    virtual ~CIndicator();
    void    InitIndicator(CWnd* parent, CRect& r, UINT id);

    bool        FormatTipText();  // Returns false if show value is unchanged.
    CPerfCount *  Counter() { return m_counter; };

protected:

    CWnd *    m_parent;
    CWnd *    m_ind_Ctl;

    double      m_LastTipValue;  // last shown value
    double      m_LastAvgVal; // Last absolute average value for last k ticks
    double      m_AvgVal;     // Current absolute average value for last k ticks

    // Generated message map functions
    //{{AFX_MSG(CIndicator)
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDICATOR_H__A2A817B5_7946_4AFA_A284_880F9F691A68__INCLUDED_)
