//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************
#pragma once

namespace SvXml
{
	template<typename TreeType>
	static HRESULT HasObsoleteItem(TreeType& rTree, std::string& rItemType, int& errorCode);
	
	
	template< typename SVTreeType >
	HRESULT CheckObsoleteItems( SVTreeType& rTree, const unsigned long& ulSVOConfigVersion, std::string& rItemType, int& errorCode );
	
} //namespace SvXml

#include "ObsoleteItemChecker.inl"
