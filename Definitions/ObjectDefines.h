///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// This file defines defines for the objects. (located before in SVObjectLibrary)
//******************************************************************************

#pragma once

#pragma region Includes
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

	////////////////////////////////////////////////////////////////////////////////
	//	Global Object Defines
	////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	//
	//
	// Output Object attributes - 19 Aug 1999 - frb.
	//
	const UINT SV_NO_ATTRIBUTES              = 0x00000000;  // default
	const UINT SV_VIEWABLE                   = 0x00000001;
	const UINT SV_PUBLISHABLE                = 0x00000002;
	const UINT SV_ARCHIVABLE                 = 0x00000004;
	const UINT SV_SELECTABLE_FOR_EQUATION    = 0x00000008;
	const UINT SV_EMBEDABLE                  = 0x00000010;
	const UINT SV_SELECTABLE_FOR_STATISTICS  = 0x00000020;
	const UINT SV_ARCHIVABLE_IMAGE           = 0x00000040;  // 11 Oct 1999 - frb.
	const UINT SV_PRINTABLE                  = 0x00000080;
	const UINT SV_REMOTELY_SETABLE           = 0x00000100;  // RY 2002 12 19
	const UINT SV_SETABLE_ONLINE             = 0x00000200;
	const UINT SV_EXTENT_OBJECT              = 0x00000400;
	const UINT SV_PUBLISH_RESULT_IMAGE       = 0x00001000;
	//Conditional History has been deprecated, these are used to check and reset the attributes of previously saved configurations
	const UINT SV_CH_CONDITIONAL             = 0x00002000;  // conditional history
	const UINT SV_CH_IMAGE                   = 0x00004000;  // conditional history
	const UINT SV_CH_VALUE                   = 0x00008000;  // conditional history 
	const UINT SV_DD_VALUE                   = 0x00010000;  // Selected for DataDefinition Value
	const UINT SV_DD_IMAGE                   = 0x00020000;  // Selected for DataDefinition Image

	const UINT SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES = 
		(
		SV_VIEWABLE
		| SV_PUBLISHABLE
		| SV_ARCHIVABLE
		| SV_SELECTABLE_FOR_EQUATION
		| SV_EMBEDABLE
		| SV_SELECTABLE_FOR_STATISTICS
		| SV_PRINTABLE
		| SV_DD_VALUE
		);

	const UINT SV_SELECTABLE_ATTRIBUTES  = (
		SV_VIEWABLE
		| SV_PUBLISHABLE
		| SV_ARCHIVABLE
		| SV_SELECTABLE_FOR_EQUATION
		| SV_SELECTABLE_FOR_STATISTICS
		);
} //namespace SvDef
