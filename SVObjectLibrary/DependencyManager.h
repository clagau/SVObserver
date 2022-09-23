//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file DependencyManager.h
//*****************************************************************************
/// Dependency manager using BGL
//******************************************************************************
#pragma once

#pragma region Includes
#include "JoinType.h"
#include "SVContainerLibrary/ObjectGraph.h"
#include "SVProtoBuf/InspectionCommands.h"
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

	SvPb::GetDependencyResponse getDependencyList(const std::set<uint32_t>& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvPb::ToolDependencyEnum ToolDependency = SvPb::ToolDependencyEnum::Client, bool allDependecies = false, LPCTSTR fileName = nullptr) const;

	SvPb::GetDependencyResponse getObjectDependency(const std::set<uint32_t>& rSourceSet, SvPb::ToolDependencyEnum ToolDependency = SvPb::ToolDependencyEnum::Client) const;
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
