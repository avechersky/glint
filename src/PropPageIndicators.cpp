// PropPageIndicators.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"

#include "math.h"
#include "glint.h"
#include "panel.h"
#include "PropPageIndicators.h"
#include "Settings.h"
#include "SettingsDlg.h"
#include "PerfData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Selected indicators numbers array. 1st element is # of selected elements.
static int  s_LastSelIndics[MAX_INDICATORS_NUM + 1] = {0,0,0 };   

///////////////////////////////////////////////////////////////////////////
inline int  LastSelIndicsCount() 
{
    return s_LastSelIndics[0];
}

inline void ClearLastSelection()
{
    s_LastSelIndics[0] = 0;
}

inline int  LastSelectionElt(int idx) 
{
    ASSERT( idx >= 0 && idx < s_LastSelIndics[0] );
    return s_LastSelIndics[idx + 1 ];
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageIndicators property page

IMPLEMENT_DYNCREATE(CPropPageIndicators, CPropertyPage)

///////////////////////////////////////////
CPropPageIndicators::CPropPageIndicators() : CPropertyPage(CPropPageIndicators::IDD)
{
    //{{AFX_DATA_INIT(CPropPageIndicators)
    m_MaxValue = DBL_MIN;
    m_Visible = FALSE;
    m_SmallTitle = _T("");
    m_ShowInMinibar = FALSE;
    m_ShowLocalVariation = FALSE;
    m_IsAdjustMaximum = FALSE;
    m_OnlyInFullMode = FALSE;
    //}}AFX_DATA_INIT

    m_color = CLR_YELLOW;
    m_Modified = false;
    m_OnActivation = true;
    m_CurrentIndicChanged = false;
}

///////////////////////////////////////////
CPropPageIndicators::~CPropPageIndicators()
{
}

///////////////////////////////////////////
void CPropPageIndicators::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPropPageIndicators)
    DDX_Check(pDX, IDC_CHECK_VISIBLE, m_Visible);
    DDX_Text(pDX, IDC_EDIT_SMALL_TITLE, m_SmallTitle);
    DDX_Check(pDX, IDC_CHECK_SHOW_IN_MINIBAR, m_ShowInMinibar);
    DDX_Check(pDX, IDC_FOLLOW_LOCAL_VARIATIONS, m_ShowLocalVariation);
    DDX_Check(pDX, IDC_CHECK_ADJUST_MAX, m_IsAdjustMaximum);
    DDX_Check(pDX, IDC_CHECK_FULL_MODE_ONLY, m_OnlyInFullMode);
    //}}AFX_DATA_MAP
}


///////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPropPageIndicators, CPropertyPage)
    //{{AFX_MSG_MAP(CPropPageIndicators)
    ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButtonColor)
    ON_WM_PAINT()
    ON_LBN_SELCHANGE(IDC_LIST_INDIC_COUNTERS, OnSelchangeListIndicCounters)
    ON_EN_CHANGE(IDC_EDIT_MAX_VALUE, OnChangeEditMaxValue)
    ON_EN_CHANGE(IDC_EDIT_SMALL_TITLE, OnChangeEditSmallTitle)
    ON_BN_CLICKED(IDC_CHECK_VISIBLE, OnCheckVisible)
    ON_BN_CLICKED(IDC_CHECK_FULL_MODE_ONLY, OnCheckFullModeOnly)
    ON_BN_CLICKED(IDC_CHECK_SHOW_IN_MINIBAR, OnCheckShowInMinibar)
    ON_BN_CLICKED(IDC_CHECK_ADJUST_MAX, OnCheckAdjustMax)
    ON_BN_CLICKED(IDC_FOLLOW_LOCAL_VARIATIONS, OnCheckLocalVariations)
    ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
    ON_BN_CLICKED(IDC_BUTTON_INDICATOR_UP, OnButtonIndicatorUp)
    ON_BN_CLICKED(IDC_BUTTON_INDICATOR_DOWN, OnButtonIndicatorDown)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageIndicators message handlers


/////////////////////////////////////////////////////////////////////////////

BOOL CPropPageIndicators::OnInitDialog() 
{
    m_indicList.SubclassDlgItem(IDC_LIST_INDIC_COUNTERS, this);
    
    CEdit* pEditLabel =  (CEdit*) GetDlgItem(IDC_EDIT_SMALL_TITLE);
    pEditLabel->SetLimitText(6);

    //////////////////////////////////////////
    CPropertyPage::OnInitDialog();
    //////////////////////////////////////////

    m_indicList.SetCurSel(-1);

    m_ButtonUp   = (CButton*) GetDlgItem(IDC_BUTTON_INDICATOR_UP);
    m_ButtonDown = (CButton*) GetDlgItem(IDC_BUTTON_INDICATOR_DOWN);
    m_CheckAdjustMax   = (CButton*) GetDlgItem(IDC_CHECK_ADJUST_MAX);
    m_CheckLocalVariations = (CButton*) GetDlgItem(IDC_FOLLOW_LOCAL_VARIATIONS);

    FillIndicatorsList();
    m_CurrentIndicChanged = false;

    m_indicList.SetFocus();
    return FALSE;  // return TRUE unless you set the focus to a control
                   // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageIndicators::OnButtonColor() 
{
    CColorDialog dlg( m_color, CC_ANYCOLOR | CC_FULLOPEN | CC_SOLIDCOLOR, this );
    if ( dlg.DoModal() == IDOK ) 
    {
        m_color = dlg.GetColor();
        PaintColorButton();

        m_CurrentIndicChanged = true;
        Modify();
    }
}

// ************************************************************************************************
void CPropPageIndicators::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    if ( LastSelIndicsCount() > 0 )
    {
        PaintColorButton(); 
        PaintMaxValue();
    }
    // Do not call CPropertyPage::OnPaint() for painting messages
}

// -----------------------------------------------------------------------------------------------
void CPropPageIndicators::PaintMaxValue() 
{
    CWnd* pWnd = GetDlgItem(IDC_EDIT_MAX_VALUE);
    pWnd->SetWindowText( ( m_MaxValue == DBL_MIN ) ? "?" : ValueToString(m_MaxValue) );
    pWnd->RedrawWindow();
}

// -----------------------------------------------------------------------------------------------
void CPropPageIndicators::PaintColorButton() 
{
    CWnd* pWnd = GetDlgItem(IDC_BUTTON_COLOR);
    CDC* pButtonDc = pWnd->GetDC();

    pWnd->Invalidate();
    pWnd->UpdateWindow();

    CRect r1; 
    pWnd->GetClientRect(&r1);
    CRect r = r1; 
    r.left = 2;
    r.top = 2;
    r.right -= 2;
    r.bottom -= 2;

    // pButtonDc->SelectObject(&CBrush(m_color));
    // pButtonDc->Rectangle(&r);    // CLR_BLACK
    pButtonDc->Draw3dRect(&r1, CLR_GREY_LIGHT, CLR_BLACK );
    pButtonDc->FillSolidRect(&r, m_color);
    // pButtonDc->DrawFrameControl(&r, DFC_BUTTON , DFCS_BUTTONPUSH );

    // pWnd->RedrawWindow();

    pWnd->ReleaseDC(pButtonDc);
}


// ************************************************************************************
// **   Fill list of indicators on start or after selecting or moving indicators.    **
// ************************************************************************************
void CPropPageIndicators::FillIndicatorsList() 
{
    m_indicList.ResetContent();

    POSITION pos = optsets.m_indicSets.GetHeadPosition();
    for ( int i_ind = 0; i_ind < optsets.m_indicSets.GetCount(); i_ind++ )
    {
        CIndicatorSettings* indset = optsets.m_indicSets.GetNext(pos);
        DWORD objid = GetCounterId((CPerfDataBase*) g_DlgPfDataDB, *indset);
        
        int idx = m_indicList.AddString(indset->m_IndText);
        SetListHorizontalExtent(&m_indicList, indset->m_IndText);
        ASSERT( idx >= 0 );
        m_indicList.SetItemData( idx, (ULONG)indset);
    }

    if ( LastSelIndicsCount() != 0 )
        SetIndicatorsSelection();
    else 
        m_indicList.SetSel(-1, FALSE);

    m_indicList.RedrawWindow();
}

/////////////////////////////////////////////////////////////////////
//  Restore indicators list selection.
void CPropPageIndicators::SetIndicatorsSelection()
{   
    m_indicList.SetCurSel(-1);        // Reset Selection
    int count = m_indicList.GetCount();

    if ( count < 1 )
    {
        ClearLastSelection();
        return;
    }

    int k = LastSelIndicsCount();
    if ( k == 0 ) {
        m_indicList.SetCurSel(-1);
        GetIndicatorListSelection();
        k = 1;
    }

    for ( int i = 0; i < k; i++ ) {
        if ( LastSelectionElt(i) >= count )
        {
            ClearLastSelection();
            return;
        }
        m_indicList.SetSel( LastSelectionElt(i) );
    }

    SetMoveButtons();
}

/////////////////////////////////////////////////////////////////////
void CPropPageIndicators::SetMoveButtons()
{
    BOOL up_enable = TRUE;
    BOOL down_enable = TRUE;

    int k = LastSelIndicsCount();
    int last_ind_num = optsets.m_indicSets.GetCount() - 1;

    for ( int i = 0; i < k; i++ ) {
        int sel = LastSelectionElt(i);
        if ( sel == 0 )
            up_enable = FALSE;
        if ( sel == last_ind_num )
            down_enable = FALSE;
    }

    m_ButtonUp->EnableWindow(up_enable);  
    m_ButtonDown->EnableWindow(down_enable);

}

/////////////////////////////////////////////////////////////////////////////
//      Mark current indicator as changed
void CPropPageIndicators::MarkCurrentChanged()
{
    if ( !m_OnActivation )
    {
        m_CurrentIndicChanged = true;
    }
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageIndicators::Modify()
{
    if ( !m_OnActivation )
    {
        g_SchemaWasModified = true;
        g_LastLadedSchemaChanged = true;
        m_Modified = true;
        SetModified(TRUE);
    }
}

//============================================================================
void CPropPageIndicators::GetIndicatorListSelection()
{
    int sel_count = m_indicList.GetSelCount();
    if ( sel_count <= 0 ) 
        return;

    s_LastSelIndics[0] = sel_count;
    m_indicList.GetSelItems(sel_count, &(s_LastSelIndics[1]) );
}

// ***********************************************************************************************************
void CPropPageIndicators::EnableAllControls(BOOL  enable)
{
    EnableEdit  (this, IDC_EDIT_MAX_VALUE, enable );
    EnableEdit  (this, IDC_EDIT_SMALL_TITLE, enable );
    EnableButton(this, IDC_CHECK_VISIBLE, enable); 
    EnableButton(this, IDC_CHECK_FULL_MODE_ONLY, enable); 
    EnableButton(this, IDC_CHECK_SHOW_IN_MINIBAR, enable); 
    EnableButton(this, IDC_CHECK_ADJUST_MAX, enable); 
    EnableButton(this, IDC_FOLLOW_LOCAL_VARIATIONS, enable); 
}


// ***********************************************************************************************************
void CPropPageIndicators::OnSelchangeListIndicCounters() 
{
    // Update previous selection if necessary.
    UpdateIndicSets();

    //TRACE( "OnSelchangeListIndicCounters()  m_OnActivation:%d   LastSelCount:%d  \r\n", m_OnActivation, LastSelIndicsCount() );
    m_OnActivation = true;
    GetIndicatorListSelection();
    SetMoveButtons();

    GetIndicatorListSelection();
    int k = LastSelIndicsCount();
    
    EnableAllControls( k > 0);
    
    if ( k == 0)
        return;

    CIndicatorSettings* pindset = (CIndicatorSettings*)m_indicList.GetItemData( LastSelectionElt(0) );
    ASSERT(pindset);
    m_MaxValue = pindset->m_IndMaxV;       // get 1st max value

    for (int i = 0; i < k; i++ ) 
    { 
        pindset = (CIndicatorSettings*)m_indicList.GetItemData( LastSelectionElt(i) );
        ASSERT(pindset);

        if ( FLAG_CHECK(pindset->m_flags, INDSW_MAX_ABSOLUTE) )
        {
            FLAG_CLEAR(pindset->m_flags, INDSW_ADJUST_MAX);
            FLAG_CLEAR(pindset->m_flags, INDSW_LOCAL_VARIATIONS);
        }

        UpdCheck3state(this, IDC_CHECK_ADJUST_MAX, m_IsAdjustMaximum, FLAG_CHECK(pindset->m_flags, INDSW_ADJUST_MAX), i);
        UpdCheck3state(this, IDC_FOLLOW_LOCAL_VARIATIONS, m_ShowLocalVariation, FLAG_CHECK(pindset->m_flags, INDSW_LOCAL_VARIATIONS), i);
        UpdCheck3state(this, IDC_CHECK_SHOW_IN_MINIBAR, m_ShowInMinibar, !FLAG_CHECK(pindset->m_flags, INDSW_NOT_IN_MINIBAR), i);
        UpdCheck3state(this, IDC_CHECK_FULL_MODE_ONLY, m_OnlyInFullMode, FLAG_CHECK(pindset->m_flags, INDSW_FULL_SET_ONLY), i);
        UpdCheck3state(this, IDC_CHECK_VISIBLE, m_Visible, !FLAG_CHECK(pindset->m_flags, INDSW_NOT_VISIBLE), i );

        if ( i > 0 ) 
        {
            if ( m_color !=  pindset->m_color)
                m_color = CLR_INDIC_INVALID;

            if ( m_MaxValue != pindset->m_IndMaxV )
                m_MaxValue = DBL_MIN;

            if ( m_SmallTitle != pindset->m_strSmallTitle ) {
                m_SmallTitle = "";
                UpdEdit(this, IDC_EDIT_SMALL_TITLE, m_SmallTitle);
            }

            PaintMaxValue();
            EnableButton(this, IDC_CHECK_ADJUST_MAX, TRUE );
            EnableButton(this, IDC_FOLLOW_LOCAL_VARIATIONS, TRUE);
        }
        else
        {
            m_color = pindset->m_color;
            m_MaxValue = pindset->m_IndMaxV;
            m_SmallTitle = pindset->m_strSmallTitle;

            UpdEdit(this, IDC_EDIT_SMALL_TITLE, m_SmallTitle);

            // Correct controls for counters with predefined maximum.
            UpdEdit(this, IDC_EDIT_MAX_VALUE, FloatToString(m_MaxValue),        
                          // (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_MAX_ABSOLUTE) &&
                          // (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_ADJUST_MAX) &&
                          (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_LOCAL_VARIATIONS) );

            EnableButton(this, IDC_CHECK_ADJUST_MAX, (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_LOCAL_VARIATIONS)  );
            EnableButton(this, IDC_CHECK_ADJUST_MAX, TRUE); 
                         // (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_MAX_ABSOLUTE) &&
                         // (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_LOCAL_VARIATIONS) ); 

            EnableButton(this, IDC_FOLLOW_LOCAL_VARIATIONS, TRUE);
                         // (BOOL) !FLAG_CHECK(pindset->m_flags, INDSW_MAX_ABSOLUTE) ); 
        }

    }

    CButton* pButLocVariations =  (CButton*) this->GetDlgItem(IDC_FOLLOW_LOCAL_VARIATIONS);
    EnableEdit(this, IDC_EDIT_MAX_VALUE, ( pButLocVariations->GetCheck() != BST_CHECKED ) );
    EnableButton(this, IDC_CHECK_ADJUST_MAX, ( pButLocVariations->GetCheck() != BST_CHECKED ) );

    PaintColorButton(); 
    m_OnActivation = false;
}


/////////////////////////////////////////////////////////////////////////////
void CPropPageIndicators::UpdateIndicSets()
{
    if ( m_OnActivation || !m_CurrentIndicChanged )
        return;

    int k = LastSelIndicsCount();

    for (int i = 0; i < k; i++ ) 
    { 
        // Update indicator settings
        UpdateData();

        CIndicatorSettings* pindset = (CIndicatorSettings*)m_indicList.GetItemData( LastSelectionElt(i) );
        ASSERT(pindset);

        if ( m_SmallTitle.GetLength() > 0 )
           pindset->m_strSmallTitle = m_SmallTitle;
        if ( m_MaxValue != DBL_MIN )
            pindset->m_IndMaxV = m_MaxValue;
        if ( m_color != CLR_INDIC_INVALID )
            pindset->m_color = m_color;

        #define TCHCK(var,flg)  {if(var == TRUE) FLAG_SET(pindset->m_flags,flg); else if (var == FALSE) FLAG_CLEAR(pindset->m_flags,flg); }
        TCHCK( m_IsAdjustMaximum, INDSW_ADJUST_MAX );
        TCHCK( m_ShowLocalVariation, INDSW_LOCAL_VARIATIONS );
        TCHCK( !m_ShowInMinibar, INDSW_NOT_IN_MINIBAR );
        TCHCK( m_OnlyInFullMode, INDSW_FULL_SET_ONLY );
        TCHCK( !m_Visible, INDSW_NOT_VISIBLE );
        #undef  TCHCK

        if ( !FLAG_CHECK(pindset->m_flags, INDSW_ADJUST_MAX) && !FLAG_CHECK(pindset->m_flags, INDSW_LOCAL_VARIATIONS) )
        {
            FLAG_SET(pindset->m_flags, INDSW_MAX_ABSOLUTE); 
        }
    }

    m_CurrentIndicChanged = false;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageIndicators::OnKillActive() 
{
    UpdateIndicSets();
    return CPropertyPage::OnKillActive();
}


// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnChangeEditMaxValue() 
{
    CEdit* pEdit =  (CEdit*) this->GetDlgItem(IDC_EDIT_MAX_VALUE);

    CString  str;
    pEdit->GetWindowText( str );
    if ( !isdigit( str[0] ) && str[0] != '.' )  // may just start with a dot
        return;
    double v = atof( (LPCSTR) str);

    // Check the value is positive. 
    if ( v < 0 )
    {
        pEdit->SetWindowText( "?" );
        pEdit->RedrawWindow();
    }

    m_MaxValue = v;
    MarkCurrentChanged();
    Modify();
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnChangeEditSmallTitle() 
{
    MarkCurrentChanged();
    Modify();
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnCheckVisible() 
{
    ModifyCheck3State(IDC_CHECK_VISIBLE, m_Visible);    
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnCheckFullModeOnly() 
{
    ModifyCheck3State(IDC_CHECK_FULL_MODE_ONLY, m_OnlyInFullMode);    
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnCheckShowInMinibar() 
{
    ModifyCheck3State(IDC_CHECK_SHOW_IN_MINIBAR, m_ShowInMinibar);
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnCheckAdjustMax() 
{
    BOOL is_on_adjust = IsCheckBoxOn(this, IDC_CHECK_ADJUST_MAX);
    BOOL is_on_loc_var = IsCheckBoxOn(this, IDC_FOLLOW_LOCAL_VARIATIONS);
    EnableEdit(this, IDC_EDIT_MAX_VALUE, !is_on_loc_var);

    ModifyCheck3State(IDC_CHECK_ADJUST_MAX, m_IsAdjustMaximum);
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::OnCheckLocalVariations() 
{
    BOOL is_on_loc_var = IsCheckBoxOn(this, IDC_FOLLOW_LOCAL_VARIATIONS);
    BOOL is_on_adjust = IsCheckBoxOn(this, IDC_CHECK_ADJUST_MAX);
    EnableEdit(this, IDC_EDIT_MAX_VALUE, !is_on_loc_var );
    EnableButton(this, IDC_CHECK_ADJUST_MAX, !is_on_loc_var); 

    ModifyCheck3State(IDC_FOLLOW_LOCAL_VARIATIONS, m_ShowLocalVariation);
}

// ----------------------------------------------------------------------------------------
void CPropPageIndicators::ModifyCheck3State(int id, BOOL& var) 
{
    CButton* pWnd =  (CButton*) this->GetDlgItem(id);
    MarkCurrentChanged();
    Modify();

    //TRACE("ModifyCheck3State()  id:%d var:%d state:%d  SelIndCount:%d \r\n", id, var, pWnd->GetCheck(), LastSelIndicsCount() );
    if ( LastSelIndicsCount() > 1 )    
        return;     // already definite or multiple selections

    var = !var;
    pWnd->SetCheck( var ? BST_CHECKED : BST_UNCHECKED );
    pWnd->RedrawWindow();
}


/////////////////////////////////////////////////////////////////////////////

BOOL CPropPageIndicators::OnApply()     // not used yet
{
    UpdateIndicSets();

    SetModified(FALSE);
    return CPropertyPage::OnApply();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPropPageIndicators::OnQuerySiblings( WPARAM wParam, LPARAM lParam )
{
    if ( wParam == IDC_BUTTON_SCHEME_LOAD ||
        wParam == IDC_BUTTON_REMOVE ||
        wParam == IDC_BUTTON_ADD )
    {
        ClearLastSelection();
        FillIndicatorsList();
    }

    if ( wParam == IDC_BUTTON_SCHEME_LOAD || wParam == IDC_BUTTON_SCHEME_SAVE )
    {
        m_Modified = false;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPropPageIndicators::OnSetActive() 
{
    m_OnActivation = true;

    g_szHelpTxt = csz_HelpText_IndicatorsSettings;
    g_szHelpTitle = csz_HelpTitle_IndicatorsSettings;

    OnSelchangeListIndicCounters();
    m_Modified = false;

    BOOL res = CPropertyPage::OnSetActive();
    
    m_OnActivation = false;
    return res;
}


/////////////////////////////////////////////////////////////////////////////
// a) check if selection set can be moved 
//     NOTE: better to do it on ChangeSelection in list and dis(en)able buttons.
// b) form new indic setttins list
// c) encrease (decrease) each element from 's_LastSelIndics'.
void CPropPageIndicators::MoveIndicator(int direction)  //  1 : down; -1  : up
{
    ASSERT(direction == 1 || direction == -1);
    int i, j, j1;

    GetIndicatorListSelection();
    #define VALID_LIST_NUM(i) ( i >= 0  && i < iListCount)

    int iListCount = m_indicList.GetCount();  
    int k = LastSelIndicsCount();


    AutoPtr<int>  new_pos;  // 
    new_pos = (int*)new int[iListCount + 1];

    // fill new positions with -1
    for ( i = 0; i < iListCount; i++ ) {
        new_pos[i] = -1;
    }

    // fill new positions for selected indics with their old postions
    for ( j = 0; j < k; j++ ) {
        new_pos[ s_LastSelIndics[j+1] + direction ] = s_LastSelIndics[j+1]; 
    }

    // fill new positions for not-selected indics with their old postions
    for ( i = 0; i < iListCount; i++ ) {
        if ( new_pos[i] >= 0 )
            continue;               // already know old pos

        for ( j = i; VALID_LIST_NUM(j); j += direction ) {
            bool was_busy = false;
            for ( j1 = 0; j1 < k; j1++ ) {
                if ( s_LastSelIndics[j1+1] == j) {
                    was_busy = true;
                    break;
                }
            }

            if ( was_busy )
                continue;
            else    {
                new_pos[i] = j;     // found old postion for this point
                break;
            }
        }
        if ( new_pos[i] < 0)
            new_pos[i] = i;     // then leave the same 
    }

    // fill new list of indicators pointers
    AutoPtr<LPCTSTR>  new_list;
    new_list = (LPCTSTR*)new LPCTSTR[iListCount + 1];

    for ( i = 0; i < iListCount; i++ ) {
        new_list[i] = (LPCTSTR) m_indicList.GetItemData( new_pos[i] );
    }


    // update indicators pointers
    POSITION pos = optsets.m_indicSets.GetHeadPosition();
    for (i = 0; i < iListCount && pos != NULL; i++ )
    {
        optsets.m_indicSets.SetAt(pos, (CIndicatorSettings*)new_list[i]);
        CIndicatorSettings* pindset = optsets.m_indicSets.GetNext(pos);
    }

    // update selections list
    for ( j = 0; j < k; j++ ) {
        s_LastSelIndics[j+1] +=direction ;  
    }

    FillIndicatorsList();

    m_Modified = true;
    Modify();
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageIndicators::OnButtonIndicatorUp() 
{
    MoveIndicator(-1);
}

/////////////////////////////////////////////////////////////////////////////
void CPropPageIndicators::OnButtonIndicatorDown() 
{
    MoveIndicator(1);
}

/////////////////////////////////////////////////////////////////////////////
void UpdCheck3state(CWnd * parent, int id, BOOL& var, BOOL value, int idx) 
{
    var = value;
    CButton* pWnd =  (CButton*) parent->GetDlgItem(id);

    if ( pWnd->GetCheck() != value && idx > 0 )
        pWnd->SetCheck(CHECK_STATE_UNDEFINED);
    else
        pWnd->SetCheck(value);
    pWnd->RedrawWindow();
}


//