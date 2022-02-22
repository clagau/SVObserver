//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file ObjectAttribute.h
/// This structure is used to set the object attributes during loading
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

struct ObjectAttribute
{
	uint32_t m_objectID{ 0UL };
	UINT m_Attribute{ 0 };
	std::vector<int> m_attributeIndex;
};
using ObjectAttributeList = std::vector<ObjectAttribute>;
using ObjectAttributeInserter = std::insert_iterator<ObjectAttributeList>;
