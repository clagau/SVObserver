///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader, Markt Schwaben
/// All Rights Reserved
///*****************************************************************************
/// IDependencyManager is the interface to obtain dependencies
///******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <iterator>
#include "SVProtoBuf/SVO-Enum.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOi
{
	typedef std::back_insert_iterator<SvDef::StringPairVector> StringPairInserter;
	typedef std::insert_iterator<std::set<uint32_t>> SvObjectIdSetInserter;

	enum ToolDependencyEnum
	{
		Client = 0x01,
		Supplier = 0x02,
		ClientAndSupplier = 0x03,
	};

	//! Return the dependency list of the given sources
	//! \param Inserter [in] Iterator to insert the named dependency pairs
	//! \param rSourceSet [in] reference to the source set of IDs
	//! \param nameToObjectType [in] Object type to define the dotted name start
	//! \param rFileName [in] Optional file name to save the graph file 
	void getToolDependency( StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, ToolDependencyEnum ToolDependency = Client, const std::string& rFileName = std::string());

	//! Return the tool dependency of the given sources
	//! \param Inserter [in] Iterator to insert the tool ID
	//! \param rSourceSet [in] reference to the source set of IDs
	void getToolDependency(SvOi::SvObjectIdSetInserter Inserter, const std::set<uint32_t>& rSourceSet);

	void getObjectDependency(SvOi::StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvOi::ToolDependencyEnum ToolDependency = SvOi::ToolDependencyEnum::Client);
} //namespace SvOi