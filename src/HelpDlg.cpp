// HelpDlg.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "HelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg dialog


CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CHelpDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CHelpDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHelpDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
    //{{AFX_MSG_MAP(CHelpDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg message handlers

BOOL CHelpDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    // TODO: Add extra initialization here
    CWnd* pHelpTitle = GetDlgItem( IDC_HELP_TITLE );
    ASSERT(pHelpTitle);
    pHelpTitle->SetWindowText(g_szHelpTitle); 

    CWnd* pHelpTxt = GetDlgItem( IDC_EDIT_HELP );
    ASSERT(pHelpTxt);
    pHelpTxt->SetWindowText(g_szHelpTxt); 
    pHelpTxt->SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
                   // EXCEPTION: OCX Property Pages should return FALSE
}

const char * g_szHelpTxt;
const char * g_szHelpTitle;

const char * csz_HelpTitle_GeneralSettings = "Help on General Options dialog";
const char * csz_HelpTitle_CountersSettings = "Help on Counters Selection dialog";
const char * csz_HelpTitle_IndicatorsSettings = "Help on Indicator Options dialog";

#define _rn_  "\r\n"

//////////////////////////////////////////////////////////

const char * csz_HelpText_GeneralSettings =

"This dialog controls the common settings of the program."  _rn_
""  _rn_
" ---------------------------------"  _rn_
"   Schemes group box:"  _rn_
" ---------------------------------"  _rn_
"This group box allows to store and retrieve the whole sets of program options,"  _rn_
"called schemes.   "  _rn_
"   "  _rn_
"  'Load' button "  _rn_
"restores the previously stored settings with the selected scheme name in "
"the stored schemes list. "  _rn_
""  _rn_
"  'Save As' button "  _rn_
"stores the current settings by any user defined name. There are several "
"predefined schemes, which are generated automatically during the first "
"run of the program. "  _rn_
""  _rn_
"  'Delete' button "  _rn_
"deletes the selected scheme. "  _rn_
"                       "  _rn_
"  'Restore Default Schemes' button "  _rn_
" reinitializes the predefined schemes, if they have been changed by the user. "
" The operation does not affect other schemes saved by the user."  _rn_
""  _rn_
" ------------------------------"  _rn_
"   General Options"  _rn_
" -------------------------------"  _rn_
""  _rn_
"    'Panel Display mode' selection list"  _rn_
"changes the display mode of Glint panel. The choices are: "  _rn_
"   - Columns Bars display mode (key 'O')"  _rn_
"   - Graphs display mode (key 'A') showing last 35 average values per second "  _rn_
"   - Glinting Lights display mode (key 'G')"  _rn_
""  _rn_
"    'Mini bar view' checkbox (key 'M' or Mouse Double Click)"  _rn_
"toggles between normal and small view of the panel."  _rn_
""  _rn_
"    'Large view' checkbox (key 'B')"  _rn_
"toggles the enlarged panel view. The large mode is useful for high resolution "
"screens, i.e. 1600x1200 and higher."  _rn_
""  _rn_
"    'Full set view' checkbox (key 'F')"  _rn_
"toggles between full/abridged indicators sets. An abridged indicators set can "
"be defined, using Settings dialog (checkbox 'Full mode only' from the Indicator "
"Options)  and quickly switch to full set and back.  This mode allows to "
"temporarily switch off costly or rarely used indicators. "  _rn_
""  _rn_
"    'Show Values' checkbox (key 'V')."  _rn_
"If this mode is on, the program shows current absolute values " 
"(average value per last second) inside normal glinting indicators. "
"In graphs and column bar modes the current counter value is " 
"displayed below each indicator and the maximum counter value on top of it."  _rn_
""  _rn_
"    'Show Processes' checkbox (key 'C')"  _rn_
"toggles display of the most active processes in system in descending "
"CPU activity order. This mode has a slight performance penalty "
"on some Win2000/NT4 systems."  _rn_
""  _rn_
"    'Show Labels' checkbox (key 'L')"  _rn_
"toggles display of tool tips, containing a counter name and the last " 
"second average value."  _rn_
""  _rn_
"    'Indicators update interval (msec)' field."  _rn_
"Range: [150,2000], default is 333."  _rn_
"The main time interval for requesting counter values and updating indicators. " 
"This parameter can be also easily adjusted with 'Left' and 'Right' arrows keys " 
"during normal work of the program. 'Reset' command sets it to default value."  _rn_
""  _rn_
"    'Processes update interval (msec)' field."  _rn_
"Range: [500, 5000], default is 1300."  _rn_
"The time interval for updating the most active processes in system panel. "
"It has no effect if 'Show Processes' mode is off."  _rn_
""  _rn_
"    'Values update interval (msec)' field."  _rn_
"Range: [500, 5000], default is 1300."  _rn_
"The time interval for calculating and showing the average counter value per second. "  
"The average value per second is shown inside indicator, if 'Show values' mode is on. " 
"This interval is also used to update linear graphs of recent values."  _rn_
" "  _rn_
"    'Indicators in row' field."  _rn_
"Range: [1, 30]."  _rn_
"Defines the number of indicators in each row for the normal view mode. "
"In mini-bar view mode this number multiplied by 2 is used."  _rn_
""  _rn_
"    'Always on Top' checkbox (key 'T')."  _rn_
"f this mode is on, the program always stays above other windows on the screen. "  _rn_
""  _rn_
""  _rn_
"    'Show Tool Tips' checkbox (key 'I')."  _rn_
"It toggles display of tool tips, containing a counter name and its last "
"average value per second."  _rn_
""  _rn_
"    'Show system task activity' "   _rn_
"Add system processes, including Glint itself, to Active Processes tooltip. "  _rn_
""  _rn_
"    'Activity tracking time (sec)' field. "  _rn_
"Range: [20, 180], default is 60."  _rn_
"Set the activity time interval in seconds for Active Processes tooltip. "  _rn_
""  _rn_
;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const char * csz_HelpText_CountersSettings =
"This dialog allows to choose system counters for indication."  _rn_
""  _rn_
"   'Counter Group' Combo list"  _rn_
"Selects one of several common groups of counter objects. i.e. categories."  _rn_
""  _rn_
"   'Computer: machine-name' button"  _rn_
"This button shows the name of currently selected machine to watch and "  
"also allows to connect to another machine, if pressed. "
"Administrative rights in the remote system are required. " _rn_
""  _rn_
"   'Objects/Counters' tree view."  _rn_
"The tree view shows all counters currently available in the system " 
"and allows to add them to the list of selected counters.  "  _rn_
""  _rn_
"   'Counter Object Instances' listbox "  _rn_
"The listbox shows the currently active instances in system for the selected counter. " 
"A new copy of an indicator can be created with any instance name from the list. "  
"Multiple selection is supported in this listbox."  _rn_
""  _rn_
"   'Selected Counters'  listbox "  _rn_
"shows counters currently selected for indication. " 
"Counters that are not active in the system are displayed with gray color."  _rn_
""  _rn_
"   'Add' button."  _rn_
"adds a counter from the 'Objects/Counter' list. " 
"If no instances are selected from 'Counter Object Instances' listbox then " 
"only one counter with an empty instance name is added. "  
"For each selected item in 'Counter Object Instances' list a separate indicator " 
"is added. "  _rn_
"If a counter has been added without instance name, then the program " 
"uses '_Total' instance name or the first instance, if there is no instance " 
"with the name '_Total' .  "  _rn_
"   "  _rn_
"   'Remove' button."  _rn_
"removes counters selected in 'Selected Performance Counters' list. " 
"Multiple selection is supported."  _rn_

;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const char * csz_HelpText_IndicatorsSettings =

"This dialog sets display options for specific counters."  _rn_
""  _rn_
"   'Counters' list "  _rn_
"This list shows all counters used for indication with their instance names. "  
"When a counter in the list is selected, its indicator attributes are displayed "
" on the right side of the dialog and can be changed there. "  _rn_
"A counter title has the format:  'Object: Counter' or 'Object: Counter (Instance)' "  _rn_
"The counters listed in the same order they are shown in the program panel. "  
"Multiple selection is supported in this listbox."  _rn_
""  _rn_
"   'Move Up/Move Down' buttons."  _rn_
"These buttons change the order, in which indicators are shown in the display panel. " 
"Indicators on top are displayed first, i.e. from left to right."  _rn_
""  _rn_
" ----------------------------------"  _rn_
"  Indicator Properties "  _rn_
" ----------------------------------"  _rn_
"NOTE: If checkbox in properties is grayed, it means that its value is undefined " 
"and the previous value will be used."  _rn_
""  _rn_
"   'Color' button. "  _rn_
"The button allows to define an indicator color. In glinting lights display mode "
"it means indicator color in its maximum brightness."  _rn_
"If multiple counters with different colors are selected in 'Indicators Counter List', " 
"then button will become gray, and you can set any new color for all selected counters."  _rn_
""  _rn_
"   'Indicator max value' edit box. "  _rn_
"This is the maximum value shown on graph. It's also used to calculate the current "
"counter brightness or column bar height. "
"This value is ignored, if 'Display local variations' option is on."  _rn_
""  _rn_
"   'Label' edit box."  _rn_
"This string (up to 6 characters) will be shown above the indicator."  _rn_
""  _rn_
"   'Visible' checkbox. "  _rn_
"If this checkbox is off, then indicator is not shown by the program. This "
"option can be used to preserve indicator settings for the future possible use. "  _rn_
""  _rn_
"   'Full mode only' checkbox. "  _rn_
"If this checkbox is on, the indicator is displayed only in full show "
"mode. Full/Short modes is toggled by 'F' key."  _rn_
""  _rn_
"   'Visible in mini-bar' checkbox. "  _rn_
"If this checkbox is off, then indicator is displayed only in "
"normal display mode. Check this button to show it also in mini-bar mode."  _rn_
""  _rn_
"   'Maximum automatically adjusted' checkbox. "  _rn_
"If this checkbox is on and current value is bigger than 'Indicator max value' "
"then max value is adjusted to a current value, but it can not become less than "
"the value from 'Indicator max value' edit box. "  _rn_
""  _rn_
"   'Display local variations' checkbox. "  _rn_
"If this checkbox is on, the min and max values in graph are automatically " 
"adjusted to local history and are not remembered. This option is useful to show local variations "
"in counter value, such as amount of memory or network activity counters. "
"If this checkbox is on, then 'Indicator max value' is ignored. "  _rn_

;

//////////////////////////////////////////////////////////