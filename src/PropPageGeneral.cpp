// PropPageGeneral.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "panel.h"
#include "Settings.h"
#include "SettingsDlg.h"
#include "PropPageGeneral.h"
#include "RemoteConnectionDlg.h"
#include "SchemaSave.h"
#include "PerfData.h"
#include "Gadgets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_inCheckRange = false;    // true, when we already inside check range function
bool g_SchemaWasModified = false;
bool g_LastLadedSchemaChanged = false;  // true: when schema was changhed in current dialog since last load/save

#define CHECK_RANGE(txt,v,vmin,vmax) \
    Modify();   \
    if ( CheckRange(txt, &v, vmin, vmax) ) \
        { UpdateControlsData(true); }

/////////////////////////////////////////////////////////////////////////////
// CPropPageGeneral property page

IMPLEMENT_DYNCREATE(CPropPageGeneral, CPropertyPage)

CPropPageGeneral::CPropPageGeneral() : CPropertyPage(CPropPageGeneral::IDD)
{
    //{{AFX_DATA_INIT(CPropPageGeneral)
    m_FullView = FALSE;
    m_MiniBar = FALSE;
    m_ShowProcesses = FALSE;
    m_ShowValues = FALSE;
    m_ShowLabels = FALSE;
    m_SystemTaskActivity = FALSE;
    m_AlwaysOnTop = FALSE;
    m_IndicsInterval = 0;
    m_IndicsInRow = 0;
    m_ProcessesInterval = 0;
    m_ProcessTrackTime = 0;
    m_ValueShowInterval = 0;
    //m_BrigtnessCoeff = 0.0;
    m_ShowToolTips = FALSE;
    m_LargeView = FALSE;
    m_DispMode = DISP_COLUMN;
    //}}AFX_DATA_INIT

    m_Modified = false;
    m_IsInitialized = false;
}

CPropPageGeneral::~CPropPageGeneral()
{
}

void CPropPageGeneral::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPropPageGeneral)
    DDX_Check(pDX, IDC_CHECK_FULL_VIEW, m_FullView);
    DDX_Check(pDX, IDC_CHECK_MINI_BAR, m_MiniBar);
    DDX_Check(pDX, IDC_CHECK_SHOW_PROCESSES, m_ShowProcesses);
    DDX_Check(pDX, IDC_CHECK_SHOW_VALUES, m_ShowValues);
    DDX_Check(pDX, IDC_CHECK_SHOW_LABELS, m_ShowLabels);
    DDX_Check(pDX, IDC_CHECK_ALWAYS_ON_TOP, m_AlwaysOnTop);
    DDX_Check(pDX, IDC_CHECK_SHOW_SYS_TASKS_ACTIVITY, m_SystemTaskActivity);
    DDX_Text(pDX, IDC_EDIT_INDICATORS_INTERVAL, m_IndicsInterval);
    DDX_Text(pDX, IDC_EDIT_INDICS_IN_ROW, m_IndicsInRow);
    DDX_Text(pDX, IDC_EDIT_PROCESSES_INTERVAL2, m_ProcessesInterval);
    DDX_Text(pDX, IDC_EDIT_PROCESSES_TRACK_INTERVAL, m_ProcessTrackTime);
    DDX_Text(pDX, IDC_EDIT_VALUES_INTERVAL, m_ValueShowInterval);
    //DDX_Text(pDX, IDC_EDIT_Brightness, m_BrigtnessCoeff);
    DDX_Check(pDX, IDC_CHECK_SHOW_TIPS, m_ShowToolTips);
    DDX_Check(pDX, IDC_CHECK_LARGE_VIEW, m_LargeView);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageGeneral, CPropertyPage)
    //{{AFX_MSG_MAP(CPropPageGeneral)
    ON_BN_CLICKED(IDC_CHECK_MINI_BAR, OnCheckMiniBar)
    ON_BN_CLICKED(IDC_CHECK_FULL_VIEW, OnCheckFullView)
    ON_BN_CLICKED(IDC_CHECK_SHOW_VALUES, OnCheckShowValues)
    ON_BN_CLICKED(IDC_CHECK_SHOW_PROCESSES, OnCheckShowProcesses)
    ON_BN_CLICKED(IDC_CHECK_ALWAYS_ON_TOP, OnCheckAlwaysOnTop)
    ON_BN_CLICKED(IDC_CHECK_SHOW_LABELS, OnCheckShowLabels)
    ON_BN_CLICKED(IDC_BUTTON_SCHEME_LOAD, OnButtonSchemeLoad)
    ON_BN_CLICKED(IDC_BUTTON_SCHEME_SAVE, OnButtonSchemeSave)
    ON_BN_CLICKED(IDC_BUTTON_RESET_DEFAULTS, OnButtonResetDefaults)
    ON_BN_CLICKED(IDC_CHECK_SHOW_TIPS, OnCheckShowTips)
    ON_EN_KILLFOCUS(IDC_EDIT_INDICATORS_INTERVAL, OnKillfocusEditIndicatorsInterval)
    ON_EN_KILLFOCUS(IDC_EDIT_VALUES_INTERVAL, OnKillfocusEditValuesInterval)
    ON_EN_KILLFOCUS(IDC_EDIT_PROCESSES_INTERVAL2, OnKillfocusEditProcessesInterval2)
    ON_EN_KILLFOCUS(IDC_EDIT_PROCESSES_TRACK_INTERVAL, OnKillfocusEditProcessesTrackInterval)
    ON_EN_KILLFOCUS(IDC_EDIT_INDICS_IN_ROW, OnKillfocusEditIndicsInRow)
    //ON_EN_KILLFOCUS(IDC_EDIT_Brightness, OnKillfocusEDITBrightness)
    //ON_EN_CHANGE(IDC_EDIT_Brightness, OnChangeEDITBrightness)
    ON_EN_CHANGE(IDC_EDIT_INDICATORS_INTERVAL, OnChangeEditIndicatorsInterval)
    ON_EN_CHANGE(IDC_EDIT_INDICS_IN_ROW, OnChangeEditIndicsInRow)
    ON_EN_CHANGE(IDC_EDIT_PROCESSES_INTERVAL2, OnChangeEditProcessesInterval2)
    ON_EN_CHANGE(IDC_EDIT_PROCESSES_TRACK_INTERVAL, OnChangeEditProcessTrackTime)
    ON_EN_CHANGE(IDC_EDIT_VALUES_INTERVAL, OnChangeEditValuesInterval)
    ON_BN_CLICKED(IDC_CHECK_LARGE_VIEW, OnCheckLargeView)
    ON_BN_CLICKED(IDC_ONLY_ONE_INSTANCE, OnCheckAllowOnlyOneInstance)
    ON_BN_CLICKED(IDC_DELETE, OnSchemeDelete)
    ON_LBN_DBLCLK(IDC_LIST_SCHEMES, OnDblclkListSchemes)
    ON_CBN_SELCHANGE(IDC_COMBO_VIEW_MODES, OnCbnSelchangeCombo1)
    ON_BN_CLICKED(IDC_CHECK_SHOW_SYS_TASKS_ACTIVITY, OnBnClickedCheckShowSysTasksActivity)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageGeneral message handlers

BOOL CPropPageGeneral::OnApply() 
{
    UpdateSettings();
    if ( g_IsSchemaModified )
    {
        //optsets.m_CurrentSchema = "";
    }
    SetModified(FALSE);

    sets = optsets;
    g_IsSetsChanged = true;

    CancelToClose();
    return CPropertyPage::OnApply();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageGeneral::OnInitDialog() 
{
    bool was_scheme_modified = g_IsSchemaModified;
    m_IsInitialized = false;

    CPropertyPage::OnInitDialog();
    g_szHelpTxt = csz_HelpText_GeneralSettings;
    g_szHelpTitle = csz_HelpTitle_GeneralSettings;

    FillSchemeList();
    RedrawWindow();

    UpdateControlsData();
    SetModified(FALSE);

    // NOTE: Restore Schema modification flag only after all assignments are done, 
    //       otherwise the flag will be set on initializations. 
    g_IsSchemaModified = was_scheme_modified;
    SetSchemaNameText();

    m_IsInitialized = true;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

// ------------------------------------------------------------------------------------------
//      Fill list of view modes.
// ------------------------------------------------------------------------------------------
void    CPropPageGeneral::FillViewModeList()
{
    static struct ModeDataElt {
        const char*  Title;
        EDisplayMode Mode;
    } s_ModeData[] = {
        { "Columns",        DISP_COLUMN, },
        { "Graphs",         DISP_GRAPH,  },
        { "Glint lights",   DISP_GLINT,  },
//      { "Dots",           DISP_DOT,    },
        { 0,                DISP_UNKNOWN }
    };

    m_DispMode = optsets.m_DispMode;
    CComboBox* combo =  (CComboBox*) GetDlgItem(IDC_COMBO_VIEW_MODES);
    combo->Clear();

    for ( int k = 0; k < sizeof(s_ModeData) / sizeof(ModeDataElt); ++k )
    {
        if ( s_ModeData[k].Title == 0 )
            break;

        int idx = combo->AddString( s_ModeData[k].Title );
        combo->SetItemData(idx, s_ModeData[k].Mode );
        
        if ( optsets.m_DispMode == s_ModeData[k].Mode )    // Put selection
            combo->SetCurSel(idx);
    }
}

// ------------------------------------------------------------------------------------------
//      Fill schemes list ListBox
// ------------------------------------------------------------------------------------------
void CPropPageGeneral::FillSchemeList()
{
    optsets.GetSchemaList(m_schemes);
    CListBox* list =  (CListBox*) GetDlgItem(IDC_LIST_SCHEMES);
    list->ResetContent();

    for (int i = 0; i < m_schemes.GetSize(); i++ )
    {
        CString schm = m_schemes.GetAt(i);
        if ( schm == CURRENT_SETTINGS_SECTION )
            continue;
        int res = list->AddString( schm );
        SetListHorizontalExtent(list, schm);
        ASSERT( res >= 0 );
    }
    list->SetCurSel(-1);

    SetSchemaNameText();
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::SetSchemaNameText()
{
    char buf[256];

    if ( optsets.m_CurrentSchema.GetLength() < 1 || 
         optsets.m_CurrentSchema == CURRENT_SETTINGS_SECTION )
        strcpy(buf, "Scheme: ");
    else
        sprintf(buf, "Scheme:  %s%s", (LPCTSTR)optsets.m_CurrentSchema,
                                     ((g_IsSchemaModified || g_LastLadedSchemaChanged) ? "  (modified)": "") );

    CStatic* stname =  (CStatic*) GetDlgItem(IDC_STATIC_SCHEME_NAME);
    stname->SetWindowText(buf);
    stname->RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::UpdateControlsData(bool edits_only)
{
    if ( !edits_only )
    {
        UpdCheck(this, IDC_CHECK_LARGE_VIEW, m_LargeView, optsets.m_LargeView);
        UpdCheck(this, IDC_CHECK_FULL_VIEW, m_FullView, optsets.m_FullSet);
        UpdCheck(this, IDC_CHECK_MINI_BAR, m_MiniBar, optsets.m_isMiniBar);
        UpdCheck(this, IDC_CHECK_SHOW_PROCESSES, m_ShowProcesses, optsets.m_isShowProcesses);
        UpdCheck(this, IDC_CHECK_SHOW_VALUES, m_ShowValues, !optsets.m_NoNumbersShow);
        UpdCheck(this, IDC_CHECK_SHOW_LABELS, m_ShowLabels, optsets.m_ShowLabels);
        UpdCheck(this, IDC_CHECK_SHOW_TIPS, m_ShowToolTips, optsets.m_ShowToolTips);
        UpdCheck(this, IDC_CHECK_ALWAYS_ON_TOP, m_AlwaysOnTop, optsets.m_isOnTop);
        UpdCheck(this, IDC_CHECK_SHOW_SYS_TASKS_ACTIVITY, m_SystemTaskActivity, optsets.m_ShowSystemTasksActivity);
    }

    CButton* butt1 =  (CButton*) this->GetDlgItem(IDC_ONLY_ONE_INSTANCE);
    butt1->SetCheck( sets.m_AllowOnlyOneInstance ? BST_CHECKED : BST_UNCHECKED );

    if ( !edits_only )
        m_IndicsInterval    = optsets.m_IndicTimeInterval   ;
    UpdEdit(this, IDC_EDIT_INDICATORS_INTERVAL, IntToString(m_IndicsInterval) );

    if ( !edits_only )
        m_IndicsInRow       = optsets.m_IndicatorsInRow     ;
    UpdEdit(this, IDC_EDIT_INDICS_IN_ROW, IntToString(m_IndicsInRow) );

    if ( !edits_only )
        m_ProcessTrackTime = optsets.m_ProcessTrackingTime ;
    UpdEdit(this, IDC_EDIT_PROCESSES_TRACK_INTERVAL, IntToString(m_ProcessTrackTime) );

    if ( !edits_only )
        m_ProcessesInterval = optsets.m_ProcessTimeInterval ;
    UpdEdit(this, IDC_EDIT_PROCESSES_INTERVAL2, IntToString(m_ProcessesInterval) );

    if ( !edits_only )
        m_ValueShowInterval = optsets.m_NumberShowInterval  ;
    UpdEdit(this, IDC_EDIT_VALUES_INTERVAL, IntToString(m_ValueShowInterval) );

    //if ( !edits_only )
    //    m_BrigtnessCoeff    = 1.0 / optsets.m_BrightCoeff   ;
    //UpdEdit(this, IDC_EDIT_Brightness, FloatToString(m_BrigtnessCoeff) );

    FillViewModeList();
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::UpdateSettings()
{
    optsets.m_isMiniBar         = m_MiniBar;
    optsets.m_isOnTop           = m_AlwaysOnTop;
    optsets.m_isShowProcesses   = m_ShowProcesses;
    optsets.m_LargeView         = m_LargeView;          
    optsets.m_FullSet           = m_FullView;           
    optsets.m_ShowLabels        = m_ShowLabels;     
    optsets.m_DispMode          = m_DispMode;         
    optsets.m_NoNumbersShow     = !m_ShowValues;    
    optsets.m_ShowSystemTasksActivity = m_SystemTaskActivity;    

    optsets.m_IndicTimeInterval     = m_IndicsInterval;
    optsets.m_IndicatorsInRow       = m_IndicsInRow;
    optsets.m_ProcessTimeInterval   = m_ProcessesInterval;
    optsets.m_ProcessTrackingTime   = m_ProcessTrackTime;
    optsets.m_NumberShowInterval    = m_ValueShowInterval;
    //optsets.m_BrightCoeff           = 1.0 / m_BrigtnessCoeff;
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::Modify()
{
    if ( !g_inCheckRange )
        UpdateData();

    SetModified(TRUE);
    m_Modified = true;
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnCheckMiniBar() 
{
    Modify();
}

void CPropPageGeneral::OnCheckLargeView() 
{
    Modify();
}

void CPropPageGeneral::OnCheckAllowOnlyOneInstance() 
{
    CButton* button = (CButton*) GetDlgItem(IDC_ONLY_ONE_INSTANCE);
    int isToAllow = ((button->GetCheck() == BST_CHECKED ) ? 1 : 0) ;

    sets.m_AllowOnlyOneInstance = (isToAllow ? true : false) ;
    optsets.m_AllowOnlyOneInstance = sets.m_AllowOnlyOneInstance;     

    // Just change global value
    // theApp.WriteProfileInt(OPIONS_SETTINGS_SECTION, "AllowOnlyOneInstance", isToAllow);  // not that simple :-)
}

void CPropPageGeneral::OnCheckFullView() 
{
    Modify();
}

void CPropPageGeneral::OnCheckShowValues() 
{
    Modify();
}

void CPropPageGeneral::OnCheckShowProcesses() 
{
    Modify();
}

void CPropPageGeneral::OnCheckAlwaysOnTop() 
{
    Modify();
}

void CPropPageGeneral::OnCheckShowLabels() 
{
    Modify();
}

void CPropPageGeneral::OnCheckShowTips() 
{
    Modify();
}

void CPropPageGeneral::OnBnClickedCheckShowSysTasksActivity()
{
    Modify();
}

////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnKillfocusEditIndicatorsInterval() 
{
    CHECK_RANGE("Indicators update interval (msec)", 
                 m_IndicsInterval, 
                 MIN_INTERVAL_INDIC, MAX_INTERVAL_INDIC); 
}

void CPropPageGeneral::OnKillfocusEditValuesInterval() 
{
    CHECK_RANGE("Values update interval (msec)", 
        m_ValueShowInterval, MIN_INTERVAL_VALUES, MAX_INTERVAL_VALUES);
}

void CPropPageGeneral::OnKillfocusEditProcessesInterval2() 
{
    CHECK_RANGE( "Processes update interval (msec)",
        m_ProcessesInterval, MIN_INTERVAL_PROCESSES, MAX_INTERVAL_PROCESSES);
}

void CPropPageGeneral::OnKillfocusEditProcessesTrackInterval() 
{
    CHECK_RANGE( "Process activity tracking time (sec)",
        m_ProcessTrackTime, MIN_PROCESSES_TRACKING_SECS, MAX_PROCESSES_TRACKING_SECS);
}

void CPropPageGeneral::OnKillfocusEditIndicsInRow() 
{
    CHECK_RANGE( "Indicators in row", 
        m_IndicsInRow, 1, 30);
}

void CPropPageGeneral::OnKillfocusEDITBrightness() 
{
    //CHECK_RANGE( "Brightness coefficient", m_BrigtnessCoeff, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
}

/////////////////////////////////////////////
void CPropPageGeneral::OnSchemeDelete() 
{
    CString schema_name = GetSelectedSchema();
    if ( schema_name.GetLength() == 0 )
        return;

    char sz[400];
    sprintf(sz, "Do you realy want to delete a scheme: %s ?", (LPCSTR)schema_name);
    if ( IDYES !=  AfxMessageBox(_T(sz), MB_YESNO ) )
        return;

    DeleteScheme(schema_name);

    if ( schema_name.Compare(optsets.m_CurrentSchema) == 0 )
        optsets.m_CurrentSchema = "";

    FillSchemeList();
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::DeleteScheme(CString &schema_name)
{
    HKEY mainKey = optsets.GetMainRegistryKey();
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
        if ( strcmpi(keyName, schema_name) == 0 )   {
            RegDeleteKey(mainKey, keyName);
        }
    }
    RegCloseKey(mainKey);

}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnDblclkListSchemes() 
{
    OnButtonSchemeLoad();
}


/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnButtonSchemeLoad() 
{
    CString schema_name = GetSelectedSchema();
    if ( schema_name.GetLength() == 0 )
        return;

    if ( schema_name == optsets.m_CurrentSchema && !g_IsSchemaModified )
        return;                     // nothing changed

    CSettings oldsets;
    oldsets = optsets;

    LoadSchema(schema_name, false /*do  not finish*/);

    if ( !CheckConnection(optsets.m_ComputerName) )
    {
        // Ask for another computer name.
        CRemoteConnectionDlg dlg(this, optsets.m_ComputerName);
        while ( dlg.DoModal() != IDCANCEL )
        {
            if ( CheckConnection(dlg.m_machine) ) {
                optsets.m_ComputerName = dlg.m_machine;

                // Computer name in schema has been changed
                // optsets.m_CurrentSchema = "";        // don't consider different computer as different schema 
                goto finish;
            }
        }

        // Restore old scheme if cancelled. 
        optsets = oldsets;  
        return;
    }

 finish:

    optsets.m_computers.Add(optsets.m_ComputerName);
    FinishLoadingSchema();
    //g_CenterPosX = INT_MIN; g_CenterPosY = INT_MIN;   <--- do not restore default center
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnButtonSchemeSave() 
{
    CSchemaSave dlg(this);
    dlg.m_SchemaName = optsets.m_CurrentSchema; //GetSelectedSchema();

    int res = dlg.DoModal();
    if ( res == IDOK )
    {
        CListBox* list =  (CListBox*) GetDlgItem(IDC_LIST_SCHEMES);
        if ( list->FindString( -1, dlg.m_SchemaName ) == LB_ERR )
        {
            int res = list->AddString( dlg.m_SchemaName );
            SetListHorizontalExtent(list, dlg.m_SchemaName);
            ASSERT( res >= 0 );
        }
        list->SetCurSel(-1);

        optsets.m_CurrentSchema = dlg.m_SchemaName;
        ClearSchemaModified();

        optsets.SaveSettings(CURRENT_SETTINGS_SECTION); // save current
        optsets.SaveSettings( dlg.m_SchemaName );

        g_LastLadedSchemaChanged = false;
        QuerySiblings(IDC_BUTTON_SCHEME_SAVE, 0L);      // signal to update another pages
    }

    SetSchemaNameText();
}

/////////////////////////////////////////////////////////////////////////////
CString CPropPageGeneral::GetSelectedSchema()
{
    CString selText;
    CListBox* list =  (CListBox*) GetDlgItem(IDC_LIST_SCHEMES);
    int i = list->GetCurSel();  
    if ( i != LB_ERR )
        list->GetText(i, selText); 
    return selText;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CPropPageGeneral::OnKillActive() 
{
    UpdateSettings();
    return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnButtonResetDefaults() 
{
    optsets.SaveSettings(CURRENT_SETTINGS_SECTION); // save current
    optsets.SetDefaults();
    FillSchemeList();
    LoadSchema(CURRENT_SETTINGS_SECTION);   // restore current
}

/////////////////////////////////////////////////////////////////////////////

void CPropPageGeneral::LoadSchema(const char * schemaName, bool to_finish)
{
    CSettings tmp_sets;
    tmp_sets = optsets;

    //g_Gadgets->Clear();
    if ( !optsets.LoadSettings(schemaName) )
    {
        optsets = tmp_sets;     // restore settings
        return;
    }

    if ( to_finish ) {
        FinishLoadingSchema();
    }
}

/////////////////////////////////////////////////////////////////////////////

void CPropPageGeneral::FinishLoadingSchema() {

    UpdateControlsData();   

    ResetDlgPerfDatabase();
    QuerySiblings(IDC_BUTTON_SCHEME_LOAD, 0L);  // signal to update another pages

    CListBox* list =  (CListBox*) GetDlgItem(IDC_LIST_SCHEMES);
    list->SetCurSel( list->FindString(0, optsets.m_CurrentSchema ) );

    ClearSchemaModified();
    g_LastLadedSchemaChanged = false;
    SetSchemaNameText();
    RedrawWindow();
}

////////////////////////////////////////////////////////////////////////////////////

void CPropPageGeneral::OnChangeEDITBrightness() 
{
    Modify();
}

void CPropPageGeneral::OnChangeEditIndicatorsInterval() 
{
    Modify();
}

void CPropPageGeneral::OnChangeEditIndicsInRow() 
{
    Modify();
}

void CPropPageGeneral::OnChangeEditProcessesInterval2() 
{
    Modify();
}

void CPropPageGeneral::OnChangeEditProcessTrackTime() 
{
    Modify();
}

void CPropPageGeneral::OnChangeEditValuesInterval() 
{
    Modify();
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CPropPageGeneral::OnSetActive() 
{
    g_szHelpTxt = csz_HelpText_GeneralSettings;
    g_szHelpTitle = csz_HelpTitle_GeneralSettings;

    SetSchemaNameText();    
    return CPropertyPage::OnSetActive();
}


////////////////////////////////////////////////////////////////////////////////////
void CPropPageGeneral::OnCbnSelchangeCombo1()
{
    if ( !m_IsInitialized )
        return;

    CComboBox* combo =  (CComboBox*) GetDlgItem(IDC_COMBO_VIEW_MODES);
    int idx = combo->GetCurSel();
    if ( idx < 0 )
        return;

    m_DispMode = (EDisplayMode) combo->GetItemData(idx);
    Modify();
}

