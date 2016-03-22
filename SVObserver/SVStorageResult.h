//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorageResult
//* .File Name       : $Workfile:   SVStorageResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:36  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <set>
#include "SVStorage.h"

struct SVStorageResult
{
	SVStorageResult();
	SVStorageResult( const SVStorage& p_rStorage, HRESULT p_Status, unsigned long p_TriggerCount );

	SVStorage m_Storage;
	HRESULT m_Status;
	unsigned long m_TriggerCount;
};

typedef std::set< SVString > SVNameSet;
typedef std::map< SVString, SVStorageResult > SVNameStorageResultMap;
