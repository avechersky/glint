// RemoteConnectionDlg.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "RemoteConnectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoteConnectionDlg dialog


CRemoteConnectionDlg::CRemoteConnectionDlg(CWnd* pParent /*=NULL*/, CString m_current_machine)
    : CDialog(CRemoteConnectionDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CRemoteConnectionDlg)
    //m_machine = _T("");
    m_machine = m_current_machine;
    //}}AFX_DATA_INIT
}


void CRemoteConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRemoteConnectionDlg)
    DDX_CBString(pDX, IDC_COMPUTER_NAME, m_machine);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemoteConnectionDlg, CDialog)
    //{{AFX_MSG_MAP(CRemoteConnectionDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoteConnectionDlg message handlers

BOOL CRemoteConnectionDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    // TODO: Add extra initialization here
    
    FillComputerNames();

    return FALSE;  // return TRUE unless you set the focus to a control
                   // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////
void CRemoteConnectionDlg::FillComputerNames()
{
    CComboBox*  box = (CComboBox*)GetDlgItem(IDC_COMPUTER_NAME);
    
    if ( m_machine.GetLength() > 0) {
        box->AddString(m_machine);
    }    

    box->AddString(LOCAL_MACHINE_NAME);

    int sz = optsets.m_computers.GetSize();
    for (int i =0; i < sz; i++) {
        CString* pstr = new CString( optsets.m_computers.GetAt(i) ); 
        if ( box->FindString(0, *pstr ) < 0  )
        {
            int k = box->InsertString(-1, *pstr );
            ASSERT(k >= 0);
        }
    }

    if ( m_machine.GetLength() > 0) {
        box->SelectString(0, m_machine);
    }
    else { 
        box->SelectString(0, LOCAL_MACHINE_NAME);
    }

    box->SetFocus();
}

///////////////////////////////////////////////////////////////////////
void CRemoteConnectionDlg::OnOK() 
{
    CComboBox*  box = (CComboBox*)GetDlgItem(IDC_COMPUTER_NAME);
    CString  computer_name = "";

    box->GetWindowText(computer_name);
    if ( computer_name.Compare(LOCAL_MACHINE_NAME) == 0 ) {
        computer_name = "";
    }
    box->SetWindowText((LPCTSTR) computer_name);

    //TRACE( "[glint] CRemoteConnectionDlg::OnOK() machine='%s' \n", (LPCTSTR)computer_name) ;

    CDialog::OnOK();
}
