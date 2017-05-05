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
#include "ObjectInterfaces/ITool.h"
#include "SVObjectManagerClass.h"
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

	#pragma region Private Methods
	private:
	struct DependencySort
	{
		DependencySort(bool SortRight) : m_SortRight(SortRight) {};

		bool operator()(const Dependency &rLhs, const Dependency &rRhs)
		{
			bool isSmaller = false;
			SVGUID GuidLhs = m_SortRight ? rLhs.second : rLhs.first;
			SVGUID GuidRhs = m_SortRight ? rRhs.second : rRhs.first;
			SVObjectClass* pLhs = SVObjectManagerClass::Instance().GetObject(GuidLhs);
			SVObjectClass* pRhs = SVObjectManagerClass::Instance().GetObject(GuidRhs);
			if (nullptr != pLhs && nullptr != pRhs)
			{
				bool isSupplier = pLhs->GetObjectType() == SVToolObjectType;
				SvOi::ITool* pToolLhs = dynamic_cast<SvOi::ITool*> (isSupplier ? pLhs : pLhs->GetAncestor(SVToolObjectType));
				isSupplier = pRhs->GetObjectType() == SVToolObjectType;
				SvOi::ITool* pToolRhs = dynamic_cast<SvOi::ITool*> (isSupplier ? pRhs : pRhs->GetAncestor(SVToolObjectType));
				if (nullptr != pToolLhs && nullptr != pToolRhs)
				{
					long LhsPosition = pToolLhs->getToolPosition();
					long RhsPosition = pToolRhs->getToolPosition();
					if (-1 != LhsPosition && -1 != RhsPosition && LhsPosition < RhsPosition)
					{
						isSmaller = true;
					}
					else
					{
						isSmaller = false;
					}
				}
			}
			return isSmaller;
		}

	private:
		bool m_SortRight;
	};
	#pragma endregion Private Methods
	};
} /* namespace SVObjectLibrary */ } /* namespace Seidenader */

namespace SvOl = Seidenader::SVObjectLibrary;
