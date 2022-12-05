// CountersListBox.cpp : implementation file
// Author: Alexander Vechersky, 2010 

#include "stdafx.h"
#include "glint.h"
#include "CountersListBox.h"
#include "Settings.h"
#include "IndicatorSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCountersListBox

CCountersListBox::CCountersListBox()
{
}

CCountersListBox::~CCountersListBox()
{
}


BEGIN_MESSAGE_MAP(CCountersListBox, CListBox)
    //{{AFX_MSG_MAP(CCountersListBox)
    ON_WM_DRAWITEM()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCountersListBox message handlers

void CCountersListBox::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    // TODO: Add your message handler code here and/or call default

   CListBox::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////
// CMyListBox is my owner-drawn list box derived from CListBox. This 
// example draws an item's text centered vertically and horizontally. The 
// list box control was created with the following code:
//   pmyListBox->Create(
//      WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|
//      LBS_SORT|LBS_MULTIPLESEL|LBS_OWNERDRAWVARIABLE,
//      myRect, pParentWnd, 1);
//
void CMyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
   LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
   ASSERT(lpszText != NULL);
   CDC dc;

   dc.Attach(lpDrawItemStruct->hDC);

   // Save these value to restore them when done drawing.
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor = dc.GetBkColor();

   // If this item is selected, set the background color 
   // and the text color to appropriate values. Also, erase
   // rect by filling it with the background color.
   if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
      (lpDrawItemStruct->itemState & ODS_SELECTED))
   {
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
         ::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

   // If this item has the focus, draw a red frame around the
   // item's rect.
   if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
      (lpDrawItemStruct->itemState & ODS_FOCUS))
   {
      CBrush br(RGB(255, 0, 0));
      dc.FrameRect(&lpDrawItemStruct->rcItem, &br);
   }

   // Draw the text.
   dc.DrawText(
      lpszText,
      strlen(lpszText),
      &lpDrawItemStruct->rcItem,
      DT_CENTER|DT_SINGLELINE|DT_VCENTER);

   // Reset the background color and the text color back to their
   // original values.
   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);

   dc.Detach();
}

*/

/////////////////////////////////////////////////////////////////////////////////////////////
int CCountersListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
    // TODO: Add your code to determine the sorting order of the specified items
    // return -1 = item 1 sorts before item 2
    // return 0 = item 1 and item 2 sort the same
    // return 1 = item 1 sorts after item 2

   CIndicatorSettings* indset1 = (CIndicatorSettings*) lpCompareItemStruct->itemData1;
   CIndicatorSettings* indset2 = (CIndicatorSettings*) lpCompareItemStruct->itemData2;
   return strcmpi(indset1->m_IndText, indset2->m_IndText) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CCountersListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
   CDC dc;
   dc.Attach(lpDrawItemStruct->hDC);

   CIndicatorSettings* indset = (CIndicatorSettings*) lpDrawItemStruct->itemData;
   LPCTSTR lpszText = (LPCTSTR) indset->m_IndText;
 
   int str_len = strlen(lpszText);
   ASSERT(str_len > 0);

   // Save these value to restore them when done drawing.
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor = dc.GetBkColor();

   //----------------------------------------------------------------------
   //   Set the background color and the text color to appropriate values
   //----------------------------------------------------------------------
   if ( (indset->m_ObjId == 0 || (indset->m_strInst.GetLength() > 0 && !indset->m_isInstanceFound) ) 
       && (lpDrawItemStruct->itemState & ODS_SELECTED) == 0 )  // draw disabled in grey
   {
      dc.SetTextColor(CLR_GREY_DARK);
      dc.SetBkColor(CLR_GREY_LIGHT);
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
         CLR_GREY_LIGHT);
   }
   else if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
      (lpDrawItemStruct->itemState & ODS_SELECTED))
   {
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
         ::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

   
#if 1
   // If this item has the focus, draw a blue frame around the item's rect.
   if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
      (lpDrawItemStruct->itemState & ODS_FOCUS))
   {
      CBrush br(RGB(0, 0, 255));
      dc.FrameRect(&lpDrawItemStruct->rcItem, &br);
   }
#endif

   dc.DrawText(
      lpszText,
      str_len,
      &lpDrawItemStruct->rcItem,
      DT_SINGLELINE );                               // DT_SINGLELINE , DT_CALCRECT

   // Reset the background color and the text color back.
   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);

   dc.Detach();
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CCountersListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
    OnMeasureItem( lpMeasureItemStruct->CtlID, lpMeasureItemStruct );
    lpMeasureItemStruct->itemHeight -= 2 ;
    // lpMeasureItemStruct->itemHeight = 14;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  Search for the same object/counter/instance.
bool CCountersListBox::CheckForSameInstance(CIndicatorSettings *pindsets)
{
    int k = GetCount();

    for (int i = 0; i < k; i++ ) 
    { 
        CIndicatorSettings* ind = (CIndicatorSettings*)GetItemData( i );
        ASSERT(ind);

        if ( strcmpi(ind->m_strCategory, pindsets->m_strCategory) == 0 &&
             strcmpi(ind->m_strCounter, pindsets->m_strCounter) == 0 )
        {
            if ( ( ind->m_strInst.GetLength() == 0 && pindsets->m_strInst.GetLength() == 0 ) ||
                 strcmpi(ind->m_strInst,pindsets->m_strInst ) == 0 )
            return true;
        }
    }

    return false;
}
