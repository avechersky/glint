// Gadgets.cpp: implementation of the CGadgets class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "Settings.h"
#include "IndicatorSettings.h"
#include "Gadgets.h"
#include "PerfCount.h"
#include "PerfData.h"
#include "Indicator.h"
#include "Processes.h"
#include "Label.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGadgets*    g_Gadgets = 0;
class CIndicatorSettings;

//////////////////////////////////////////////////////////////////////
// Class:    CGadgets
//////////////////////////////////////////////////////////////////////

CGadgets::CGadgets()
{
    m_NumOfIndics   = 0;
}

CGadgets::~CGadgets()
{
    FUNCTIONLOG(8, "CGadgets::destr");
    Clear();
}

////////////////////////////////////////////////////////////////////////////

void CGadgets::InitGagets()
{
    Clear();
    m_NumOfIndics   = 0;
    m_NumOfIndics = sets.m_indicSets.GetCount();
    sets.m_OurFrequency = 1000.0 / sets.m_IndicTimeInterval;
    AutoPtr<CPerfDataBase> pfcDB;

    try {
        pfcDB = new CPerfDataBase(L"Global", sets.m_ComputerName);  // L"Global" - all
    }
    catch (CWin32ApiException e)
    {
        Win32ErrorExit();
    }
    catch (...)
    {
        ErrorExit(NotAbleToGetCounters);
    }

    CList<int,int> objList(32); // object nunbers to watch

    m_pfcDB = new CPerfData(sets.m_ComputerName);


    // ---------------------------------------------
    //  Iterate through Indicators Settings
    // ---------------------------------------------
    POSITION pos = sets.m_indicSets.GetHeadPosition();
    for ( int i_ind = 0; i_ind < sets.m_indicSets.GetCount(); i_ind++ )
    {
        CIndicatorSettings& indset = * (sets.m_indicSets.GetNext(pos));
        //TRACE("[glint] CGadgets::InitGagets() Indic='%s' \n", (LPCSTR)indset.m_strCounter );


        // NOTE: Do not call indset.IsVisible(sets), because 'INDSW_NOT_IN_MINIBAR' has to be ignored here.
        if ( ( FLAG_CHECK(indset.m_flags, INDSW_FULL_SET_ONLY) && !sets.m_FullSet ) ||
             ( FLAG_CHECK(indset.m_flags, INDSW_NOT_VISIBLE) )
           )
            continue;

        CPerfCount* pfcnt = new CPerfCount(indset.m_strCategory, indset.m_strCounter, indset.m_strInst, &indset);
        pfcnt->m_cntDB = m_pfcDB;

        if ( !GetCounterIds(pfcnt, indset, pfcDB) )         
        {
            TRACE(" *** Counter (%s,%s,%s) not created \n", indset.m_strCategory, indset.m_strCounter, indset.m_strInst);
            m_NumOfIndics--;
            delete pfcnt;
            continue;
        }

        if ( objList.Find(pfcnt->m_ObjectId) == NULL )
            objList.AddTail(pfcnt->m_ObjectId);

        CIndicator* ind = new CIndicator( pfcnt, &indset);
        CorrectIndicFlags(pfcnt);

        m_indicators.AddTail(ind);        
        //TRACE("[glint] CGadgets::InitGagets() Indic='%s' added.\n", (LPCSTR)indset.m_strCounter );
    }


    // ---------------------------------------------
    //      Prepare request for Registry read
    // ---------------------------------------------
    POSITION posList = objList.GetHeadPosition();
    wchar_t wszRegRequest[MAX_REG_REQUEST_SIZE + 2]; // request to registry
    wszRegRequest[0] = 0;
    for (int i=0;i < objList.GetCount();i++)
    {
        wchar_t ws[44];
        _snwprintf(ws,40,L"%s%d", ((i==0)?L"":L" "), (int)objList.GetNext(posList) );
        if ( wcslen(wszRegRequest) + wcslen(ws) <= MAX_REG_REQUEST_SIZE )
            wcscat(wszRegRequest, ws);
    }
    m_pfcDB->SetRequestWsz(wszRegRequest);


    // ---------------------------------------------
    //      Prepare data for Processes Show
    // ---------------------------------------------
    CIndicatorSettings* pSetsPsCPU = new CIndicatorSettings("Process", "% Processor Time", "_Total", 
                                    CLR_YELLOW, 
                                    INDSW_STANDARD, "PsCp", 100.0);

    CIndicatorSettings* pSetsPsIO  = new CIndicatorSettings("Process", "IO Data Bytes/sec", "_Total", 
                                    CLR_GREEN, 
                                    INDSW_STANDARD, "PsIO", 100.0);

    CPerfCount* pcntCPU = new CPerfCount(pSetsPsCPU->m_strCategory, pSetsPsCPU->m_strCounter, pSetsPsCPU->m_strInst, pSetsPsCPU);
    CPerfCount* pcntIO  = new CPerfCount(pSetsPsIO->m_strCategory, pSetsPsIO->m_strCounter, pSetsPsIO->m_strInst, pSetsPsIO);

    if ( GetCounterIds(pcntCPU, *pSetsPsCPU, pfcDB) && GetCounterIds(pcntIO, *pSetsPsIO, pfcDB) ) 
    {        
        wszRegRequest[0] = 0;
        _snwprintf(wszRegRequest,40,L"%d", pcntCPU->m_ObjectId);

        m_processesDB = new CProcesses(wszRegRequest, sets.m_ComputerName);
        
        m_processesDB->m_ModelSettingsCpu = pSetsPsCPU;
        m_processesDB->m_ModelCounterCpu = pcntCPU;

        m_processesDB->m_ModelSettingsIO = pSetsPsIO;
        m_processesDB->m_ModelCounterIO = pcntIO;

        pcntCPU->m_cntDB = m_processesDB;
        pcntIO->m_cntDB  = m_processesDB;
    }
    else
    {
        BAD_COND_MESSAGE("Can not create counters for showing processes.");
        sets.m_isShowProcesses = false;
    }

}

////////////////////////////////////////////////////////////////////////////

bool CGadgets::GetCounterIds(CPerfCount* pfcnt, CIndicatorSettings& indset, CPerfDataBase* pfcDB)
{
    indset.m_CounterId = 0;
    indset.m_ObjId = 0;

    // ========= Get Counter Ids ============
    pfcnt->m_ObjectId = pfcDB->FindObjectId(indset.m_strCategory);
    LONG instCount = 0;     // # of instances in counter

    if ( pfcnt->m_ObjectId == 0) 
        return false;

    // Search for Counter.
    PPERF_COUNTER_DEFINITION cnt = pfcDB->FindCounter(indset.m_strCategory, indset.m_strCounter);
    if ( cnt == 0 || cnt->CounterNameTitleIndex == 0 )
        return false;

    pfcnt->m_CounterId = cnt->CounterNameTitleIndex;
    pfcnt->m_CounterType = cnt->CounterType;
    pfcnt->m_CounterSize = cnt->CounterSize;
    pfcnt->m_BaseSize = cnt->CounterSize;   // Not always so. Correct later ??? TODO
    pfcnt->m_maxVal  = indset.m_IndMaxV;
    
    if ( FLAG_CHECK(indset.m_flags, INDSW_LOCAL_VARIATIONS) )
    {
        pfcnt->m_maxVal = indset.m_IndMaxV * 2;    // do not flash indicators at start.  (was: EPSILON)
    }

    instCount = pfcDB->GetInstancesCount(pfcnt->m_ObjectId);

    indset.m_isInstanceFound = true;

    if ( instCount > 0 )
    {
        indset.m_isInstanceFound = false;

        indset.m_strInstFound = indset.m_strInst;
        if ( indset.m_strInst.GetLength() == 0 )
        {
            indset.m_strInstFound = pfcDB->FindDefaultInstance(pfcnt->m_ObjectId, pfcnt->m_CounterId);
        }

        // Search for Instance.
        if ( ConvertToUni(indset.m_strInstFound, pfcnt->m_InstWsz, MAX_INST_NAME) > 0 )
        {
            long inst_found;
            indset.m_isInstanceFound = pfcDB->FindInstanceId(pfcnt->m_ObjectId, 
                                        pfcnt->m_CounterId, 
                                        pfcnt->m_InstWsz,
                                        &inst_found );
            pfcnt->m_InstId = inst_found;
        }
    }

    pfcnt->SetActive(    pfcnt->m_ObjectId 
                      && pfcnt->m_CounterId 
                      && pfcnt->IsCounterTypeSupported(pfcnt->m_CounterType)
                      && indset.m_isInstanceFound );

    ///////////////////////////////////////////////////////////////////
    //      Format Real Tool Tip Title
    ///////////////////////////////////////////////////////////////////
    if ( indset.m_strInstFound.GetLength() > 0 )
    {
        indset.m_strIndicTitle.Format( "%s: %s (%s)" ,
                    (LPCSTR)indset.m_strCategory, (LPCSTR)indset.m_strCounter,
                    (LPCSTR)indset.m_strInstFound
                    ); 
    }
    else
    {
        indset.m_strIndicTitle.Format( "%s: %s" , 
                    (LPCSTR)indset.m_strCategory, (LPCSTR)indset.m_strCounter
                    ); 
    }

    return pfcnt->IsActive();
}

////////////////////////////////////////////////////////////////////////////

void CGadgets::Clear(bool isDestroyWindows)
{
    while ( !m_indicators.IsEmpty() )
    {
        CIndicator* ind = m_indicators.GetHead();
        if ( ind && ind->m_hWnd )
        {        
            if ( ind->m_indicSettings && ind->m_indicSettings->IsVisible() )
            {
                if ( ind->m_ToolTip.m_hWnd != 0 )
                {
                    ind->m_ToolTip.Pop();
                    ind->m_ToolTip.Activate(FALSE);
                    ind->m_ToolTip.DestroyWindow();
                }
                
            }

            if ( isDestroyWindows )
                ind->DestroyWindow();
        }

        delete ind;
        m_indicators.RemoveHead();
    }
}

////////////////////////////////////////////////////////////////////////////

int CGadgets::GetVisibleIndicsCount()
{
    CVisibleIndicatorsIterator iter(*this);
    CIndicator* pind;
    int count = 0;
    while ( iter.GetNext(&pind) ) { 
        count++;
    }
    return count;
}




/////////////////////////////////////////////////////////////////////////////
//     class: CVisibleIndicatorsIterator
/////////////////////////////////////////////////////////////////////////////

CVisibleIndicatorsIterator::CVisibleIndicatorsIterator(CGadgets& gadgets) : m_gadgets(gadgets)
{
    Reset();
}

/////////////////////////////////////////////////////////////////////////////
void CVisibleIndicatorsIterator::Reset()
{
    if ( m_gadgets.m_indicators.IsEmpty() ) {
        m_pos = 0;
    }
    else {
        m_pos = m_gadgets.m_indicators.GetHeadPosition();
    }
    is_used = false;
}

///////////////////////////////////////////////////////////////
bool  CVisibleIndicatorsIterator::GetNext(CIndicator* * p_ind)
{
    while ( m_pos )
    {
        CIndicator* ind = m_gadgets.m_indicators.GetNext(m_pos);
        ASSERT(ind);

        CIndicatorSettings* indset = ind->m_indicSettings;

        if ( !indset->IsVisible() )
            continue;

        is_used = true;
        *p_ind = ind;
        return true;
    }

    *p_ind = 0;
    return false;
}

// ##########################################################################
//      Make new set of gadgets (all inicators values are lost).
// ##########################################################################
void  MakeNewGadgets() 
{
    SET_WAIT_CURSOR;
    
    if (g_Gadgets) 
        delete  g_Gadgets;

    g_Gadgets = new CGadgets;
    
    g_Gadgets->InitGagets();
    strcpy(g_ProcessesToolTipTitle, " Recent Activity ");
    strcpy(g_ProcessesToolTipText,  " ( not yet collected ) ");
    RESTORE_CURSOR;
}

