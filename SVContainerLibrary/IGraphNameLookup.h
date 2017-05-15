//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IGraphNameLookup.h
/// All Rights Reserved
//*****************************************************************************
/// This is an interface for the boost graph to lookup the names
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace SvCl
{
	class IGraphNameLookup
	{
	public:
		IGraphNameLookup() {}
		virtual ~IGraphNameLookup() {}

		//! Gets the name of the graph vertex
		//! \param rGuid [in] reference to the Graph vertex Guid
		//! \returns the name of the vertex
		virtual LPCTSTR getVertexName( const SVGUID& rGuid ) const = 0;
	};
} //namespace SvCl
