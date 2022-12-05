// GraphValues.cpp: implementation of the CGraphValues class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "glint.h"
#include "GraphValues.h"
#include "Settings.h"
#include "Indicator.h"
#include "PerfCount.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
//  CValuesArray
//////////////////////////////////////////////////////////////////////

CValuesArray::CValuesArray()
{
    m_filled = false;
    m_first = 0;
    m_last = -1;
    m_idx = - 1;    // for iteration
}

/////////////////////////////////////////////////////////////
void   CValuesArray::NewValue(double v) // add new value to cyclic buffer
{
    m_idx = -1;     // invalidate the iterator
    m_last++;

    if ( m_last >= MAX_GRAPH_VALUES )
    {
        m_last = 0;
        m_first = 1;
        m_filled = true;
    }

    m_values[m_last] = v;
    
    if ( m_filled )
    {
        m_first = m_last + 1;
        if ( m_first >= MAX_GRAPH_VALUES )
        {
            m_first = 0;
        }
    }
}

/////////////////////////////////////////////////////////////
bool   CValuesArray::Next()
{
    ASSERT( m_idx >= 0 );
    m_idx++;
    if ( m_idx == m_last ) 
    {
        m_idx = -1;
        return false;
    }

    if ( m_idx >= MAX_GRAPH_VALUES )
    {
        m_idx = 0;
    }
    return true;
}

/////////////////////////////////////////////////////////////
bool   CValuesArray::Prev()
{
    ASSERT( m_idx >= 0 );
    m_idx--;
    if ( m_idx == m_first ) 
    {
        m_idx = -1;
        return false;
    }

    if ( m_idx < 0 )
    {
        m_idx = MAX_GRAPH_VALUES - 1;
    }
    return true;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraphValues::CGraphValues(CIndicator*  indic)
:  m_indic(indic)
{
    m_IndSets = m_indic->m_indicSettings;
    InitGraphValues();
}

CGraphValues::~CGraphValues()
{

}

//////////////////////////////////////////////////////////////////////
void CGraphValues::Repaint(CWnd *wnd, CPaintDC &dc, CRect& rect)
{

    if ( m_vmax == 0 )
        m_vmax = 0.1;

    Repaint(wnd, dc, rect, m_vmax, m_vmin);
}

//////////////////////////////////////////////////////////////////////
void CGraphValues::Repaint( CWnd* wnd, CPaintDC &dc, CRect& rect, double vmax, double vmin)
{
    // ScrollWindow( int xAmount, int yAmount, LPCRECT lpRect = NULL, LPCRECT lpClipRect = NULL );
    if ( !m_vals.IsFilled() && m_vals.Count() < 1 )
        return;

    ASSERT(vmax >= vmin);
    m_vmax_show = vmax;
    m_vmin_show = (vmin == DBL_MAX) ? 0 : vmin ;

    // m_vmax_show -= fabs(m_vmax - m_vmin) * 0.05 * sets.m_GraphShiftMax;  // (0.01 - 0.5) of delta
    // m_vmin_show += fabs(m_vmax - m_vmin) * 0.05 * sets.m_GraphShiftMin;  // (0.01 - 0.5) of delta

    m_YMax = rect.bottom - 2;
    m_XMax = rect.right - 2;
    m_delta = ( m_vmax_show - m_vmin_show ) * 1.01 ;

    int k = SMALL_GRAPH_MAX;

    m_vals.ToLast();
    dc.MoveTo( m_XMax, CalcYPos(m_vals.Last() - 1 ) );

    while( m_vals.Prev() )
    {
        if ( --k < 0 ) 
            break;
        dc.LineTo( CalcXPos(k, SMALL_GRAPH_MAX), CalcYPos(m_vals.Current() - 1) );
    }

}

//////////////////////////////////////////////////////////////////////
void CGraphValues::NewValue(double v)
{
    
    m_vals.NewValue(v);
    m_SinceLastMax++;
    m_SinceLastMin++;

    double a = -2.0;
    double change_coeff;    // relative ratio of extremum change 

    // FindLastMax();
    // m_vmax = RoundTo( m_values[m_max], true );

    // FindLastMin();
    // m_vmin = RoundTo( m_values[m_min], false );

    if ( m_vmax < v )
    {
        m_SinceLastMax = 0;
        m_vmax = RoundTo( v, true );        // Save new rounded max
    }
    if ( m_vmin > v )
    {
        m_SinceLastMin = 0;
        m_vmin = RoundTo( v , false );      // Save new rounded min
    }


    if (m_SinceLastMax > SMALL_GRAPH_MAX )
    {   
        a = FindLastMax(SMALL_GRAPH_MAX);
        if ( a < EPSILON && 
             ( FLAG_CHECK(m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS) ||
               m_IndSets->m_IndMaxV == 0.0 )
            )
        {
            // Making zero values graph lookin reasonable.
            m_vmin = 0.0;
            m_vmax = 0.1;
            m_SinceLastMin = 0;
            return;
        }

        change_coeff = ( fabs(m_vmax - m_vmin) > EPSILON ) ?
                        fabs((a - m_vmax)/(m_vmax - m_vmin)) : 1.0;

        if ( change_coeff > LESS_COEFF_TO_ADJUST_GRAPH_MAX )
        {
            // gradually decrease graph maximum.
            m_vmax -= fabs(m_vmax - a) * LESS_COEFF_TO_ADJUST_GRAPH_MAX;  
            m_SinceLastMax = SMALL_GRAPH_MAX;                             // repeat next time
        }
        else if ( change_coeff > 0.05  )
        {
            m_vmax = RoundTo( a, true );    // maximum significantly decreased
        }

    }

    if (m_SinceLastMin > SMALL_GRAPH_MAX )
    {
        a =  FindLastMin(SMALL_GRAPH_MAX);  // Save new rounded min
        ASSERT( a >= m_vmin ); 

        change_coeff = ( fabs(m_vmax - m_vmin) > EPSILON ) ?
                        fabs((a - m_vmin)/(m_vmax - m_vmin)) : 1.0;

        if ( m_vmax > EPSILON && a/m_vmax < 0.25 )
        {
            m_vmin = 0.0;
        }
        else if ( change_coeff > 0.5 )
        {
            m_vmin += fabs(a - m_vmin) * 0.5;       // gradually increase
            m_SinceLastMin = SMALL_GRAPH_MAX;   // repeat next time
        }
        else if ( change_coeff > 0.05 )
        {
            m_vmin = RoundTo(a, false);   // minimum significantly decreased
        }
        
        // At last simple correction
    }

    if ( m_vmin > m_vmax )
    {
        m_vmax = RoundTo( FindLastMax(SMALL_GRAPH_MAX), true) ;
        m_vmin = RoundTo( FindLastMin(SMALL_GRAPH_MAX), false);
    }

    if ( !FLAG_CHECK(m_IndSets->m_flags,INDSW_LOCAL_VARIATIONS) &&
          m_IndSets->m_IndMaxV != 0 && m_vmax < m_IndSets->m_IndMaxV )
    {
        //TRACE("CGraphValues::NewValue.  Corrected vmax from :%f \r\n", m_vmax); 
        m_vmax = m_IndSets->m_IndMaxV;

        m_indic->m_counter->m_maxVal = m_vmax;
        m_indic->m_counter->m_maxValUsed = m_vmax;
    }
}

//////////////////////////////////////////////////////////////////////
void CGraphValues::InitGraphValues()
{
    // m_bitmap = 0;
    m_vmax_show = m_vmax = 0;
    m_vmin_show = 0;
    m_vmin = DBL_MAX;
    m_SinceLastMax = 0;
    m_SinceLastMin = 0;
}

//////////////////////////////////////////////////////////////////////
double CGraphValues::FindLastMax(int n)
{
    double fm = 0;
    double a = 0;
    m_vals.ToLast();
    fm = m_vals.Current();

    for( int k =0; k < n && m_vals.Prev(); k++ )
    {
        a = m_vals.Current();
        if ( a > fm )
        {
            fm = a;
            m_SinceLastMax = k;
        }
    }
    return fm;
}

//////////////////////////////////////////////////////////////////////
double CGraphValues::FindLastMin(int n)
{
    double fm = 1.0E+27;
    double a = 0;

    m_vals.ToLast();
    fm = m_vals.Current();

    for( int k =0; k < n && m_vals.Prev(); k++ )
    {
        a = m_vals.Current();
        if ( a < fm )
        {
            fm = a;
            m_SinceLastMin = k;
        }
    }
    return fm;
}

//////////////////////////////////////////////////////////////////////
int CGraphValues::CalcYPos(double val)
{
    int iy0 = m_YMax - (int) ( m_YMax * ( val - m_vmin_show ) / m_delta) ;
    if ( iy0 >= m_YMax )
        iy0 = m_YMax;
    else if ( iy0 < 1 )
        iy0 = 1;
    return iy0;
}

//////////////////////////////////////////////////////////////////////
int CGraphValues::CalcXPos(int idx, int idx_max)
{
    int ix0 = (idx * m_XMax)/idx_max + 1 ;      // + 1 ?
    ASSERT(ix0 <= m_XMax);
    if ( ix0 > m_XMax ) ix0 = m_XMax;
    return ix0;
}

//////////////////////////////////////////////////////////////////////
double CGraphValues::RoundTo(double val, bool up)   // up == false => round to less value
{
    double a = fabs(m_vmax - m_vmin);
    if ( up )
    {
        if ( m_vals.IsFilled() )
            return (val + a * 0.1);
        else
            return (val*1.002);
    }
    else
    {
        if ( m_vals.IsFilled() )
        {
            double b = val - a * 0.1;
            return ( b > EPSILON ?  b : 0.0 );
        }
        else
            return (val*0.998);
    }

 /*
    double a = log10(val);
    int k;
    if ( up)
    {
        k = ceil(a*10) + 1; // round to the next 0.1 of the value
    }
    else
    {
        k = ceil(a*10) -1;  // round to the previous 0.1 of the value
    }
    double m = pow(10, (float)k / 10.0 );
    return m;
 */
}
