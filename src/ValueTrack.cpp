//////////////////////////////////////////////////////////////////////
// ValueTrack.cpp
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include  <stdio.h>
#include  <math.h>
#include  <stdlib.h>
#include  <assert.h>

#include "cdefs.h"
#include "ValueTrack.h"

// -------------------------------------------------------------------------------
CValueTrack::CValueTrack(void)
{
    m_TrackSize = DEFAULT_TRACK_SIZE;
    Clear();
}

// -------------------------------------------------------------------------------
CValueTrack::~CValueTrack()
{
    FUNCTIONLOG(7, "CValueTrack::destr");
}

// -------------------------------------------------------------------------------
void  CValueTrack::Clear()
{
    m_Last = 0 ;
    m_Curr = -1 ;
    m_Sum = 0;
}

// -------------------------------------------------------------------------------
void  CValueTrack::SetTrackSize(int size)
{
    if ( size < 1 )
        return;

    if ( size > MAX_TRACK_VALUES)
        size = MAX_TRACK_VALUES;

    int diff = abs(size - m_TrackSize);
    int cnt = GetCount();
    int k = 0;

    // ---------------------------------------
    // -         Size decreased              -
    // ---------------------------------------
    if ( size < m_TrackSize &&  cnt > size )
    {
        if ( cnt < m_TrackSize )
            diff = cnt - size;

        // Roll back a sum of values.
        for (k = 0; k < diff; ++k )
        {
            m_Sum -= m_values[m_Last];
            ++m_Last;
            if ( m_Last >= m_TrackSize )
                m_Last = 0;
        }

        if ( m_Last >= m_Curr )
        {
            // Fill the gap with removed values.
            for (k = 0; k < diff; ++k )
            {
                m_values[m_Curr + k + 1 ] = m_values[m_Curr + k + 1 + diff ];
            }
            m_Last = m_Curr + 1;
        }    
        else  //  (m_Last < m_Curr)
        {
            if ( cnt == m_TrackSize ) 
            {
                // Buffer was full and now m_Last < m_Curr.
                assert( m_Last <= diff );

                for (k = 0; k < size; ++k )
                {
                    m_values[k] = m_values[m_Last + k];
                }
            }
            else // (cnt < m_TrackSize) 
            {
                // Buffer was nor full, just shift it to zero remove gap
                for (k = 0; k < size; ++k )
                {
                    m_values[k] = m_values[k + diff];

                }
            }
            m_Last = 0;
            m_Curr = size - 1;
        }


    }

    // ---------------------------------------
    // -         Size increased              -
    // ---------------------------------------
    else if ( size > m_TrackSize && cnt >= m_TrackSize &&  m_Curr < m_Last )
    {

        // Shift values after m_Last to the end of buffer.  
        for ( k = m_Last ; k  < m_TrackSize ; ++k )
        {
            m_values[k + diff ] = m_values[k];
        }

        // Fill the gap with zeroes. 
        for ( k = 0 ; k  < diff ; ++k )
        {
            m_values[m_Last + k] = 0;
        }

        // NOTE: Do not change m_Last or m_Curr.
    }

    m_TrackSize = size;
}

// -------------------------------------------------------------------------------
//      Get count of valid values in Track
// -------------------------------------------------------------------------------
int   CValueTrack::GetCount()  
{ 
    if ( m_Curr < 0 )
        return 0;

    int k = m_Curr - m_Last + 1;
    if ( m_Curr > m_Last ) 
    {
        assert( m_Last == 0 );              // invariant
        return m_Curr + 1;
    }
    else if ( m_Curr < m_Last ) 
    {
        assert( m_Last ==  m_Curr + 1);     // invariant
        return m_TrackSize;
    }
    return 1;
}

// -------------------------------------------------------------------------------
// Store new value into the list and logically shift it.
// -------------------------------------------------------------------------------
void  CValueTrack::PutValue(TValueTrack  v)
{
    int cnt = GetCount();

    if ( GetCount() >= m_TrackSize  )
    {
        m_Sum -= m_values[m_Last];

        ++m_Last;
        if ( m_Last >= m_TrackSize )
        {
            m_Last = 0;
        }
    }

    ++m_Curr;
    if ( m_Curr >= m_TrackSize )
    {
        m_Curr = 0;
    }
    m_values[m_Curr] = v;

    m_Sum += v;
}

// -------------------------------------------------------------------------------
// Add value to the last stored (do not shift the list).
// -------------------------------------------------------------------------------
void  CValueTrack::AddToLastValue(TValueTrack  v)
{
    assert(m_Curr >= 0);
    m_values[m_Curr] += v;
    m_Sum += v;
}

// -------------------------------------------------------------------------------
