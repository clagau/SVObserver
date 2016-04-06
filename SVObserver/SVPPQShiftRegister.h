//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQShiftRegister
//* .File Name       : $Workfile:   SVPPQShiftRegister.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:31:16  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"
#pragma region Includes

struct SVProductInfoStruct;

class SVPPQShiftRegister
{
public:
	SVPPQShiftRegister();
	virtual ~SVPPQShiftRegister();

	size_t size() const;
	void resize( size_t p_Size );
	void clear();

	SVProductInfoStruct* GetProductAt( size_t p_Index ) const;

	HRESULT SetProductAt( size_t p_Index, SVProductInfoStruct* p_pProduct );

	SVProductInfoStruct* IndexRegister();

	HRESULT IsProductAlive( long p_TriggerCount ) const;

	HRESULT GetIndexByTriggerCount( long& p_rIndex, long p_TriggerCount ) const;
	HRESULT GetProductByTriggerCount( SVProductInfoStruct*& p_rpProduct, long p_TriggerCount ) const;

	HRESULT GetIndexByTriggerTimeStamp( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp, SVClock::SVTimeStamp p_LowerThresholdInMilliseconds = 0.0 ) const;
	HRESULT GetIndexByTriggerTimeStamp( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp, SVClock::SVTimeStamp p_LowerThresholdInMilliseconds, SVClock::SVTimeStamp p_UpperThresholdInMilliseconds ) const;

	HRESULT GetProductStates( SVString& p_rProductStates ) const;

private:
	typedef std::vector< SVProductInfoStruct* > SVProductVector;

	SVProductVector m_Products;

	long m_HeadTriggerCount;
	long m_TailTriggerCount;
};

