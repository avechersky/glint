//////////////////////////////////////////////////////////////////////
// ValueTrack.h
// Author: Alexander Vechersky, 2010 
//////////////////////////////////////////////////////////////////////

#if !defined(VALUETRACK_H_IS_INCLUDED_)
#define VALUETRACK_H_IS_INCLUDED_

#pragma once

typedef  unsigned long long  TValueTrack;
#define  MAX_TRACK_VALUES     200
#define  DEFAULT_TRACK_SIZE   MAX_TRACK_VALUES

// ------------------------------------------------------------------------------------
class CValueTrack
{
public:
    CValueTrack();
    ~CValueTrack();

    // Change current Track Size and reorganize data.
    void  SetTrackSize(int size);

    int   GetTrackSize()  { return m_TrackSize; }

    void  PutValue(TValueTrack  v);         // Store new value into the list and logically shift it.
    void  AddToLastValue(TValueTrack  v);   // Add value to the last stored (do not shift the list).

    int    GetCount();    // Get count of valid values in Track

    TValueTrack  GetSum() { return m_Sum; }

    void  Clear();

protected:
// DATA
    int             m_Last;         // idx of last stored value  (when buffer is full => m_Last == m_Curr + 1 )
    int             m_Curr;         // idx of current value
    TValueTrack     m_Sum;          //  current sum of values
    int             m_TrackSize;    // # of values to store in buffer

    TValueTrack     m_values[MAX_TRACK_VALUES + 1];     // stored values
};

// ------------------------------------------------------------------------------------

#endif // VALUETRACK_H_IS_INCLUDED_