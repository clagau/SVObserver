// ******************************************************************************
// * COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageDefinitionStruct
// * .File Name       : $Workfile:   SVImageDefinitionStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:11:12  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <comdef.h>
#include "Definitions/SVImageFormatEnum.h"

#pragma pack (push, 1)
struct SVImageDefinitionStruct
{
	BSTR bstrDisplayName;
	long lHeight;
	long lWidth;
	SvDef::SVImageFormatEnum eImageFormat;
public:
	SVImageDefinitionStruct( );
	~SVImageDefinitionStruct( );
	SVImageDefinitionStruct(const SVImageDefinitionStruct& rhs );
	const SVImageDefinitionStruct& operator = (const SVImageDefinitionStruct& rhs );
private:
	void Clear();
	void Init();
};


#pragma pack (pop)

typedef std::vector<SVImageDefinitionStruct> SVImageDefinitionStructArray;

