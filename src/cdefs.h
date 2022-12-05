//////////////////////////////////////////////////////////////////////
//  cdefs.h: Common definitions
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDEFS_H__3B137A9C_EFCB_4B87_875A_66A23077DB68__INCLUDED_)
#define AFX_CDEFS_H__3B137A9C_EFCB_4B87_875A_66A23077DB68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LIMITS.H>
#include <FLOAT.H>

#include "Errors.h"

// ===============   B U I L D   M O D E   D E F I N E S ====================

#if !defined(_DEBUG)
   #define GLINT_NO_LOGGING              // switch off logging code
   #define GLINT_NO_DATA_ERR_MSG_BOXES   // switch off messages to descktop about data collection errors
#endif

// ------------------ Logging ----------------------
#define MAX_DATA_ERR_MESSAGE  400

#ifndef GLINT_NO_LOGGING
    #ifdef _DEBUG
        #define OUTLOG(lvl,frm,...)  { if ( g_LogLevel >= lvl && g_FLog ) {fprintf(g_FLog,"%*s  ",g_FuncLogNesting*3,""); fprintf(g_FLog,frm,__VA_ARGS__); fputs("\n",g_FLog); } }
    #else
        #define OUTLOG(lvl,frm,...)  { if ( g_LogLevel >= lvl && g_FLog ) {fprintf(g_FLog,"%*s  ",g_FuncLogNesting*3,""); fprintf(g_FLog,frm,__VA_ARGS__); fputs("\n",g_FLog); fflush(g_FLog); } }
    #endif
    #define FUNCTIONLOG(lvl,txt)  CFunctionLog autoFnctionLogObj(lvl,txt); // automatic tracer for entering/exiting a function
    #define BAD_COND_MESSAGE(cond) { if (cond) AfxMessageBox( #cond, MB_OK | MB_ICONSTOP); }
#else
    #define OUTLOG(lvl,...)  {  }
    #define FUNCTIONLOG(lvl,txt)
    #define BAD_COND_MESSAGE(cond)
#endif

// ---------------- Err msg boxes ------------------
#ifndef GLINT_NO_DATA_ERR_MSG_BOXES
    extern char g_dataErrMessage[MAX_DATA_ERR_MESSAGE + 20];
    #define DATA_ERR_MESSAGE_BOX(txt,...)  { sprintf(g_dataErrMessage,txt,__VA_ARGS__); OUTLOG(1,g_dataErrMessage); AfxMessageBox(g_dataErrMessage, MB_OK | MB_ICONSTOP); }
#else
    #define DATA_ERR_MESSAGE_BOX(txt,...)  OUTLOG(1,txt,__VA_ARGS__)
#endif

// =======================================================================
#define EPSILON 0.000001    // small value: if v < EPSILON, then zero is supposed

#define TIMER_COUNTERS      1
#define TIMER_PROCESSES     2

//#define WINDOWS_PANEL_DECORATIONS
#ifdef WINDOWS_PANEL_DECORATIONS
    #define PANEL_STYLE_0       (WS_POPUP | WS_VISIBLE | WS_BORDER)
    #define PANEL_EX_STYLE_0    (WS_EX_PALETTEWINDOW | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE)
    #define PANEL_BORDER_HORIZ     0
    #define PANEL_BORDER_VERT      0
    #define WINDOWS_BORDER_SIZE    5
    #define PROCESSES_WIN_X_SHIFT       2    // shift processes window to left for mini-bar
    #define PROCESSES_WIN_X_SHIFT_FULL  6    // shift processes window to left for full mode
#else
    #define PANEL_STYLE_0       (WS_POPUP | WS_VISIBLE | SS_BLACKRECT )  // SS_BLACKFRAME)
    #define PANEL_EX_STYLE_0    (WS_EX_PALETTEWINDOW)
    #define PANEL_BORDER_HORIZ     3
    #define PANEL_BORDER_VERT      3
    #define WINDOWS_BORDER_SIZE    PANEL_BORDER_VERT
    #define PROCESSES_WIN_X_SHIFT       3    // shift processes window to left for mini-bar
    #define PROCESSES_WIN_X_SHIFT_FULL  3    // shift processes window to left for  full mode
#endif //WINDOWS_PANEL_DECORATIONS

#define STICK_AREA_AT_SCREEN_X   80    // x distance from right where Panel sticks to the right
#define STICK_AREA_AT_SCREEN_Y   60     // x distance from Bottom where Panel sticks to the bottom

#define MIN_WINDOWS_X_SIZE  140 
#define MIN_WINDOWS_Y_SIZE  100 
#define MINIBAR_WINDOWS_Y_SIZE      20
#define MINIBAR_WINDOWS_Y_SIZE_L    26

#define LIGHT_X_SIZE    (sets.m_LargeView ? 38 : 24)
#define LIGHT_Y_SIZE    (sets.m_LargeView ? 36 : 23) 
#define LIGHT_X_DIST    (sets.m_LargeView ? 9 : 8) 
#define LIGHT_Y_DIST    (sets.m_LargeView ? 23 : 20)
#define LABEL_Y_SIZE    (sets.m_LargeView ? 16 : 13)
#define VALUE_Y_SIZE    (sets.m_LargeView ? 15 : 12)
#define VALUE_Y_SHIFT   (sets.m_LargeView ?  3 :  2)

#define PROCESSES_Y_DIST (sets.m_LargeView ? 14 : 12)
#define PROCESSES_WIN_X_SIZE    (sets.m_LargeView ? 112 : 80)

// Colors
#define CLR_RED             RGB(255, 0, 0) 
#define CLR_GREEN           RGB(0, 255, 0) 
#define CLR_BLUE            RGB(0, 0, 255) 

#define CLR_BLUE_LIGHT      RGB(51, 143, 247) 
#define CLR_YELLOW          RGB(255, 255, 0) 
#define CLR_YELLOW_DARK     RGB(170, 170, 0) 
#define CLR_PINK            RGB(255, 125, 125) 
#define CLR_CYAN            RGB(0, 200, 200) 
#define CLR_CYAN_DARK       RGB(0,125,125)
#define CLR_MAGENTA         RGB(255, 0, 255) 

#define CLR_WHITE           RGB(255, 255, 255) 
#define CLR_BLACK           RGB(0, 0, 0) 
#define CLR_GREY            RGB(150,150,150)
#define CLR_GREY_DARK       RGB(100,100, 100)
#define CLR_GREY_LIGHT      RGB(190,190,190)

#define  CLR_CURRENT_ACTIVE_TITLE   RGB(180,190,190)   // lighter grey     
#define  CLR_INACTIVE_TITLE_BORDER  RGB(120,120,120)   // grey     
#define  CLR_ACTIVE_TITLE_BORDER    RGB(120, 160, 160) // cyanish grey
#define  CLR_INACTIVE_GRAPH_BORDER  RGB(100,100,100)   // inactive graphs border: darker grey     
#define  CLR_ACTIVE_GRAPH_BORDER  RGB(145,160,165)     // active graphs border: lighter grey     

#define CLR_INACTIVE_INDIC_BORDER    RGB(40,40,40)
#define CLR_ACTIVE_INDIC_BORDER      RGB(70,70,70)
#define CLR_INACTIVE_HOVER_BORDER   RGB(90,90,110)
#define CLR_ACTIVE_HOVER_BORDER   RGB(110,110,110)

#define CLR_ACTIVE_BORDER_LINE_DARK    RGB(96,96,96)     // (110,110,110) with yellow:(168,162, 30)
#define CLR_ACTIVE_BORDER_LINE_LIGHT   RGB(150,160,170)     // RGB(200,200,200) with yellow:(219,219,170)
#define CLR_INACTIVE_BORDER_LINE_DARK    RGB(50,50,50)
#define CLR_INACTIVE_BORDER_LINE_LIGHT   RGB(100,110,120)


#define CLR_INDIC_INVALID   CLR_GREY   // for filling multiple selected indicators dialog

#define DEFAULT_BRIGHT_COEFF            1.0     // no bright change
#define BRIGHT_COEFF_INCREASE           2.0     // internal additional coeff, depending on program logic 
#define DEFAULT_INDIC_TIME_INTERVAL     333
#define LESS_COEFF_TO_ADJUST_MAX        0.85    // coeff to adjust max value at a time, if last values are smaller 
#define LESS_COEFF_TO_ADJUST_GRAPH_MAX  0.7     // coeff to adjust max value on graph at a time, if all values are smaller 

// Dynamic Elements Ids
#define ID_INDICATOR_FIRST  15000
#define ID_LABEL_FIRST      16000
#define ID_PROCESS_WINDOWS  17000
#define ID_VALUETEXT_FIRST      18000
#define ID_VALUEMAXTEXT_FIRST   19000


// indicators flags
#define INDSW_STANDARD          0           // no special flags
#define INDSW_MAXWHITE          0x0001      // make white on 100%
#define INDSW_FADING            0x0010      // make fading when MAX*0.95 reached
#define INDSW_FULL_SET_ONLY     0x0020      // indic only present in full set
#define INDSW_ADJUST_MAX        0x0040      // only MO is considered (i.e. always brighter)
#define INDSW_LOCAL_VARIATIONS  0x0080      // only local MO is considered (to oscilate more)
#define INDSW_NOT_IN_MINIBAR    0x0100      // show indicator in minibar
#define INDSW_NOT_VISIBLE       0x0200      // indicator is not visible
#define INDSW_MAX_ABSOLUTE      0x0400      // max value is absolute (not changing)

#define  LOCAL_MACHINE_NAME     "<Local>"     // name to show as 'local computer'

#define  LIST_DELETE(var)  { \
   while (!var.IsEmpty() )  { \
       delete var.GetHead(); \
       var.RemoveHead();     \
    }    \
}

#define IS_INDICATOR_INVISIBLE  ( m_Indicator == 0 || m_Indicator->m_indicSettings == 0 || !m_Indicator->m_indicSettings->IsVisible() )

// ====================================================================================
//   Start and restore wait cursor
#define SET_WAIT_CURSOR \
        HCURSOR save_cursor = ::GetCursor();\
        HCURSOR cursor = theApp.LoadStandardCursor(IDC_WAIT);\
        ::SetCursor(cursor);

#define RESTORE_CURSOR ::SetCursor(save_cursor);

////////////////////////////////////////////////////////////////////////////
template<class T>
bool CheckRange(const char * txt, T* v, T vmin, T vmax)
{
    bool was_invalid = false;
    if ( *v > vmax )    
    {
        was_invalid = true;
        *v = vmax; 
    }
    else if ( *v < vmin )
    {
        was_invalid = true;
        *v = vmin; 
    }
    if ( !was_invalid )
        return false;       // nothing's modified

    g_inCheckRange = true;
    char buf[402]; 
    _snprintf(buf, 400, 
            "Value of '%s' should be \r\n" 
            "between %s and %s. \r\n "
            " The value is corrected.",
            txt, 
            (LPCTSTR)ValueToString(vmin), 
            (LPCTSTR)ValueToString(vmax) );
    AfxMessageBox(buf);
    g_inCheckRange = false;
    return true;            // field is modified
}

/////////////////////////////////////////////////////////////////////////////
#define FLAG_CHECK(w,f) ( ((w) & (f)) != 0 )
#define FLAG_SET(w,f)   ((w) |= (f))
#define FLAG_CLEAR(w,f) ((w) &= (~(f)))

/////////////////////////////////////////////////////////////////////////////
// Windows macroes 
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

// ====================================================================================
enum Os_Version {
    OS_VERSION_INVALID = 0,
    OS_VERSION_NT3,
    OS_VERSION_NT4,
    OS_VERSION_2000,
    OS_VERSION_XP,
    OS_VERSION_SERVER_2003,
    OS_VERSION_VISTA,
    OS_VERSION_SERVER_2008,
    OS_VERSION_7 
};

// ====================================================================================
enum EDisplayMode {        // Display mode
    DISP_NONE   = 0,       // No Window at all !!!
    DISP_COLUMN = 1,
    DISP_GRAPH  = 2,
    DISP_GLINT  = 3,       // glinting lights
    DISP_DOT    = 4,       // flickering dots  (future)
    DISP_UNKNOWN
};

/////////////////////////////////////////////////////////////////////////////
//          Include declarations of global vars and funcs. 
#include "Globals.h"

#endif // !defined(AFX_CDEFS_H__3B137A9C_EFCB_4B87_875A_66A23077DB68__INCLUDED_)
