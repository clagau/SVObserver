//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerFeatureEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerFeatureEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:32  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

/**
@SVObjectName Matrox Digitizer Feature Enum

@SVObjectOverview This enum is used to get/set the Gige "features" of the Digitizer.

@SVObjectOperations None

*/

struct SVMatroxDigitizerFeature
{
	// Feature Type
	enum SVFeatureTypeEnum
	{
		SVTypeCommand,				// M_TYPE_COMMAND
		SVTypeInt32,				// M_TYPE_MIL_INT32
		SVTypeDouble,				// M_TYPE_DOUBLE
		SVTypeString,				// M_TYPE_STRING
		SVTypeIntegerEnumeration,	// M_TYPE_INTEGER_ENUMERATION - by mil9, M_TYPE_MIL_INT32 by mil10, maybe can replaced SVTypeInt32 for MIL10
		SVTypeStringEnumeration,	// M_TYPE_STRING_ENUMERATION - by mil9, M_TYPE_STRING by mil10, maybe can replaced by SVTypeString for MIL10
		SVTypeBool,					// M_TYPE_BOOLEAN
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef std::vector<std::pair<SVFeatureTypeEnum, long long>> SVFeatureTypeEnumPair;
	static SVFeatureTypeEnumPair m_FeatureTypeEnumConvertor;
};

