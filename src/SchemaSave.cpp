// SchemaSave.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "SchemaSave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSchemaSave dialog


CSchemaSave::CSchemaSave(CWnd* pParent /*=NULL*/)
    : CDialog(CSchemaSave::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSchemaSave)
    m_SchemaName = _T("");
    //}}AFX_DATA_INIT
}


void CSchemaSave::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSchemaSave)
    DDX_Text(pDX, IDC_EDIT_SCH_NAME, m_SchemaName);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchemaSave, CDialog)
    //{{AFX_MSG_MAP(CSchemaSave)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchemaSave message handlers
