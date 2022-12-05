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
// CIndicator

CIndicator::CIndicator(CPerfCount*  counter, CIndicatorSettings* settings)
{
    m_counter = counter;
    m_indicSettings = settings;
    ASSERT(settings && counter);

    m_graph = new CGraphValues(this);

    m_parent  = 0;
    m_AvgVal  = 0.0;
    m_LastTipValue = 0.0;
    m_LastAvgVal = 0.0;
    m_MouseHover = false;
    m_TicksTextRefresh = ( sets.m_IndicTimeInterval >= sets.m_NumberShowInterval ) ? 1 : (sets.m_NumberShowInterval / sets.m_IndicTimeInterval + 1);
    m_Label = 0;
    m_ValueText = 0;
    m_ValueMaxText = 0;
    m_UpdateDisplayedValues = false;

    m_ForceDrawToolTip   = false;
    m_UpdateToolTip = false;
}

CIndicator::~CIndicator()
{   
    FUNCTIONLOG(8, "CIndicator::destr");
    m_ToolTip.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CIndicator, CStatic)
    //{{AFX_MSG_MAP(CIndicator)
    ON_WM_PAINT()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndicator message handlers

void CIndicator::InitIndicator(CWnd* parent, CRect& r, UINT id)
{
    m_Label = 0;
    m_ValueText = 0;
    m_ValueMaxText = 0;
    m_UpdateDisplayedValues = false;
    m_ForceDrawToolTip = true;
    m_UpdateToolTip = false;

    bool firstCreation = (m_parent == 0);
    if ( !firstCreation )
    {
        DestroyWindow();
    }

    if ( m_indicSettings == 0 || !m_indicSettings->IsVisible() )
        return;

    if ( Create( "?", WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_BORDER | SS_BLACKRECT,   // SS_BLACKFRAME,
                 r,  parent, id ) == FALSE)
    { 
        DATA_ERR_MESSAGE_BOX("CIndicator not created");
        return;
    }

    m_parent  = parent;
    GetWindowRect(&m_Rect);
    GetFullRect(this, &m_ClientRect);    //GetClientRect(&m_ClientRect);
    m_ClientRect.OffsetRect(-1,-1); 

    //this.CreateEx(WS_EX_CLIENTEDGE, "STATIC", 
    //                   NULL /*lpszWindowName*/, 
    //                   r.left, r.top, r.Width(), r.Height(), 
    //                   m_parent, NULL, NULL); 
    ShowWindow(SW_SHOW); 

    FormatTipText();
    m_ToolTip.Init(parent, id);
}

////////////////////////////////////////////////////////
// Returns 'true' if value text should be updated. 
void  CIndicator::RefreshData()
{
    m_UpdateDisplayedValues = false;
    m_UpdateToolTip         = false;

    m_AbsValue = Counter()->GetAbsoluteValue();
    m_rval = Counter()->GetRelativeValue();

    if ( CheckDispMode(DISP_GLINT) && fabs( sets.m_BrightCoeff - 1.0 ) > 0.05 )
    {
        m_rval = pow(m_rval, sets.m_BrightCoeff * BRIGHT_COEFF_INCREASE );        
    }

    m_UpdateDisplayedValues = g_TimeTicks > 1 && ((g_TimeTicks-1) % m_TicksTextRefresh == 0) ;

    if ( m_UpdateDisplayedValues ) 
    {
        m_LastAvgVal = m_AvgVal / m_TicksTextRefresh;
        m_AvgVal = 0.0;
        if ( sets.m_ShowToolTips && m_hWnd != 0 &&
             m_parent && m_MouseHover && m_ToolTip.m_hWnd != 0 )
        {
            m_UpdateToolTip = FormatTipText();
        }

        m_graph->NewValue(m_LastAvgVal);
    }
    m_AvgVal += m_AbsValue;

    // -------------------------------------
    //    Update window, if necessary
    // -------------------------------------
    if ( m_hWnd != 0 && theApp.m_panel->IsReady() && m_counter->IsActive() ) 
    { 
        if ( CheckDispMode(DISP_GRAPH) )
        {
            if ( m_UpdateDisplayedValues ) 
            {
                RedrawWindow();
            }
        }
        else
        {
            RedrawWindow();
        }
    }

    Counter()->AdjustMax();
 /*
    // Draw Borders
    CPaintDC dc(this); // device context for painting
    if ( CheckDispMode(DISP_GRAPH) ) 
    {
        dc.FrameRect( m_ClientRect, 
                     &CBrush( (m_MouseHover)  ? CLR_GREY_LIGHT : CLR_GREY) );
    }
    else
    {
        dc.FrameRect( m_ClientRect, 
                     &CBrush( (m_MouseHover)  ? CLR_GREY_LIGHT : sets.m_bordersColor) );
    }
 */
}

///////////////////////////////////////////////////////////////////////////
void CIndicator::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    FUNCTIONLOG( 55, "CIndicator::OnPaint");
    if ( m_indicSettings == 0 || !m_indicSettings->IsVisible() )
        return;

    if ( !theApp.m_panel->IsReady() || !m_counter->IsActive() ||
         ( !CheckDispMode(DISP_GRAPH) && m_counter->SampleCount() < 2  )
       )
        return;

    OUTLOG(55, "  ####  CIndicator '%s'  PAINT mode=%d  sample=%d ", 
           (LPCTSTR)m_indicSettings->m_strIndicTitle, sets.m_DispMode, m_counter->SampleCount() );

    // ************** Graphs *****************
    if ( CheckDispMode(DISP_GRAPH) ) 
    {
        dc.FillRect( m_ClientRect, &CBrush(CLR_BLACK) );
        CPen pen(PS_SOLID,  1, m_indicSettings->m_color );
        dc.SelectObject(pen);

        dc.SetTextColor(m_indicSettings->m_color); 
        dc.SetBkColor(CLR_BLACK); 

        if (   !FLAG_CHECK( Counter()->m_flags, PFCFLG_MAX_UNDEFINED)  ||
             ( !FLAG_CHECK(m_indicSettings->m_flags, INDSW_ADJUST_MAX)  && 
               !FLAG_CHECK(m_indicSettings->m_flags, INDSW_LOCAL_VARIATIONS )  )
           )
        {
            m_graph->m_vmax = Counter()->m_maxVal;
            m_graph->m_vmin = 0;
            m_graph->Repaint(this, dc, m_ClientRect, Counter()->m_maxVal, 0 );
        }
        else
        {
            m_graph->Repaint(this, dc, m_ClientRect );
        }
        dc.FrameRect( m_ClientRect, 
                     &CBrush( (m_MouseHover)  ? CLR_ACTIVE_GRAPH_BORDER : CLR_INACTIVE_GRAPH_BORDER) );
    }

    // ************** Columns *****************
    else if ( CheckDispMode(DISP_COLUMN) )
    {
        dc.FillRect( m_ClientRect, &CBrush(CLR_BLACK) );
        CRect r = m_ClientRect;
        r.top++;
        r.bottom--;
        r.right--;
        r.left++;

        int new_top = r.bottom - (r.bottom - r.top) * m_rval;
        r.top = max(new_top, r.top) ;

        if ( r.top <= r.bottom )
        {
            dc.FillRect( r, &CBrush(m_indicSettings->m_color) );
        }
        dc.FrameRect( m_ClientRect, 
                     &CBrush( (m_MouseHover)  ? sets.m_hoverBordersColor : sets.m_bordersColor) );
    }

    // ************** Glint Lights *****************
    else if ( CheckDispMode(DISP_GLINT) )
    {

        #if defined(_DEBUG) && 0
        if (  strcmp( Counter()->m_strCounter, "% Processor Time") == 0 )
            OUTLOG(999, " ... CPU counter: AbsValue=%g  rval=%g  max=%g LastAvgVal=%g ", m_AbsValue, m_rval, Counter()->m_maxVal, m_LastAvgVal);
        #endif

        COLORREF clr = CalcColor( m_indicSettings->m_color, m_rval, m_indicSettings->m_flags);
        dc.SetBkColor(clr); 
        dc.FillRect( m_ClientRect, &CBrush(clr) );


        // Ouput absolute value 
        if ( !sets.m_isMiniBar && g_TimeTicks > (m_TicksTextRefresh+1) && !sets.m_NoNumbersShow) 
        {
            CFont* oldFont = (CFont*)dc.SelectObject(sets.m_LargeView ? &g_font2L : &g_font2);
            COLORREF txtclr = CLR_GREY_DARK;
            //if ( m_rval < 0.02 )
            //    txtclr = CLR_GREY_LIGHT; 
            // else 
            if ( m_rval < 0.40 )
                txtclr = CLR_GREY_LIGHT; // CLR_WHITE   
            else // if ( m_rval < 0.85 )
                txtclr = CLR_BLACK;      // CLR_GREY
            dc.SetTextColor(txtclr);
            
            // if ( fabs(m_LastAvgVal) > 1E-12 )
            LPCTSTR sz_val = GetSmallValueStr(m_LastAvgVal);

            CRect rct(m_ClientRect);
            rct.top  = 2;
            rct.left = 1;
            rct.right -= 1;
            dc.DrawText( GetSmallValueStr(m_LastAvgVal), -1, rct, DT_VCENTER );

            // dc.SelectObject( oldFont );
        }

        dc.FrameRect( m_ClientRect, 
                     &CBrush( (m_MouseHover)  ? sets.m_hoverBordersColor : sets.m_bordersColor) );
    }
    
    if ( m_parent && m_MouseHover && (m_ForceDrawToolTip || m_UpdateToolTip) )
    {
        if ( m_ForceDrawToolTip )
        {
            FormatTipText();    // prepare tool tip text for the 1st time 
        }

        OUTLOG(55, " ** CIndicator::OnPaint  UpdateTipText() ** ");
        //TRACE(" @@@ CIndicator::OnPaint  UpdateTipText() ticks:%05d \r\n", (GetTickCount()% 100000) );
        
        m_ToolTip.UpdateTipText( m_ToolTip.m_TipText, m_parent);
        //m_ToolTip.Update();   -- do not call this

        m_ForceDrawToolTip  = false;
        m_UpdateToolTip     = false;
    }

    // Do not call CStatic::OnPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
// Compute color from base to 
COLORREF CalcColor(COLORREF c0, double val, UINT flags)
{
#define CCCALC(v, c) int v = (coeff*c); if (v > 255) isColorMax = true;

    float coeff ;
    float color;

    bool isColorMax = false;    // true when 255 was reached on any basic color value (R,G,B)
    if ( val <= 0.02) 
        return CLR_BLACK;
    if (val > 0.99) {
        // if (flags & INDSW_MAXWHITE) return CLR_WHITE;
    }

    coeff = val;
    // coeff = val * val;
    // coeff = 1.0 - (1.0 - val) * (1.0 - val); // soft

    // Add harshness
    int ic = (int) (23.1 * coeff ) + 7.0;
    coeff = (float)ic / 30.0;

    CCCALC(c_r, (GetRValue(c0)) ); 
    CCCALC(c_g, (GetGValue(c0)) ); 
    CCCALC(c_b, (GetBValue(c0)) ); 

    COLORREF clr = c0;
    if ( !isColorMax )
        clr = RGB(c_r, c_g, c_b );
    return clr;
}
#undef CCCALC


///////////////////////////////////////////////////////////////////////////// 
// Prepare tool tip text.
// Returns false if show value is unchanged.
bool CIndicator::FormatTipText()
{
    FUNCTIONLOG(20, "CIndicator::FormatTipText()");
    ASSERT(m_indicSettings);
    double v = m_LastAvgVal;
    bool res = false;
    char        m_lastTipText[INDIC_TIP_SIZE+2];
    strcpy( m_lastTipText, m_ToolTip.m_TipText);    // save previous

    CString compname = "";
    if ( sets.m_ComputerName.GetLength() > 0 )
    {
        compname = "\\\\" + sets.m_ComputerName + "\\"; 
    }

    if ( CheckDispMode(DISP_GRAPH) ) 
    {
        CString strVal = GetSmallValueStr(v);
        CString strMin = ( ( m_graph->m_vmin == DBL_MAX ) ? "0" : GetSmallValueStr(m_graph->m_vmin) );
        CString strMax = GetSmallValueStr(m_graph->m_vmax);
        _snprintf(  m_ToolTip.m_TipText, 
                    INDIC_TIP_SIZE, 
                    "%s%s = %s  {%s , %s}",
                    (LPCTSTR)compname,
                    (LPCTSTR)m_indicSettings->m_strIndicTitle,
                    (LPCTSTR)strVal,
                    (LPCTSTR)strMin,
                    (LPCTSTR)strMax);
        m_LastTipValue = 1E+27;
        res = true;
    }

    else
    {
        if ( CheckDispMode(DISP_COLUMN) ) 
        {
            double  v_max = m_counter->GetCurrentMaxValue();
            CString strValMax = GetSmallValueStr( v_max );
            _snprintf(  m_ToolTip.m_TipText, 
                        INDIC_TIP_SIZE, 
                        "%s%s = %s {%s}", 
                        (LPCTSTR)compname,
                        (LPCTSTR)m_indicSettings->m_strIndicTitle,
                        GetSmallValueStr(v), 
                        (LPCTSTR)strValMax);
            res = (m_LastTipValue != v); 
            m_LastTipValue = v + m_counter->m_maxVal * 1000000;
        }
        else
        {
            _snprintf(m_ToolTip.m_TipText, INDIC_TIP_SIZE, "%s%s = %s" , 
                        (LPCTSTR)compname,
                        (LPCTSTR)m_indicSettings->m_strIndicTitle,
                        GetSmallValueStr(v) ); 
            res = (m_LastTipValue != v); 
            m_LastTipValue = v;
        }
    }
    

#ifdef OLD_VALUES_TOOLTIPS
    if ( m_indicSettings->m_strInst.GetLength() > 0 )
    {
        _snprintf(m_ToolTip.m_TipText, INDIC_TIP_SIZE, "%s%s: %s (%s) = %s" , 
                    (LPCTSTR)compname,
                    (LPCSTR)m_indicSettings->m_strCategory,
                    (LPCSTR)m_indicSettings->m_strCounter,
                    (LPCSTR) m_indicSettings->m_strInst,
                    GetSmallValueStr(v)
                    ); 
    }
    else
    {
        _snprintf(m_ToolTip.m_TipText, INDIC_TIP_SIZE, "%s%s: %s = %s" , 
                    (LPCTSTR)compname,
                    (LPCSTR)m_indicSettings->m_strCategory,
                    (LPCSTR)m_indicSettings->m_strCounter,
                    GetSmallValueStr(v)
                    ); 
    }
#endif //OLD_VALUES_TOOLTIPS

    return ( strcmp( m_lastTipText, m_ToolTip.m_TipText) != 0 );
}


///////////////////////////////////////////////////////////////////////////// 
int CIndicator::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    // CPaintDC dc(this); // device context for painting // is it allowed here ?

    // TODO: Add your specialized creation code here
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////
void CIndicator::RedrawLabels()
{
    FUNCTIONLOG(55, "CIndicator::RedrawLabels()");
    if ( m_indicSettings == 0 || !m_indicSettings->IsVisible() )
        return;

    if ( m_hWnd == 0 )
        return;

   OUTLOG(55, "  ^^^^^ Indicator '%s'  RedrawLabels()", (LPCTSTR)m_indicSettings->m_strIndicTitle );

    if ( m_Label && m_Label->m_hWnd )
        m_Label->RedrawWindow();

    if ( m_ValueText && m_ValueText->m_hWnd )
        m_ValueText->RedrawWindow();

    if ( m_ValueMaxText && m_ValueMaxText->m_hWnd )
        m_ValueMaxText->RedrawWindow();
}


