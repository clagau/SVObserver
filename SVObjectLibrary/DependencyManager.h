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
#include "SVContainerLibrary/ObjectGraph.h"
#pragma endregion Includes

namespace SvOi
{
class ITool;
}

namespace SvOl
{
	class DependencyManager : public SvCl::ObjectGraph<uint32_t, JoinType>
	{
	public:
		typedef SvCl::ObjectGraph<uint32_t, JoinType>::DependencySet Dependencies;
		typedef SvCl::ObjectGraph<uint32_t, JoinType>::DependencyInserter DependencyInserter;

	#pragma region Constructor
	public:
		DependencyManager();

		virtual ~DependencyManager();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
	static DependencyManager& Instance();

	//! Return the tool dependency of the given sources
	//! \param Inserter [in] Iterator to insert the named dependency pairs
	//! \param rSourceSet [in] reference to the source set of IDs
	//! \param nameToObjectType [in] Object type to define the dotted name start
	//! \param ToolDependency [in] The tool dependency types to get
	//! \param rFileName [in] Optional file name to save the graph file 
	void getToolDependency(SvOi::StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency = SvOi::ToolDependencyEnum::Client, const std::string& rFileName = std::string()) const;

	//! Return the tool dependency of the given sources
	//! \param Inserter [in] Iterator to insert the tool ID
	//! \param rSourceSet [in] reference to the source set of IDs
	void getToolDependency(SvOi::SvObjectIdSetInserter Inserter, const std::set<uint32_t>& rSourceSet) const;
#pragma endregion Public Methods

	#pragma region Private Methods
	private:
	struct DependencySort
	{
		explicit DependencySort(bool SortRight) : m_SortRight(SortRight) {};

		bool operator()(const Dependency &rLhs, const Dependency &rRhs);

	private:
		bool m_SortRight;
	};
	#pragma endregion Private Methods
	};
} //namespace SvOl
