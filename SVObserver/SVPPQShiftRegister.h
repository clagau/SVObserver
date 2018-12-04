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
#pragma endregion Includes

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

	long GetIndexByTriggerCount(long triggerCount) const;
	SVProductInfoStruct* GetProductByTriggerCount(long triggerCount) const;

	long GetIndexByTriggerTimeStamp(SvTl::SVTimeStamp checkTime, int cameraID) const;

	HRESULT GetProductStates( std::string& p_rProductStates ) const;

private:
	bool findTimeMatch(SvTl::SVTimeStamp checkTime, SvTl::SVTimeStamp triggerTime, bool isPPQ1) const;

	typedef std::vector< SVProductInfoStruct* > SVProductVector;

	SVProductVector m_Products;

	long m_HeadTriggerCount;
	long m_TailTriggerCount;
};

