///******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// This file defines defines for the objects. (located before in SVObjectLibrary)
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

#define SV_IS_KIND_OF( XP, XCLASSNAME )	( nullptr != dynamic_cast< XCLASSNAME* >( XP ) )

namespace SvDef
{
	constexpr uint32_t InvalidObjectId = 0;

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
		SvPb::audittrail |
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

	constexpr int c_maxTableColumn = 50;
	constexpr int c_numberOfSortFeature = 3;
	constexpr int c_maxLoopGroupDepth = 3;
} //namespace SvDef
