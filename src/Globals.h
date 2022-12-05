// Globals.h: interface for the CSettings class.
// Author: Alexander Vechersky, 2010 

//////////////////////////////////////////////////////////////////////
#if !defined(GLOBALS_H_IS_INCLUDED_)
#define GLOBALS_H_IS_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PROGRAM_VERSION                 "1.28"

// ====================================================================================
//          G l o b a l    D a t a
// ====================================================================================

extern  int     g_LogLevel;    // level 0:   OFF 1:minimum  9: maximum 
extern  FILE*   g_FLog;        // log file 

extern Os_Version   g_OSVersion;
extern DWORD        g_NumberCPUs;
extern double       g_version_program  ;          // program version
extern double       g_version_settings ;          // read version of settings
extern HANDLE       g_GlintSemaphoreHandle;      // global semaphore handle
extern int          g_FuncLogNesting ;           // current nesting level of logged function

extern  CFont   g_font1;     // for labels
extern  CFont   g_font2;     // for numbers
extern  CFont   g_font3;     // Processes
extern  CFont   g_font1L;     // for labels - Large
extern  CFont   g_font2L;     // for numbers - Large
extern  CFont   g_font3L;     // Processes - Large
extern  CFont   g_fontTTipFix;     // Fixed tool tips for Processes History
extern  ULONG   g_TimeTicks; // indicators timer ticks

extern  bool g_IsSetsChanged;       // 'sets' changed in Settings dialogs by 'Apply' button
extern  bool g_IsMenuActive;        // true, when Context menu is Active
extern  bool g_IsSchemaModified;    // true: if current schema was modified in Settings (restored on Load/Save schema)

//  Screen variables
extern bool g_is_multi_monitor; // true: multimonitor system   
extern int  g_max_screen_X;      // max screen position on X
extern int  g_max_screen_Y;      // max screen position on Y
extern int  g_min_screen_X;      // min screen position on X
extern int  g_min_screen_Y;      // min screen position on Y

extern int  g_CenterPosX;        // current center position
extern int  g_CenterPosY;        // current center position

// Help strings for Settings dialogs 
extern const char * g_szHelpTxt;
extern const char * g_szHelpTitle;

extern const char * csz_HelpText_GeneralSettings;
extern const char * csz_HelpText_CountersSettings;
extern const char * csz_HelpText_IndicatorsSettings;

extern const char * csz_HelpTitle_GeneralSettings;
extern const char * csz_HelpTitle_CountersSettings;
extern const char * csz_HelpTitle_IndicatorsSettings;

//-------------------------------------------------------------------------------------
// Processes tool tips 
#define MAX_PROC_TOOLTIP_TEXT   1000
#define MAX_TOOL_TIP_WIDTH      MAX_PROC_TOOLTIP_TEXT + 2
extern char  g_ProcessesToolTipTitle[200];                    
extern char  g_ProcessesToolTipText[MAX_PROC_TOOLTIP_TEXT + 100];

// ====================================================================================
//          G l o b a l    F u n c t i o n
// ====================================================================================

bool  IsAnotherInstanceRunning();   // check if Glint instance is already running
bool  IsIniFileBusy();              // check if another Glint instance already uses same INI-file

void LoadStringsArray(LPCTSTR section, LPCTSTR key_name, CStringArray& strings);    // loads from string in [_Current_]/key - separated by ';'
void SaveStringsArray(LPCTSTR section, LPCTSTR key_name, CStringArray& strings);    // Saves to string  in [_Current_]/key - separated by ';'

CString GetPrintableCompName(LPCTSTR comp_name);

bool    CorrectWindowPosition();
void    GetFullRect(CWnd* wnd, LPRECT lpRect);  
COLORREF    CalcColor(COLORREF c0, double val, UINT flags);
int         ColorToInt(COLORREF co);
COLORREF    IntToColor(int i);
bool        GetScreenMetrics();    // returns true, if metrics were changed

void    ShowAbout(); 
CString ConvertFromUni(const wchar_t * wsz);
DWORD   ConvertToUni(const CString str, wchar_t * wsz_out, DWORD max_out_chars); // returns new length

const char* GetCounterTypeStr(DWORD t);     // prepare couter type for printing 

// Form a small representation of a number. 'delim' - string to insert between number and 'K','M' or 'T'.
const char* GetSmallValueStr(double val, bool show_small_integers = true, const char* delim = "");

void EnableButton(CWnd * parent, int id, BOOL value); 
BOOL IsCheckBoxOn(CWnd * parent, int id); 
void UpdCheck(CWnd * parent, int id, BOOL& var, BOOL value); // update dialog checkbox & var
void UpdCheck3state(CWnd * parent, int id, BOOL& var, BOOL value, int idx);
void UpdEdit(CWnd * parent, int id, LPCTSTR value, BOOL enable = TRUE); // update dialog edit ctrl
void EnableEdit(CWnd * parent, int id, BOOL enable); 
void UpdStatic(CWnd * parent, int id, LPCTSTR txt); 
void SetListHorizontalExtent(CListBox* list, LPCTSTR sz);

double      SubLL(LARGE_INTEGER& la, LARGE_INTEGER& lb);
double      LToD(LARGE_INTEGER& li);
CString     FloatToString(double f, bool is_compress_format = false);
CString     IntToString(int i);
CString  inline ValueToString(int i)    { return IntToString(i);   }
CString  inline ValueToString(double f) { return FloatToString(f); }

void    MakeNewGadgets();         //  Make new set of gadgets (all inicators values are lost) 
bool    CheckConnection(CString& computer_name);  // 'true', if computer can be connected to.  


// ====================================================================================
//          G l o b a l    C l a s s e s
// ====================================================================================

// --------------------------------------------------------------------
//    Class to automaticly trace entering/exiting a function
// --------------------------------------------------------------------
class   CFunctionLog {
public:
    CFunctionLog(int lvl, const char* txt);
    ~CFunctionLog();
private:
    int             m_DebLvl;
    const char*     m_Txt;
};


#endif // !defined(GLOBALS_H_IS_INCLUDED_)
