// GraphValues.h: interface for the CGraphValues class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHVALUES_H__1E9723EA_EA3A_4A25_A53E_A63E3FE56100__INCLUDED_)
#define AFX_GRAPHVALUES_H__1E9723EA_EA3A_4A25_A53E_A63E3FE56100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AutoPtr.h"

#define MAX_GRAPH_VALUES    48
#define SMALL_GRAPH_MAX     36      // how many to show in normal mode

class   CIndicator;
class   CIndicatorSettings;

////////////////////////////////////////////////////////////////
//    C V a l u e s A r r a y
////////////////////////////////////////////////////////////////
class CValuesArray  
{
public:
    CValuesArray(); 
    virtual ~CValuesArray() {};

    void   NewValue(double v);  // add new value to cyclic buffer

    double operator[]( int i) { return m_values[i]; }
    bool   IsFilled() { return m_filled; };
    int    Count() { return (m_filled ? MAX_GRAPH_VALUES : m_last); };
    double Last() 
            { 
            ASSERT(m_last >= 0);
            return m_values[m_last]; 
            };

// Navigation & Iteration funcs
    void   MoveTo(int idx) { m_idx = idx; }
    void   ToFirst()
                { 
                ASSERT(m_last >= 0);
                m_idx = m_first; 
                };
    void   ToLast() 
                { 
                ASSERT(m_last >= 0);
                m_idx = m_last; 
                };

    bool   Next();
    bool   Prev();
    double Current() 
            { 
            ASSERT(m_idx >= 0);
            return m_values[m_idx]; 
            };


protected:
    int     m_first;
    int     m_last;
    bool    m_filled;   // true: if buffer is full and began to cycle

    double  m_values[MAX_GRAPH_VALUES + 1];

    int     m_idx;  // for iteration
};

////////////////////////////////////////////////////////////////
//    C G r a p h V a l u e s
////////////////////////////////////////////////////////////////

class CGraphValues  
{
public:
    CGraphValues(CIndicator*  indic);
    virtual ~CGraphValues();
    void InitGraphValues();

    void Repaint( CWnd* wnd, CPaintDC &dc, CRect& rect);
    void Repaint( CWnd* wnd, CPaintDC &dc, CRect& rect, double vmax, double vmin);

    int CalcXPos(int idx, int idx_max);
    int CalcYPos(double val);

    double FindLastMin(int n);
    double FindLastMax(int n);

   double GetDisplayMin() { return m_vmin_show; };
   double GetDisplayMax() { return m_vmax_show; };

    void     NewValue(double v);
    double RoundTo(double val, bool up);    // round double to some surrounding value


// DATA
public:
    double  m_vmin;
    double  m_vmax;
protected:

    CValuesArray    m_vals;
    int     m_SinceLastMax;
    int     m_SinceLastMin;
    
// TEMPORARY DATA FOR REPAINT
    double  m_delta;    // for Y calculation

    double  m_vmin_show;
    double  m_vmax_show;

    int     m_YMax;
    int     m_XMax;

    CIndicator*             m_indic;
    CIndicatorSettings*     m_IndSets;
    // AutoPtr<CBitmap> m_bitmap;
};

#endif // !defined(AFX_GRAPHVALUES_H__1E9723EA_EA3A_4A25_A53E_A63E3FE56100__INCLUDED_)
