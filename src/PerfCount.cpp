// PerfCount.cpp: implementation of the CPerfCount class.
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glint.h"
#include "PerfCount.h"
#include "PerfData.h"
#include "IndicatorSettings.h"
#include "Settings.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPerfCount::CPerfCount() 
{ 
    m_IndSets = 0;
    InitPerfCount();
}

CPerfCount::CPerfCount(LPCSTR  szCategory, LPCSTR  szCounter, LPCSTR  szInst, CIndicatorSettings* pIndSets)
        : m_strCategory(szCategory), m_strCounter(szCounter), m_strInst(szInst), m_IndSets(pIndSets)
{
    InitPerfCount();
}

void CPerfCount::InitPerfCount() 
{ 
    m_val = 0.0;
    m_isActive      = false;
    m_ObjectId      = 0;
    m_CounterId     = 0;
    m_InstId        = 0;
    m_MO            = 0.0;
    m_maxValUsed    = 0;
    m_maxVal        = 0;
    m_RecentSamples = 0;
    m_samples       = 0;
    m_flags         = PFCFLG_MAX_UNDEFINED;
    m_PreviousValue = 0;
    memset(&m_LastRawData, 0, sizeof(m_LastRawData)) ;
    m_strInst.Trim();
}

CPerfCount::~CPerfCount()
{
    FUNCTIONLOG(7, "CPerfCount::destr");
}

//**********************************************************************************************
//              Obtain next value (from 0 to 1.0 )
//**********************************************************************************************
void  CPerfCount::MakeValue()         
{   
    // RegQueryValueEx(HKEY_PERFORMANCE_DATA,...) 
    ASSERT(IsActive() && m_cntDB);
    PBYTE data = m_cntDB->FindInstanceData(m_ObjectId, m_CounterId, m_InstWsz, m_InstId);

    if ( !data ) {      
        // Next attempt. Instance could change its Id. Search it again.
        if ( m_cntDB->FindInstanceId(m_ObjectId, m_CounterId, m_InstWsz, &m_InstId) )
        {
            data = m_cntDB->FindInstanceData(m_ObjectId, m_CounterId, m_InstWsz, m_InstId );
        }
    }

    if ( !data ) {      // instance vanished
        m_val = 0.0;
        return ;
    }
    MakeValue(data);
}

//**********************************************************************************************
void  CPerfCount::MakeValue(PBYTE data)         
{   
    memcpy(&m_LastRawData, &m_RawData, m_CounterSize);
    memcpy(&m_RawData, data,m_CounterSize );

    // NOTE: sometimes Base has different size than Counter Data !!!

    PBYTE baseData = data + m_CounterSize;      
    memcpy(&m_LastBaseData, &m_BaseData, m_BaseSize);
    memcpy(&m_BaseData, data,m_BaseSize );      

    if ( m_cntDB->IsDataReady() && FLAG_CHECK(m_flags,PFCFLG_PREV_READ) ) 
    {   
        Calculate();
    }
    FLAG_SET(m_flags,PFCFLG_PREV_READ);
}

//**********************************************************************************************
//          Calc current values from sample(s) and base(s)
//**********************************************************************************************
void  CPerfCount::Calculate()         
{   
    double a, b;
    #define CHECK_PERCENTAGE(v)  if ( m_val < 0.0 ) m_val = 0.0;    \
                                 else if ( m_val > 100.0 ) m_val = 100.0;       

    m_PreviousValue = m_val;

    m_samples++;
    m_RecentSamples++;
    switch(m_CounterType) 
    {
        case PERF_COUNTER_COUNTER :
            a = (double) ( m_RawData.LowPart - m_LastRawData.LowPart );
            b = SubLL( m_cntDB->PerfDataBlk()->PerfTime,  m_cntDB->m_OldDataBlk.PerfTime);
            if ( b != 0.0 )
                m_val = a / (b /  m_cntDB->m_Freq);  
            else
                m_val = 0.0;
            break;

        case PERF_COUNTER_BULK_COUNT :
            a = SubLL( m_RawData,  m_LastRawData);
            b = SubLL( m_cntDB->PerfDataBlk()->PerfTime,  m_cntDB->m_OldDataBlk.PerfTime);
            if ( b != 0.0 )
                m_val = a / (b /  m_cntDB->m_Freq);  
            else
                m_val = 0.0;
            break;


        case PERF_100NSEC_TIMER :
            a = SubLL( m_RawData,  m_LastRawData);
            b = SubLL( m_cntDB->PerfDataBlk()->PerfTime100nSec,  m_cntDB->m_OldDataBlk.PerfTime100nSec);
            if ( b != 0.0 )
                m_val = 100.0 * a / b ;
            else
                m_val = 0.0;
            CHECK_PERCENTAGE(m_val);
            break;

        case PERF_100NSEC_TIMER_INV :
            a = SubLL( m_RawData,  m_LastRawData);
            b = SubLL( m_cntDB->PerfDataBlk()->PerfTime100nSec,  m_cntDB->m_OldDataBlk.PerfTime100nSec);
            if ( b != 0.0 )
                m_val = 100.0 * (1 - (a / b) );
            else
                m_val = 0.0;
            CHECK_PERCENTAGE(m_val);
            break;

        case PERF_COUNTER_RAWCOUNT :
            // The problem: you can not substract greater unsigned from the lesser.
            a = (double) (  ( m_RawData.LowPart >= m_LastRawData.LowPart) ? 
                            ( m_RawData.LowPart - m_LastRawData.LowPart ) : 
                            ( m_LastRawData.LowPart - m_RawData.LowPart ) ) ;
            m_val = a / sets.m_OurFrequency ;
            break;

        case PERF_COUNTER_LARGE_RAWCOUNT :
            a = SubLL( m_RawData,  m_LastRawData);
            a = fabs(a);
            m_val = a / sets.m_OurFrequency ;
            break;

        case PERF_COUNTER_TIMER :
        case PERF_COUNTER_QUEUELEN_TYPE :
        case PERF_COUNTER_LARGE_QUEUELEN_TYPE :
        case PERF_COUNTER_TEXT :
        case PERF_COUNTER_RAWCOUNT_HEX :
        case PERF_COUNTER_LARGE_RAWCOUNT_HEX :
        case PERF_SAMPLE_FRACTION :
        case PERF_SAMPLE_COUNTER :
        case PERF_COUNTER_NODATA :
        case PERF_COUNTER_TIMER_INV :
        case PERF_SAMPLE_BASE :
        case PERF_AVERAGE_TIMER :
        case PERF_AVERAGE_BASE :
        case PERF_AVERAGE_BULK :
        case PERF_COUNTER_MULTI_TIMER :
        case PERF_COUNTER_MULTI_TIMER_INV :
        case PERF_COUNTER_MULTI_BASE :
        case PERF_100NSEC_MULTI_TIMER :
        case PERF_100NSEC_MULTI_TIMER_INV :
        case PERF_RAW_FRACTION :
        case PERF_RAW_BASE :
        case PERF_ELAPSED_TIME :
        case PERF_COUNTER_HISTOGRAM_TYPE :
        case PERF_COUNTER_DELTA :
        case PERF_COUNTER_LARGE_DELTA :
        default:
            break;
    }

    if ( m_val < -1.0E+20 || m_val > 1.0E+20 )
    {
        CString str;
        str.Format("***** Wrong counter [%s,%s] value is calculated : %g ****  Zero vealu used.", (LPCSTR)m_strCounter, (LPCSTR)m_strInst, m_val );
        DATA_ERR_MESSAGE_BOX(str, MB_OK | MB_ICONSTOP);
        m_val = 0.0;
    }

    if ( m_val < 0 )    m_val = -m_val; // to be sure its positive
    m_MO = ( m_MO *(m_RecentSamples - 1) + m_val ) / m_RecentSamples;

    // clean-up mean to collect again
    if ( FLAG_CHECK(m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS) )
    {
        if (m_RecentSamples > 70 )  m_RecentSamples = 10;  
    }
    else
        if (m_RecentSamples > 100000 )  m_RecentSamples = 1000;  

}

//**********************************************************************************************
//      Obtain next value (from 0 to 1.0 )
//**********************************************************************************************
double  CPerfCount::GetRelativeValue()         
{   
    ASSERT(IsActive());
    double v;
    double vtmp = m_val;
    double vmax =  0;

    if ( FLAG_CHECK(m_IndSets->m_flags, INDSW_MAX_ABSOLUTE) ) 
    {
        // =======================================
        //    Absolute maximum is provoded. 
        // =======================================
        vmax = m_IndSets->m_IndMaxV;
        vtmp = GetAbsoluteValue(); 
    }
    else 
    {
        // Update maximum if it is not fixed.
        if (  // FLAG_CHECK(m_flags,PFCFLG_MAX_UNDEFINED) &&
              FLAG_CHECK(m_IndSets->m_flags,INDSW_ADJUST_MAX) 
            )
        {
            if ( m_val > m_maxVal ) 
            {
                if ( m_maxVal/m_val < 0.9 )
                    m_maxVal = m_val;
            }
        }

        vmax =  m_maxVal;

        // =======================================
        //    Calculate sliding mean max value
        // =======================================
        if (  FLAG_CHECK(m_flags,PFCFLG_MAX_SMART) && 
              FLAG_CHECK(m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS) 
           )
        {
            vmax = m_MO *  2.5;
            if ( m_MO < EPSILON )
            {
                vmax = 0.1;
            }
            // correct abs max for columns show if difference is high
            if ( CheckDispMode(DISP_COLUMN) &&
                ( m_maxVal/vmax < 0.8  || m_maxVal/vmax > 1.2 ) 
               )
                m_maxVal = vmax;
        }
        // =======================================
        //    Adjust maximum for Graphs 
        // =======================================
        else if (  CheckDispMode(DISP_GRAPH) && 
                   FLAG_CHECK(m_flags,PFCFLG_MAX_SMART) && 
                   FLAG_CHECK(m_IndSets->m_flags,INDSW_LOCAL_VARIATIONS) && 
                   (m_samples > 3) )
        {
            if ( m_MO < EPSILON || vmax < EPSILON )
            {
                vmax = 0.1;
            }
            else 
            {
                if ( m_MO*1.001 >= m_maxVal )
                    vmax = m_MO*1.001;

                float m = m_maxVal/ m_MO;
                if ( m > 2.0 )
                {   
                    vmax = m_MO * ( 2.0 + log10( m - 2.0 + 1.0) );
                }

                #if SUPPRESS_LARGE_VALUES
                else if ( m < 1.3 )   // MO close to max 
                {
                    double fk = fabs( 1.0 / ( m - 1.0 ) ) ;
                    vtmp /= m_maxVal;
                    vtmp = pow( vtmp, fk );
                    vmax = 1.0;
                    // DEBUGOUTPUT3("[glint] GetRelativeValue: m_maxVal=%lf m_val=%lf => vtmp=%lf \n", m_maxVal, m_val, vtmp );
                }
                #endif SUPPRESS_LARGE_VALUES
            }
        }

        if ( FLAG_CHECK(m_IndSets->m_flags,INDSW_ADJUST_MAX) )
        {
            m_LastValues.Put(vtmp);
        }


        // Now don't allow maximum to drop below provided threshold
        if ( CheckDispMode(DISP_COLUMN) && m_IndSets->m_IndMaxV == 0  )
        {
            //vmax = m_IndSets->m_IndMaxV;
        }
        else if ( !FLAG_CHECK(m_IndSets->m_flags, INDSW_LOCAL_VARIATIONS) &&
                  m_IndSets->m_IndMaxV != 0 && 
                  vmax < m_IndSets->m_IndMaxV )
        {
            vmax = m_IndSets->m_IndMaxV;
        }
    }

    // =======================================
    //    Final relative value calculation
    // =======================================

    m_maxValUsed = vmax;
    v = vtmp / vmax ;

    #if defined(_DEBUG) && 0
    if (  strcmp( m_strCounter, "% Processor Time") == 0 )
        TRACE( "GetRV[%s] flg:(%x,%x)  v=%f  m_val=%f  vtmp=%f  vmax=%f \r\n",   (LPCSTR)m_IndSets->m_strSmallTitle,  m_flags, m_IndSets->m_flags, v, m_val, vtmp, vmax);
    #endif

    // Last tuning
    if ( v < EPSILON ) v = 0.0;
    else if ( v > 1.0) v = 1.0;
    
    return v;
}

//**********************************************************************************************
//    Adjust maximum 
//**********************************************************************************************
void  CPerfCount::AdjustMax()         
{   
    if ( !FLAG_CHECK(m_IndSets->m_flags,INDSW_ADJUST_MAX) || CheckDispMode(DISP_GRAPH) )
        return;

    if ( m_LastValues.IsLess( m_maxValUsed * LESS_COEFF_TO_ADJUST_MAX ) )   // all less than threshold
    {
        m_maxVal *= LESS_COEFF_TO_ADJUST_MAX; 

        // Now don't allow maximum to drop below provided threshold
        if ( m_IndSets->m_IndMaxV != 0 && m_maxVal < m_IndSets->m_IndMaxV )
        {
            m_maxVal = m_IndSets->m_IndMaxV;
        }
    }
}

//**********************************************************************************************
//      Obtain absolute value of counter
//**********************************************************************************************
double  CPerfCount::GetAbsoluteValue()         
{   
    double a;
    ASSERT(IsActive());
    if ( !IsActive() ) 
        return 0;

    switch(m_CounterType) 
    {
        case PERF_COUNTER_RAWCOUNT :
        case PERF_COUNTER_RAWCOUNT_HEX :
            a = (double) m_RawData.LowPart;
            return a;

        case PERF_COUNTER_LARGE_RAWCOUNT :
        case PERF_COUNTER_LARGE_RAWCOUNT_HEX :
            return LToD(m_RawData);

        default:
            //ASSERT( m_CounterType == 0 );
            break;
    }
    return m_val;
}

//**********************************************************************************************
//      Obtain next value (from 0 to 1.0 )
//**********************************************************************************************
double  CPerfCount::GetPreviousAbsValue()         
{   
    double a;
    ASSERT(IsActive());
    switch(m_CounterType) 
    {
        case PERF_COUNTER_RAWCOUNT :
        case PERF_COUNTER_RAWCOUNT_HEX :
            a = (double) m_LastRawData.LowPart;
            return a;

        case PERF_COUNTER_LARGE_RAWCOUNT :
        case PERF_COUNTER_LARGE_RAWCOUNT_HEX :
            return LToD(m_LastRawData);

        default:
            ASSERT( m_CounterType == 0 );
    }

    return m_PreviousValue;
}

//**********************************************************************************************
//      Obtain actual max value used for the indicator
//**********************************************************************************************
double  CPerfCount::GetCurrentMaxValue()         
{   
    ASSERT(IsActive());

    // Return some believable value if we don't know the actual one.....
    if (  FLAG_CHECK(m_flags, PFCFLG_MAX_UNDEFINED) && 
        ( m_CounterType == PERF_COUNTER_RAWCOUNT || m_CounterType == PERF_COUNTER_LARGE_RAWCOUNT ) 
        )
    {
        if (m_maxValUsed < EPSILON)
            return 0;

        double a  = GetAbsoluteValue();
        double a1 = GetPreviousAbsValue();
        a = min(a, a1) + m_maxValUsed;
        if ( a < 100000. ) {
            unsigned long ibig = a;      // round to interger
            a = ibig;
        }
        return a;
    }

    return m_maxValUsed;
}

//**********************************************************************************************
//  Obtain value to be used for calculation of MO (which has been used in relative value calc).
//**********************************************************************************************
double CPerfCount::GetRealCounterValue()
{
    if ( m_CounterType == PERF_COUNTER_RAWCOUNT ||
         m_CounterType == PERF_COUNTER_LARGE_RAWCOUNT )
    {
        return m_val;   // difference, not abs value.
    }

    return GetAbsoluteValue();
}

//**********************************************************************************************
bool CPerfCount::IsCounterTypeSupported(DWORD t) 
{

    switch(t) 
    {
        case PERF_COUNTER_COUNTER :
        case PERF_COUNTER_BULK_COUNT :
            FLAG_SET(m_flags, PFCFLG_MAX_SMART);
            return true;

        case PERF_100NSEC_TIMER_INV :
        case PERF_100NSEC_TIMER :
            FLAG_CLEAR(m_flags, PFCFLG_MAX_UNDEFINED);
            m_maxVal = 100.0;
            if ( m_IndSets )
            {
                FLAG_SET(m_IndSets->m_flags, INDSW_MAX_ABSOLUTE);
                m_IndSets->m_IndMaxV = 100.0;
            }
            return true;

        case PERF_COUNTER_RAWCOUNT :
        case PERF_COUNTER_LARGE_RAWCOUNT :
            if ( m_IndSets )
            {
                // Settings are already initialized
                if ( !FLAG_CHECK(m_IndSets->m_flags, INDSW_MAX_ABSOLUTE) )
                {
                    FLAG_SET(m_flags, PFCFLG_MAX_SMART);
                }
                else 
                {
                    //FLAG_CLEAR(m_flags, PFCFLG_MAX_UNDEFINED);    // ???
                }
            }
            return true;

        // Not used in system yet
        case PERF_COUNTER_DELTA :
        case PERF_COUNTER_LARGE_DELTA :
            // TODO
            break;

        // difficult to implement & not used yet.
        case PERF_COUNTER_TIMER :
        case PERF_COUNTER_QUEUELEN_TYPE :
        case PERF_COUNTER_LARGE_QUEUELEN_TYPE :
        case PERF_COUNTER_TEXT :
        case PERF_COUNTER_RAWCOUNT_HEX :
        case PERF_COUNTER_LARGE_RAWCOUNT_HEX :
        case PERF_SAMPLE_FRACTION :
        case PERF_SAMPLE_COUNTER :
        case PERF_COUNTER_NODATA :
        case PERF_COUNTER_TIMER_INV :
        case PERF_SAMPLE_BASE :
        case PERF_AVERAGE_TIMER :
        case PERF_AVERAGE_BASE :
        case PERF_AVERAGE_BULK :
        case PERF_COUNTER_MULTI_TIMER :
        case PERF_COUNTER_MULTI_TIMER_INV :
        case PERF_COUNTER_MULTI_BASE :
        case PERF_100NSEC_MULTI_TIMER :
        case PERF_100NSEC_MULTI_TIMER_INV :
        case PERF_RAW_FRACTION :
        case PERF_RAW_BASE :
        case PERF_ELAPSED_TIME :
        case PERF_COUNTER_HISTOGRAM_TYPE :
        default:
            break;
    }
    return false;
}


//----------------------------------------------------------------
CStoredValues::CStoredValues()
{
    Reset();
}

//----------------------------------------------------------------
void    CStoredValues::Reset()
{
    memset( m_data, 0, sizeof(m_data) );
    m_lastIdx = 0;
}

//----------------------------------------------------------------
bool    CStoredValues::IsLess(double v)
{
    for (int k = 0; k < NUM_LAST_VALUES; ++k )
    {
        if ( m_data[k] >= v )
            return false;
    }
    return true;
}

//----------------------------------------------------------------
void    CStoredValues::Put(double v)
{
    m_data[m_lastIdx] = v;
    if ( ++m_lastIdx >= NUM_LAST_VALUES )
        m_lastIdx = 0;
}
//----------------------------------------------------------------
