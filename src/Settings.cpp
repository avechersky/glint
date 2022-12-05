// CSettings.cpp: implementation of the CSettings class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "IndSetDefs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStringArray    CSettings::m_CounterGroupsNames;     // remote machines names history
CStringArray    CSettings::m_CounterGroups[MAX_COUNTER_GROUPS];
int             CSettings::m_CounterGroupsNumber;
CSavedStringsSet  CSettings::m_computers;           // remote machines names history
static bool g_FinalSaveDone = false;                // true: final settings saving is done

/////////////////////////////////////////////////////////////////////////
// Copy contents of one list to another
void CopyTIndSetsList(TIndSetsList& list_dest, TIndSetsList& list_src)
{
    // Clear the destination without deleting the pointed objects.
    while (!list_dest.IsEmpty())
    {
       list_dest.RemoveHead();
    }

    // Copy from source.
    POSITION pos = list_src.GetHeadPosition();
    for ( int i_ind = 0; i_ind < list_src.GetCount(); i_ind++ )
    {
        CIndicatorSettings* pindset = list_src.GetNext(pos);
        list_dest.AddTail(pindset); 
    }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
CSettings::CSettings()
{
    m_CounterGroupsNumber = 8;
    m_ProcessTimeInterval = 0;
    m_ProcessTrackingTime   = 60;
    m_ShowSystemTasksActivity = false;
    m_Indicators = 0;
    m_IndicatorsInRow = 0;
    m_CurrentSchema = CURRENT_SETTINGS_SECTION;
    m_OurFrequency = 1.0;
}

/////////////////////////////////////////////////////////////////////////
CSettings::~CSettings()
{
    FUNCTIONLOG(7, "CSettings::destr");
    Clear();
}

/////////////////////////////////////////////////////////////////////////
void CSettings::Clear()
{
    while (!m_indicSets.IsEmpty())
    {
       delete m_indicSets.GetHead();
       m_indicSets.RemoveHead();
    }
}

/////////////////////////////////////////////////////////////////////////
CSettings& CSettings::operator =( const CSettings& o)       // Copy constructor
{
    // m_schemas = o.m_schemas;             // can not copy
    m_CurrentSchema = o.m_CurrentSchema;  
    m_Version = o.m_Version;  
    m_ComputerName = o.m_ComputerName; 

    m_isMiniBar = o.m_isMiniBar;        
    m_isOnTop = o.m_isOnTop;
    m_isShowProcesses = o.m_isShowProcesses;
    m_ShowSystemTasksActivity = o.m_ShowSystemTasksActivity;
    m_FullSet = o.m_FullSet;            
    m_LargeView = o.m_LargeView;            
    m_AllowOnlyOneInstance = o.m_AllowOnlyOneInstance;            
    m_ShowLabels = o.m_ShowLabels;      
    m_ShowToolTips = o.m_ShowToolTips;      
    m_NoNumbersShow = o.m_NoNumbersShow;    
    m_NoMaxNumbersShow = o.m_NoMaxNumbersShow;  
    m_DispMode = o.m_DispMode;  

    m_IndicatorsInRow = o.m_IndicatorsInRow;        
    m_IndicTimeInterval = o.m_IndicTimeInterval;   
    m_NumberShowInterval = o.m_NumberShowInterval;  
    m_BrightCoeff = o.m_BrightCoeff;            
    m_ProcessTimeInterval = o.m_ProcessTimeInterval; 
    m_ProcessTrackingTime = o.m_ProcessTrackingTime; 


    // current positions
    m_PosX = o.m_PosX;
    m_PosY = o.m_PosY;
    m_PosWidth = o.m_PosWidth;
    m_PosHeight = o.m_PosHeight;
    m_LastMinibarPosX = o.m_LastMinibarPosX;
    m_LastMinibarPosY = o.m_LastMinibarPosY;

    m_titlesColor = o.m_titlesColor;
    m_bordersColor = o.m_bordersColor;
    m_hoverBordersColor = o.m_hoverBordersColor;
    m_processesColor = o.m_processesColor;
    m_BorderLineColorDark  = o.m_BorderLineColorDark ;
    m_BorderLineColorLight = o.m_BorderLineColorLight;

    m_Indicators = o.m_indicSets.GetCount();

    Clear();
    POSITION pos = o.m_indicSets.GetHeadPosition();
    for (int i_ind = 0; i_ind < m_Indicators; i_ind++)
    {
        CIndicatorSettings* indset1 = o.m_indicSets.GetNext(pos);
        CIndicatorSettings* isets = new CIndicatorSettings;
        *isets = *indset1;
        m_indicSets.AddTail( isets );
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////
void CSettings::InitSettings()      // call it after CWinApp object is ready.
{
    if ( theApp.m_PortableMode )
    {
        //Change the name of the INI file to current working directory.
        free((void*)theApp.m_pszProfileName);
        theApp.m_pszProfileName=_tcsdup( INI_FULL_FILE_NAME );
    }
    else
    {
        theApp.SetRegistryKey(INI_AUTHOR_NAME); // Comment the line, if you want to make glint.ini in Windows dir.
    }

    m_Version = theApp.GetProfileString(CURRENT_SETTINGS_SECTION, "Version");
    m_CurrentSchema = theApp.GetProfileString(CURRENT_SETTINGS_SECTION, "Scheme");  // save last scheme name 
    m_AllowOnlyOneInstance = theApp.GetProfileInt(OPIONS_SETTINGS_SECTION, "AllowOnlyOneInstance",0);  

    LoadCountersGroups();
    m_computers.Load(OPIONS_SETTINGS_SECTION, "Computers");
}

///////////////////////////////////////////////////////////////////////
void CSettings::GetHashedName(CString & str, DWORD* arr, DWORD size)
{
    str = "";
    for (int j=0; j < size/sizeof(DWORD); j++ )
    {
        int i = j + 1;
        int n = arr[j];
        DWORD k = n - i*17;
        char c = (k/23) >> 5;
        str += c;
    }
}

//////////////////////////////////////////////////////////////////////
void CSettings::SetDefaultModes()
{
    m_isOnTop = true;
    m_AllowOnlyOneInstance = false;
    m_isMiniBar = true;            
    m_isShowProcesses = true;
    m_FullSet = true;
    m_LargeView = false;
    m_ShowLabels = true;
    m_ShowToolTips = true;
    m_NoNumbersShow = false;
    m_NoMaxNumbersShow = false;
    
    m_DispMode = DISP_COLUMN;
    if ( g_OSVersion >= OS_VERSION_VISTA )
    {
        m_IndicTimeInterval = 400;
    }
    else
    {
        m_IndicTimeInterval = 300;
    }

    m_IndicTimeInterval     = DEFAULT_INDIC_TIME_INTERVAL;
    m_ProcessTimeInterval   = 1300;
    m_NumberShowInterval    = 1300;
    m_ProcessTrackingTime   = 60;
    m_BrightCoeff   = DEFAULT_BRIGHT_COEFF; // no bright change

    m_PosX = g_min_screen_X + (int)((g_max_screen_X - g_min_screen_X) * 0.66);
    g_CenterPosX = m_LastMinibarPosX = m_PosX;
    m_LastMinibarPosY = m_PosY = 15;
    g_CenterPosX = STICK_AREA_AT_SCREEN_Y;

    m_PosWidth = 200;
    m_PosHeight = 150;

    m_titlesColor = CLR_ACTIVE_TITLE_BORDER;
    m_hoverBordersColor = CLR_ACTIVE_HOVER_BORDER; 
    m_bordersColor = CLR_ACTIVE_INDIC_BORDER;
    m_processesColor = CLR_YELLOW;

    m_IndicatorsInRow       = 12;

    m_BorderLineColorDark  = CLR_INACTIVE_BORDER_LINE_DARK;
    m_BorderLineColorLight = CLR_INACTIVE_BORDER_LINE_LIGHT;
}

//////////////////////////////////////////////////////////////////////
void CSettings::SetDefaults(bool to_load_scheme) 
{
    m_Version = PROGRAM_VERSION;
    m_ComputerName = "";
    SetDefaultModes();

    SaveDefaultCountersGroups(); 

    // m_IndicTimeInterval      = ( g_OSVersion == OS_VERSION_XP ) ? DEFAULT_INDIC_TIME_INTERVAL*0.85 : DEFAULT_INDIC_TIME_INTERVAL;
    SetScheme_Standard();
    SaveSettings(SCHEMAS_SECTION_NAME_STANDARD);

    SetScheme_Large();
    SaveSettings(SCHEMAS_SECTION_NAME_LARGE);

    SetScheme_Small();
    SaveSettings(SCHEMAS_SECTION_NAME_DEFAULT_SMALL);

    SetScheme_Few_Graphs();
    SaveSettings(SCHEMAS_SECTION_NAME_DEFAULT_FEW_GRAPHS);

    if ( to_load_scheme ) 
    {
        LoadSettings(SCHEMAS_SECTION_NAME_STANDARD);
    }
}

////////////////////////////////////////////////////////////////////////////////

#define INISTR(s)  m_##s = theApp.GetProfileString(sec, #s); 
#define INIFLT(s)  str = theApp.GetProfileString(sec, #s); m_##s = atof((LPCSTR)str);
#define INIINT(s)  m_##s = theApp.GetProfileInt(sec, #s, 0);
#define INIINTV(s,defv)  m_##s = theApp.GetProfileInt(sec, #s, defv);
#define INIBOOL(s) m_##s = (bool) theApp.GetProfileInt(sec, #s, 0);
#define INICLR(s)  m_##s = IntToColor( theApp.GetProfileInt(sec, #s, 0) );

bool CSettings::LoadSettings(const char * sec)
{
    CString str;
    int k;

    Clear();

    m_CurrentSchema = sec;
    // detect if settings are present
    INIINT(ProcessTimeInterval);
    INIINTV(ProcessTrackingTime, 60);
    INIINT(IndicatorsInRow);
    if ( m_ProcessTimeInterval < MIN_INTERVAL_PROCESSES*0.5 || 
         m_ProcessTimeInterval > MAX_INTERVAL_PROCESSES*2 ||
         m_IndicatorsInRow < 1 || m_IndicatorsInRow > 50 )  
        return false;               // no settings

    INIINT(NumberShowInterval);
    if ( m_NumberShowInterval < MIN_INTERVAL_VALUES )
        m_NumberShowInterval = 1200;

    INIBOOL(isOnTop);
    INIBOOL(isMiniBar);         
    INIBOOL(isShowProcesses);
    INIBOOL(FullSet);
    INIBOOL(LargeView);
    INIBOOL(ShowLabels);
    INIBOOL(ShowToolTips);
    INIBOOL(NoNumbersShow);
    INIBOOL(NoMaxNumbersShow);
    INIBOOL(ShowSystemTasksActivity);

    m_DispMode = (EDisplayMode)theApp.GetProfileInt(m_CurrentSchema, "DispMode", DISP_UNKNOWN);
    if ( m_DispMode == DISP_UNKNOWN ) 
    {
        m_DispMode = DISP_COLUMN;
        //  Support old settings files
        if ( theApp.GetProfileInt(m_CurrentSchema, "ShowGraphs", 0) > 0 )
            m_DispMode = DISP_GRAPH;
        else if ( theApp.GetProfileInt(m_CurrentSchema, "ShowLights", 0) > 0 )
            m_DispMode = DISP_GLINT;
    }

    INISTR(ComputerName);

    INIINT(IndicTimeInterval);
    if ( m_IndicTimeInterval > MAX_INTERVAL_INDIC*2 || m_IndicTimeInterval < MIN_INTERVAL_INDIC*0.5 )
        return false;               // invalid settings
    INIFLT(BrightCoeff);
    if ( m_BrightCoeff > MAX_BRIGHTNESS*2 || m_BrightCoeff < MIN_BRIGHTNESS*0.5 )
        return false;               // invalid settings

    INIINT(PosX);
    INIINT(PosY);
    INIINTV(LastMinibarPosX,m_PosX);
    INIINTV(LastMinibarPosY,m_PosY);
    INIINT(PosWidth);
    //TRACE("LoadSettings() m_PosX:%d  m_LastMinibarPosX:%d\r\n",m_PosX,m_LastMinibarPosX);

    INICLR(titlesColor);
    INICLR(bordersColor);
    INICLR(processesColor);

    #define MKINDNAME(x)  sprintf(sb, "%s%03d", x, ind); 
    #define INDGETSTR(s,mem) MKINDNAME(s); isets->mem = theApp.GetProfileString(sec, sb);
    #define INDGETFLT(s,mem) MKINDNAME(s); str = theApp.GetProfileString(sec, sb); isets->mem = atof((LPCSTR)str);
    #define INDGETINT(s,mem) MKINDNAME(s); isets->mem = theApp.GetProfileInt(sec, sb, 0); 
    #define INDGETITYP(s,mem,typ) MKINDNAME(s); isets->mem = (typ)theApp.GetProfileInt(sec, sb, 0); 
    #define INDGETCLR(s,mem) MKINDNAME(s); isets->mem = IntToColor( theApp.GetProfileInt(sec, sb, 0) ); 

    // Read indicators data
    m_Indicators = theApp.GetProfileInt(sec, "Indicators", 0);
    if ( m_Indicators > MAX_INDICATORS_NUM) m_Indicators = MAX_INDICATORS_NUM;

    char sb[40];
    for (int ind = 0; ind < m_Indicators; ind++)
    {
        // Check Object Name first
        sprintf(sb, "%s%03d", "ObjName", ind);
        str = theApp.GetProfileString(sec, sb);
        if ( str.GetLength() == 0 )
            continue;

        CIndicatorSettings* isets = new CIndicatorSettings;
        isets->m_strCategory = str;

        INDGETSTR("Counter", m_strCounter);
        INDGETSTR("Instance", m_strInst);
        INDGETSTR("SmallTitle", m_strSmallTitle);
        INDGETFLT("MaxValue", m_IndMaxV);
        INDGETINT("Flags", m_flags);
        INDGETCLR("Color", m_color);

        isets->MakeText();
        m_indicSets.AddTail( isets );
    }

    if ( m_indicSets.GetCount() == 0 )
        return false;

    return true;
}
#undef INISTR
#undef INIINT
#undef INIFLT
#undef INICLR
#undef INIBOOL

//////////////////////////////////////////////////////////////////////////////

static char str_save_buffer[MAX_PATH+2];
#define INISTR(s)   theApp.WriteProfileString(sec, #s, m_##s )
#define INIINT(s)   theApp.WriteProfileInt(sec, #s, m_##s )
#define INIFLT(s)   sprintf(str_save_buffer,"%.8g", m_##s); theApp.WriteProfileString(sec, #s, str_save_buffer); 
#define INIBOOL(s)  theApp.WriteProfileInt(sec, #s, (int)m_##s);
#define INICLR(s)   theApp.WriteProfileInt(sec, #s, ColorToInt(m_##s));

// Save only modes
void CSettings::SaveModes(const char * sec)
{
    INIBOOL(isOnTop);
    INIBOOL(isMiniBar);         
    INIBOOL(isShowProcesses);
    INIBOOL(FullSet);
    INIBOOL(LargeView);
    INIBOOL(ShowLabels);
    INIBOOL(ShowToolTips);    
    INIBOOL(NoNumbersShow);
    INIBOOL(NoMaxNumbersShow);
    INIBOOL(ShowSystemTasksActivity);
    theApp.WriteProfileInt(sec, "DispMode", (int)m_DispMode );

    INIINT(IndicatorsInRow);
    INIINT(IndicTimeInterval);
    INIINT(ProcessTimeInterval);
    INIINT(ProcessTrackingTime);
    INIINT(NumberShowInterval);
    INIFLT(BrightCoeff);

    INIINT(PosX);
    INIINT(PosY);
    INIINT(LastMinibarPosX);
    INIINT(LastMinibarPosY);
    INIINT(PosWidth);

    INICLR(titlesColor);
    INICLR(bordersColor);
    INICLR(processesColor);

    INISTR(ComputerName);
}

// Save all current settings
void CSettings::SaveSettings(const char * sec)
{
    g_version_settings = atof(sets.m_Version); 
    theApp.WriteProfileString(CURRENT_SETTINGS_SECTION, "Version", m_Version); // set program version
    theApp.WriteProfileString(CURRENT_SETTINGS_SECTION, "Scheme", m_CurrentSchema); // save last scheme name 
    theApp.WriteProfileInt(CURRENT_SETTINGS_SECTION, "Modified", g_IsSchemaModified); // save scheme modified flag

    // m_CurrentSchema = sec;
    SaveModes(sec);

    int k;
    m_Indicators = m_indicSets.GetCount();
    INIINT(Indicators);

    #define INDSETSTR(s,mem) MKINDNAME(s); theApp.WriteProfileString(sec, sb, isets->mem);
    #define INDSETFLT(s,mem) MKINDNAME(s); sprintf(str_save_buffer, "%.8g", isets->mem); theApp.WriteProfileString(sec, sb, str_save_buffer); 
    #define INDSETINT(s,mem) MKINDNAME(s); theApp.WriteProfileInt(sec, sb, (int)isets->mem ); 
    #define INDSETCLR(s,mem) MKINDNAME(s); theApp.WriteProfileInt(sec, sb, ColorToInt(isets->mem) ); 

    char sb[40];
    POSITION pos = m_indicSets.GetHeadPosition();
    for ( int ind = 0; ind < m_indicSets.GetCount(); ind++ )
    {
        CIndicatorSettings* isets = m_indicSets.GetNext(pos);
        INDSETSTR("ObjName", m_strCategory);
        INDSETSTR("Counter", m_strCounter);
        INDSETSTR("Instance", m_strInst);
        INDSETSTR("SmallTitle", m_strSmallTitle);
        INDSETFLT("MaxValue", m_IndMaxV);
        INDSETINT("Flags", m_flags);
        INDSETCLR("Color", m_color);
    }

}

//////////////////////////////////////////////////////////////
/*  Removed to avoid overhead.
void CSettings::SavePosition()  
{
    const char * sec = CURRENT_SETTINGS_SECTION;
    INIINT(PosX);
    INIINT(PosY);
    INIINT(LastMinibarPosX);
    INIINT(LastMinibarPosY);
    INIINT(PosWidth);
}   */

#undef INISTR
#undef INIINT
#undef INIFLT
#undef INICLR
#undef INIBOOL


    
// *******************************************************************************************************
//                                  Saving current settings before exit
// *******************************************************************************************************
void CSettings::FinalSave()
{
    if ( !g_FinalSaveDone )
    {
        SaveSettings(CURRENT_SETTINGS_SECTION); // save current
        // sets.Clear();  ? overhead
        g_FinalSaveDone = true;
    }

    if ( g_LogLevel > 0 && g_FLog != 0 ) 
    {
        fclose(g_FLog);
        g_FLog = 0;
    }
}



/////////////////////////////////////////////////////////////////////////////////////////
CIndicatorSettings* CSettings::FindIndicatorSetsByTitle(LPCTSTR indText)
{
    POSITION pos = m_indicSets.GetHeadPosition();
    for ( int ind = 0; ind < m_indicSets.GetCount(); ind++ )
    {
        CIndicatorSettings* isets = m_indicSets.GetNext(pos);
        if ( strcmp(isets->m_IndText, indText) ==0 )
            return isets;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// Gets Registry Key:  "HKEY_CURRENT_USER\Software\<company name>\<application name>"
HKEY CSettings::GetMainRegistryKey()
{
    char    keyName[250];
    HKEY    key;
    DWORD   disposition;

    sprintf(keyName,"Software\\%s\\%s", INI_AUTHOR_NAME, theApp.m_pszAppName);

    DWORD hr = RegOpenKeyEx( HKEY_CURRENT_USER,  keyName, 0, 
                             KEY_ALL_ACCESS, &key);

    if ( !FAILED(hr) ) 
        return key;

    hr = RegCreateKeyEx( HKEY_CURRENT_USER,  keyName, 0, 0,
                         REG_OPTION_NON_VOLATILE, 
                         KEY_ALL_ACCESS, 0,  // SAM, security
                         &key, &disposition );

    if ( FAILED(hr) ) {
        AfxMessageBox(_T("Registry key 'Hkey_Current_User' is inaccessible"));
        return 0;
    }

    return key;
}

//////////////////////////////////////////////////////////////////////////////
void CSettings::GetSchemaList(CStringArray& schemes)
{
    schemes.RemoveAll();

    if ( theApp.m_PortableMode )
    {
        // -------------------------------------------
        //      Get Sections names from INI-file   
        // -------------------------------------------
        CStdioFile  iniFile;
        if ( iniFile.Open(INI_FULL_FILE_NAME, CFile::modeRead) )
        {
            CString line;
            while( iniFile.ReadString(line) ) 
            {
                if ( line.GetLength() < 2 || line[0] != '[' )
                    continue;
                int pos = line.Find(']');
                if ( pos < 2 )
                    continue;
                CString  sectName = line.Mid(1, pos - 1 );
                if ( sectName.CompareNoCase(CURRENT_SETTINGS_SECTION) != 0 &&
                     sectName.CompareNoCase(INFO_SETTINGS_SECTION) != 0 &&
                     sectName.CompareNoCase(OPIONS_SETTINGS_SECTION) != 0 )
                    schemes.Add( sectName );
            }
            iniFile.Close();
        }
    }
    else
    {
        // -----------------------------
        //      Get from Registry   
        // -----------------------------
        HKEY mainKey = GetMainRegistryKey();
        if ( mainKey == 0 ) return; 

        char    keyName[300];
        HKEY    key;
        DWORD   hr = 0;
        DWORD   idx = 0;
        DWORD   size = 0;

        FILETIME last_time;

        while ( 1 ) 
        {
            size = 256;
            hr = RegEnumKeyEx( mainKey, idx, keyName, &size, 
                                 0, // reserved
                                 0, 0, &last_time);     // class, class-buffer, time
            if ( FAILED(hr) || hr == ERROR_NO_MORE_ITEMS ) 
                break;

            idx++;
            if ( strcmpi(keyName,CURRENT_SETTINGS_SECTION) != 0 &&
                 strcmpi(keyName,INFO_SETTINGS_SECTION) != 0    &&
                 strcmpi(keyName,OPIONS_SETTINGS_SECTION) != 0 )
                schemes.Add( keyName );
        }

        RegCloseKey(mainKey);
    }
}


/////////////////////////////////////////////////////////////////////
COLORREF IntToColor(int i)
{
    return RGB( ((i>>16)&0xff), ((i>>8)&0xff), (i&0xff) );
}

/////////////////////////////////////////////////////////////////////
int ColorToInt(COLORREF c0)
{
    return (int) ( (GetRValue(c0) << 16 ) | (GetGValue(c0) << 8 ) | GetBValue(c0) );
}

/////////////////////////////////////////////////////////////////////
void CSettings::LoadCountersGroups()
{
    int k = theApp.GetProfileInt(OPIONS_SETTINGS_SECTION, "CounterGroupsNumber", 0);
    if ( m_CounterGroupsNumber > k )
    {
        SaveDefaultCountersGroups();
    }

    LoadStringsArray(OPIONS_SETTINGS_SECTION, "CounterGroupsNames", m_CounterGroupsNames);       
    for (int i = 0; i < m_CounterGroupsNumber && i < MAX_COUNTER_GROUPS; i++)
    {
        CString skey;
        skey.Format("CounterGroup%03d" , i);
        LoadStringsArray(OPIONS_SETTINGS_SECTION, skey, m_CounterGroups[i]);       
    }

}

////////////////////////////////////////////////////////////////////
void CSettings::SaveDefaultCountersGroups()
{
    // NOTE. Don't forget to put into cinstructor: m_CounterGroupsNumber = 8;    !!!

    static LPCTSTR counter_groups_names_def = "System;Server;IO;Internet;Networking;Connections;Processes;.Net";
    
    static LPCTSTR counter_groups_def[] = {
    /*0*/  "$System$;$Processor;$ProcessorPerformance;$Memory$;Battery;$Paging;$Event;$Objects$;WMI;$Windows;Security System;$Synchronization;MSDTC;",
    /*1*/  "$Server;$Active Server;MSSQL;IPHTTPS;Client Side;Database;",
    /*2*/  "LogicalDisk;PhysicalDisk;$Cache$;USB;$Offline Files;",
    /*3*/  "Http;HTTP;TCP;Browser;IAS;Web;SMTP;Internet;Teredo;IPsec;$WFPv4;$WFPv6;$WFP;$TCPv4;$TCPv6;$UDPv4;$UDPv6;$ICMP;$ICMPv6;BITS;Peer Name;",
    /*4*/  "Redirector;ICMP;IP;NetWare;NWLink;NetBIOS;$NBT;Networking;Network;Distributed;$IPv4;$IPv6;$Pacer;",
    /*5*/  "UDP;RAS;VPN;Terminal;ACS;Telephony;MSMQ;Fax;$RSVP;Outlook;",
    /*6*/  "Process;Print Queue;Job Object;Indexing;",
    /*7*/  ".Net;.NET;ASP.NET;SMSvcHost;",
           0 //  Other
    };

    theApp.WriteProfileInt(OPIONS_SETTINGS_SECTION, "CounterGroupsNumber", m_CounterGroupsNumber);
    theApp.WriteProfileString(OPIONS_SETTINGS_SECTION, "CounterGroupsNames", counter_groups_names_def);       

    for (int i = 0; i < m_CounterGroupsNumber && i < MAX_COUNTER_GROUPS; i++)
    {
        CString skey;
        skey.Format("CounterGroup%03d" , i);
        theApp.WriteProfileString(OPIONS_SETTINGS_SECTION, skey, counter_groups_def[i]);       
    }

}

////////////////////////////////////////////////////////////////////
//   Check if string exists and if not add it to the array.
void AddToStringsArray(CStringArray &strings, LPCTSTR str, bool case_sensitive)
{
    if ( str == 0 || strlen(str) == 0 )
        return;
    for (int i = 0; i < strings.GetSize() ; i++) {
        if ( ( case_sensitive && strcmp(str, strings.GetAt(i) ) == 0 ) ||
             (!case_sensitive && strcmpi(str, strings.GetAt(i) ) == 0 ) ) 
            return;
    }
    strings.Add(str);
}

///////////////////////////////////////////////////////////////////
// Loads from string in [_Current_]/key - separated by ';'
void LoadStringsArray(LPCTSTR section, LPCTSTR key_name, CStringArray& strings)  
{    
    CString str = theApp.GetProfileString(section, key_name, "");
    CString s1;

    strings.RemoveAll();

    if ( str.GetLength() == 0 )
        return;

    const char *p = (LPCTSTR)str;    
    int i1 = 0;             // start of element
    int i2 = 0;             // end of elment

    while(i2 < str.GetLength() )
    {
        if (p[i2] == ';') {
            if (i2 > i1) {
                s1 = str.Mid(i1, i2 - i1);
                strings.Add((LPCTSTR)s1);
            }
            i2++;
            i1 = i2;
        }
        i2++;
    }
    if (i2 > i1 && i1 < str.GetLength()) {
        s1 = str.Mid(i1);
        strings.Add((LPCTSTR)s1);
    }

}

//////////////////////////////////////////////////////////////////
// Saves to string  in [_Current_]/key - separated by ';'
void SaveStringsArray(LPCTSTR section, const char * key_name, CStringArray& strings) {    
    CString str = "";

    for (int i = 0; i < strings.GetSize() ; i++) {
        if ( i > 0) 
            str += ";";
        str += strings.GetAt(i);
    }

    theApp.WriteProfileString(section, key_name, (LPCTSTR)str);
}

//////////////////////////////////////////////////////////////////
CString GetPrintableCompName(LPCTSTR comp_name)
{
    CString  str = comp_name;
    if ( str.GetLength() == 0 ) str = LOCAL_MACHINE_NAME;
    return str;
}

//===============================================================
//          C S a v e d S t r i n g s S e t
//===============================================================

void    CSavedStringsSet::Load(LPCTSTR section, LPCTSTR key)
{
    LoadStringsArray(section, key, *this);
    m_SectionName = section;
    m_KeyName = key;
}

///////////////////////////////////////////////////////////////////
// add and save list (empty string not added)
void    CSavedStringsSet::Add(LPCTSTR str)                   
{
    ASSERT( m_SectionName.GetLength() != 0 && m_KeyName.GetLength() || 0 );
    AddToStringsArray(*this, str, false /*case_insensitive*/);

    if ( GetSize() > MAX_HISTORY_STRINGS )
    {
        RemoveAt(0);        // Remove the oldest element.
    }

    SaveStringsArray(m_SectionName, m_KeyName, *this);    
}

///////////////////////////////////////////////////////////////////////
void  ClearSchemaModified() 
{
    g_IsSchemaModified = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
void CSettings::SetScheme_Standard()
{
    SetDefaultModes();
    Clear();

    SS_CPU  

    if ( g_OSVersion >= OS_VERSION_VISTA )
    {
        SS_CPU_PERF
    }
    else
    {
        SS_CACHE_FAULTS
    }
    
    SS_CACHE_BYTES  
    SS_NETWK_RECVD
    SS_NETWK_SEND

    SS_SYSCALLS 
    SS_PAGES

    SS_PHDISKR
    SS_PHDISKW
    SS_FILE_READS   
    SS_FILE_WRITES

    SS_EXCEPTNS 
    SS_OBJ_THREADS
    SS_MEMAVL   
}

/////////////////////////////////////////////////////////////////////////////////////////
void CSettings::SetScheme_Large()
{
    SetDefaultModes();
    Clear();
    m_IndicatorsInRow       = 13;
    m_DispMode      = DISP_GRAPH;
    m_isMiniBar     = false;
    m_LargeView         = true;

    SS_CPU
    if ( g_OSVersion >= OS_VERSION_VISTA )
    {
        SS_CPU_PERF
    }
    else
    {
        SS_CPUQUELEN    
    }

    SS_CPU_PRVG
    SS_INTS 
    SS_CACHE_FAULTS

    SS_PAGES
    SS_PGFLT
    SS_MEMC_OUT     

    SS_TCPSEGS_SENT 
    SS_TCPSEGS_RECVD
    SS_IP_RCVD
    SS_IP_SEND
    SS_NETWK_RECVD
    SS_NETWK_SEND
    SS_NETWK_RCV_ERR
    SS_NETWK_SND_ERR

    SS_REDIRECTOR_TOT

    SS_CONTXT_SWTCH
    SS_SYSCALLS
    SS_XP_COMTRANS  
    SS_ALIGN_FIX

    SS_PHDISKR
    SS_PHDISKW
    SS_FILE_READS   
    SS_FILE_WRITES

    SS_OBJ_EVENTS 
    SS_OBJ_THREADS
    
    SS_EXCEPTNS 
    SS_CACHE_BYTES 
    SS_MEMAVL   
}

/////////////////////////////////////////////////////////////////////////////////////////
void CSettings::SetScheme_Small()
{
    SetDefaultModes();
    Clear();
    m_DispMode      = DISP_GLINT;

    m_isShowProcesses   = false;
    m_FullSet           = true;
    m_IndicatorsInRow   = 12;

    SS_CPU      
    if ( g_OSVersion >= OS_VERSION_VISTA )
    {
        SS_CPU_PERF
    }

    SS_SYSCALLS     
    SS_PAGES    
    SS_CACHE_BYTES
    if ( g_OSVersion < OS_VERSION_VISTA )
    {
        SS_CACHE_FAULTS
    }

    if ( g_OSVersion == OS_VERSION_NT4 ) { 
        SS_NETWK_SEGM /* NT4 */
        SS_FILE_READS   
        SS_FILE_WRITES  
    }
    else {
        SS_NETWK_RECVD
        SS_NETWK_SEND
        SS_PHDISK       /* Win 2000, XP */
        SS_FILE_OPS
    }

    SS_MEMAVL       
}

/////////////////////////////////////////////////////////////////////////////////////////
void CSettings::SetScheme_Few_Graphs()
{
    SetDefaultModes();

    Clear();
    m_isMiniBar         = false;
    m_LargeView         = true;
    m_isShowProcesses   = false;
    m_IndicatorsInRow   = 8;
    m_DispMode      = DISP_GRAPH;

    // Only one of these pairs will be active
    SS_CPU
    SS_PHDISKR
    SS_PHDISKW
    SS_NETWK_RECVD
    SS_NETWK_SEND 
    SS_MEMAVL       
}



