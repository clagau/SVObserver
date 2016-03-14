//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerFeatureEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerFeatureEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:26  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxDigitizerFeatureEnums.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <mil.h>

// Assign mappings for Feature Types
SVMatroxDigitizerFeature::SVFeatureTypeEnumMap SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerFeature::SVTypeCommand,				M_TYPE_COMMAND)
(SVMatroxDigitizerFeature::SVTypeInt32,					M_TYPE_MIL_INT32)
(SVMatroxDigitizerFeature::SVTypeDouble,				M_TYPE_DOUBLE)
(SVMatroxDigitizerFeature::SVTypeString,				M_TYPE_STRING)
(SVMatroxDigitizerFeature::SVTypeIntegerEnumeration,	M_TYPE_INTEGER_ENUMERATION)
(SVMatroxDigitizerFeature::SVTypeStringEnumeration,		M_TYPE_STRING_ENUMERATION)
(SVMatroxDigitizerFeature::SVTypeBool,					M_TYPE_BOOLEAN);

