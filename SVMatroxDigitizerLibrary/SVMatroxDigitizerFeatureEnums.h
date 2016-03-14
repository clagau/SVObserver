//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerFeatureEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerFeatureEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:32  $
//******************************************************************************

#ifndef SVMATROXDIGITIZERFEATUREENUMS_H
#define SVMATROXDIGITIZERFEATUREENUMS_H

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"

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
		SVTypeIntegerEnumeration,	// M_TYPE_INTEGER_ENUMERATION
		SVTypeStringEnumeration,	// M_TYPE_STRING_ENUMERATION
		SVTypeBool,					// M_TYPE_BOOLEAN
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVFeatureTypeEnum, long> SVFeatureTypeEnumMap;
	static SVFeatureTypeEnumMap m_FeatureTypeEnumConvertor;
};

#endif

