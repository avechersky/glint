// IndicatorSettings.h: interface for the CIndicatorSettings class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INDICATORSETTINGS_H__5BAC35F4_D56E_42B5_A9C4_ED7545E9541A__INCLUDED_)
#define AFX_INDICATORSETTINGS_H__5BAC35F4_D56E_42B5_A9C4_ED7545E9541A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cdefs.h"
class   CPerfCount;
class   CSettings;

////////////////////////////////////////////////////////////////
class CIndicatorSettings  
{
public:
    CIndicatorSettings();
    virtual ~CIndicatorSettings();
    
    CString     m_strCategory;
    CString     m_strCounter;
    CString     m_strInst;          // Counter instance name
    CString     m_strInstFound;     // Found instance name, nothing was supplied

    CString     m_strSmallTitle;    // to display near indicator
    double      m_IndMaxV;          // max value provided by user
    COLORREF    m_color;
    UINT        m_flags;

    // Temporary data stored    
    DWORD       m_ObjId;        // temp data
    DWORD       m_CounterId;    // temp data
    bool        m_isInstanceFound;  // true if real instance (icluding default) for this counter is not empty and found


#define INDIC_TEXT_SIZE  256
    char    m_IndText[INDIC_TEXT_SIZE+2];   // full indicator title to display in dialogs
    CString m_strIndicTitle;                // real tool tip title (possibly with default instance)

public:
    void MakeText();    // create text of indicator
    CIndicatorSettings(LPCSTR  szCategory, LPCSTR  szCounter, LPCSTR  szInst,
                        COLORREF clr, UINT  flags, 
                        LPCSTR  szSmallTitle, double maxValStart);

    // Check if indicator is visible in current settings
    bool IsVisible(); 
};

// Global functions
void CorrectIndicFlags(CPerfCount* pfcnt);

#endif // !defined(AFX_INDICATORSETTINGS_H__5BAC35F4_D56E_42B5_A9C4_ED7545E9541A__INCLUDED_)
