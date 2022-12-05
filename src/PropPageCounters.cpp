// PropPageCounters.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "panel.h"
#include "Settings.h"
#include "SettingsDlg.h"
#include "PropPageCounters.h"
#include "RemoteConnectionDlg.h"
#include "PerfData.h"
#include "PerfCount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int s_LastCounterGroup = 0;

/////////////////////////////////////////////////////////////////////////////
// CPropPageCounters property page

IMPLEMENT_DYNCREATE(CPropPageCounters, CPropertyPage)

CPropPageCounters::CPropPageCounters() : CPropertyPage(CPropPageCounters::IDD)
{
    //{{AFX_DATA_INIT(CPropPageCounters)
    //}}AFX_DATA_INIT
    
    m_Modified = false;
}

CPropPageCounters::~CPropPageCounters()
{
}

void CPropPageCounters::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPropPageCounters)
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageCounters, CPropertyPage)
    //{{AFX_MSG_MAP(CPropPageCounters)
    ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
    ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
    ON_LBN_SELCHANGE(IDC_LIST_SELECTED_COUNTERS, OnSelchangeListSelectedCounters)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SYSTEM_COUNTERS, OnSelchangedTreeSystemCounters)
    ON_LBN_SETFOCUS(IDC_LIST_SELECTED_COUNTERS, OnSetfocusListSelectedCounters)
    ON_NOTIFY(NM_SETFOCUS, IDC_TREE_SYSTEM_COUNTERS, OnSetfocusTreeSystemCounters)
    ON_CBN_SELCHANGE(IDC_COUNTER_GROUP, OnSelchangeCounterGroup)
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
    ON_LBN_SELCHANGE(IDC_LIST_INSTANCES, &CPropPageCounters::OnLbnSelchangeListInstances)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageCounters message handlers

BOOL CPropPageCounters::OnApply() 
{
    if ( g_IsSchemaModified )
    {
        //optsets.m_CurrentSchema = "";
    }
    SetModified(FALSE);

    return CPropertyPage::OnApply();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageCounters::OnInitDialog() 
{
    bool was_scheme_modified = g_IsSchemaModified;
    m_indicList.SubclassDlgItem(IDC_LIST_SELECTED_COUNTERS, this);   

    CPropertyPage::OnInitDialog();

/*  ---- Try to do DrawItem -----
    CListBox* list =  (CListBox*) GetDlgItem(IDC_LIST_SELECTED_COUNTERS);
    CRect = Get
    m_indicList->Create(
        WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|
        LBS_SORT|LBS_MULTIPLESEL|LBS_OWNERDRAWVARIABLE,
        myRect, this, 1);
    -------------------------------  */

    m_ButtonAdd = (CButton*)GetDlgItem(IDC_BUTTON_ADD);
    m_ButtonRemove = (CButton*)GetDlgItem(IDC_BUTTON_REMOVE);

    FillComputerName();
    FillCountersTree();
    FillCountersList();

    CTreeCtrl*  tree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SYSTEM_COUNTERS);
    tree->SetFocus();

    SetModified(FALSE);
    g_IsSchemaModified = was_scheme_modified;

    return FALSE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageCounters::Modify()
{
    g_SchemaWasModified = true;
    g_LastLadedSchemaChanged = true;
    m_Modified = true;

    SetModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPropPageCounters::OnQuerySiblings( WPARAM wParam, LPARAM lParam )
{
    if (   wParam == IDC_BUTTON_REMOVE 
        || wParam == IDC_BUTTON_ADD )
    {
        FillCountersList();
    }
    
    if (wParam == IDC_BUTTON_SCHEME_LOAD)
    {
        ResetCountersTree();
    }

    if ( wParam == IDC_BUTTON_SCHEME_LOAD || wParam == IDC_BUTTON_SCHEME_SAVE )
    {
        m_Modified = false;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageCounters::FillCountersList()
{
    CCountersListBox* list =  (CCountersListBox*) GetDlgItem(IDC_LIST_SELECTED_COUNTERS);
    list->ResetContent();

    POSITION pos = optsets.m_indicSets.GetHeadPosition();
    for ( int i_ind = 0; i_ind < optsets.m_indicSets.GetCount(); i_ind++ )
    {
        CIndicatorSettings* indset = optsets.m_indicSets.GetNext(pos);
        DWORD objid = GetCounterId((CPerfDataBase*) g_DlgPfDataDB, *indset);

        int idx = list->AddString(indset->m_IndText);
        SetListHorizontalExtent(list, indset->m_IndText);
        ASSERT( idx >= 0 );
        list->SetItemData( idx, (ULONG)indset);
    }
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnSelchangeListSelectedCounters() 
{
    m_ButtonAdd->EnableWindow( FALSE );
    m_ButtonRemove->EnableWindow( TRUE );

    CCountersListBox* list =  (CCountersListBox*) GetDlgItem(IDC_LIST_SELECTED_COUNTERS);
    int i = list->GetCaretIndex();  
    if ( i < 0 ) return;

    CIndicatorSettings* pindset = (CIndicatorSettings*)list->GetItemData(i);
    DWORD counterId = pindset->m_CounterId;

    if (  ( counterId != 0 ) &&
          g_DlgPfDataDB->GetHelpText(counterId, m_txtHelp, MAX_HELP_TEXT_SIZE) )
        UpdEdit(this, IDC_EDIT_DESCRIPTION, m_txtHelp);
    else
        UpdEdit(this, IDC_EDIT_DESCRIPTION, "" );
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnSelchangedTreeSystemCounters(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // inserted by MFC
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Clear list box selection
    CCountersListBox* list =  (CCountersListBox*) GetDlgItem(IDC_LIST_SELECTED_COUNTERS);
    list->SetCurSel( -1 );

    CTreeCtrl*  tree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SYSTEM_COUNTERS);
    HTREEITEM item = tree->GetSelectedItem();
    if (item == 0)
        return;

    DWORD objId = tree->GetItemData(item);

    if ( g_DlgPfDataDB->GetHelpText(objId, m_txtHelp, MAX_HELP_TEXT_SIZE) )
        UpdEdit(this, IDC_EDIT_DESCRIPTION, m_txtHelp);
    else
        UpdEdit(this, IDC_EDIT_DESCRIPTION, "" );

    EnablingAddButton(true);

    // inserted by MFC
    *pResult = 0;
}

// ===============================================================================
//      Enable Add button if necessary.
void CPropPageCounters::EnablingAddButton(bool isFillInstances) 
{
    CTreeCtrl*  tree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SYSTEM_COUNTERS);
    HTREEITEM item = tree->GetSelectedItem();
    if (item == 0)
        return;

    DWORD objId = tree->GetItemData(item);
    bool addEnable = false;
    PPERF_COUNTER_DEFINITION cntDef = 0;
    HTREEITEM itemParent = tree->GetParentItem(item);

    if ( tree->GetChildItem(item) == 0 )    // check that it's a tree list
    {
        objId = tree->GetItemData(itemParent);

        CString strCategory = tree->GetItemText(itemParent);
        CString strCounter = tree->GetItemText(item);
        cntDef = g_DlgPfDataDB->FindCounter(strCategory, strCounter);
        addEnable = (cntDef != 0 );

        POSITION pos = optsets.m_indicSets.GetHeadPosition();
        for ( int i_ind = 0; i_ind < optsets.m_indicSets.GetCount(); i_ind++ )
        {
            CIndicatorSettings* pindset = optsets.m_indicSets.GetNext(pos);
            if ( pindset->m_strCategory == strCategory &&
                 pindset->m_strCounter  == strCounter)
            {
                addEnable = false;
            }
        }
    }

    CListBox* inst_list =  (CListBox*) GetDlgItem(IDC_LIST_INSTANCES);
    if ( isFillInstances )
    {        
        g_DlgPfDataDB->FillInstancesList(inst_list, objId);
    }
    if ( inst_list->GetCount() > 0 ) 
    {
        addEnable = (cntDef != 0);
    }

    m_ButtonAdd->EnableWindow( addEnable && 
            optsets.m_indicSets.GetCount() < MAX_INDICATORS_NUM );
    m_ButtonRemove->EnableWindow( FALSE );
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnSetfocusListSelectedCounters() 
{
    OnSelchangeListSelectedCounters();
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnSetfocusTreeSystemCounters(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // TODO: Add your control notification handler code here
    OnSelchangedTreeSystemCounters(pNMHDR, pResult);    

    // MFC generated
    *pResult = 0;
}
///////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnButtonAdd() 
{
    AddCounterFromTree();
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnButtonRemove() 
{
    RemoveSelectedCounter();    
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::RemoveSelectedCounter()
{
    int i, j;
    CCountersListBox* list =  (CCountersListBox*) GetDlgItem(IDC_LIST_SELECTED_COUNTERS);

    ////////////////////////////////////////
    //    Get List of Selected counters
    ////////////////////////////////////////
    int sel_count = list->GetSelCount();    // for multiple selection
    if ( sel_count <= 0 ) return;   
    if ( sel_count >= list->GetCount() )
    {
        AfxMessageBox("Can no remove all counters");
        list->SetSel(-1, FALSE);
        return;
    }
    
    AutoPtr<int>  selects; 
    selects = (int *)new int[sel_count + 1]; 
    list->GetSelItems(sel_count,  (int*)selects );

    AutoPtr<LPCTSTR>  indsets_ptrs;   // array of CIndicatorSettings*  
    indsets_ptrs = (LPCTSTR*)new LPCTSTR[sel_count + 1]; 

    for (i=0; i < sel_count; i++)
    {
        indsets_ptrs[i] = (LPCTSTR) list->GetItemData( selects[i] );
    }

    // CIndicatorSettings* sel_indset = (CIndicatorSettings*)list->GetItemData(i);
    TIndSetsList newIndics;     // create a new indicators settings list
    POSITION pos = optsets.m_indicSets.GetHeadPosition();
    for ( int i_ind = 0; i_ind < optsets.m_indicSets.GetCount(); i_ind++ )
    {
        CIndicatorSettings* pindset = optsets.m_indicSets.GetNext(pos);
        CIndicatorSettings* pindset_to_delete = 0;

        // Search for the given indicator in list for removal
        for (j=0; j < sel_count; j++)
        {
            if ( (LPCTSTR)pindset == indsets_ptrs[j] )
            {
                pindset_to_delete = pindset;
                break;
            }
        }

        if ( pindset_to_delete != 0)
        {
            delete pindset_to_delete;     // delete found element and skip it from copying
            SetModifyState();
        }
        else
        {
            newIndics.AddTail(pindset); 
        }
    }

    CopyTIndSetsList(optsets.m_indicSets, newIndics);

    FillCountersList();
    QuerySiblings(IDC_BUTTON_REMOVE, 0L);   // signal to update another pages
    Modify();
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::SetModifyState()
{ 
    SetModified(TRUE);  
    m_Modified = true; 
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::AddCounterFromTree()
{
    CTreeCtrl*  tree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SYSTEM_COUNTERS);
    CCountersListBox* cnt_list =  (CCountersListBox*) GetDlgItem(IDC_LIST_SELECTED_COUNTERS);
    ASSERT(tree);

    HTREEITEM item = tree->GetSelectedItem();

    HTREEITEM itemParent = tree->GetParentItem(item);
    if ( tree->GetChildItem(item) != 0 )    // not a tree list
        return;

    DWORD counterId = tree->GetItemData(item);
    DWORD categoryId = tree->GetItemData(itemParent);

    CString strCategory = tree->GetItemText(itemParent);
    CString strCounter = tree->GetItemText(item);

    CString strSmallTitle = strCategory[0];
    strSmallTitle += strCounter[0];
    strSmallTitle += strCounter[1];
    strSmallTitle += strCounter[2];

    // Form a new counter sets
    PPERF_COUNTER_DEFINITION cntDef = g_DlgPfDataDB->FindCounter(strCategory, strCounter);
    ASSERT(cntDef);

    ///////////////////////////////
    //    Get Instance from list
    ///////////////////////////////
    CListBox* inst_list = (CListBox*) GetDlgItem(IDC_LIST_INSTANCES);
    int inst_sel_count = inst_list->GetSelCount();    // for multiple selection

    AutoPtr<int>  selects; 
    if ( inst_sel_count > 0 )   
    {
        selects = (int *)new int[inst_sel_count + 1]; 
        inst_list->GetSelItems(inst_sel_count,  (int*)selects );
    }

    int iselect = 0;
    CString strInstance = "";

    do 
    {
        if ( inst_sel_count > 0 )   
        {
            inst_list->GetText(selects[iselect], strInstance);
        }

        CIndicatorSettings* pindsets = new CIndicatorSettings(
                            (LPCSTR)  strCategory, (LPCSTR)  strCounter, 
                            (LPCSTR)  strInstance /*instance*/,
                            CLR_YELLOW, 
                            INDSW_LOCAL_VARIATIONS, 
                            (LPCSTR)  strSmallTitle, 
                            0.0 /*maxValStart*/);


        CPerfCount counter((LPCSTR)  strCategory, (LPCSTR)  strCounter, (LPCSTR) strInstance, pindsets);
        counter.IsCounterTypeSupported(cntDef->CounterType); // set some flags
        CorrectIndicFlags(&counter);

        pindsets->MakeText();
        if ( !cnt_list->CheckForSameInstance(pindsets) ) 
        {
            GetCounterId(g_DlgPfDataDB, *pindsets);     // fill object & counter IDs
            optsets.m_indicSets.AddTail(pindsets); 
            SetModifyState();
        }
        else 
        {
            delete pindsets;    // do not add repeared counters
        }
        iselect++;

    }
    while ( --inst_sel_count > 0 && optsets.m_indicSets.GetCount() < MAX_INDICATORS_NUM);

    QuerySiblings(IDC_BUTTON_ADD, 0L);  // signal to update another pages
    Modify();
}


///////////////////////////////////////////////////////////////////////
void CPropPageCounters::FillCountersTree()
{
    CTreeCtrl*  tree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SYSTEM_COUNTERS);

    //    Clear Tree
    tree->SetRedraw(FALSE);             // prevent flickering in tree window
    int k = tree->DeleteAllItems();
    ASSERT(k != 0);

    FillCounterGroups();
    
    g_DlgPfDataDB->FillCountersTree(tree, optsets.m_ComputerName, s_LastCounterGroup, FILL_COUNTERS_TREE );

    tree->SetRedraw(TRUE);
    tree->Invalidate();                 // redraw tree window 

    CListBox* inst_list =  (CListBox*) GetDlgItem(IDC_LIST_INSTANCES);
    inst_list->ResetContent();
}

////////////////////////////////////////////////////////////////////////
void CPropPageCounters::FillCounterGroups()
{
    CComboBox*  box = (CComboBox*)GetDlgItem(IDC_COUNTER_GROUP);
    CTreeCtrl*  tree = (CTreeCtrl*)GetDlgItem(IDC_TREE_SYSTEM_COUNTERS);
    int k;
    int count;

    box->ResetContent();
    int i = 0;
    for ( ; i < CSettings::m_CounterGroupsNumber; i++) {
        LPCTSTR pstr = CSettings::m_CounterGroupsNames.GetAt(i); 
        if ( box->FindString(0, pstr ) < 0  )
        {
            count = g_DlgPfDataDB->FillCountersTree(tree, optsets.m_ComputerName, i, GET_OBJECTS_COUNT );
            if ( count > 0) 
            {
                k = box->InsertString(-1, pstr );
                ASSERT(k >= 0);
                box->SetItemData(k, i);
            }
        }
    }

    count = g_DlgPfDataDB->FillCountersTree(tree, optsets.m_ComputerName, i, GET_OBJECTS_COUNT);
    if ( count > 0) 
    {
        k = box->InsertString(-1, "Other" );
        box->SetItemData(k, i);
    }

    box->SetCurSel(s_LastCounterGroup);
}


///////////////////////////////////////////////////////////////////////
DWORD GetCounterId(CPerfDataBase* pfcDB, CIndicatorSettings& indset)
{
    indset.m_CounterId = 0;
    indset.m_ObjId = 0;

    // ========= Get Counter Ids ============
    DWORD objectId = pfcDB->FindObjectId(indset.m_strCategory);
    if ( objectId == 0) 
        return 0;

    PPERF_COUNTER_DEFINITION cnt = pfcDB->FindCounter(indset.m_strCategory, indset.m_strCounter);
    if ( cnt == 0 || cnt->CounterNameTitleIndex == 0 )
        return 0;

    // Consider  cnt->CounterSize  later ???
    indset.m_CounterId = cnt->CounterNameTitleIndex;
    indset.m_ObjId = objectId;

    indset.m_isInstanceFound = true;

    // Find Instance
    LONG    instCount = pfcDB->GetInstancesCount(objectId);
    if ( (indset.m_strInst.GetLength() > 0) && ( instCount > 0 ) )
    {
        wchar_t wszInst[MAX_INST_NAME + 50];
        long inst_id;
        if ( ConvertToUni(indset.m_strInst, wszInst, MAX_INST_NAME) > 0 )
        {

            indset.m_isInstanceFound = pfcDB->FindInstanceId(indset.m_ObjId, 
                                    indset.m_CounterId, 
                                    wszInst,
                                    &inst_id);
        }
    }

    return indset.m_CounterId;
}


///////////////////////////////////////////////////////////////////////
BOOL CPropPageCounters::OnSetActive() 
{
    g_szHelpTxt = csz_HelpText_CountersSettings;
    g_szHelpTitle = csz_HelpTitle_CountersSettings;
    
    return CPropertyPage::OnSetActive();
}

///////////////////////////////////////////////////////////////////////
void CPropPageCounters::FillComputerName()
{
    // CStatic* stext = (CStatic*)GetDlgItem(IDC_SCOMPUTER_NAME);
    CWnd*   stext = (CWnd*)GetDlgItem(IDC_BUTTON_CONNECT);
    CString str;

    str.Format("Computer: %s", (LPCTSTR) GetPrintableCompName(optsets.m_ComputerName) );
    stext->SetWindowText(str);
}


////////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnSelchangeCounterGroup() 
{
    CComboBox*  box = (CComboBox*)GetDlgItem(IDC_COUNTER_GROUP);
    int sel = box->GetCurSel();
    
    s_LastCounterGroup = box->GetItemData(sel);
    FillCountersTree();

    box->SetCurSel(sel);
}

/////////////////////////////////////////////////////////////////////////
void CPropPageCounters::OnLbnSelchangeListInstances()
{
    CListBox* inst_list = (CListBox*) GetDlgItem(IDC_LIST_INSTANCES);
    int inst_sel_count = inst_list->GetSelCount();    // for multiple selection
    
    if ( inst_sel_count <= 0 )   
        return;
    
    EnablingAddButton(false);    
}


////////////////////////////////////////////////////////////////////////
//  Connect to another computer
void CPropPageCounters::OnButtonConnect() 
{
    CString  computer_name = "";

    // Ask for another computer name.
    CRemoteConnectionDlg dlg(this, optsets.m_ComputerName);
    while ( dlg.DoModal() != IDCANCEL )
    {
        if ( CheckConnection(dlg.m_machine) ) {
            computer_name = dlg.m_machine;
            optsets.m_computers.Add(computer_name);
            goto finish;
        }
    }
    return;

finish:

    if ( optsets.m_ComputerName == computer_name )
        return;

    optsets.m_ComputerName = computer_name;

    ResetDlgPerfDatabase();
    ResetCountersTree();
    
    SetModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////
//  Reinit counters base and all lists after reconnection.
void CPropPageCounters::ResetCountersTree()
{
    // *****************************
    //     Reinitialize Counters
    // *****************************
    s_LastCounterGroup = 0;

    FillCountersTree();
    FillCountersList();
    FillComputerName();
}

////////////////////////////////////////////////////////////////////////
// Re-Initialize counters database for new machine.
void  ResetDlgPerfDatabase()
{
    SET_WAIT_CURSOR;
    try {
        g_DlgPfDataDB = new CPerfDataBase(L"Global", optsets.m_ComputerName );  // L"Global" - all
    }
    catch (CWin32ApiException e)
    {
        Win32ErrorExit();
    }
    catch (...)
    {
        ErrorExit(NotAbleToGetCounters);
    }
    RESTORE_CURSOR;
}

/////////////////////////////////////////////////////////////////////////
//  Return true if computer with the given name can be connected or computer is local.
bool CheckConnection(CString& computer_name) 
{
    bool ret = true;
    if ( computer_name.GetLength() == 0 )
        return true;

    HKEY perf_key = GetMachineKey((LPCSTR)computer_name, HKEY_PERFORMANCE_DATA);

    if (  perf_key == HKEY_PERFORMANCE_DATA ) {
        CString msg;
        msg.Format("Can not connect to computer '%s'", computer_name);
        AfxMessageBox(msg);
        ret = false;
    }

    RegCloseKey(perf_key);
    return ret;
}

