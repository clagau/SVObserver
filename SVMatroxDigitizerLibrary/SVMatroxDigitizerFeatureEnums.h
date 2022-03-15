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
#pragma endregion Includes

/**
@SVObjectName Matrox Digitizer Feature Enum

@SVObjectOverview This enum is used to get/set the Gige "features" of the Digitizer.

@SVObjectOperations None

*/

// Feature Type
enum SVFeatureTypeEnum
{
	SVTypeCommand,				// M_TYPE_COMMAND
	SVTypeInt32,				// M_TYPE_MIL_INT32
	SVTypeDouble,				// M_TYPE_DOUBLE
	SVTypeString,				// M_TYPE_STRING
	SVTypeStringEnumeration,	// M_TYPE_STRING_ENUMERATION
	SVTypeBool,					// M_TYPE_BOOLEAN
};

constexpr std::array<std::pair<SVFeatureTypeEnum, LPCTSTR>, 6> cFeatureTypeStringEnums =
{{
	{SVFeatureTypeEnum::SVTypeCommand, _T("SVTypeCommand")},
	{SVFeatureTypeEnum::SVTypeInt32, _T("SVTypeInt32")},
	{SVFeatureTypeEnum::SVTypeDouble, _T("SVTypeDouble")},
	{SVFeatureTypeEnum::SVTypeString, _T("SVTypeString")},
	{SVFeatureTypeEnum::SVTypeStringEnumeration, _T("SVTypeStringEnumeration")},
	{SVFeatureTypeEnum::SVTypeBool, _T("SVTypeBool")}
}};

constexpr std::array<std::pair<SVFeatureTypeEnum, long long>, 6> cFeatureTypeEnumConvertor = 
{{
	{SVFeatureTypeEnum::SVTypeCommand,	M_TYPE_COMMAND},
	{SVFeatureTypeEnum::SVTypeInt32, M_TYPE_MIL_INT32},
	{SVFeatureTypeEnum::SVTypeDouble, M_TYPE_DOUBLE},
	{SVFeatureTypeEnum::SVTypeString, M_TYPE_STRING},
	{SVFeatureTypeEnum::SVTypeStringEnumeration, M_TYPE_STRING},
	{SVFeatureTypeEnum::SVTypeBool,	M_TYPE_BOOLEAN}
}};
