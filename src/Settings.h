// CSettings.h: interface for the CSettings class.
// Author: Alexander Vechersky, 2010 

//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SETTINGS_H__D018C685_AA94_444D_B237_4116957A291A__INCLUDED_)
#define AFX_SETTINGS_H__D018C685_AA94_444D_B237_4116957A291A__INCLUDED_

#include <Afxtempl.h>
#include "AutoPtr.h"
#include "IndicatorSettings.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define INI_AUTHOR_NAME             "Alexander Vechersky"
#define INI_FULL_FILE_NAME          _T(".\\glint.ini")
#define CURRENT_SETTINGS_SECTION        "_Current_"
#define OPIONS_SETTINGS_SECTION         "_Options_"
#define INFO_SETTINGS_SECTION             "__Info__"

#define MAX_INDICATORS_NUM      500
#define SCHEMES_SECTION_SIZE    16000 

#define  MAX_INTERVAL_INDIC  5000
#define  MIN_INTERVAL_INDIC  150

#define  MAX_INTERVAL_VALUES 5000
#define  MIN_INTERVAL_VALUES 500

#define  MAX_INTERVAL_PROCESSES 5000
#define  MIN_INTERVAL_PROCESSES 500

#define MIN_PROCESSES_TRACKING_SECS     20      // min CPU history in seconds.
#define MAX_PROCESSES_TRACKING_SECS     180     // max CPU history in seconds.

#define  MAX_BRIGHTNESS     8.0
#define  MIN_BRIGHTNESS     0.15

#define  MAX_COUNTER_GROUPS     30
#define  MAX_HISTORY_STRINGS    20

#define SCHEMAS_SECTION_NAME_STANDARD            "Standard"
#define SCHEMAS_SECTION_NAME_LARGE               "Large"
#define SCHEMAS_SECTION_NAME_DEFAULT_SMALL       "Small"
#define SCHEMAS_SECTION_NAME_DEFAULT_FEW_GRAPHS  "Few_Graphs"

typedef CTypedPtrList<CPtrList,CIndicatorSettings*> TIndSetsList;


/////////////////////////////////////////////////////////////////
class CSavedStringsSet : public CStringArray
{
public:
    void    Load(LPCTSTR section, LPCTSTR key);
    void    Add(LPCTSTR str);                   // add and save list (empty string not added)

protected:
    CString     m_SectionName;          // section to store list 
    CString     m_KeyName;              // key to store list
};

/////////////////////////////////////////////////////////////////
class CSettings  
{
public:
    CSettings();
    virtual ~CSettings();
    CSettings& operator =( const CSettings& o);     // Copy constructor

    AutoPtr<char> m_schemas;    // buffer to contain schemas, name section
    CString     m_CurrentSchema;  
    CString     m_ComputerName;  // remote computer name to watch ("" => local)
    CString     m_Version;  

    bool    m_isMiniBar;        // true : minimum-size bar (10 hight)
    bool    m_isOnTop;
    bool    m_isShowProcesses;
    bool    m_LargeView;        // false: show only basis counters
    bool    m_AllowOnlyOneInstance;        // false: allow many instances
    bool    m_FullSet;          // false: show only basis counters
    bool    m_ShowLabels;       // false: show only indicators
    bool    m_ShowToolTips;     // false: don't show tooltips
    bool    m_NoNumbersShow;    // false: show numbers inside/below indicators
    bool    m_NoMaxNumbersShow; // false: show max value above indicators
    bool    m_ShowSystemTasksActivity;   // false: do not show system processes in prceses ToolTip
    
    EDisplayMode   m_DispMode;           // Display mode

    int     m_IndicatorsInRow;          // # of indicators in a each row
    int     m_IndicTimeInterval;        // msec
    int     m_NumberShowInterval;       // msec
    double  m_BrightCoeff;              // 1.0  - multiply relative value by
    int     m_ProcessTimeInterval;      // msec
    int     m_ProcessTrackingTime;      // time sec to track process activity


    // current positions
    int     m_PosX;
    int     m_PosY;
    int     m_PosWidth;
    int     m_PosHeight;
    int     m_LastMinibarPosX;
    int     m_LastMinibarPosY;


    COLORREF    m_titlesColor;
    COLORREF    m_bordersColor;             // border of non-hovered indicator
    COLORREF    m_hoverBordersColor;        // border of hovered indicator
    COLORREF    m_processesColor;

    COLORREF    m_BorderLineColorDark  ;
    COLORREF    m_BorderLineColorLight ;

    int     m_Indicators;      // # of Indicators
    TIndSetsList m_indicSets;


// OPTIONS SECTION DATA
    static CStringArray    m_CounterGroupsNames;     // remote machines names history
    static CStringArray    m_CounterGroups[MAX_COUNTER_GROUPS];
    static int             m_CounterGroupsNumber;

    static  CSavedStringsSet   m_computers;     // remote machines names history

// TEMP DATA
    double  m_OurFrequency;

public:

    void SetDefaultModes();
    HKEY GetMainRegistryKey();
    CIndicatorSettings* FindIndicatorSetsByTitle(LPCTSTR indText);

    void SetScheme_Small();
    void SetScheme_Standard();
    void SetScheme_Large();
    void SetScheme_Few_Graphs();

    void Clear();           // clear indicator settings
    void InitSettings();
    void FinalSave();
    //void SavePosition();
    void SaveSettings(const char * section);
    bool LoadSettings(const char * section);
    void SetDefaults(bool to_load_scheme = true);   // load appropriate scheme afterwards

    void GetSchemaList(CStringArray& schemas);

    static void SaveDefaultCountersGroups();
    static void LoadCountersGroups();

private:
    void GetHashedName(CString & str, DWORD* arr, DWORD size);

public:
    void SaveModes(const char * sec = CURRENT_SETTINGS_SECTION);    // save only modes (single values)
};

/////////////////////////////////////////////////////////////////////
//   G l o b a l    D a t a
extern  CSettings  sets;        // stores current settings
extern  CSettings  optsets;     // stores settings for Options dialog

/////////////////////////////////////////////////////////////////////
//   G l o b a l    F u n c t i o n
void CopyTIndSetsList(TIndSetsList& list_dest, TIndSetsList& list_src);
void AddToStringsArray(CStringArray& strings, LPCTSTR string, bool case_sensitive);
void  ClearSchemaModified();
bool  inline CheckDispMode(EDisplayMode mode) { return sets.m_DispMode == mode; }
bool    CheckSystemProcessName(LPCSTR instName);

#endif // !defined(AFX_SETTINGS_H__D018C685_AA94_444D_B237_4116957A291A__INCLUDED_)
