///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader, Markt Schwaben
/// All Rights Reserved
///*****************************************************************************
/// IDependencyManager is the interface to obtain dependencies
///******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <iterator>
#include "SVObjectTypeInfoStruct.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

namespace Seidenader { namespace ObjectInterfaces
{
	typedef std::back_insert_iterator<StringPairVector> StringPairInserter;

	enum ToolDependencyEnum
	{
		Client = 0x01,
		Supplier = 0x02,
		ClientAndSupplier = 0x03,
	};

	//! Return the dependency list of the given sources
	//! \param Inserter [in] Iterator to insert the named dependency pairs
	//! \param rSourceSet [in] reference to the source set of GUIDs
	//! \param nameToObjectType [in] Object type to define the dotted name start
	void getToolDependency( StringPairInserter Inserter, const SVGuidSet& rSourceSet, SVObjectTypeEnum nameToObjectType, ToolDependencyEnum ToolDependency = Client );

} /* namespace ObjectInterfaces */ } /* namespace Seidenader */

namespace SvOi = Seidenader::ObjectInterfaces;