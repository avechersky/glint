// Indicator.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "PerfCount.h"
#include "Label.h"
#include "ValueText.h"
#include "ValueMaxText.h"
#include "Indicator.h"
#include "Panel.h"
#include  <math.h>
#include "IndicatorToolTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CIndicatorToolTip

//------------------------------------------------------------------------------------------------
CIndicatorToolTip::CIndicatorToolTip()
{
    m_TipText[0] = 0;
}

//------------------------------------------------------------------------------------------------
CIndicatorToolTip::~CIndicatorToolTip()
{
}

//------------------------------------------------------------------------------------------------
void    CIndicatorToolTip::Init(CWnd* parent, UINT id)
{
    if ( m_hWnd != 0 || !sets.m_ShowToolTips )
        return;

    // Prepare Too Tip
    Create(parent, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOPREFIX );

    PTOOLINFO pTI = &m_ToolInfo;
    pTI->cbSize = sizeof(TOOLINFO);
    pTI->hwnd   = parent->m_hWnd;
    pTI->lpszText = LPSTR_TEXTCALLBACK;   //    or:  (char*)m_TipText
    pTI->uId    = (DWORD)m_hWnd;          // (DWORD)GetDlgCtrlID() or m_hWnd
    pTI->uFlags |= TTF_IDISHWND | TTF_SUBCLASS;        // TTF_IDISHWND is necessary 
    pTI->uFlags |= TTF_TRANSPARENT;
    pTI->uFlags |= TTF_NOTBUTTON;        // | TTF_CENTERTIP
    pTI->hinst = (HINSTANCE)id;          // (DWORD)GetDlgCtrlID() or m_hWnd

    SetDelayTime(TTDT_AUTOPOP, 24*60*60*1000);
    SetDelayTime(TTDT_INITIAL, 200);
    SetDelayTime(TTDT_RESHOW, 200);
    SetToolInfo( &m_ToolInfo );
    
    //AddTool(parent , m_TipText);
    AddTool(parent , LPSTR_TEXTCALLBACK);

    //if ( theApp.m_panel->IsReady() )
    {
        Activate(TRUE);
    }
}

//------------------------------------------------------------------------------------------------
void CIndicatorToolTip::OnTtnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTTDISPINFO pNMTTDI = reinterpret_cast<LPNMTTDISPINFO>(pNMHDR);

    OUTLOG(99, " <<< ToolTip::OnTtnGetDispInfo  m_hWnd:%x >>> ", (UINT)m_hWnd);
    //TRACE(" <<<  OnTtnGetDispInfo >>> \r\n");

    if (m_hWnd == 0)
        return;

    SetMaxTipWidth(MAX_TOOL_TIP_WIDTH);

    pNMTTDI->lpszText = (char*)m_TipText;
    pNMTTDI->hinst = NULL;
    pNMTTDI->szText[0] = 0;

    *pResult = 0;
}

//------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CIndicatorToolTip, CToolTipCtrl)
    //{{AFX_MSG_MAP(CIndicatorToolTip)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    ON_NOTIFY_REFLECT(TTN_GETDISPINFO, &CIndicatorToolTip::OnTtnGetDispInfo)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndicatorToolTip message handlers



