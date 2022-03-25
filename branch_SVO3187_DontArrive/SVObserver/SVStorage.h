//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStorage
//* .File Name       : $Workfile:   SVStorage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:12  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>

#include "SVVisionProcessorConstants.h"
#pragma endregion Includes
//#define TRACE_SVSTORAGE TRUE
struct SVStorage
{
	SVStorage();
	SVStorage( SVVisionProcessor::SVStorageType p_StorageType, const _variant_t& p_rVariant );
	#if defined (TRACE_THEM_ALL) || defined (TRACE_SVSTORAGE)
	SVStorage(const SVStorage& rhs) : m_StorageType(rhs.m_StorageType),
										m_Variant(rhs.m_Variant)
	{
		TRACE0("++++++SVstorage copy Constructor+++\n");
	}

	SVStorage(const SVStorage&& rhs) : m_StorageType(std::move(rhs.m_StorageType)),
		m_Variant(std::move(rhs.m_Variant))
	{
		TRACE0("++++++SVstorage move Constructor++++\n");
	}


	SVStorage& operator=(const SVStorage& rhs)
	{
		if (&rhs == this)
			return *this;
		TRACE0("+++++++SVstorage copy assignment=()++++++\n");
		m_StorageType = rhs.m_StorageType;
		m_Variant = rhs.m_Variant;
		return *this;
	}

	SVStorage& operator=(const SVStorage&& rhs)
	{
		if (&rhs == this)
			return *this;
		TRACE0("+++++++SVstorage move assignment=()+++++++++++\n");
		m_StorageType = rhs.m_StorageType;
		m_Variant = std::move(rhs.m_Variant);
		return *this;
	}
	#endif 

	SVVisionProcessor::SVStorageType m_StorageType;
	_variant_t m_Variant;
};

typedef std::map<std::string, SVStorage> SVNameStorageMap;
typedef std::map<std::string, HRESULT> SVNameStatusMap;

