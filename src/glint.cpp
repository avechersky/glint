// glint.cpp : Defines the class behaviors for the application.
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "panel.h"
#include "Settings.h"
#include "RemoteConnectionDlg.h"
#include "Gadgets.h"
#include "HelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WINDOWS_CLASS_NAME "Glint"

CFont g_font1;  // Labels
CFont g_font2;  // small digits inside indicators
CFont g_font3;  // Processes

CFont g_font1L; // Labels
CFont g_font2L; // small digits inside indicators
CFont g_font3L; // Processes
CFont g_fontTTipFix;     // Fixed tool tips for Processes History

CSettings  sets;     // stores current settings
HICON icon = 0;
ULONG   g_TimeTicks = 0; // indicators timer ticks

static Os_Version   RetrieveOsVersion(); 
static DWORD        RetrieveSystemInfo(); 
Os_Version          g_OSVersion;
DWORD               g_NumberCPUs = 1;
int                 g_FuncLogNesting = 0;           // current nesting level of logged function

double g_version_program  = 0;
double g_version_settings = 0; 
int     g_LogLevel = 0;         // level 0:   OFF 1:minimum  9: maximum 
FILE*   g_FLog = 0;             // log file 

/////////////////////////////////////////////////////////////////////////////
// CGlintApp

BEGIN_MESSAGE_MAP(CGlintApp, CWinApp)
    //{{AFX_MSG_MAP(CGlintApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
//  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    ON_COMMAND(ID_HELP, CGlintApp::OnHelp)
    ON_COMMAND(IDM_EXIT, OnExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlintApp construction

CGlintApp::CGlintApp()
{
    m_RestartPanel = true;
    m_PortableMode = true;
}

CGlintApp::~CGlintApp()
{
    delete g_Gadgets;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGlintApp object

CGlintApp theApp;

/////////////////////////////////////////////////////////////////////////////
//              CGlintApp initialization
/////////////////////////////////////////////////////////////////////////////
BOOL CGlintApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.
    INT rc = 0;

    g_version_program = atof(PROGRAM_VERSION); 
    g_OSVersion = RetrieveOsVersion();
    g_NumberCPUs = RetrieveSystemInfo();

    if ( g_OSVersion == OS_VERSION_INVALID )
    {
        AfxMessageBox("Can not run on this OS.", MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    GetScreenMetrics();

    ::SetPriorityClass( ::GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    icon = LoadIcon(IDI_ICON1); ASSERT( icon != 0);
    // HWND mainWnd = m_pMainWnd->m_hWnd;

    // ========================================
    //      Process Command Line
    // ========================================
    char scheme_arg[256 + 2] = "";
    const char* pszMoreArg = ProcessArgKeys(m_lpCmdLine);
    if ( pszMoreArg && pszMoreArg[0] != 0 )
    {
        strncpy(scheme_arg, pszMoreArg, 256);
    }

    // ========================================
    //      Initialize Settings    
    // ========================================
    sets.InitSettings();
    g_version_settings = atof(sets.m_Version); 

    // ========================================
    //      Check  other  Instances
    // ========================================
    if ( sets.m_AllowOnlyOneInstance && IsAnotherInstanceRunning() )
        return FALSE;   // immediately exit application

    if ( IsIniFileBusy() )
    {
        if ( m_PortableMode )
        {
            AfxMessageBox("Another instance uses the same working dir"
                          " with profile file  glint.ini.\n"
                          "   Use another directory or -R command line option.\n"
                          , MB_OK | MB_ICONSTOP );
            return FALSE;
        }

        g_LogLevel = 0;     // do not use Log file in the same direcory 
    }

    // ========================================
    //      Initialize Logging    
    // ========================================
    if ( g_LogLevel > 0 ) g_FLog = fopen("GLINT_LOG.TXT", "wt");
    if ( g_LogLevel > 0 ) { fprintf(g_FLog, " ===== Glint Log file ===== lvl:%d \n",g_LogLevel); fflush(g_FLog); }

    // ========================================
    //      Process Settings    
    // ========================================
    if ( sets.m_Version.GetLength() == 0 )
    {
        ShowAbout();
        sets.SetDefaults();
    }
    else 
    { 
        if ( scheme_arg[0] != 0 && sets.LoadSettings(scheme_arg) )
        {
            sets.m_CurrentSchema = scheme_arg;
        }
        else {
            if ( sets.LoadSettings(CURRENT_SETTINGS_SECTION) )
            {
                sets.m_CurrentSchema = this->GetProfileString(CURRENT_SETTINGS_SECTION, "Scheme");  // restore last scheme name 
                if ( sets.m_CurrentSchema.GetLength() > 0 )
                {
                    g_IsSchemaModified = this->GetProfileInt(CURRENT_SETTINGS_SECTION, "Modified", 0); // get scheme modified flag
                }
            }
            else 
            {
                ShowAbout();
                sets.SetDefaults();
            }
        }

        // Reset default settings, if they are outdated.    
        if (  g_version_program > g_version_settings ) 
        {
            if ( g_version_settings < 1.1 )       // have to change values since version '1.05'
            {
                ShowAbout();
                sets.SetDefaults();
            }
            else if ( g_version_settings < 1.27 )    // update standard schemes
            {
                // Only change standard schemes, but do not change current settings.
                // sets.SaveSettings(CURRENT_SETTINGS_SECTION); // save current
                sets.SaveDefaultCountersGroups();
                sets.LoadCountersGroups();

                sets.SetDefaults(false /* don't load standard scheme */);
                sets.LoadSettings(CURRENT_SETTINGS_SECTION);    // restore previous settings
            }
        }

        sets.m_Version = PROGRAM_VERSION;    // set program version
    }

    // ===========================================
    //      Process Remote Computer connection
    // ===========================================
    if ( !CheckConnection(sets.m_ComputerName) )
    {
        // Computer not connected, ask for another computer name.
        CRemoteConnectionDlg dlg(0, sets.m_ComputerName);
        while ( TRUE )
        {
            if ( dlg.DoModal() == IDCANCEL )
                return FALSE;                   // leave program
            if ( CheckConnection(dlg.m_machine) ) 
            {
                sets.m_ComputerName = dlg.m_machine;
                break;
            }
        }
    }

    // ======================
    //     Create Panel
    // ======================
    VERIFY(  g_font1.CreateFont(    // ***** for labels *******
               12,                        // nHeight
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_NORMAL,                 // nWeight
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               PROOF_QUALITY,           // nQuality
               DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
               "Arial")
       );                 // lpszFacename

    VERIFY(  g_font2.CreateFont(  // ***** for numbers *******
               11,                        // nHeight
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_NORMAL,                 // nWeight  - FW_SEMIBOLD, FW_MEDIUM, FW_NORMAL
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               DEFAULT_QUALITY,           // nQuality   - DRAFT_QUALITY
               DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily - FF_ROMAN, FF_SWISS, FF_DONTCARE, VARIABLE_PITCH 
               "Times New")                   // ... Times New, Arial
       );                 // lpszFacename

    VERIFY(  g_font3.CreateFont(    // ***** for processes *******
               12,                        // nHeight
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_NORMAL,                 // nWeight      - FW_BOLD, FW_SEMIBOLD
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               PROOF_QUALITY,             // nQuality
               VARIABLE_PITCH | FF_SWISS, // nPitchAndFamily
               "Times New")
       );                 // lpszFacename

    //============== Large fonts ====================
    VERIFY(  g_font1L.CreateFont(   // ***** for labels *******
               15,                        // nHeight    [12]
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_MEDIUM,                 // nWeight
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               PROOF_QUALITY,           // nQuality
               DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
               "Arial")
       );                 // lpszFacename

    VERIFY(  g_font2L.CreateFont(  // ***** for numbers *******
               13,                        // nHeight    [11]
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_SEMIBOLD,               // nWeight  - FW_SEMIBOLD, FW_MEDIUM, FW_NORMAL
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               DEFAULT_QUALITY,           // nQuality   - DRAFT_QUALITY
               DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily - FF_ROMAN, FF_SWISS, FF_DONTCARE, VARIABLE_PITCH 
               "Times New")                   // ... Times New, Arial
       );                 // lpszFacename

    VERIFY(  g_font3L.CreateFont(   // ***** for processes *******
               14,                        // nHeight    [12]
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_SEMIBOLD,               // nWeight      - FW_BOLD, FW_SEMIBOLD, FW_NORMAL
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               PROOF_QUALITY,             // nQuality
               VARIABLE_PITCH | FF_SWISS, // nPitchAndFamily
               "Times New")
       );                 // lpszFacename

    VERIFY(  g_fontTTipFix.CreateFont(   // ***** fortool tips for Processes History *******
               12,                        // nHeight
               0,                         // nWidth
               0,                         // nEscapement
               0,                         // nOrientation
               FW_NORMAL,                 // nWeight      - FW_BOLD, FW_SEMIBOLD
               FALSE,                     // bItalic
               FALSE,                     // bUnderline
               0,                         // cStrikeOut
               ANSI_CHARSET,              // nCharSet
               OUT_DEFAULT_PRECIS,        // nOutPrecision
               CLIP_DEFAULT_PRECIS,       // nClipPrecision
               PROOF_QUALITY,             // nQuality
               FIXED_PITCH | FF_DONTCARE, // nPitchAndFamily
               "Lucida Console")                                              // "Fixedsys"
       );                 // lpszFacename


    // ===========================================
    //   Output  information about the system.
    // ===========================================
    theApp.WriteProfileInt(INFO_SETTINGS_SECTION, " Processors ", g_NumberCPUs );

    // ===========================================
    //      Main Loop for updating panel.
    // ===========================================
    while ( m_RestartPanel )
    {
        if ( sets.m_indicSets.GetCount() == 0 )
        {
            ShowErrorMsg( NoActiveIndicators );
            sets.SetDefaults();
        }

        MakeNewGadgets(); 

        if ( g_Gadgets->IsEmpty() ) 
        {
            if ( AfxMessageBox( "ERROR: No active system counters in Registry. \n Do you want to use a standard scheme?" , 
                  MB_YESNO ) != IDYES )
            {
                exit(999);
            }
            
            sets.SetDefaults();
            MakeNewGadgets(); 

            if ( g_Gadgets->IsEmpty() ) 
            {
                ErrorExit( NoActiveIndicators );
            }
        }

        m_MyClass = 
            AfxRegisterWndClass(
                    CS_DBLCLKS, //CS_GLOBALCLASS, CS_PARENTDC | CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW,  
                    LoadCursor(IDC_CURSOR1),                // ::LoadCursor(NULL, IDC_ARROW),
                    (HBRUSH) ::GetStockObject(BLACK_PEN),   // WHITE_BRUSH
                    icon                                    // ::LoadIcon(NULL, IDI_APPLICATION)
                    );

        m_RestartPanel = false;
        m_panel = new CPanel();

        m_pActiveWnd = m_panel;
        ::SetClassLong(m_panel->m_hWnd,GCL_HICON, (long)icon ); // does not show


        // Initialize and Run Window
        if ( !CreatePanel() )
            return FALSE;
        m_panel->Initialize();

        // Shell_NotifyIcon();          // set/chande icon in status bar
        m_panel->RunModalLoop();        
        m_panel->m_isReady = false;

        if ( m_panel->m_hWnd != 0)
        {
            m_panel->KillTimer(TIMER_COUNTERS); 
            m_panel->KillTimer(TIMER_PROCESSES);    
        }

        ::UnregisterClass(WINDOWS_CLASS_NAME, CWinApp::m_hInstance);
    }

    sets.FinalSave();

    m_panel = 0;  // delete CPanel
    return TRUE;  // FALSE: leave application
}


///////////////////////////////////////////////////////////////////
//      Read keys from command line
///////////////////////////////////////////////////////////////////
const char* CGlintApp::ProcessArgKeys(const char* cmdLine)
{
    bool    isWrongCmdLine = false;
    const char* pc = cmdLine;

    while ( *pc )
    {
        if ( *pc == ' ' || *pc == '\t' )
        {
            ++pc;
            continue;
        }
        else if ( *pc != '-' )
        {
            break;
        }

        ++pc;
        switch ( *pc )
        {
            case 'r' :
            case 'R' :
                m_PortableMode = false;
                
                // chaeck space after "-R"
                ++pc;
                if (*pc && *pc != ' ' && *pc != '\t' )
                {
                    isWrongCmdLine = true;
                    break;
                }
                break;

            case 'l' :
            case 'L' :          // -L0 (defualt) or -L1, ...., -L9  <--- logging level
                ++pc;
                g_LogLevel = atoi(pc);
                
                // skip characters after "-L"
                while (*pc && *pc != ' ' && *pc != '\t' ) 
                {
                    ++pc;
                }
                --pc;
                break;

            default:
                isWrongCmdLine = true;
                break;
        }

    }

    if ( isWrongCmdLine )
    {
        AfxMessageBox( "Invalid command-line parameters. \n" 
                       "Valid keys are: \n"
                       "  -r  : use Registry for Settings instead of INI-file.\n"
                       "\nUsing defaults."
                     , MB_OK | MB_ICONSTOP);
    }
    return pc;
}


///////////////////////////////////////////////////////////////////
BOOL CGlintApp::CreatePanel()
{
    BOOL rc = FALSE;

    if ( m_panel->m_hWnd != 0)
    {
        // m_panel->KillTimer(TIMER_COUNTERS);  
        // m_panel->KillTimer(TIMER_PROCESSES); 
        // m_panel->DestroyWindow();
    }

    // window created as non-on-top always (switch in CPanel::InitPanel later)
    if ( !sets.m_isMiniBar ) 
        rc = m_panel->CreateEx( PANEL_EX_STYLE_0 | WS_EX_TOOLWINDOW,   // WS_EX_APPWINDOW
                m_MyClass,  WINDOWS_CLASS_NAME,
                PANEL_STYLE_0,                          
                sets.m_PosX, sets.m_PosY, sets.m_PosWidth, sets.m_PosHeight,     // X, Y, Width, Hight
                0, // parent
                0);  // menu or child
    else 
        rc = m_panel->CreateEx( PANEL_EX_STYLE_0 |WS_EX_TOOLWINDOW,  // WS_EX_APPWINDOW
                m_MyClass,  WINDOWS_CLASS_NAME,
                PANEL_STYLE_0,                          
                sets.m_PosX, sets.m_PosY, sets.m_PosWidth, sets.m_PosHeight, 
                //( sets.m_LargeView ? MINIBAR_WINDOWS_Y_SIZE_L : MINIBAR_WINDOWS_Y_SIZE ),       // X, Y, Width, Hight
                0, // parent
                0);  // menu or child

    if ( rc == FALSE ) 
    {
        AfxMessageBox(_T("Window not created"));
    }
    else {
        if ( m_panel->SetTimer(TIMER_COUNTERS, sets.m_IndicTimeInterval, NULL) == FALSE ||
             m_panel->SetTimer(TIMER_PROCESSES, sets.m_ProcessTimeInterval, NULL) == FALSE ) {  
            AfxMessageBox(_T("Timer not created"));
            return FALSE;
        }
    }

    return rc;
}

//////////////////////////////////////////////////////////////////
int CGlintApp::Run() 
{
    // return CWinApp::Run();
    return 0;   
}

//////////////////////////////////////////////////////////////////
// Returns number of CPUs in the system
static DWORD RetrieveSystemInfo() 
{
    SYSTEM_INFO sysInfo;

    GetSystemInfo( &sysInfo );
    return sysInfo.dwNumberOfProcessors;
}

//////////////////////////////////////////////////////////////////
// Returns 'OS_VERSION_INVALID' if it's not NT
static Os_Version RetrieveOsVersion() 
{
    OSVERSIONINFO       osvi;
    OSVERSIONINFOEX     osvi_ex;

    memset(&osvi, 0, sizeof(OSVERSIONINFO) );
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if ( ! GetVersionEx(&osvi) )
        return OS_VERSION_INVALID;

    if ( osvi.dwMajorVersion >= 5 ) 
    {
        memset(&osvi_ex, 0, sizeof(OSVERSIONINFOEX) );
        osvi_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

        if ( ! GetVersionEx ( (OSVERSIONINFO*) &osvi_ex) )
        {
            osvi_ex.wProductType = VER_NT_WORKSTATION;  // assume workstation, if not clear.
        }
    } 


    if ( osvi.dwPlatformId != VER_PLATFORM_WIN32_NT )
        return OS_VERSION_INVALID;
    
    if ( osvi.dwMajorVersion < 4 )
        return OS_VERSION_NT3;

    if ( osvi.dwMajorVersion == 4 )
        return OS_VERSION_NT4;

    if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
        return OS_VERSION_2000;

    if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
        return OS_VERSION_XP;

    if ( osvi.dwMajorVersion == 5 )             
        return OS_VERSION_SERVER_2003;      // 5.2

    if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
    {
        return ( osvi_ex.wProductType == VER_NT_WORKSTATION ) ? OS_VERSION_VISTA : OS_VERSION_SERVER_2008 ;
    }

    return ( osvi_ex.wProductType == VER_NT_WORKSTATION ) ? OS_VERSION_7 : OS_VERSION_SERVER_2008 ;
}

//////////////////////////////////////////////////////////////////
void CGlintApp::OnHelp() 
{
    // AfxMessageBox(_T("Help"));
   CHelpDlg helpDlg(0);
   helpDlg.DoModal();
}
//  afx_msg void OnHelp();

//////////////////////////////////////////////////////////////////
void CGlintApp::OnExit()
{
    sets.FinalSave();
}

//////////////////////////////////////////////////////////////////
void CGlintApp::OnFinalRelease()
{
    // TODO: Add your specialized code here and/or call the base class

    CWinApp::OnFinalRelease();
}


