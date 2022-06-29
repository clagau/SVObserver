//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
// ObsoleteItemChecker.cpp
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "ObsoleteItemChecker.h"
#include "SVHardwareManifest.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

namespace SvXml
{
#pragma region String Defines
constexpr const wchar_t* TAG_ALIAS = L"alias ";
constexpr const wchar_t* TAG_SINGLE_QUOTE = L"\'";
constexpr const char* TAG_VIPER = "Viper_";
constexpr const char* TAG_1394 = "Matrox_1394";
constexpr const char* TAG_GAGETOOL = "Gage Tool";
constexpr const char* TAG_PROFILETOOL = "Profile Tool";
constexpr const char* TAG_OCROCVANALYZER = "OCR  OCV Analyzer";
constexpr const char* TAG_OCROCVGRAYANALYZER = "OCR  OCV Gray Scale Analyzer";

constexpr const char* ITEM_ANALOG_CAMERA = _T("Analog Camera - Obsolete");
constexpr const char* ITEM_1394_CAMERA = _T("1394 Camera - Obsolete");
constexpr const char* ITEM_GAGE_TOOL = _T("Gage Tool - Obsolete");
constexpr const char* ITEM_PROFILE_TOOL = _T("Profile Tool - Obsolete");
constexpr const char* ITEM_FAST_OCR = _T("Fast OCR - Obsolete");
constexpr const char* ITEM_PRODUCTTYPE = _T("Unknown Product Type");
#pragma endregion String Defines

struct ObsoleteItem
{
	std::string m_tag;
	std::string m_itemType;
	int m_errorNo;
	ObsoleteItem(const std::string& rTag, const std::string& rItemType, int errorNo)
	: m_tag(rTag), m_itemType(rItemType), m_errorNo(errorNo)
	{
	}
};
typedef std::deque<ObsoleteItem> ObsoleteItems;

		
		
HRESULT CheckObsoleteItems(SvXml::SVXMLMaterialsTree& rTree, const unsigned long& ulSVOConfigVersion, std::string& rItemType, int& errorCode )
{
	HRESULT hr = S_OK;
	if (0x00072800 > ulSVOConfigVersion) // if 7.40 or greater no need to check
	{
		hr = HasObsoleteItem(rTree, rItemType, errorCode);
	}
	return hr;
}
		
		
bool HasAcquisitionDevice(SvXml::SVXMLMaterialsTree& rTree, const ObsoleteItems& rItems, std::string& rItemType, int& rErrorCode)
{
	SvXml::SVXMLMaterialsTree::SVBranchHandle hChild( nullptr );
	SvXml::SVXMLMaterialsTree::SVBranchHandle hBoardChild( nullptr );

	bool bFound = false;
	bool bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_ACQUISITION_DEVICE, nullptr, hChild );
	hBoardChild = rTree.getFirstBranch( hChild );

	while ( !bFound && bOk && nullptr != hBoardChild )
	{
		const std::string& rBoardName = rTree.getBranchName( hBoardChild );
		for (ObsoleteItems::const_iterator it = rItems.begin();it != rItems.end() && !bFound; ++it)
		{
			size_t pos = rBoardName.find(it->m_tag);
			if (std::string::npos != pos) 
			{
				bFound = true;
				rErrorCode = it->m_errorNo;
				rItemType = it->m_itemType;
			}
		}
		hBoardChild = rTree.getNextBranch( hChild, hBoardChild );
	}
	return bFound;
}
		
bool HasTaskObject(SvXml::SVXMLMaterialsTree& rTree, const ObsoleteItems& rItems, std::string& rItemType, int& rErrorCode)
{
	bool bFound = false;

	SvXml::SVXMLMaterialsTree::SVBranchHandle htiChild(nullptr );
	if( SVNavigateTree::GetItemBranch( rTree, CTAG_INSPECTION, nullptr, htiChild ) )
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle htiSubChild( rTree.getFirstBranch( htiChild ) );
		
		while( !bFound && nullptr != htiSubChild )
		{
			SvXml::SVXMLMaterialsTree::SVBranchHandle htiDataChild( rTree.getFirstBranch( htiSubChild ) );
			
			while( !bFound && nullptr != htiDataChild )
			{
				const std::string& rDataName = rTree.getBranchName( htiDataChild );

				if ( 0 == rDataName.compare( CTAG_SVIPDOC ) )
				{
					SvXml::SVXMLMaterialsTree::SVBranchHandle htiSVIPDoc = htiDataChild;
					_variant_t value;
					if ( SVNavigateTree::GetItem( rTree, CTAG_TOOLSET_SCRIPT, htiSVIPDoc, value ) )
					{
						_bstr_t str( value );
						for (ObsoleteItems::const_iterator it = rItems.begin();it != rItems.end() && !bFound; ++it)
						{
							std::wstring alias_tag( TAG_ALIAS );
							alias_tag += TAG_SINGLE_QUOTE;
							alias_tag += SvUl::to_utf16( it->m_tag );
							alias_tag += TAG_SINGLE_QUOTE;

							if ( nullptr != wcsstr( static_cast<LPCWSTR>(str), alias_tag.c_str() ) )
							{
								bFound = true;
								rErrorCode = it->m_errorNo;
								rItemType = it->m_itemType;
							}
						}
					}
				}
				else if ( 0 == rDataName.compare( CTAG_INSPECTION_PROCESS ) )
				{
					SvXml::SVXMLMaterialsTree::SVBranchHandle hItemToolset;
					if( SVNavigateTree::GetItemBranch( rTree, CTAG_TOOLSET_SET, htiDataChild, hItemToolset ) )
					{
						for (ObsoleteItems::const_iterator it = rItems.begin();it != rItems.end() && !bFound; ++it)
						{
							SvXml::SVXMLMaterialsTree::SVBranchHandle node(nullptr );
							if ( SVNavigateTree::GetItemBranch( rTree, it->m_tag.c_str(), hItemToolset, node ) )
							{
								bFound = true;
								rErrorCode = it->m_errorNo;
								rItemType = it->m_itemType;
							}
						}
					}
				}
				htiDataChild = rTree.getNextBranch( htiSubChild, htiDataChild );
			}
			htiSubChild = rTree.getNextBranch( htiChild, htiSubChild );
		}
	}
	return bFound;
}
		
bool HasInvalidProductType(SvXml::SVXMLMaterialsTree& rTree, std::string& rItemType, int& rErrorCode)
{
	bool bInvalid = false;
	SvXml::SVXMLMaterialsTree::SVBranchHandle hChild(nullptr);

	if (SVNavigateTree::GetItemBranch(rTree, CTAG_ENVIRONMENT, nullptr, hChild))
	{
		_variant_t svValue;

		if (SVNavigateTree::GetItem(rTree, CTAG_CONFIGURATION_TYPE, hChild, svValue))
		{
			int iType = svValue;
			if (!SVHardwareManifest::IsValidProductType(static_cast<SVIMProductEnum>(iType)))
			{
				bInvalid = true;
				rErrorCode = SvStl::Err_15043_UnknownProductType;
				rItemType = ITEM_PRODUCTTYPE;
			}
		}
	}
	return bInvalid;
}

HRESULT HasObsoleteItem(SvXml::SVXMLMaterialsTree& rTree, std::string& rItemType, int& rErrorCode)
{
	ObsoleteItems cameraItems
	{
		{TAG_VIPER, ITEM_ANALOG_CAMERA,	SvStl::Err_15038_AnalogCamera},
		{TAG_1394, ITEM_1394_CAMERA,	SvStl::Err_15039_1394Camera}
	};

	ObsoleteItems taskItems
	{
		{TAG_GAGETOOL,				ITEM_GAGE_TOOL,		SvStl::Err_15040_GageTool},
		{TAG_PROFILETOOL,			ITEM_PROFILE_TOOL,	SvStl::Err_15041_ProfileTool},
		{TAG_OCROCVANALYZER,		ITEM_FAST_OCR,		SvStl::Err_15042_FastOCR},
		{TAG_OCROCVGRAYANALYZER,	ITEM_FAST_OCR,		SvStl::Err_15042_FastOCR}
	};

	HRESULT hr = S_OK;
	rErrorCode = 0;
	if (!HasAcquisitionDevice(rTree, cameraItems, rItemType, rErrorCode))
	{
		if (!HasTaskObject(rTree, taskItems, rItemType, rErrorCode))
		{
			HasInvalidProductType(rTree, rItemType, rErrorCode);
		}
	}
	if (rErrorCode)
	{
		hr = CLASS_E_CLASSNOTAVAILABLE;
	}
	return hr;
}
} //namespace SvXml
