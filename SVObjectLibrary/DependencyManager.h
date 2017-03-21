//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file DependencyManager.h
//*****************************************************************************
/// Dependency manager using BGL
//******************************************************************************
#pragma once

#pragma region Includes
#include "JoinType.h"
#include "ObjectInterfaces/IDependencyManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVContainerLibrary/ObjectGraph.h"
#pragma endregion Includes

namespace Seidenader { namespace SVObjectLibrary
{
	class DependencyManager : public SvCl::ObjectGraph<SVGUID, JoinType>
	{
	public:
		typedef SvCl::ObjectGraph<SVGUID, JoinType>::Dependencies Dependencies;
		typedef SvCl::ObjectGraph<SVGUID, JoinType>::DependencyInserter DependencyInserter;

	#pragma region Constructor
	public:
		DependencyManager();

		~DependencyManager();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
	static DependencyManager& Instance();

	//! Return the tool dependency of the given sources
	//! \param Inserter [in] Iterator to insert the named dependency pairs
	//! \param rSourceSet [in] reference to the source set of GUIDs
	//! \param nameToObjectType [in] Object type to define the dotted name start
	//! \param ToolDependency [in] The tool dependency types to get
	void getToolDependency( SvOi::StringPairInserter Inserter, const SVGuidSet& rSourceSet, SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency = SvOi::ToolDependencyEnum::Client ) const;

	#pragma endregion Public Methods
	};
} /* namespace SVObjectLibrary */ } /* namespace Seidenader */

namespace SvOl = Seidenader::SVObjectLibrary;
