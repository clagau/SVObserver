// ******************************************************************************
// * COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataDefinitionStruct
// * .File Name       : $Workfile:   SVDataDefinitionStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   04 Nov 2013 14:35:26  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <string>
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma pack (push, 1)
struct SVDataDefinitionStruct
{
	std::string m_Name;
	bool m_Writable;
	std::string m_Type;
	SvDef::StringVector m_AdditionalInfo;
public:
	SVDataDefinitionStruct( );
	~SVDataDefinitionStruct( );
	SVDataDefinitionStruct(const SVDataDefinitionStruct& rhs );
	const SVDataDefinitionStruct& operator = (const SVDataDefinitionStruct& rhs );
	bool operator == (const SVDataDefinitionStruct& rhs);
private:
	void Clear();
	void Init();
};
#pragma pack (pop)

typedef std::vector<SVDataDefinitionStruct> SVDataDefinitionStructVector;

