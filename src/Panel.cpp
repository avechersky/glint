// Panel.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "Panel.h"
#include "Gadgets.h"
#include "Settings.h"
#include "PerfCount.h"
#include "PerfData.h"
#include "Indicator.h"
#include "Label.h"
#include "ValueText.h"
#include "ValueMaxText.h"
#include "Processes.h"
#include "ProcessWindow.h"
#include "RemoteConnectionDlg.h"
#include "About.h"
#include "TrayIcon.h"
#include "SettingsDlg.h"
#include  <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanel

CPanel::CPanel()
{
    g_TimeTicks = 0;
    m_isReady = false;
    m_isSkipDraw = false;
    m_isShowValuesText = false;
    m_sizingStarted = false;
    m_LastMousePosX = INT_MIN;
    m_LastMousePosY = INT_MIN;
    m_LeftDown = false;
    m_TempNormalView = false;
    m_SettingsDlgOpen = false;
    m_BorderSize = 10;      // (sets.m_isMiniBar ? 5 : 10)
    m_OldMouseTrackFlags = 0xFFFFFFFF;
    m_recent_err_count = 0;
}

CPanel::~CPanel()
{
}

BEGIN_MESSAGE_MAP(CPanel, CWnd)
    //{{AFX_MSG_MAP(CPanel)
    ON_WM_KEYUP()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(IDM_EXIT, OnExit)
    ON_COMMAND(IDM_ON_TOP_TOGGLE, OnTopToggle)
    ON_COMMAND(IDM_PAUSE_TOGGLE, OnPauseToggle)
    ON_COMMAND(IDM_SMALL_TOGGLE, OnSmallToggle)
    ON_WM_ACTIVATE()
    ON_WM_PAINT()
    ON_COMMAND(ID_POPUP1_FULL, OnFullToggle)
    ON_COMMAND(ID_POPUP1_PROCESSES, OnShowProcessesToggle)
    ON_COMMAND(ID_POPUP1_RESETSPEEDBRIGHTNESS, OnResetCmd)
    ON_WM_DESTROY()
    ON_COMMAND(ID_POPUP1_LABELS, OnLabelsToggle)
    ON_COMMAND(IDM_ABOUT, OnAbout)
    ON_COMMAND(ID_POPUP1_SHOWVALUES, OnShowValuesToggle)
    ON_COMMAND(ID_POPUP1_SETTINGS, OnPopupSettings)
    ON_COMMAND(ID_POPUP1_TIPS, OnPopupToolTipsToggle)
    ON_COMMAND(ID_POPUP1_LARGEVIEW, OnLargeViewToggle)
    ON_COMMAND(ID_POPUP1_GRAPHS, OnShowGraphsToggle)
    ON_COMMAND(ID_POPUP1_COLUMNS, OnPopup1ColumnsToggle)
    ON_WM_KEYDOWN()
    ON_WM_SYSKEYDOWN()
    ON_COMMAND(ID_POPUP1_GLINTLIGHTS, OnPopupGlintlights)
    ON_COMMAND(ID_POPUP1_CHANGECOMPUTER, OnPopup1Changecomputer)
    //}}AFX_MSG_MAP
    ON_WM_CLOSE()
    ON_WM_ENDSESSION()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPanel message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CPanel::PreCreateWindow(CREATESTRUCT& cs) 
{
    WNDCLASS* cls = (WNDCLASS*)cs.lpszClass;
    return CWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if ( g_IsMenuActive )
    {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    switch ( nChar)
    {
        case  VK_CANCEL:
        //case  'X':                             // VK_END, (nFlags & 1<<13) - Alt
        //  if ( (nFlags & (1<<13) != 0 ) )  // Alt
        //      OnExit();
            break;
        case  'P':
            OnPauseToggle();
            break;
        case  VK_F1:
            OnAbout();
            break;
        case  'L':
            OnLabelsToggle();
            break;
        case  'I':
            OnPopupToolTipsToggle();
            break;
        case  'B':
            OnLargeViewToggle();
            break;
        case  'G':
            OnPopupGlintlights();
            break;
        case  'M':
            OnSmallToggle();
            break;
        case  'A':
            OnShowGraphsToggle();
            break;
        case  'O':
            OnPopup1ColumnsToggle();
            break;
        case  'T':
            OnTopToggle();
            break;
        case  'N':
            OnPopup1Changecomputer();
            break;
        case  'F':
            OnFullToggle();
            break;
        case  'V':
            OnShowValuesToggle();
            break;
        case  'C':
            OnShowProcessesToggle(); 
            break;
        case  'R':                  
            OnResetCmd();
            break;
        case  'S':                  
            OnPopupSettings();
            break;

        case  VK_RIGHT:
            if ( !CheckDispMode(DISP_GRAPH) )
            {
                if ( sets.m_IndicTimeInterval < 2*1000) 
                    sets.m_IndicTimeInterval *= 1.2;        // slower
                ResetIndicatorsTimer();
            }
            break;

        case  VK_LEFT:
            if ( !CheckDispMode(DISP_GRAPH) )
            {
                if ( sets.m_IndicTimeInterval > 100 ) 
                    sets.m_IndicTimeInterval    /= 1.2;     // faster
                ResetIndicatorsTimer();
            }
            break;

        case  VK_UP:
            if ( CheckDispMode(DISP_GLINT) )
            {
                if (sets.m_BrightCoeff > MIN_BRIGHTNESS)
                    sets.m_BrightCoeff  /= 1.15;            // brighter
            }
            break;

        case  VK_DOWN:                      
            if ( CheckDispMode(DISP_GLINT) )
            {
                if (sets.m_BrightCoeff < MAX_BRIGHTNESS)
                    sets.m_BrightCoeff  *= 1.15;            // dimmer
            }
            break;

        case  'H':
        case  'K':
        case  'Q':
        default:
            CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
    }

}

//////////////////////////////////////////////////////////////////

void CPanel::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    switch ( nChar)
    {
        case  VK_CANCEL:
        case  'X':                           // VK_END, (nFlags & 1<<13) - Alt
            if ( (nFlags & (1<<13) != 0 ) )  // Alt
                OnExit();
            break;

        default :
            CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
    }
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
void CPanel::OnMouseMove(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    if ( m_LeftDown && (nFlags & MK_LBUTTON) ) {

        if ( (m_LastMousePosX != INT_MIN && point.x != m_LastMousePosX) || 
             (m_LastMousePosY != INT_MIN && point.y != m_LastMousePosY )
            ) {
            int deltax = (point.x - m_LastMousePosX);
            int deltay = (point.y - m_LastMousePosY);
            sets.m_PosX += deltax;
            sets.m_PosY += deltay;
            CorrectWindowPosition();

            if ( sets.m_isMiniBar )
            {
                sets.m_LastMinibarPosX = sets.m_PosX;
                sets.m_LastMinibarPosY = sets.m_PosY;
            }
            else if ( abs(sets.m_LastMinibarPosX - sets.m_PosX) > sets.m_PosWidth  ||
                      abs(sets.m_LastMinibarPosY - sets.m_PosY) > sets.m_PosHeight )
            {
                // Reset minibar to current main window position
                sets.m_LastMinibarPosX = INT_MIN;
                sets.m_LastMinibarPosY = INT_MIN;
            }

            SetWindowPos( 0, // (sets.m_isOnTop ? &wndTopMost : &wndNoTopMost), //  const CWnd* pWndInsertAfter, 
                      sets.m_PosX,  //  int x, pos0.left 
                      sets.m_PosY,  //  int y, pos0.top 
                      0, 0,                 //  int cx, int cy, 
                      SWP_NOSIZE | SWP_SHOWWINDOW );    // | SWP_DRAWFRAME | SWP_NOREDRAW 

            ASSERT(m_LastMousePosX > -1 && m_LastMousePosX < 10000 );
            ASSERT(m_LastMousePosY > -1 && m_LastMousePosY < 10000 );
        }
    }

    CWnd::OnMouseMove(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSizing(UINT fwSide, LPRECT pRect) 
{
    CWnd::OnSizing(fwSide, pRect);
    m_sizingStarted = true;
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    m_LeftDown = true;
    m_LastMousePosX = point.x; 
    m_LastMousePosY = point.y; 
    SetCapture();
    
    CWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnLButtonUp(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    m_LeftDown = false;
    m_LastMousePosX = INT_MIN;
    m_LastMousePosY = INT_MIN;
    
    ::ReleaseCapture();

    g_CenterPosX = sets.m_PosX + sets.m_PosWidth/2;
    g_CenterPosY = sets.m_PosY + sets.m_PosHeight/2;

    //sets.SavePosition();
    UpdateIndicatorsRects();

    CWnd::OnLButtonUp(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
//  Fill indicators rects for mouse tracking.
void CPanel::UpdateIndicatorsRects()
{
    CVisibleIndicatorsIterator iter(*g_Gadgets);
    CIndicator* ind;

    if ( m_ProcessesWin && sets.m_isShowProcesses )
    {
        m_ProcessesWin->GetWindowRect( m_ProcessesWin->Rect() );
    }

    for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
    {
        if ( ind->m_hWnd )
            ind->GetWindowRect( &(ind->m_Rect) );

        if ( sets.m_ShowLabels && ind->m_Label != 0 ) 
            ind->m_Label->GetWindowRect( ind->m_Label->Rect() );

        if ( !sets.m_NoNumbersShow && ind->m_ValueText != 0 )
            ind->m_ValueText->GetWindowRect( ind->m_ValueText->Rect() );

        if ( !sets.m_NoNumbersShow && ind->m_ValueMaxText != 0 )
            ind->m_ValueMaxText->GetWindowRect( ind->m_ValueMaxText->Rect() );
    }
}


//////////////////////////////////////////////////////////////////////////////
void CPanel::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMenu menu;
    menu.LoadMenu(IDR_POPUP_MENU1);

    // Put computer name inside menu
    CString str_comp;
    str_comp.Format("  COMPUTER:  %s", (LPCTSTR)GetPrintableCompName(sets.m_ComputerName) );
    menu.GetSubMenu(0)->ModifyMenu(ID_POPUP1_GLINTMONITOR, MF_BYCOMMAND |MF_DISABLED| MF_STRING | MF_GRAYED, ID_POPUP1_GLINTMONITOR, (LPCTSTR)str_comp);
    // menu.GetSubMenu(0)->EnableMenuItem (ID_POPUP1_GLINTMONITOR, FALSE);

    menu.GetSubMenu(0)->CheckMenuItem(IDM_PAUSE_TOGGLE, (m_isSkipDraw ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(IDM_ON_TOP_TOGGLE, (sets.m_isOnTop ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(IDM_SMALL_TOGGLE, (sets.m_isMiniBar ? MF_CHECKED : MF_UNCHECKED) );

    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_FULL, (sets.m_FullSet ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_LARGEVIEW, (sets.m_LargeView ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_PROCESSES, (sets.m_isShowProcesses ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_LABELS, (sets.m_ShowLabels ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_TIPS, (sets.m_ShowToolTips ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_SHOWVALUES, (!sets.m_NoNumbersShow ? MF_CHECKED : MF_UNCHECKED) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_GRAPHS, ( CheckDispMode(DISP_GRAPH) ? MF_CHECKED : MF_UNCHECKED ) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_COLUMNS, ( CheckDispMode(DISP_COLUMN) ? MF_CHECKED : MF_UNCHECKED ) );
    menu.GetSubMenu(0)->CheckMenuItem(ID_POPUP1_GLINTLIGHTS, ( CheckDispMode(DISP_GLINT) ? MF_CHECKED : MF_UNCHECKED ) );

    g_IsMenuActive = true;
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                        point.x, point.y, this);
    g_IsMenuActive = false;
}


/////////////////////////////////////////////////////////////////////////////
void CPanel::PositionWindow(DWORD flags)
{
    SetWindowPos( (sets.m_isOnTop ? &wndTopMost : &wndNoTopMost),   //  const CWnd* pWndInsertAfter, 
                  sets.m_PosX, sets.m_PosY, 
                  sets.m_PosWidth, 
                  sets.m_PosHeight,  
                  SWP_SHOWWINDOW | SWP_DRAWFRAME | flags);
                  // SWP_NOMOVE | SWP_SHOWWINDOW | SWP_DRAWFRAME | flags);

    g_CenterPosX = sets.m_PosX + sets.m_PosWidth/2;
    g_CenterPosY = sets.m_PosY + sets.m_PosHeight/2;
    UpdateIndicatorsRects();
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
    CWnd::OnActivate(nState, pWndOther, bMinimized);

    // TODO: Add your message handler code here
    if ( nState == WA_INACTIVE ) 
    {
        sets.m_titlesColor = CLR_INACTIVE_TITLE_BORDER;     
        sets.m_bordersColor = CLR_INACTIVE_INDIC_BORDER;       // (sets.m_isMiniBar ? CLR_INACTIVE_INDIC_BORDER : CLR_GREY_DARK);
        sets.m_hoverBordersColor = CLR_INACTIVE_HOVER_BORDER;  
        sets.m_BorderLineColorDark  = CLR_INACTIVE_BORDER_LINE_DARK;
        sets.m_BorderLineColorLight = CLR_INACTIVE_BORDER_LINE_LIGHT;
    }
    else 
    {
        sets.m_titlesColor = CLR_ACTIVE_TITLE_BORDER;       
        sets.m_bordersColor = CLR_ACTIVE_INDIC_BORDER;
        sets.m_hoverBordersColor = CLR_ACTIVE_HOVER_BORDER; 
        sets.m_BorderLineColorDark  = CLR_ACTIVE_BORDER_LINE_DARK;
        sets.m_BorderLineColorLight = CLR_ACTIVE_BORDER_LINE_LIGHT;
    }

    if ( m_isReady )
    {
        RedrawWindow();

        if ( !sets.m_isMiniBar )
        {
            CVisibleIndicatorsIterator iter(*g_Gadgets);
            CIndicator* ind;
            for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
            {
                if ( ind == 0 || ind->m_hWnd == 0 )
                    continue;

                if ( sets.m_ShowLabels &&  ind->m_Label != 0 && ind->m_Label->m_hWnd != 0 )
                {
                    ind->m_Label->RedrawWindow();
                }

                if (m_isShowValuesText && ind->m_ValueText != 0 && ind->m_ValueText->m_hWnd != 0 )
                {
                    ind->m_ValueText->Update();
                    if (!sets.m_NoMaxNumbersShow && ind->m_ValueMaxText->m_hWnd != 0 ) 
                    {
                        ind->m_ValueMaxText->Update();
                    }
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == TIMER_COUNTERS) {

        if ( m_isReady  && !m_isSkipDraw) {
            g_TimeTicks++;
            try {
                RefreshIndicators();
                m_recent_err_count = 0;
            }
            catch (...) {
                HandlePfReadException();
            }
        }

    }
    else if (nIDEvent == TIMER_PROCESSES) {
        if ( m_isReady  && !m_isSkipDraw ) {
            try {
                RefreshProcess();
                m_recent_err_count = 0;
            }
            catch (...) {
                HandlePfReadException();
            }
        }
    }

    CWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// Handle exception when reading the Performance Counters
void CPanel::HandlePfReadException()
{
    m_isReady = false;

    {
        CString str;
        str.Format(" Can not read performance data from computer: %s \n Windows error: %s" , 
                   (LPCTSTR)GetPrintableCompName(sets.m_ComputerName), last_win32_error_txt );
        DATA_ERR_MESSAGE_BOX(str, MB_OK | MB_ICONSTOP);
    }

    // PostMessage( WM_COMMAND, ID_POPUP1_CHANGECOMPUTER, 0 );   // try to change computer
    PostMessage( WM_KEYDOWN, 'N', 0 );      // try to change computer

    if ( m_recent_err_count++ > 5 /* || !CheckComputerConnection() */ )  {
        PostMessage( WM_SYSKEYDOWN, 'X', 0xfffff );     // try to change computer
        // PostMessage( WM_SYSKEYDOWN, VK_CANCEL, 0xfffff );        // try to change computer
    }

}

/////////////////////////////////////////////////////////////////////////////
//           C O M M A N D     H A N D L E R S
/////////////////////////////////////////////////////////////////////////////


void CPanel::OnExit() 
{
    // TODO: Add your command handler code here
    // PostMessage(WM_QUIT);

    m_isReady = false;
    theApp.m_RestartPanel = false;
    EndModalLoop( 0 );
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    OnSmallToggle();

    CWnd::OnLButtonDblClk(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CPanel::OnSmallToggle()
{
    sets.m_isMiniBar = ! sets.m_isMiniBar;
    OUTLOG(15, " ____ TOGGLE MINIBAR STARTS __  (TO %s) ", (sets.m_isMiniBar ? "mini" : "normal" ) );
    Reinit();
    OUTLOG(15, " ____ TOGGLE MINIBAR ENDS __  (TO %s) ", (sets.m_isMiniBar ? "mini" : "normal" ) );
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnTopToggle() 
{
    sets.m_isOnTop = !sets.m_isOnTop;

#ifdef WINDOWS_PANEL_DECORATIONS
    if ( sets.m_isOnTop ) {
        ModifyStyleEx( WS_EX_TOPMOST ,          // Remove
                       WS_EX_OVERLAPPEDWINDOW |WS_EX_DLGMODALFRAME, // Add  - WS_EX_WINDOWEDGE, WS_EX_OVERLAPPEDWINDOW
                       SWP_NOMOVE);
        ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE,     // Remove 
                       0,               // Add 
                       SWP_NOMOVE);
        ModifyStyle (  WS_DLGFRAME,     // remove
                       WS_BORDER,           // add (WS_BORDER)
                       SWP_NOMOVE);
    }
    else {
        ModifyStyleEx( WS_EX_OVERLAPPEDWINDOW ,     // Remove  -  WS_EX_WINDOWEDGE, WS_EX_OVERLAPPEDWINDOW
                       WS_EX_TOPMOST | WS_EX_DLGMODALFRAME,             // Add 
                       SWP_NOMOVE);
        ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE,     // Remove 
                       0,               // Add 
                       SWP_NOMOVE);
        ModifyStyle (  WS_DLGFRAME,     // remove
                       WS_BORDER,           // add (WS_BORDER)
                       SWP_NOMOVE);
    }
#else  //WINDOWS_PANEL_DECORATIONS
    if ( sets.m_isOnTop ) {
        ModifyStyleEx( WS_EX_TOPMOST ,          // Remove
                       0, 
                       SWP_NOMOVE);
    }
    else {
        ModifyStyleEx( 0 ,     // Remove  -  WS_EX_WINDOWEDGE, WS_EX_OVERLAPPEDWINDOW
                       WS_EX_TOPMOST,            // Add 
                       SWP_NOMOVE);
    }
#endif //WINDOWS_PANEL_DECORATIONS

    PositionWindow(SWP_NOSIZE);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::OnPauseToggle() 
{
    m_isSkipDraw = !m_isSkipDraw;
}

//////////////////////////////////////////////////////////////////////////////
void CPanel::OnFullToggle() 
{
    sets.m_FullSet = ! sets.m_FullSet;
    ResetPanel();
}

//////////////////////////////////////////////////////////////////////////////
void CPanel::OnShowProcessesToggle() 
{
    sets.m_isShowProcesses = ! sets.m_isShowProcesses;
    Reinit();
}

//////////////////////////////////////////////////////////////////////////////
void CPanel::OnLabelsToggle() 
{
    sets.m_ShowLabels = !sets.m_ShowLabels;
    Reinit();
}

//////////////////////////////////////////////////////////////////////////////
void CPanel::OnPopupToolTipsToggle() 
{
    sets.m_ShowToolTips = !sets.m_ShowToolTips;
    if ( !sets.m_ShowToolTips )
        ClearToolTips();
    Reinit();
}

////////////////////////////////////////////////////////////
void CPanel::OnShowValuesToggle() 
{
    sets.m_NoNumbersShow = !sets.m_NoNumbersShow;   
    if ( !CheckDispMode(DISP_GLINT) )   {
        Reinit();
    }
}

//////////////////////////////////////////////////////////////////////////////
void CPanel::OnLargeViewToggle() 
{
    sets.m_LargeView = ! sets.m_LargeView;
    Reinit();
}

//////////////////////////////////////////////////////////////////
void CPanel::OnShowGraphsToggle()
{
    sets.m_DispMode = DISP_GRAPH;
    Reinit();
}

/////////////////////////////////////////////////////////////////
void CPanel::OnPopup1ColumnsToggle() 
{
    sets.m_DispMode = DISP_COLUMN;
    Reinit();
}

/////////////////////////////////////////////////////////////////

void CPanel::OnPopupGlintlights() 
{
    sets.m_DispMode = DISP_GLINT;
    Reinit();
}

/////////////////////////////////////////////////////////////////
void CPanel::OnPopup1Changecomputer() 
{
    m_isReady = false;
    m_SettingsDlgOpen = true;

    CString  computer_name = "";

    // Ask for another computer name.
    {
        CRemoteConnectionDlg dlg(this, sets.m_ComputerName);
        while ( dlg.DoModal() != IDCANCEL )
        {
            if ( CheckConnection(dlg.m_machine) ) {
                computer_name = dlg.m_machine;
                optsets.m_computers.Add(computer_name);
                break;
            }
        }
    }

    if ( sets.m_ComputerName != computer_name )
    {
        sets.m_ComputerName = computer_name;
        ResetPanel();
    }

    m_SettingsDlgOpen = false;
    m_isReady = true;
    return;
}

/////////////////////////////////////////////////////////////////////////////
//  Simply rearrange panel window (inicators values preserved)
void CPanel::Reinit()
{
    FUNCTIONLOG(5, "CPanel::Reinit()");
    RemoveLists();
    InitPanel();
    RefreshIndicators();
    RedrawIndicators();
}

//////////////////////////////////////////////////////////////////////////////
bool CPanel::CheckComputerConnection()
{ 
    // Ask for another computer name.
    if ( CheckConnection(sets.m_ComputerName) )
        return true;

    // Ask for another computer name.
    CRemoteConnectionDlg dlg(this, sets.m_ComputerName);
    while ( dlg.DoModal() != IDCANCEL )
    {
        if ( CheckConnection(dlg.m_machine) ) {
            if ( strcmpi(sets.m_ComputerName, dlg.m_machine ) != 0 )
            {
                sets.m_ComputerName = dlg.m_machine;
                ResetPanel();
            }
            return true;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
//  Make full indicators reinitialization after their perf counters or computer is changed.
//  Inicators values are lost.
void CPanel::ResetPanel()
{
    FUNCTIONLOG(5, "CPanel::ResetPanel()");
    m_isReady = false;
    RemoveLists();
    MakeNewGadgets();
    g_TimeTicks = 0;
    InitPanel();

    RefreshIndicators();
    RedrawIndicators();
}

//////////////////////////////////////////////////////////////////////////////
// Reset default speed & brightness 
void CPanel::OnResetCmd() 
{
    if ( !CheckDispMode(DISP_GRAPH) )
    {
        sets.m_BrightCoeff          = DEFAULT_BRIGHT_COEFF;
        // sets.m_IndicTimeInterval = ( g_OSVersion == OS_VERSION_XP ) ? DEFAULT_INDIC_TIME_INTERVAL*0.85 : DEFAULT_INDIC_TIME_INTERVAL;
        sets.m_IndicTimeInterval    = DEFAULT_INDIC_TIME_INTERVAL;
        ResetIndicatorsTimer();
    }

    ResetPanel();   // reset all counters and values ??
}


////////////////////////////////////////////////////////////////////////////
void CPanel::OnDestroy() 
{
    FUNCTIONLOG(5, "CPanel::OnDestroy()");
    CWnd::OnDestroy();
    
    // TODO: Add your message handler code here

    PostQuitMessage(0); 
    EndModalLoop( 0 );
}

///////////////////////////////////////////////////////////////////

void CPanel::OnPopupSettings() 
{
    m_isReady = false;
    m_SettingsDlgOpen = true;
    optsets = sets;
    g_IsSetsChanged = false;

    SetForegroundWindow();

    AutoPtr<CSettingsDlg> dlg; 
    int res = IDCANCEL;
    {
        dlg = new CSettingsDlg("Glint Monitor Settings", this);
        res = dlg->DoModal();
    }

    if ( res == IDOK )
    {
        g_IsSetsChanged = dlg->WasAnythingModified();

        if ( g_IsSetsChanged )
        {
            delete  g_Gadgets;
            g_Gadgets = 0;

            sets = optsets;
            g_IsSchemaModified = dlg->IsSchemaModified();
            theApp.m_RestartPanel = true;

            //theApp.WriteProfileString(CURRENT_SETTINGS_SECTION, "Scheme", sets.m_CurrentSchema); // save last scheme name 
            //sets.SaveSettings(CURRENT_SETTINGS_SECTION);        // save current
            
            EndModalLoop(0);    // reinitialize the Panel
            return;
        }
    }

    // Redraw panel, because Gadgets could have been changed.
    m_isReady = true;
    RedrawWindow();                      

    //RedrawIndicators();
    RefreshIndicators();

    m_SettingsDlgOpen = false;
}

////////////////////////////////////////////////////////////////////////////
void CPanel::OnPaint() 
{
   DrawBorders(false);
}


////////////////////////////////////////////////////////////////////////////
void CPanel::OnAbout() 
{
   ShowAbout();
}

////////////////////////////////////////////////////////////////////////////
void ShowAbout() 
{
   CAbout aboutDlg(0);
   aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
//           I N I T I A L I Z E R S
/////////////////////////////////////////////////////////////////////////////

void CPanel::Initialize()
{
    FUNCTIONLOG(5, "CPanel::Initialize()");
    InitPanel();
    //RedrawIndicators();
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::InitPanel()
{
    m_isReady = false;
    m_isShowValuesText = false;
    FUNCTIONLOG(5, "CPanel::InitPanel()");
    //CleanPanelWindow();

    sets.m_OurFrequency = 1000.0 / sets.m_IndicTimeInterval;

    // -------------------------------------------------------------------
    //      If there are no availbale gadgets, make an empty window.
    // -------------------------------------------------------------------
    if ( g_Gadgets->IsEmpty() ) 
    {
        sets.m_PosWidth  = 50;
        sets.m_PosHeight = 20;

        CorrectWindowPosition();
        PositionWindow();
        RedrawWindow();

        SetMouseTracking(TME_LEAVE);    
        return;
    }

    // Message used to determine if explorer has restarted
    m_TaskbarCreatedMsg = RegisterWindowMessage(TEXT("TaskbarCreated"));

    // Put Tray Icon with current computer name.
    m_TrayIcon = new CTrayIcon; 
    m_TrayIcon->AddIcon(m_hWnd /*HWND*/, ICON_TRAY_ID, icon); 

    POINT offset_x;
    POINT offset_y;
    CRect coord0;
    short  light_distance_y = LIGHT_Y_DIST;

    if ( !sets.m_isMiniBar ) 
    {
        //===================== Regular view (not Mini-Bar) =====================
        coord0 = CRect(9, light_distance_y, 9 + LIGHT_X_SIZE, light_distance_y + LIGHT_Y_SIZE);    //  (left, top, right, bottom)
        offset_x = CPoint(LIGHT_X_SIZE + LIGHT_X_DIST, 0);
        offset_y = CPoint(0, LIGHT_Y_SIZE + light_distance_y);

        if ( !sets.m_ShowLabels ) 
        {
            coord0.top -= LABEL_Y_SIZE;
            coord0.bottom -= LABEL_Y_SIZE;
            offset_y.y -= LABEL_Y_SIZE;     // decrease Y distance, cause no labels
        }

        if ( !sets.m_NoNumbersShow && !CheckDispMode(DISP_GLINT) ) 
        {
            offset_y.y += VALUE_Y_SIZE + 2; 
            light_distance_y += VALUE_Y_SIZE + 2;
            m_isShowValuesText = true;
            if ( !sets.m_NoMaxNumbersShow) 
            {
                offset_y.y += VALUE_Y_SIZE + 3;
                coord0.top += VALUE_Y_SIZE + 1;
                coord0.bottom += VALUE_Y_SIZE + 1;
            }
        }

    }
    else 
    {
        //===================== Mini-Bar =====================
        light_distance_y = (PANEL_BORDER_VERT * 2) ;
        if ( sets.m_LargeView )
        {
            coord0 = CRect(0, 1, 18, 18 );    //  (left, top, right, bottom)    was: (1, 1, 19, 19 )
            offset_x = CPoint(23, 0);         //   was: (22, 0)
            offset_y = CPoint(0, 22);         //   was: (21, 0)
        }
        else
        {
            coord0 = CRect(0, 1, 11, 11);    //  (left, top, right, bottom)     was: (1, 1, 13, 13)
            offset_x = CPoint(15, 0);        // was: (15, 0)
            offset_y = CPoint(0, 15);        // was: (0, 15)
        }
    }

    if ( CheckDispMode(DISP_COLUMN) )
    {
        int width_delta = 0;
        if ( sets.m_LargeView )
        {
            width_delta = (coord0.right - coord0.left) * 0.42;
            offset_x.x -= (sets.m_isMiniBar) ? width_delta : width_delta*0.8;
        }
        else
        {
            width_delta = (coord0.right - coord0.left) * 0.38;
            offset_x.x -= (sets.m_isMiniBar) ? width_delta : width_delta*0.5;
        }
        coord0.right -= width_delta;
    }

    int k_visible = g_Gadgets->GetVisibleIndicsCount();
    ASSERT( k_visible > 0 );


    // ============================================
    //    Calculate number of rows
    // ============================================
    int kIndicatorsInRow =  sets.m_IndicatorsInRow;
    if ( sets.m_isMiniBar ) kIndicatorsInRow *= 1.5;

    int k_lights_rows = ( (sets.m_IndicatorsInRow == 1) ? k_visible : (k_visible / kIndicatorsInRow) );
    if (kIndicatorsInRow > 1 && (k_visible % kIndicatorsInRow) != 0 )
        k_lights_rows++;        // add last partially filled row

    int light_in_row = (k_visible + k_lights_rows - 1 ) / k_lights_rows ;


    // ===================================================================
    //    Iterate through Indicators Settings and create child windows
    // ===================================================================
    int row = 0;
    int col = 0;
    int max_col = 0;
    int max_row = 0;

    int i_ind = 0;
    CIndicator* ind;
    POSITION indListPos = g_Gadgets->m_indicators.GetHeadPosition();
    while ( indListPos )
    {
        CIndicator* ind = g_Gadgets->m_indicators.GetNext(indListPos);
        ASSERT(ind);
        CIndicatorSettings* indset = ind->m_indicSettings;

        if (ind->m_hWnd)
        {
            ind->ShowWindow( indset->IsVisible() ? TRUE : FALSE );
        }

        if ( !indset->IsVisible() )
        {
            continue;
        }

        // TRACE("[glint] CPanel::InitPanel(): Indic: '%s' \n", (LPCSTR)indset->m_strCounter );
        // =========== Calculate indicators Positions =============

        CRect r = coord0;
        CPoint  pt_borders(PANEL_BORDER_HORIZ, PANEL_BORDER_VERT);
        r += pt_borders;

        for (int i1=0; i1<col; i1++ ) r += offset_x;
        for (int i2=0; i2<row; i2++ ) r += offset_y;

        ASSERT( indset->IsVisible() );
        ind->InitIndicator(this, r, ID_INDICATOR_FIRST + i_ind);
        max_col = max(col, max_col);

        col++; 

        if ( !sets.m_isMiniBar ) {
            CRect rect_label(r.left, r.top - LABEL_Y_SIZE, r.right + 4, r.top - 1);
            CRect rect_v(r.left, r.bottom + VALUE_Y_SHIFT, r.right + 8, r.bottom + VALUE_Y_SHIFT + VALUE_Y_SIZE );
            CRect rect_v_max(r.left, r.top - VALUE_Y_SIZE - 1, r.right + 8, r.top - 1 );


            if ( !sets.m_NoNumbersShow && !CheckDispMode(DISP_GLINT) )
            {
                ValueText* value_text = new ValueText(ind);
                value_text->InitValueText(this, rect_v, "", ID_VALUETEXT_FIRST + i_ind );
                m_valueTexts.AddTail(value_text);        
                value_text->RedrawWindow();   //  ???
            if ( !sets.m_NoMaxNumbersShow) 
                {
                   ValueMaxText* value_max_text = new ValueMaxText(ind);
                   value_max_text->InitValueMaxText(this, rect_v_max, "", ID_VALUEMAXTEXT_FIRST + i_ind );
                   m_valueMaxTexts.AddTail(value_max_text);        
                   value_max_text->RedrawWindow();    //  ???
                   rect_label.top -= VALUE_Y_SIZE + 1;
                   rect_label.bottom -= VALUE_Y_SIZE + 1;
                }
            }

            if ( sets.m_ShowLabels )
            {
                Label* lbl = new Label(ind);
                lbl->InitLabel(this, rect_label, (LPCSTR)indset->m_strSmallTitle, ID_LABEL_FIRST + i_ind );
                m_labels.AddTail(lbl);        
                lbl->RedrawWindow();        // ???
            }

        }

        max_row = max(row, max_row);
        // prepare next position
        if ( col >= light_in_row) 
        {
            row++;
            col = 0; 
        }

        ind->RedrawWindow();    //  ???
        ++i_ind;
    }


    // ================================================
    //      Update windows position and size
    // ================================================
    CRect r = coord0;
    for (int i1=0; i1 < max_col; i1++ ) r += offset_x;
    for (int i2=0; i2 < max_row; i2++ ) r += offset_y;
    r.left = 0;
    r.top = 0;
    if ( !sets.m_isMiniBar ) 
        r += CRect(0,0, coord0.left + WINDOWS_BORDER_SIZE , light_distance_y - 10);
    else
        r += CRect(0,0, coord0.left + WINDOWS_BORDER_SIZE , light_distance_y + PANEL_BORDER_VERT - 1);

    // ================================================
    //     Calculate rect for Processes Windows
    // ================================================
    if ( sets.m_isShowProcesses ) {

        m_xProcesses = ( sets.m_isMiniBar ? r.right + PROCESSES_WIN_X_SHIFT : r.right + PROCESSES_WIN_X_SHIFT_FULL ) ;
        m_yProcesses = ( sets.m_isMiniBar ? PANEL_BORDER_VERT + 1 : WINDOWS_BORDER_SIZE + 2) ;
        if ( sets.m_LargeView ) m_yProcesses += 2; 

        r.right = m_xProcesses + PROCESSES_WIN_X_SIZE + WINDOWS_BORDER_SIZE + 2;       // add room for showing processes
        if ( sets.m_LargeView )
            r.right += 2;

        CRect rectProc( m_xProcesses, m_yProcesses, 
                        r.right - WINDOWS_BORDER_SIZE - 2, r.bottom - WINDOWS_BORDER_SIZE);


        m_ProcessesWin = new CProcessWindow;
        m_ProcessesWin->InitProcWin(this, rectProc, g_Gadgets);
        //m_ProcessesWin->RedrawWindow();
    }
    else
    {
        r.right += WINDOWS_BORDER_SIZE;
    }

#ifdef WINDOWS_PANEL_DECORATIONS
    if ( sets.m_isMiniBar || !sets.m_isShowProcesses ) 
           r.right += 2;    // slightly shift minBar right border
    if ( !sets.m_isMiniBar && !m_isShowValuesText)      // sets.m_ShowLights
           r.bottom += 4;   // slightly shift Panel bottom border
#else
    if ( sets.m_isMiniBar || !sets.m_isShowProcesses ) 
    {
           r.right += PANEL_BORDER_HORIZ;    // slightly shift minBar right border
    }
    if ( !sets.m_isMiniBar && !m_isShowValuesText)  // sets.m_ShowLights
    {
           r.bottom += PANEL_BORDER_VERT;       // slightly shift Panel bottom border
    }
#endif

    sets.m_PosWidth  = r.right;
    sets.m_PosHeight = r.bottom;

    // =========================================================================
    //    Try to stick Panel bottom to screen border area.
    // =========================================================================

    //    Try to stick Panel bottom to vertical border area.
    if ( g_CenterPosY + r.bottom/2 >= g_max_screen_Y - STICK_AREA_AT_SCREEN_Y ||
         g_CenterPosY + sets.m_PosHeight/2 >= g_max_screen_Y - STICK_AREA_AT_SCREEN_Y )
    {
        sets.m_PosY  += (sets.m_PosHeight - r.bottom);  // stick to bottom
    }    
    else if ( g_CenterPosY - r.bottom/2 <= g_min_screen_Y + STICK_AREA_AT_SCREEN_X ||
              g_CenterPosY - sets.m_PosHeight/2 <= g_min_screen_Y + STICK_AREA_AT_SCREEN_X )        
    {
        // sets.m_PosY  does not changed        // stick to top
    }    
    else if ( g_CenterPosY != INT_MIN )   // not the first show
    {
        if ( sets.m_LastMinibarPosY != INT_MIN )
        {
            sets.m_PosY = sets.m_LastMinibarPosY;
        }
        else
        {
            sets.m_PosY = g_CenterPosY - r.bottom/2;
        }
    }

    //  Try to stick Panel right to horizontal border area.
    if ( g_CenterPosX + r.right/2 >= g_max_screen_X - STICK_AREA_AT_SCREEN_X ||
         g_CenterPosX + sets.m_PosWidth/2 >= g_max_screen_X - STICK_AREA_AT_SCREEN_X )
    {
        sets.m_PosX  += (sets.m_PosWidth - r.right);    // stick to right
    }    
    else if ( g_CenterPosX - r.right/2 <= g_min_screen_X + STICK_AREA_AT_SCREEN_X ||
              g_CenterPosX - sets.m_PosWidth/2 <= g_min_screen_X + STICK_AREA_AT_SCREEN_X )        
    {
        // sets.m_PosX  does not changed        // stick to left
    }    
    else if ( g_CenterPosX != INT_MIN  ) // not the first show
    {
        if ( sets.m_LastMinibarPosX != INT_MIN )
        {
            sets.m_PosX = sets.m_LastMinibarPosX;
        }
        else
        {
            sets.m_PosX = g_CenterPosX - r.right/2;
        }
    }


    CorrectWindowPosition();
    PositionWindow();
    CleanPanelWindow();
    RedrawWindow();   

    SetMouseTracking(TME_LEAVE);    // TME_HOVER
    m_isReady = true;
}


/////////////////////////////////////////////////////////////////////////////
void CPanel::RefreshIndicators()
{
    // EnableToolTips(FALSE);
    // SetIcon( icon, TRUE );  SetIcon( icon, FALSE );

    if ( g_Gadgets == 0 || g_Gadgets->m_NumOfIndics == 0 || m_hWnd == 0 )
        return;
    
    FUNCTIONLOG(8, "CPanel::RefreshIndicators()");
    g_Gadgets->m_pfcDB->Read();
    bool is_data_ready = g_Gadgets->m_pfcDB->IsDataReady();
    if ( !is_data_ready )
        g_TimeTicks = 0;    // return counter back to zero, until data is ready

    CVisibleIndicatorsIterator iter(*g_Gadgets);
    CIndicator* ind;
    for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
    {
        ind->Counter()->MakeValue();
        if ( is_data_ready )
        {
            ind->RefreshData();
            if ( ind->m_UpdateDisplayedValues ) 
            {
                if (m_isReady && m_isShowValuesText) {
                    ind->m_ValueText->Update();
                if (!sets.m_NoMaxNumbersShow)
                    ind->m_ValueMaxText->Update();
                }
            }
        }
    }
    // EnableToolTips(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::RefreshProcess()
{
    FUNCTIONLOG(8, "CPanel::RefreshProcess()");
    if ( !sets.m_isShowProcesses || g_Gadgets->m_processesDB == 0 || 
          m_ProcessesWin == 0 || m_ProcessesWin->m_hWnd == 0)
    {
        return;
    }

    if ( g_Gadgets->m_processesDB->Read() > 0 && g_Gadgets->m_processesDB->GetData() )
    {
        g_Gadgets->m_processesDB->GetSortedData();  // Sort processes

        if ( m_ProcessesWin->m_hWnd != 0 )
            m_ProcessesWin->RedrawWindow();
    }
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::RemoveLists()
{
   m_isReady = false;
   ClearToolTips();

   LIST_DELETE(m_labels);
   LIST_DELETE(m_valueTexts);
   LIST_DELETE(m_valueMaxTexts);
   m_ProcessesWin = 0;
}

///////////////////////////////////////////////////////////////////////////////
void CPanel::CleanPanelWindow()
{
    // Clear area with black.
    CPaintDC dc(this);    
    CRect rect;
    GetFullRect(this, &rect);
    // rect.OffsetRect(-1,-1); 
    dc.FillRect( rect, &CBrush(CLR_BLACK) );
    
    if ( m_hWnd )   {
        Invalidate();
        RedrawWindow();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPanel::DrawBorders(bool   invalidate)
{
#define  DRAW_PANEL_BORDERS    dc.MoveTo( rect.TopLeft() ); \
    dc.LineTo( rect.right, rect.top); dc.LineTo( rect.BottomRight() ); \
    dc.LineTo( rect.left, rect.bottom); dc.LineTo( rect.TopLeft() );
#define  CONTRACT_RECT    rect.left++; rect.right--; rect.top++; rect.bottom--;
#define  DRAW_BORDER_LINE(p)    dc.SelectObject(p);   DRAW_PANEL_BORDERS; CONTRACT_RECT;

#ifndef WINDOWS_PANEL_DECORATIONS
    CPaintDC dc(this);    

    CRect  rect (0, 0, sets.m_PosWidth -1, sets.m_PosHeight - 1);
    // GetClientRect(&rect);
    dc.FillRect( rect, &CBrush(CLR_BLACK) );
    // rect.right--;     rect.bottom--;

    CPen pen_grey   (PS_SOLID,  1, sets.m_BorderLineColorDark );      
    CPen pen_bright (PS_SOLID,  1, sets.m_BorderLineColorLight);      

    // DRAW_BORDER_LINE(pen_grey); 
    DRAW_BORDER_LINE(pen_bright); 
    DRAW_BORDER_LINE(pen_grey); 
#endif //WINDOWS_PANEL_DECORATIONS
}

///////////////////////////////////////////////////////////////////////////////
void CPanel::ResetIndicatorsTimer()
{
    KillTimer(TIMER_COUNTERS);  
    SetTimer(TIMER_COUNTERS, sets.m_IndicTimeInterval, NULL);

    sets.m_OurFrequency = 1000.0 / sets.m_IndicTimeInterval;

    CVisibleIndicatorsIterator iter(*g_Gadgets);
    CIndicator* ind;
    for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
    {
        ind->m_TicksTextRefresh = ( sets.m_IndicTimeInterval >= sets.m_NumberShowInterval ) 
                  ? 1 : (sets.m_NumberShowInterval / sets.m_IndicTimeInterval + 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
//      M E S S A G E    H A N D L E R S
///////////////////////////////////////////////////////////////////////////////

LRESULT CPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

    // Restore taskbar icon if explorer is restarted
    if(message == m_TaskbarCreatedMsg)
    {
        m_TrayIcon->AddIcon(m_hWnd /*HWND*/, ICON_TRAY_ID, icon);       // reinstate tray icon
    }

    switch (message)
    {
        case ICON_TRAY_MSG_ID:          // ICON_TRAY_ID
            {
            // DEBUGOUTPUT("CPanel::WindowProc()  ICON_TRAY_MEASSAGE \n");
            HandleTrayEvent(message, wParam, lParam);           
            }
            break;

    }

    OUTLOG(99, " .... WindowProc. Msg:%x wParam:%x", message, wParam);
    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
            if ( m_isReady )
            {
                // Mouse event comes only if you mouse cursor is above Panel window.
                CPoint abs_point(sets.m_PosX + GET_X_LPARAM(lParam), 
                                 sets.m_PosY + GET_Y_LPARAM(lParam));   // abs point position

                MSG msg;
                msg.hwnd = m_hWnd;
                msg.message = message;
                msg.wParam = wParam;
                msg.lParam = lParam;

                if ( sets.m_isShowProcesses && m_ProcessesWin && sets.m_ShowToolTips )
                {
                    bool isMouseHoverBefore = m_ProcessesWin->m_MouseHover;
                    if ( theApp.m_panel->m_ProcessesWin->Rect().PtInRect(abs_point) )
                    {
                        //if ( !isMouseHoverBefore )
                        {
                            m_ProcessesWin->m_ToolTip.RelayEvent(&msg);
                        }
                        m_ProcessesWin->m_MouseHover = true;
                    }
                    else
                    {
                        m_ProcessesWin->m_ToolTip.Pop();
                        m_ProcessesWin->m_MouseHover = false;
                        if ( isMouseHoverBefore && m_ProcessesWin->m_ToolTip.m_hWnd != 0 )
                        {
                            m_ProcessesWin->RedrawWindow();
                        }
                    }
                }
                
                CVisibleIndicatorsIterator iter(*g_Gadgets);
                CIndicator* ind;
                for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
                {
                    if ( ind->m_hWnd == 0 )
                        continue;

                    bool isMouseHoverBefore = ind->m_MouseHover;

                    if (  (ind->m_Rect).PtInRect(abs_point) ||
                          ( sets.m_ShowLabels && ind->m_Label != 0 && (ind->m_Label->Rect().PtInRect(abs_point)) ) ||
                          ( !sets.m_NoNumbersShow && ind->m_ValueText != 0 && (ind->m_ValueText->Rect().PtInRect(abs_point)) ) || 
                          ( !sets.m_NoNumbersShow && ind->m_ValueMaxText != 0 && (ind->m_ValueMaxText->Rect().PtInRect(abs_point)) )
                        )
                    {
                        if (!isMouseHoverBefore)
                        {
                            ind->m_MouseHover = true;
                            ind->m_ForceDrawToolTip = true;
                            if ( sets.m_ShowToolTips ) 
                                ind->m_ToolTip.RelayEvent(&msg);
                            if ( !sets.m_isMiniBar )
                            {
                               ind->RedrawLabels();
                            }
                            ind->RedrawWindow();
                        }
                    }
                    else
                    {
                        if ( sets.m_ShowToolTips ) 
                            ind->m_ToolTip.Pop();
                        ind->m_MouseHover = false;
                        if ( isMouseHoverBefore && ind->m_ToolTip.m_hWnd != 0 )
                        {
                            if ( !sets.m_isMiniBar )
                            {
                                ind->RedrawLabels();
                            }
                            ind->RedrawWindow();
                        }
                        continue;
                    }
                }
            }
            SetMouseTracking(TME_LEAVE);
            break;

        case WM_MOUSEHOVER:     // does not work! 
            SetMouseTracking(TME_LEAVE);
            break;

        case WM_MOUSELEAVE:
            if ( m_isReady )
            {
                CVisibleIndicatorsIterator iter(*g_Gadgets);
                CIndicator* ind;
                for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
                {
                    ind->m_MouseHover = false;
                    if ( !sets.m_isMiniBar )
                    {
                       ind->RedrawLabels();
                    }
                }
            }
            SetMouseTracking(TME_HOVER);
            break;

        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
            if (  GetScreenMetrics() && CorrectWindowPosition() )
            {
                PositionWindow(SWP_NOSIZE);
            }
            break;

        case TTN_GETDISPINFO:
            {
                TRACE(" TTN_GETDISPINFO  \r\n");
            }

    }


    return CWnd::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CPanel::HandleTrayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    //  PostMessage( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
    if (m_SettingsDlgOpen || g_IsMenuActive)
    {
        return;
    } 

    switch (lParam)
    {
/*
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
        if ( sets.m_isMiniBar ) 
        {
            m_TempNormalView = true;
            PostMessage( WM_KEYDOWN, 'M', 0 );  
        }
        break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
        SetForegroundWindow();
        if ( !sets.m_isMiniBar && m_TempNormalView) 
        {
            m_TempNormalView = false;
            PostMessage( WM_KEYDOWN, 'M', 0 );  
        }
        break;
*/

    case WM_RBUTTONDOWN:
        {
        POINT pt;
        GetCursorPos(&pt);   // cursor position
        SetForegroundWindow();
        PostMessage( WM_CONTEXTMENU, (DWORD)m_hWnd, MAKELPARAM(pt.x, pt.y) );
        }
        break;

/*
    case WM_LBUTTONDOWN:
        SetForegroundWindow();
        // if ( (wParam & MK_CONTROL) != 0)
        {
            // if ( !sets.m_isMiniBar && m_TempNormalView) 
            {
                m_TempNormalView = false;
                PostMessage( WM_KEYDOWN, 'M', 0 );  
            }
        }
        break;
*/


    // case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        // OnPopupSettings();      // double click in tray brings settings
        // PostMessage( WM_CONTEXTMENU, (DWORD)m_hWnd, MAKELPARAM(sets.m_PosX, sets.m_PosY) );
        SetForegroundWindow();
        PostMessage( WM_KEYDOWN, 'M', 0 );  
        break;

    case WM_MOUSEMOVE:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPanel::SetMouseTracking(DWORD flags)
{
    if ( flags == m_OldMouseTrackFlags )
        return;
    m_OldMouseTrackFlags = flags;

    // Track mouse on indicator
    TRACKMOUSEEVENT mouseEvent;   // tracking information
    mouseEvent.cbSize = sizeof(mouseEvent);
    mouseEvent.dwFlags = flags; // TME_HOVER | TME_LEAVE;
    mouseEvent.hwndTrack = m_hWnd;
    mouseEvent.dwHoverTime = 100;
    BOOL res = ::_TrackMouseEvent(&mouseEvent);  
    ASSERT(res);
}

//////////////////////////////////////////////////////////////////
void CPanel::ClearToolTips()
{
    FUNCTIONLOG(5, "CPanel::ClearToolTips()");
    if (m_ProcessesWin && m_ProcessesWin->m_ToolTip.m_hWnd != 0 )
    {
        m_ProcessesWin->m_ToolTip.Pop();
    }

    CVisibleIndicatorsIterator iter(*g_Gadgets);
    CIndicator* ind;
    for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
    {
        if ( ind->m_hWnd == 0 )
            continue;

        if ( ind->m_ToolTip.m_hWnd != 0 )
            ind->m_ToolTip.Pop();
    }
}

//////////////////////////////////////////////////////////////////

void CPanel::RedrawIndicators()
{
    FUNCTIONLOG(8, "CPanel::RedrawIndicators()");

    CVisibleIndicatorsIterator iter(*g_Gadgets);
    CIndicator* ind;
    for (  int i_ind = 0; iter.GetNext(&ind); i_ind++ ) 
    {
        OUTLOG(55, "  ^MMM^ CPanel::RedrawIndicators(): Indicator '%s'", (LPCTSTR)ind->m_indicSettings->m_strIndicTitle );
        ind->RedrawWindow();
    }
}


//////////////////////////////////////////////////////////////////
void CPanel::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    CWnd::OnClose();
}

//////////////////////////////////////////////////////////////////
void CPanel::OnEndSession(BOOL bEnding)
{
    CWnd::OnEndSession(bEnding);
    // TODO: Add your message handler code here
    sets.FinalSave();
}
//////////////////////////////////////////////////////////////////
