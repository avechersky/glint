//////////////////////////////////////////////////////////////////////
// Utilities.cpp: Utility functions.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include <math.h>
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HANDLE  g_GlintSemaphoreHandle = 0;     // global semaphore handle
bool    g_is_multi_monitor = false; // true: multimonitor system   
int     g_max_screen_X = 800;      // max screen position on X
int     g_max_screen_Y = 600;      // max screen position on Y
int     g_min_screen_X = 0;        // min screen position on X
int     g_min_screen_Y = 0;        // min screen position on Y

int     g_CenterPosX = INT_MIN;    // current center position
int     g_CenterPosY = INT_MIN;    // current center position

bool    g_IsSetsChanged = false;   // 'sets' changed in Settings dialogs by 'Apply' button
bool    g_IsMenuActive = false;    // true, when Context menu is Active

int     g_NumOfErrors = 0;         // number of errors put in debugging log 
char    g_dataErrMessage[MAX_DATA_ERR_MESSAGE + 20];

// ----------------------------------------------------------------------
//          Checks if Glint instance is already running
// ----------------------------------------------------------------------
CFunctionLog::CFunctionLog(int lvl, const char* txt)
    : m_DebLvl(lvl), m_Txt(txt)
{ 
    ++g_FuncLogNesting; 
    if ( g_LogLevel >= m_DebLvl && g_FLog != 0 )
    {
        fprintf(g_FLog,"%*s { %s  BEGIN\n", g_FuncLogNesting*3, "", m_Txt); 
        #ifndef _DEBUG
        fflush(g_FLog);
        #endif
    }
} 

CFunctionLog::~CFunctionLog()
{ 
    if ( g_LogLevel >= m_DebLvl && g_FLog != 0 )
    {
        fprintf(g_FLog,"%*s } %s  END\n", g_FuncLogNesting*3, "", m_Txt);
        #ifndef _DEBUG
        fflush(g_FLog);
        #endif
    }
    --g_FuncLogNesting; 
} 


// ----------------------------------------------------------------------
//          Checks if Glint instance is already running
// ----------------------------------------------------------------------
bool  IsAnotherInstanceRunning()    
{
    // Create a semaphore with initial and max. counts of 10.
    g_GlintSemaphoreHandle = CreateSemaphore( 
        NULL,                   // default security attributes
        0,                      // initial count
        1,                      // maximum count
        "Glint_Program_Semaphore_to_Detect_Other_Instances");  
    
    DWORD errCode = GetLastError();

    if ( g_GlintSemaphoreHandle == NULL) 
    {   
        char szErr[200];
        sprintf(szErr, "Glint: CreateSemaphore error: %08x", GetLastError());
        AfxMessageBox(szErr, MB_OK | MB_ICONSTOP);
        exit(9);
    }
    else if ( errCode == ERROR_ALREADY_EXISTS )
    {
        return true;
    }
    return false;
}


// ----------------------------------------------------------------------
//     Check if another Glint instance already uses same INI-file
// ----------------------------------------------------------------------
bool  IsIniFileBusy()    
{
    //{CString  sDeb("working dir: "); sDeb += (LPCSTR)s_szWorkDir; AfxMessageBox(sDeb);}   // DEBUGGING
    char  s_szWorkDir[MAX_PATH + 2] = {""}; 
    _getcwd(s_szWorkDir, MAX_PATH );      // get working dir
    
    CString  strName("glint_ini_"); 
    strName += (LPCSTR)s_szWorkDir;
    strName.Replace('\\','|');
    strName.MakeLower();

    HANDLE  semHandle = CreateSemaphore( 
        NULL,                   // default security attributes
        0,                      // initial count
        1,                      // maximum count
        (LPCSTR)strName);  
    DWORD errCode = GetLastError();

    if ( semHandle == NULL) 
    {   
        char szErr[200];
        sprintf(szErr, "Glint: CreateSemaphore error: %08x", GetLastError());
        AfxMessageBox(szErr, MB_OK | MB_ICONSTOP);
        exit(9);
    }
    else if ( errCode == ERROR_ALREADY_EXISTS )
    {
        return true;
    }
    return false;
}


// ----------------------------------------------------------------------
static const char * GetErrorText(EErrorCode code)
{
    switch (code)
    {
        case NotAbleToGetCounters:  return "Unable to get Counters from Registry";
        case NoActiveIndicators:    return "No Indicators are available";
        case RegQueryValueExError:  return "RegQueryValueEx() Error on getting performance data";
    }
    return "Unknown error";
}

// ----------------------------------------------------------------------
void Win32ErrorExit()
{
    char sbuf[1400];
    sprintf("Glint: An issue when reading the Registry \n %s \n  Exiting program.", last_win32_error_txt);
    AfxMessageBox( sbuf );
    exit(9);
}

// ----------------------------------------------------------------------
void        ShowErrorMsg(EErrorCode code)
{
    char sbuf[1400];
    sprintf("Glint:  Error: \n %s ", GetErrorText(code) );
    AfxMessageBox( sbuf );
}

// ----------------------------------------------------------------------
void        ErrorExit(EErrorCode code)
{
    char sbuf[1400];
    sprintf("Glint:  Error: \n %s \n  Exiting program.", GetErrorText(code) );
    AfxMessageBox( sbuf );
    exit(9);
}

// ----------------------------------------------------------------------
void        DebugErrorPrint(EErrorCode code)
{
    ++g_NumOfErrors;
    if ( g_NumOfErrors < 500 )
    {
        TRACE("Glint. Error: %s \r\n", GetErrorText(code) );
    }
    else if ( g_NumOfErrors == 500 )
    {
        TRACE( "\r\n **** Glint. Too many errors. Stop recording \r\n " );
    }
}

/////////////////////////////////////////////////////////////////////////////
//  Get full window rectangle.
void    GetFullRect(CWnd* wnd, LPRECT lpRect)  
{
    ASSERT(wnd != 0 && lpRect != 0 );
    wnd->GetWindowRect(lpRect);                 // was GetClientRect

    lpRect->right = (lpRect->right - lpRect->left ) + 1;
    lpRect->bottom = ( lpRect->bottom - lpRect->top ) + 1;
    lpRect->left    = 0;
    lpRect->top     = 0;
}

/////////////////////////////////////////////////////////////////////////////
//  If ceneter of window is outside of screen boundaries, correct position of window center
bool CorrectWindowPosition()
{
    bool is_changed = false;

    if ( sets.m_PosX + sets.m_PosWidth > g_max_screen_X ) 
    {
        sets.m_PosX = g_max_screen_X - sets.m_PosWidth;
        is_changed = true;
    }
    if ( sets.m_PosX < g_min_screen_X ) 
    {
        sets.m_PosX = g_min_screen_X;  // (g_min_screen_X + sets.m_PosWidth + 1)/2;
        is_changed = true;
    }
    if (  sets.m_PosY + sets.m_PosHeight > g_max_screen_Y ) 
    {
        sets.m_PosY = g_max_screen_Y - sets.m_PosHeight;
        is_changed = true;
    }
    if ( sets.m_PosY < g_min_screen_Y ) 
    {
        sets.m_PosY = g_min_screen_Y;
        is_changed = true;
    }

    g_CenterPosX = sets.m_PosX + sets.m_PosWidth/2;
    g_CenterPosY = sets.m_PosY + sets.m_PosHeight/2;

    // if ( is_changed )   sets.SavePosition();

    //TRACE("CorrectWindowPosition(%d) [%d,%d] \r\n",is_changed,sets.m_PosX,sets.m_PosY);
    return is_changed;
}

/////////////////////////////////////////////////////////////////////////////
//  Returns 'true' if metrics aare changed.
bool GetScreenMetrics() 
{
    int min_screen_X;
    int min_screen_Y;
    int max_screen_X;
    int max_screen_Y;
    bool    retc = false;

    g_is_multi_monitor = ( g_OSVersion > OS_VERSION_NT4 && GetSystemMetrics(SM_CMONITORS) > 0 );

    if ( g_is_multi_monitor )
    {
        // Multi monitor sytsem
        min_screen_X = GetSystemMetrics(SM_XVIRTUALSCREEN);
        min_screen_Y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        max_screen_X = min_screen_X + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        max_screen_Y = min_screen_Y + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    else
    {
        // One monitor or old NT sytsem
        min_screen_X = 0;
        min_screen_Y = 0;
        max_screen_X = ::GetSystemMetrics(SM_CXSCREEN);   // it can change
        max_screen_Y = ::GetSystemMetrics(SM_CYSCREEN);   // it can change
    }

    if ( g_min_screen_X != min_screen_X || g_min_screen_Y != min_screen_Y ||
         g_max_screen_X != max_screen_X || g_max_screen_Y != max_screen_Y      )
    {
        g_min_screen_X = min_screen_X;
        g_min_screen_Y = min_screen_Y;
        g_max_screen_X = max_screen_X;
        g_max_screen_Y = max_screen_Y;
        retc = true;
    }

    TRACE("GetScreenMetrics(%d) X:[%d,%d] Y:[%d,%d] \r\n",retc,g_min_screen_X,g_max_screen_X,g_min_screen_Y,g_max_screen_Y);
    return retc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                  Form a small representation of a number. 
//  If 'show_small_integers' is 'true' then output number below 1K as integers. Otherwise as 0.nnK
//  'delim' - string to insert between number and 'K','M' or 'T'.
const char * GetSmallValueStr(double val, bool show_small_integers, const char* delim) {
    static char buf[50];
    double  a = fabs(val);

    if ( a < 1.0 ) {

        if ( a < 0.001 ) {
            sprintf(buf, "0" );
        }
        else if ( a < 0.1 ) {
            sprintf(buf, "%.1g", val );
        }
        else    /* if ( a >= 0.1 ) */ {
            sprintf(buf, "%.2g", val );
        }
    }
    else {  // abs > 1.0

        if ( a > 0.9999E+12 ) {
            sprintf(buf, "%.4g", val );
        }
        if (  a < 999.9 )
        {
            if ( show_small_integers ) 
            {
                if ( a <= 9.9999 ) {
                    sprintf(buf, "%.1f", val );
                }
                else {  
                    sprintf(buf, "%d", (int)val );
                }
            }
            else 
            {  
                // show integers below 1 K as 0.nK
                if ( a <= 9.9999 ) {
                    sprintf(buf, "%.3f%sK", (val + 0.00049)/1000, delim );
                }
                else if ( a <= 99.9999 ) {
                    sprintf(buf, "%.2f%sK", (val + 0.0049)/1000, delim );
                }
                else {  
                    sprintf(buf, "%.1f%sK", (val + 0.049)/1000, delim );
                }
            }
        }
        else if ( a <= 9999.0 ) {
            sprintf(buf, "%.1f%sK", val/1000, delim );
        }
        else if ( a <= 99990.0 ) {
            sprintf(buf, "%d%sK", (int)val/1000, delim );
        }
        else if ( a <= 99990.0 ) {
            sprintf(buf, "%.2g%sK", val/1000, delim );
        }
        else if ( a < 0.999001E+6 ) {
            sprintf(buf, "%.3g%sK", val/1000, delim );
        }
        else if ( a <= 0.99E+8 ) {
            sprintf(buf, "%.2g%sM", val/1E+6, delim );
        }
        else if ( a < 0.999001E+9 ) {
            sprintf(buf, "%.3g%sM", val/1E+6, delim );
        }
        else if ( a <= 0.99E+11 ) {
            sprintf(buf, "%.2g%sG", val/1E+9, delim );
        }
        else if ( a < 0.999001E+12 )  {
            sprintf(buf, "%.3g%sG", val/1E+9, delim );
        }
        else if ( a <= 0.99E+14 ) {
            sprintf(buf, "%.2g%sT", val/1E+12, delim );
        }
        else  if ( a < 1E+15 ) {
            sprintf(buf, "%.3g%sT", val/1E+12, delim );
        }
        else  {
            //ASSERT(0);
            sprintf(buf, "******");
        }
    }
    return buf;
}


//////////////////////////////////////////////////////////////////////////////////////
double      SubLL(LARGE_INTEGER& la, LARGE_INTEGER& lb)
{   
    LARGE_INTEGER r;

    r.LowPart = la.LowPart - lb.LowPart; 
    r.HighPart = la.HighPart - lb.HighPart;
    if (la.LowPart < lb.LowPart)
        r.HighPart --;      // borrow one from high part
    return LToD(r);
}

//////////////////////////////////////////////////////////////////////////////////////
double      LToD(LARGE_INTEGER& li)
{
    double d = ((double)li.HighPart) * 4294967296.00 + (double)li.LowPart; 
    return ( d );
}

//////////////////////////////////////////////////////////////////////////////////////

