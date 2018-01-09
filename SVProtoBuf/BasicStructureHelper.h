//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file BasicStructureHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtoBuf\BasicStructure.pb.h"
#pragma warning( pop )
#pragma endregion Includes

namespace SvPB
{
	/// Set a SVGUID to the protobuf-structure.
	/// \param rSVGuidID [in] The guid.
	/// \returns SvCmd::UUID The new protobuf-structure.
	SvPB::UUID setGuidToMessage(const GUID& rSVGuidID);
	/// Convert a guid in protobuf-structure to a SVGUID.
	/// \param rUUId [in] The protobuf-structure
	/// \returns SVGUID The guid.
	GUID getGuidFromMessage(const SvPB::UUID& rUUId);
} //namespace SvPB