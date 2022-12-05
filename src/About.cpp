// About.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "About.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

CAbout::CAbout(CWnd* pParent /*=NULL*/)
    : CDialog(CAbout::IDD, pParent)
{
    //{{AFX_DATA_INIT(CAbout)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CAbout::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAbout)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbout, CDialog)
    //{{AFX_MSG_MAP(CAbout)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers
#define _rn_  "\r\n"

const char* s_NotesAbout = 
"Glint monitor shows system activity by using system performance counters in local or remote computer. "
"Recent values can be shown as graphs. It also displays the most active processes in system" 
" in descending CPU activity order."  "\r\n"
""  "\r\n"
"\t    Keyboard commands:"  "\r\n"
""  _rn_
"  'M' or Mouse Double Click   - toggle normal/mini-bar view mode."  _rn_   
"  'B'   - toggle normal/large panel view."                     _rn_   
""  _rn_
"  'G'   - Display glint lights indicators."                _rn_   
"  'O'   - Display column bar indicators."                  _rn_   
"  'A'   - Display line graphs."                            _rn_   
""  _rn_
"  'S'   - view/change settings."                           _rn_   
"  'N'   - change computer, i.e. show activity of another machine." _rn_   
"          Administrative rights in the remote system are required." _rn_   
"          NOTE: Remote computer name is entered without beginning '\\'."  _rn_
""  _rn_
"  Left  - decrease the time interval of indicators changing."  _rn_
"  Right - increase the time interval of indicators changing."  _rn_
"  Up    - increase brightness of glinting indicators."         _rn_
"  Down  - decrease brightness of glinting indicators."         _rn_
""  _rn_
"  'F'   - toggle Full View mode on/off."                       _rn_
"  'I'   - toggle tooltips display with average values per second."  _rn_  
"  'T'   - toggle 'Topmost window' mode on/off."                _rn_  
"  'C'   - toggle showing most active processes on/off."        _rn_  
"  'V'   - toggle displaying last second average (max) values." _rn_  
"  'L'   - toggle showing labels above indicators on/off."      _rn_  
""  _rn_
"  'R'   - reset counters (also resets update time interval and brightness)." _rn_
"  'P'   - pause on/off."                                       _rn_  
"  'F1   - Help/About dialog."                              _rn_  
""  _rn_
"  Press Shift-F10 or Right Click for the commands menu."   _rn_
""  _rn_
"Program icon in system tray accept following mouse commands: "  _rn_
"  - Left Clck    - toggle mini-bar/normal view."  _rn_
"  - Right Click  - show program menu."  _rn_
""  _rn_
"==================================="  _rn_
"\t    Indicators tool tips format:"  _rn_
""  _rn_
"a) For glint ligts indicators view the tool tip format is:" _rn_
"      Category: Counter (Instance) = current-value"  _rn_
"The Instance name represents specific object in system"
" that counter refers to, e.g. one of devices, processes, system queues etc."
" By default 'Instance-name' equals '_Total', i.e. the sum of values"
" for all objects in a given category. "  _rn_
""  _rn_
"b) For graphs view mode the tool tip format is:"  _rn_
"      Category: Counter (Instance) = current-value {graph-min, graph-max} "  _rn_
"The 'graph-min' and 'graph-max' values show the minimum and maximum" 
" values displayed by the corresponding graph. If Graph maximum value is not fixed, the program automatically"
" adjusts the scale of the graph to show it more precisely"  _rn_
""  _rn_
"c) For column bars view the tool tip format is:"  _rn_
"      Category: Counter (Instance) = current-value {column-max}"  _rn_
""  _rn_
"d) For active processes window the tool tip format is:"  _rn_
"      Process   Average-CPU-usage  Process-IO-data-bytes"  _rn_
"The I/O data byte count includes all file, network and device data transfers for a process."
""  _rn_
"========================"  _rn_
""  _rn_
"     DISCLAIMER"  _rn_
"The program is distributed AS IS, without any charge, warranty or liability."  _rn_
""  _rn_
"See the latest version on the program site: http://sites.google.com/site/glintutility/"  _rn_
""  _rn_
"  Copyright (C) 2001-2010 Alexander Vechersky"  _rn_
"  e-mail: avechersky@gmail.com"  _rn_
;


BOOL CAbout::OnInitDialog() 
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    CWnd* pAboutNotes = GetDlgItem( IDC_EDIT_ABOUT );
    ASSERT(pAboutNotes);
    pAboutNotes->SetWindowText(s_NotesAbout); 
    pAboutNotes->SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
                   // EXCEPTION: OCX Property Pages should return FALSE
}
