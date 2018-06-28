//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

namespace SvCmd
{
enum ObjectSelectorType
{
	toolsetItems,
	globalConstantItems,
	ppqItems
};

struct SelectorOptions
{
	SelectorOptions( const std::vector<ObjectSelectorType>& rItemTypes, const GUID& rInspectionID, UINT attributesFilter, const GUID& rInstanceID=GUID_NULL, bool wholeArray=false)
		: m_ItemTypes{rItemTypes}
		, m_rInspectionID{rInspectionID}
		, m_attributesFilter{attributesFilter}
		, m_rInstanceID{rInstanceID}
		, m_wholeArray{wholeArray}
	{};

	virtual ~SelectorOptions() {};

	std::vector<ObjectSelectorType> m_ItemTypes;	//List of item types to get
	const GUID&			m_rInspectionID;			//The inspection GUID
	const GUID&			m_rInstanceID;				//The instance GUID
	UINT				m_attributesFilter;			//The attributes filter
	bool				m_wholeArray;				//Insert whole arrays
};
} //namespace SvCmd
