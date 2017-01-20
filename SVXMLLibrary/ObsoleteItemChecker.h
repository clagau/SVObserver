//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "SVConfigurationTags.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVXmlLibrary/SVNavigateTree.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#pragma endregion Includes

namespace Seidenader { namespace  SVXMLLibrary
{
	template<typename TreeType>
	static HRESULT HasObsoleteItem(TreeType& rTree, SVString& rItemType, int& errorCode);
	
	
	template< typename SVTreeType >
	HRESULT CheckObsoleteItems( SVTreeType& rTree, const unsigned long& ulSVOConfigVersion, SVString& rItemType, int& errorCode );
	
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */

#include "ObsoleteItemChecker.inl"

namespace SvXml = Seidenader::SVXMLLibrary;
