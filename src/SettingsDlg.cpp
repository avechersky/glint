// SettingsDlg.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "SettingsDlg.h"
#include "PerfData.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AutoPtr<CPerfDataBase>  g_DlgPfDataDB;  // stores Counters data for Options dialog
CSettings  optsets;         // stores settings for Options dialog
bool  g_IsSchemaModified = false;

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg

IMPLEMENT_DYNAMIC(CSettingsDlg, CPropertySheet)

CSettingsDlg::CSettingsDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    g_SchemaWasModified = false;
    g_LastLadedSchemaChanged = false;
}

CSettingsDlg::CSettingsDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    try {
        g_DlgPfDataDB = new CPerfDataBase(L"Global", sets.m_ComputerName ); // L"Global" - all
    }
    catch (CWin32ApiException e)
    {
        Win32ErrorExit();
    }
    catch (...)
    {
        ErrorExit(NotAbleToGetCounters);
    }

    AddPage( &m_PageGeneral );
    AddPage( &m_PageCounters );
    AddPage( &m_PageIndicators );

    if ( g_szHelpTxt == csz_HelpText_GeneralSettings )
        SetActivePage( &m_PageGeneral );
    else if ( g_szHelpTxt == csz_HelpText_CountersSettings )
        SetActivePage( &m_PageCounters );
    else if ( g_szHelpTxt == csz_HelpText_IndicatorsSettings )
        SetActivePage( &m_PageIndicators );

    g_SchemaWasModified = false;
    g_LastLadedSchemaChanged = false;
}

CSettingsDlg::~CSettingsDlg()
{
    g_DlgPfDataDB = 0;
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CPropertySheet)
    //{{AFX_MSG_MAP(CSettingsDlg)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers




/////////////////////////////////////////////////////////////////////////////
//  Common dialog functions
/////////////////////////////////////////////////////////////////////////////

BOOL IsCheckBoxOn(CWnd * parent, int id) 
{
    CButton* pWnd = (CButton*) parent->GetDlgItem(id);
    return ( pWnd->GetCheck() == 1);
}

/////////////////////////////////////////////////////////////////////////////
void EnableButton(CWnd * parent, int id, BOOL value) 
{
    CButton* pWnd =  (CButton*) parent->GetDlgItem(id);
    pWnd->EnableWindow(value);
}

/////////////////////////////////////////////////////////////////////////////
void UpdCheck(CWnd * parent, int id, BOOL& var, BOOL value) 
{
    var = value;
    CButton* pWnd =  (CButton*) parent->GetDlgItem(id);
    pWnd->SetCheck(value);
    pWnd->RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
void UpdEdit(CWnd * parent, int id, LPCTSTR value, BOOL enable) 
{
    CEdit* pEdit =  (CEdit*) parent->GetDlgItem(id);
    pEdit->SetWindowText( value );
    pEdit->RedrawWindow();
    pEdit->EnableWindow(enable);
}

/////////////////////////////////////////////////////////////////////////////
void EnableEdit(CWnd * parent, int id, BOOL enable) 
{
    CEdit* pEdit =  (CEdit*) parent->GetDlgItem(id);
    pEdit->EnableWindow(enable);
}


/////////////////////////////////////////////////////////////////////////////
void UpdStatic(CWnd * parent, int id, LPCTSTR txt) 
{
    CStatic* pStat =  (CStatic*) parent->GetDlgItem(id);
    pStat->SetWindowText( txt );
    pStat->RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
CString FloatToString(double f, bool is_compress_format)
{
    char buf[256];
    if ( is_compress_format )
    {
        const char * p = GetSmallValueStr(f);
        memcpy(buf, p, strlen(p) );
    }
    else
    {
        if ( fabs(f) < 0.0000001 )
            strcpy(buf, "0");
        else if ( fabs(f) < 0.01 )
            _snprintf(buf, 255, "%f", f);
        else if ( fabs(f) < 1.0 )
            _snprintf(buf, 255, "%.3f", f);
        else if ( fabs(f) < 10.0 )
            _snprintf(buf, 255, "%.2f", f);
        else if ( fabs(f) < 100.0 )
            _snprintf(buf, 255, "%.1f", f);
        else 
            _snprintf(buf, 255, "%.f", f);
    }
    CString str(buf);
    // str.LockBuffer();
    return str;
}

/////////////////////////////////////////////////////////////////////////////
CString IntToString(int i)
{
    char buf[41];
    _snprintf(buf, 40, "%d", i);
    CString str(buf);
    // str.LockBuffer();
    return str;
}

/////////////////////////////////////////////////////////////////////////////
void SetListHorizontalExtent(CListBox* list, LPCTSTR sz)
{
    if ( sz == 0 || *sz == 0)
        return;
    int     dx = list->GetHorizontalExtent();
    CDC*    pDC = list->GetDC();
    CSize   size;
    size = pDC->GetTextExtent(sz, strlen(sz));

    if (size.cx > dx)
        list->SetHorizontalExtent(size.cx);

    list->ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettingsDlg::OnInitDialog() 
{
    BOOL bResult = CPropertySheet::OnInitDialog();
    // TODO: Add your specialized code here

    SetForegroundWindow();

    return bResult;
}


/////////////////////////////////////////////////////////////////////////////
bool    CSettingsDlg::IsModified()
{
    return m_PageGeneral.m_Modified || m_PageIndicators.m_Modified || m_PageIndicators.m_Modified ;
}

/////////////////////////////////////////////////////////////////////////////
bool    CSettingsDlg::WasAnythingModified()
{
   return ( m_PageGeneral.m_Modified || g_SchemaWasModified || IsSchemaModified() );
}

/////////////////////////////////////////////////////////////////////////////
bool    CSettingsDlg::IsSchemaModified()
{
    return m_PageIndicators.m_Modified || m_PageIndicators.m_Modified ;
}

/////////////////////////////////////////////////////////////////////////////
