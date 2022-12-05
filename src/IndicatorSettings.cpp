// IndicatorSettings.cpp: implementation of the CIndicatorSettings class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "IndicatorSettings.h"
#include "PerfCount.h"
#include "Settings.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIndicatorSettings::CIndicatorSettings()
        : m_strCategory("?"), m_strCounter("?"), m_strInst("?"), 
        m_strSmallTitle("?"), m_IndMaxV(-999999.0), m_color( CLR_BLACK ), 
        m_flags(INDSW_ADJUST_MAX)
{
    m_CounterId = 0;
    m_ObjId     = 0;
    m_isInstanceFound = false;
}

CIndicatorSettings::CIndicatorSettings(LPCSTR  szCategory, LPCSTR  szCounter, LPCSTR  szInst,
                    COLORREF clr, UINT  flags, 
                    LPCSTR  szSmallTitle, 
                    double maxValStart)
        : m_strCategory(szCategory), m_strCounter(szCounter), m_strInst(szInst), 
        m_strSmallTitle(szSmallTitle), m_IndMaxV(maxValStart), m_color(clr), 
        m_flags(flags)
{

    // m_strCategory.LockBuffer();
    // m_strCounter.LockBuffer();
    // m_strInst.LockBuffer();
    // m_strSmallTitle.LockBuffer();
    MakeText();
    m_CounterId = 0;
    m_ObjId     = 0;
    m_isInstanceFound = false;
}

CIndicatorSettings::~CIndicatorSettings()
{

}

/////////////////////////////////////////////////////////////////////
void CIndicatorSettings::MakeText() // create text of indicator
{
    if ( m_strInst && m_strInst.GetLength() > 0 )
    {
        _snprintf(m_IndText, INDIC_TEXT_SIZE, "%s: %s (%s)" ,
                    (LPCSTR)m_strCategory, (LPCSTR)m_strCounter,
                    (LPCSTR)m_strInst
                    ); 
    }
    else
    {
        _snprintf(m_IndText, INDIC_TEXT_SIZE, "%s: %s" , 
                    (LPCSTR)m_strCategory, (LPCSTR)m_strCounter
                    ); 
    }
}

/////////////////////////////////////////////////////////////////////
// Check if indicator is visible in current settings
bool CIndicatorSettings::IsVisible() 
{
    if ( FLAG_CHECK(m_flags, INDSW_FULL_SET_ONLY) && !sets.m_FullSet )
        return false;
    if ( FLAG_CHECK(m_flags, INDSW_NOT_VISIBLE) )
        return false;
    if ( FLAG_CHECK(m_flags, INDSW_NOT_IN_MINIBAR)  && sets.m_isMiniBar )
        return false;
    return true;
}

/////////////////////////////////////////////////////////////////////
// Correct flags, after 'CPerfCount' has been checked
void CorrectIndicFlags(CPerfCount *pfcnt)
{
    if ( !FLAG_CHECK(pfcnt->m_flags, PFCFLG_MAX_UNDEFINED) )  
    {
        // Correct flag in settings
        pfcnt->m_IndSets->m_IndMaxV = pfcnt->m_maxVal;
        //FLAG_SET(pfcnt->m_IndSets->m_flags, INDSW_MAX_ABSOLUTE);
        FLAG_CLEAR(pfcnt->m_IndSets->m_flags, INDSW_ADJUST_MAX);
    }

    if ( pfcnt->m_IndSets->m_IndMaxV == 0.0 && 
         !FLAG_CHECK(pfcnt->m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS) )
    {
        // Correct flag in settings
        //FLAG_SET(pfcnt->m_IndSets->m_flags, INDSW_ADJUST_MAX);
        FLAG_SET(pfcnt->m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS);
    }

    if ( FLAG_CHECK(pfcnt->m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS) )
    {
        // Correct flag in settings
        //FLAG_SET(pfcnt->m_IndSets->m_flags, INDSW_ADJUST_MAX);
        FLAG_CLEAR(pfcnt->m_IndSets->m_flags, INDSW_ADJUST_MAX);
    }
}


/////////////////////////////////////////////////////////////////////
