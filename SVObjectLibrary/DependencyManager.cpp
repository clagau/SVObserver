//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file DependencyManager.cpp
//*****************************************************************************
/// Dependency manager using BGL
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DependencyManager.h"
#include "ObjectNameLookup.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOl
{
	#pragma region Constructor
	DependencyManager::DependencyManager() 
	{
	}

	DependencyManager::~DependencyManager() 
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	DependencyManager& DependencyManager::Instance()
	{
		static DependencyManager DependencyMgr;
		return DependencyMgr;
	}

	void DependencyManager::getToolDependency( SvOi::StringPairInserter Inserter, const SVGuidSet& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/, const std::string& rFileName /*= std::string()*/) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		std::vector<Dependency> DependencyVector;
		Dependencies ObjectDependencies;

		if( SvOi::ToolDependencyEnum::Client == (SvOi::ToolDependencyEnum::Client & ToolDependency) )
		{
			getChildDependents(rSourceSet, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType::Owner, SvOl::JoinType::Dependent, false);
		}
		if( SvOi::ToolDependencyEnum::Supplier == (SvOi::ToolDependencyEnum::Supplier & ToolDependency) ) 
		{
			// Here the pairs need to be reversed
			Dependencies ToolSuppliers;
			getChildDependents(rSourceSet, std::inserter(ToolSuppliers, ToolSuppliers.end()), SvOl::JoinType::Owner, SvOl::JoinType::Dependent, true);
			Dependencies::const_iterator SupplierIter(ToolSuppliers.begin());
			for (; ToolSuppliers.end() != SupplierIter; ++SupplierIter)
			{
				ObjectDependencies.insert( Dependency( SupplierIter->second, SupplierIter->first) );
			}
		}

		//! This filters dependencies which are dependent on the same tool
		std::copy_if(ObjectDependencies.begin(), ObjectDependencies.end(), std::back_inserter(DependencyVector), [&rSourceSet](const Dependency &rDependency)
		{
			bool CopyItem( false );
			//! Check if same Tool
			SVObjectClass* pSupplier = SVObjectManagerClass::Instance().GetObject( rDependency.first );
			SVObjectClass* pClient = SVObjectManagerClass::Instance().GetObject( rDependency.second );
			if (nullptr != pSupplier && nullptr != pClient)
			{
				//Basic value objects don't have tools check if main object is of type ToolObjectType
				bool isSupplier = pSupplier->GetObjectType() == SvPb::SVBasicValueObjectType || pSupplier->GetObjectType() == SvPb::SVToolObjectType;
				bool isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
				SVObjectClass* pParent = pSupplier->GetParent();
				bool isParentToolset = (nullptr != pParent) && (SvPb::SVToolSetObjectType == pParent->GetObjectType());
				SvPb::SVObjectTypeEnum supplierAncestorType = isParentToolset ? SvPb::SVToolSetObjectType	: SvPb::SVToolObjectType;
				SVObjectClass* pToolSupplier = isSupplier ? pSupplier : pSupplier->GetAncestor(supplierAncestorType);
				SVObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestor(SvPb::SVToolObjectType);
				if (nullptr != pToolSupplier && nullptr != pToolClient && pToolSupplier != pToolClient)
				{
					//One of the dependency tools must be in the source set
					if (rSourceSet.end() != rSourceSet.find(pToolSupplier->GetUniqueObjectID()) || rSourceSet.end() != rSourceSet.find(pToolClient->GetUniqueObjectID()))
					{
					CopyItem = true;
				}
			}
			}
			return CopyItem;
		});

		//! First sort the supplier then the clients
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(true));
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(false));

		ObjectDependencies.clear();
		std::vector<Dependency>::const_iterator IterDependency(DependencyVector.begin());
		for (; DependencyVector.end() != IterDependency; ++IterDependency)
		{
			SVObjectClass* pSupplier = SVObjectManagerClass::Instance().GetObject(IterDependency->first);
			SVObjectClass* pClient = SVObjectManagerClass::Instance().GetObject(IterDependency->second);
			if (nullptr != pSupplier && nullptr != pClient)
			{	
				SVObjectClass* pParent = pSupplier->GetParent();
				bool isParentToolset = (nullptr != pParent) && (SvPb::SVToolSetObjectType == pParent->GetObjectType());
				//To add also add the parent tool e.g. LoopTool if available
				std::string SupplierName = isParentToolset ? pSupplier->GetObjectNameToObjectType(SvPb::SVToolSetObjectType) : pSupplier->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				std::string ClientName = pClient->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				if (SvPb::SVToolObjectType != nameToObjectType)
				{
					SupplierName = isParentToolset ? pSupplier->GetObjectNameToObjectType(SvPb::SVToolSetObjectType) : pSupplier->GetObjectNameToObjectType(nameToObjectType);
					ClientName = pClient->GetObjectNameToObjectType(nameToObjectType);
				}

				Inserter = SvDef::StringPair(SupplierName, ClientName);

				// If the file name is not empty we want to save a tool dependency graph
				if (!rFileName.empty())
				{
					//Basic value objects don't have tools check if main object is of type ToolObjectType
					bool isSupplier = pSupplier->GetObjectType() == SvPb::SVBasicValueObjectType || pSupplier->GetObjectType() == SvPb::SVToolObjectType;
					bool isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
					SVObjectClass* pToolSupplier = isSupplier ? pSupplier : pSupplier->GetAncestor(SvPb::SVToolObjectType);
					SVObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestor(SvPb::SVToolObjectType);
					if (nullptr != pToolSupplier && nullptr != pToolClient && pToolSupplier != pToolClient)
					{
						ObjectDependencies.insert(Dependency(pToolSupplier->GetUniqueObjectID(), pToolClient->GetUniqueObjectID()));
					}
				}
			}
		}
		if (!rFileName.empty())
		{
			SvCl::ObjectGraph<SVGUID, SvOl::JoinType> OutputGraph(ObjectDependencies, SvOl::JoinType::Owner);
			SvOl::ObjectNameLookup NameLookup;
			OutputGraph.saveGraphDot(rFileName.c_str(), NameLookup);
		}
	}

	void DependencyManager::getToolDependency(SvOi::SvGuidInserter Inserter, const SVGuidSet& rSourceSet) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		Dependencies ObjectDependencies;

		getChildDependents(rSourceSet, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType::Owner, SvOl::JoinType::Dependent, false);

		for (auto const& rEntry : ObjectDependencies)
		{
			SVObjectClass* pClient = SVObjectManagerClass::Instance().GetObject(rEntry.second);
			if (nullptr != pClient)
			{
				SVObjectClass* pTool = (SvPb::SVToolObjectType == pClient->GetObjectType()) ? pClient : pClient->GetAncestor(SvPb::SVToolObjectType);
				if (nullptr != pTool)
				{
					const SVGUID& rToolID = pTool->GetUniqueObjectID();
					// If tool not in Source add it
					if (rSourceSet.end() == rSourceSet.find(rToolID))
					{
						Inserter = rToolID;
					}
				}
			}
		}
	}
	#pragma endregion Public Methods
} //namespace SvOl

#pragma region IDependencyManager
void SvOi::getToolDependency( StringPairInserter Inserter, const SVGuidSet& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/, const std::string& rFileName /*= std::string()*/)
{
	SvOl::DependencyManager::Instance().updateVertexIndex();
	SvOl::DependencyManager::Instance().getToolDependency(Inserter, rSourceSet, nameToObjectType, ToolDependency, rFileName);
}

void SvOi::getToolDependency(SvGuidInserter Inserter, const SVGuidSet& rSourceSet)
{
	SvOl::DependencyManager::Instance().updateVertexIndex();
	SvOl::DependencyManager::Instance().getToolDependency(Inserter, rSourceSet);
}
#pragma endregion IDependencyManager
