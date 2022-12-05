// ValueText.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "ValueText.h"
#include "PerfCount.h"
#include "Indicator.h"
#include "Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ValueText

ValueText::ValueText(CIndicator* ind)
{
    m_Indicator = ind;
    m_Indicator->m_ValueText = this;
}

ValueText::~ValueText()
{
    FUNCTIONLOG(15, "ValueText::destr");
}

/////////////////////////////////////////////////////////////////////////////
void ValueText::InitValueText(CWnd* parent, CRect r, LPCSTR txt, UINT id)
{
    if ( IS_INDICATOR_INVISIBLE || CheckDispMode(DISP_GLINT) || sets.m_isMiniBar )
        return;
    if ( Create( txt, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER | SS_BLACKRECT,    // SS_BLACKFRAME,
                 r,  parent, id ) == FALSE)
    { 
        DATA_ERR_MESSAGE_BOX("Value text not created");
        return;
    }
    ShowWindow(SW_SHOW); 
    GetFullRect(this, &m_ClientRect);    //GetClientRect(&m_ClientRect);
    m_ClientRect.OffsetRect(-1,-1); 
    GetWindowRect( &m_WindowRect );
}

//////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(ValueText, CStatic)
    //{{AFX_MSG_MAP(ValueText)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ValueText message handlers

void ValueText::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    if ( !theApp.m_panel->IsReady() && !m_Indicator->m_counter->IsActive() )
        return;
    if ( IS_INDICATOR_INVISIBLE || CheckDispMode(DISP_GLINT) || sets.m_isMiniBar )
        return;

    OUTLOG(60, "  ^MMM^ Indicator '%s'  ValueText::OnPaint()", (LPCTSTR)m_Indicator->m_indicSettings->m_strIndicTitle );

    CFont* oldFont = (CFont*)dc.SelectObject(sets.m_LargeView ? &g_font1L : &g_font1);
    dc.SetTextColor( m_Indicator->m_MouseHover ? CLR_CURRENT_ACTIVE_TITLE : sets.m_titlesColor );
    dc.FillRect( &m_ClientRect, &CBrush(CLR_BLACK) );
    dc.SetBkColor( CLR_BLACK );

    LPCTSTR sz_val = GetSmallValueStr(m_Indicator->GetLastAverageValue() );     //m_LastAvgVal

    dc.FillRect( &m_ClientRect, &CBrush(CLR_BLACK) );
    dc.TextOut(0, 0, sz_val, strlen(sz_val) );
    // dc.SelectObject( oldFont );
 
    // Do not call CStatic::OnPaint() for painting messages
}

//////////////////////////////////////////////////////////////////////////////
void ValueText::Update()
{
    if ( IS_INDICATOR_INVISIBLE )
        return;
    if ( m_hWnd != 0 )
    {
        OUTLOG(60, "  ^VVV^ Indicator '%s'  ValueText::Update()", (LPCTSTR)m_Indicator->m_indicSettings->m_strIndicTitle );
        RedrawWindow();
    }
}
