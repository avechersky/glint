// ProcessWindow.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "ProcessWindow.h"
#include "Gadgets.h"
#include "Settings.h"
#include "Panel.h"
#include "PerfCount.h"
#include "Processes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessWindow

CProcessWindow::CProcessWindow()
{
    m_parent = 0;
    m_MouseHover = false;
}

CProcessWindow::~CProcessWindow()
{
    m_ToolTip.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CProcessWindow, CStatic)
    //{{AFX_MSG_MAP(CProcessWindow)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////
//          CProcessWindow message handlers
/////////////////////////////////////////////////////////////////////////////////////////////////
void CProcessWindow::InitProcWin(CWnd* parent, CRect r, CGadgets* gadgets)
{
    bool firstCreation = (m_parent == 0);
    if ( !firstCreation )
    {
        DestroyWindow();
    }

    if ( Create( "", WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER | SS_BLACKRECT,  // SS_BLACKFRAME,
                 r,  parent, ID_PROCESS_WINDOWS ) == FALSE)
    { 
        DATA_ERR_MESSAGE_BOX(_T("Process Window not created"));
        return;
    }

    m_parent  = parent;

    GetFullRect(this, &m_ClientRect);    //GetClientRect(&m_ClientRect);
    m_ClientRect.OffsetRect(-1,-1); 
    m_WindowsXSize = m_ClientRect.right - m_ClientRect.left;
    GetWindowRect( &m_WindowRect );
 
    if (  sets.m_ShowToolTips  )
    {
        m_ToolTip.Init(parent, ID_PROCESS_WINDOWS);
        m_ToolTip.SetFont( &g_fontTTipFix );
        UpdateToolTip();
    }

    ShowWindow(SW_SHOW); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CProcessWindow::UpdateToolTip() 
{
    if (  !sets.m_ShowToolTips  )
        return;

    m_ToolTip.SetTitle(0, g_ProcessesToolTipTitle);
    m_ToolTip.SetMaxTipWidth(MAX_TOOL_TIP_WIDTH);
    m_ToolTip.UpdateTipText(g_ProcessesToolTipText, m_parent);
    if ( m_MouseHover )
    {
        //m_ToolTip.Update();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CProcessWindow::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    //
    //  Show processes
    //
    if ( !sets.m_isShowProcesses ) 
        return;

    CFont* oldFont = (CFont*)dc.SelectObject(sets.m_LargeView ? &g_font3L : &g_font3); 

    dc.SetBkColor( (0,0,0) );
    dc.FillRect( &m_ClientRect, &CBrush(CLR_BLACK) );

    if ( !theApp.m_panel->IsReady() )
        return;
    if ( !g_Gadgets->m_processesDB->IsDataReady() )
        return;

    CPerfCount** procs = & (g_Gadgets->m_processesDB->m_SortedProcs[0]);

    int iFirstProc = 0;     // maximum process sorting number encountered

    int kp = 0;     // counter of processes
    for (int iproc = MAX_PROC_SORT_IDX; iproc >= 0 && kp < 20 ; iproc-- )
    {
        if ( procs[iproc] == 0 ) 
            continue;
        #define MIN_CPU_BRIGHTNESS  0.33
        #define det ((1.0 - MIN_CPU_BRIGHTNESS) / ( MAX_PROC_SORT_IDX * MAX_PROC_SORT_IDX ))
        double colorValue = (double)iproc * iproc * det + MIN_CPU_BRIGHTNESS;
        if ( colorValue > 1.0 ) 
            colorValue = 1.0;

        COLORREF clr = CalcColor(sets.m_processesColor, colorValue, 0); // CLR_YELLOW
        LPCSTR procname = procs[iproc]->m_strInst;
        ASSERT( procname[0] != 0 );

        dc.SetTextColor( clr );
        dc.SetBkColor( (0,0,0) );

        // Truncate process name if necessary.
        int outputLen = strlen(procname);
        CSize outSize = dc.GetTextExtent( (LPCSTR)procname, outputLen );
        while ( outSize.cx >  m_WindowsXSize - 2 )
        {
            --outputLen;
            outSize = dc.GetTextExtent( (LPCSTR)procname, outputLen );
        }

        dc.TextOut(2, kp * PROCESSES_Y_DIST, (LPCSTR)procname, outputLen );

        kp++;
        if ( kp * PROCESSES_Y_DIST >= m_ClientRect.bottom - PROCESSES_Y_DIST )
            break;
    }

    if ( m_MouseHover )
    {
        //m_ToolTip.UpdateTipText( g_ProcessesToolTipText, m_parent);
        //m_ToolTip.Update();
    }
    // dc.SelectObject( oldFont );

    // Do not call CStatic::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////////////////////////
