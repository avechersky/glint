// Gadgets.h: interface for the CGadgets class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GADGETS_H__E3098AB3_6C45_4325_9E90_79BB7C73FDB0__INCLUDED_)
#define AFX_GADGETS_H__E3098AB3_6C45_4325_9E90_79BB7C73FDB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxtempl.h>
#include "AutoPtr.h"

class   CIndicator;
class   CPerfData;
class   CProcesses;
class   CPerfDataBase;
class   CPerfCount;
class   CIndicatorSettings;
class   CSettings;

typedef CTypedPtrList<CObList, CIndicator*> TIndicatorsList;

class CGadgets  
{
public:
    CGadgets();
    virtual ~CGadgets();

    void InitGagets();

    void Clear(bool isDestroyWindows = true);

    bool GetCounterIds(CPerfCount* pfcnt, CIndicatorSettings& indset, CPerfDataBase* pfcDB);

    int  GetVisibleIndicsCount();

    bool IsEmpty() { return m_indicators.IsEmpty(); } 
// DATA

    // indicators information
    int                 m_NumOfIndics;
    TIndicatorsList     m_indicators;

    AutoPtr<CPerfData>  m_pfcDB;
    AutoPtr<CProcesses> m_processesDB;

};

extern CGadgets* g_Gadgets;

////////////////////////////////////////////////////////////////////////////
//  Iterates through currently visible indicators only 
class CVisibleIndicatorsIterator
{
public:

    CVisibleIndicatorsIterator(CGadgets& gadgets);
    bool    GetNext(CIndicator* * p_ind);
    void    Reset();                            // allows to start again

protected:

    CGadgets&   m_gadgets;
    POSITION    m_pos;                          // 0 => finished
    bool        is_used;
};


#endif // !defined(AFX_GADGETS_H__E3098AB3_6C45_4325_9E90_79BB7C73FDB0__INCLUDED_)
