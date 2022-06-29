//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
// ObsoleteItemChecker.h
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvXml
{
class SVXMLMaterialsTree;
}

namespace SvXml
{
	HRESULT HasObsoleteItem(SvXml::SVXMLMaterialsTree& rTree, std::string& rItemType, int& errorCode);
	HRESULT CheckObsoleteItems(SvXml::SVXMLMaterialsTree& rTree, const unsigned long& ulSVOConfigVersion, std::string& rItemType, int& errorCode);
	
} //namespace SvXml
