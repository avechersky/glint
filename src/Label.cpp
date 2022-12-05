// Label.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "Label.h"
#include "Settings.h"
#include "Indicator.h"
#include "PerfCount.h"
#include "Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Label

Label::Label(CIndicator* ind)
{
    m_Indicator = ind;
    m_Indicator->m_Label = this;
}

Label::~Label()
{
    FUNCTIONLOG(15, "Label::destr");
}

/////////////////////////////////////////////////////////////////////////////
void Label::InitLabel(CWnd* parent, CRect r, LPCSTR txt, UINT id)
{
    if ( !theApp.m_panel->IsReady() && !m_Indicator->m_counter->IsActive() )
        return;
    if ( IS_INDICATOR_INVISIBLE || sets.m_isMiniBar )
        return;

    if ( Create( txt, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER | SS_BLACKRECT, // SS_BLACKFRAME,
                 r,  parent, id ) == FALSE)
    { 
        DATA_ERR_MESSAGE_BOX(_T("Label not created"));
        return;
    }
    ShowWindow(SW_SHOW); 
    m_Text = txt;
    m_TextLen = m_Text.GetLength();
    GetFullRect(this, &m_ClientRect);       // GetClientRect(&m_ClientRect);
    m_ClientRect.OffsetRect(-1,-1); 
    GetWindowRect( &m_WindowRect );
}

BEGIN_MESSAGE_MAP(Label, CStatic)
    //{{AFX_MSG_MAP(Label)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Label message handlers

/////////////////////////////////////////////////////////////////////////////
void Label::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
    if ( !theApp.m_panel->IsReady() && !m_Indicator->m_counter->IsActive() )
        return;
    if ( IS_INDICATOR_INVISIBLE || sets.m_isMiniBar )
        return;

    OUTLOG(60, "  ^MMM^ Indicator '%s'  Label::OnPaint()", (LPCTSTR)m_Indicator->m_indicSettings->m_strIndicTitle );

    CFont* oldFont = (CFont*)dc.SelectObject(sets.m_LargeView ? &g_font1L : &g_font1);
    dc.SetTextColor( m_Indicator->m_MouseHover ? CLR_CURRENT_ACTIVE_TITLE : sets.m_titlesColor );
    dc.FillRect( &m_ClientRect, &CBrush(CLR_BLACK) );
    dc.SetBkColor( CLR_BLACK );

    int label_len = m_TextLen;
    if ( CheckDispMode(DISP_COLUMN) && label_len > 4 )  
        label_len = 4;                          // limit label length 4 for columns
    dc.TextOut(0, 0, (LPCSTR) m_Text, label_len);
    // dc.SelectObject( oldFont );
 
    // Do not call CStatic::OnPaint() for painting messages
}
