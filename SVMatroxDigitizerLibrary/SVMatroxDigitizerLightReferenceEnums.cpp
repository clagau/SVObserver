//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerLightReferenceEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerLightReferenceEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:29:22  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <mil.h>
#include "SVMatroxDigitizerLightReferenceEnums.h"

// Assign mappings for Light Reference Types
SVMatroxDigitizerLightReference::SVLightReferenceEnumMap SVMatroxDigitizerLightReference::m_LightReferenceEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerLightReference::SVBrightness,		M_BRIGHTNESS_REF)
(SVMatroxDigitizerLightReference::SVContrast,		M_CONTRAST_REF)
(SVMatroxDigitizerLightReference::SVWhiteBalanceU,	M_WHITE_BALANCE_U)
(SVMatroxDigitizerLightReference::SVWhiteBalanceV,	M_WHITE_BALANCE_V)
(SVMatroxDigitizerLightReference::SVSaturation,		M_SATURATION_REF)
(SVMatroxDigitizerLightReference::SVHue,			M_HUE_REF);

