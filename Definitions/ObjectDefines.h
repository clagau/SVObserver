///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// This file defines defines for the objects. (located before in SVObjectLibrary)
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtobuf/SVO-Enum.h"
#pragma endregion Includes

#define SV_IS_KIND_OF( XP, XCLASSNAME )	( nullptr != dynamic_cast< XCLASSNAME* >( XP ) )

namespace SvDef
{
	enum SVMessageFlags
	{
		SVMFUnknown = 0x00000000,
		SVMFSetDefaultInputs = 0x00000001,
		SVMFResetObject = 0x00000002,
		SVMFResetInspection = 0x00000004,
	};

	//Combined object attributes
	const UINT defaultValueObjectAttributes =
	{
		SvPb::viewable |
		SvPb::publishable |
		SvPb::archivable |
		SvPb::selectableForEquation |
		SvPb::embedable |
		SvPb::selectableForStatistics |
		SvPb::printable |
		SvPb::dataDefinitionValue
	};

	const UINT selectableAttributes =
	{
		SvPb::viewable |
		SvPb::publishable |
		SvPb::archivable |
		SvPb::selectableForEquation |
		SvPb::selectableForStatistics
	};
} //namespace SvDef
