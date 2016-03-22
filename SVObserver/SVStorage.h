//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorage
//* .File Name       : $Workfile:   SVStorage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:12  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>
#include "SVUtilityLibrary/SVString.h"
#include "SVVisionProcessorConstants.h"

struct SVStorage
{
	SVStorage();
	SVStorage( SVVisionProcessor::SVStorageType p_StorageType, const _variant_t& p_rVariant );

	SVVisionProcessor::SVStorageType m_StorageType;
	_variant_t m_Variant;
};

typedef std::map< SVString, SVStorage > SVNameStorageMap;
typedef std::map< SVString, HRESULT > SVNameStatusMap;

