//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
// ObsoleteItemChecker.h
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVHardwareManifest.h"
#pragma endregion Includes

namespace SvXml
{
	template<typename TreeType>
	static HRESULT HasObsoleteItem(TreeType& rTree, std::string& rItemType, int& errorCode);
	
	
	template< typename SVTreeType >
	HRESULT CheckObsoleteItems( SVTreeType& rTree, const unsigned long& ulSVOConfigVersion, std::string& rItemType, int& errorCode );
	
} //namespace SvXml

#include "ObsoleteItemChecker.inl"
