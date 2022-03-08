//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file DependencyManager.cpp
//*****************************************************************************
/// Dependency manager using BGL
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DependencyManager.h"
#include "ObjectNameLookup.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ITool.h"
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

	void DependencyManager::getToolDependency( SvOi::StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/, LPCTSTR fileName /*= nullptr*/) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		std::vector<Dependency> DependencyVector;
		Dependencies ObjectDependencies;

		VertexSet childrenSet = getAllDependents(rSourceSet, SvOl::JoinType::Owner);

		if( SvOi::ToolDependencyEnum::Client == (SvOi::ToolDependencyEnum::Client & ToolDependency) )
		{
			getDependents(childrenSet, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType::Dependent, false);
		}
		if( SvOi::ToolDependencyEnum::Supplier == (SvOi::ToolDependencyEnum::Supplier & ToolDependency) ) 
		{
			// Here the pairs need to be reversed
			Dependencies ToolSuppliers;
			getDependents(childrenSet, std::inserter(ToolSuppliers, ToolSuppliers.end()), SvOl::JoinType::Dependent, true);
			for (const auto& rDependency :ToolSuppliers)
			{
				ObjectDependencies.insert({rDependency.second, rDependency.first});
			}
		}

		//! This filters dependencies which are dependent on the same tool
		std::copy_if(ObjectDependencies.begin(), ObjectDependencies.end(), std::back_inserter(DependencyVector), [&rSourceSet](const Dependency& rDependency)
		{
			//copies dependency when true is returned
			SvOi::IObjectClass* pSupplier = SvOi::getObject(rDependency.first);
			SvOi::IObjectClass* pClient = SvOi::getObject(rDependency.second);
			if (nullptr != pSupplier && nullptr != pClient)
			{
				//Basic value objects don't have tools check if main object is of type ToolObjectType
				bool isSupplier = pSupplier->GetObjectType() == SvPb::SVBasicValueObjectType || pSupplier->GetObjectType() == SvPb::SVToolObjectType;
				bool isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
				auto* pToolParent = pSupplier->GetAncestorInterface(SvPb::SVToolObjectType);
				bool isParentTool = (nullptr != pToolParent);
				SvPb::SVObjectTypeEnum supplierAncestorType = isParentTool ? SvPb::SVToolObjectType : SvPb::SVToolSetObjectType;
				SvOi::IObjectClass* pToolSupplier = pSupplier;
				if (false == isSupplier)
				{
					pToolSupplier = (isParentTool ? pToolParent : pSupplier->GetAncestorInterface(supplierAncestorType));
				}
				SvOi::IObjectClass* pTopToolSupplier = isParentTool ? pSupplier->GetAncestorInterface(SvPb::SVToolObjectType, true) : nullptr;
				SvOi::IObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
				SvOi::IObjectClass* pTopToolClient = pClient->GetAncestorInterface(SvPb::SVToolObjectType, true);
				if (nullptr != pToolSupplier && nullptr != pToolClient)
				{
					//If same Tool filter out directly
					if (pToolSupplier == pToolClient || pTopToolSupplier == pToolClient)
					{
						return false;
					}
					else
					{
						//One of the dependency tools must be in the source set
						bool isNotParentSupplier = (rSourceSet.end() != rSourceSet.find(pToolSupplier->getObjectId()) && pToolSupplier != pTopToolClient);
						if (isNotParentSupplier || rSourceSet.end() != rSourceSet.find(pToolClient->getObjectId()))
						{
							return true;
						}
					}
				}
				//This adds the dependency when a loop or group tool is the parent
				pToolSupplier = pSupplier->GetAncestorInterface(supplierAncestorType, true);
				pToolClient =pClient->GetAncestorInterface(SvPb::SVToolObjectType, true);
				if (nullptr != pToolSupplier && nullptr != pToolClient && pToolSupplier != pToolClient)
				{
					//One of the dependency tools must be in the source set
					if (rSourceSet.end() != rSourceSet.find(pToolSupplier->getObjectId()) || rSourceSet.end() != rSourceSet.find(pToolClient->getObjectId()))
					{
						return true;
					}
				}
			}
			return false;
		});

		//! First sort the supplier then the clients
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(true));
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(false));

		ObjectDependencies.clear();
		std::vector<Dependency>::const_iterator IterDependency(DependencyVector.begin());
		for (; DependencyVector.end() != IterDependency; ++IterDependency)
		{
			SvOi::IObjectClass* pSupplier = SvOi::getObject(IterDependency->first);
			SvOi::IObjectClass* pClient = SvOi::getObject(IterDependency->second);
			if (nullptr != pSupplier && nullptr != pClient)
			{	
				SvOi::IObjectClass* pParent = SvOi::getObject(pSupplier->GetParentID());
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
				if (nullptr != fileName)
				{
					//Basic value objects don't have tools check if main object is of type ToolObjectType
					bool isSupplier = pSupplier->GetObjectType() == SvPb::SVBasicValueObjectType || pSupplier->GetObjectType() == SvPb::SVToolObjectType;
					bool isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
					SvOi::IObjectClass* pToolSupplier = isSupplier ? pSupplier : pSupplier->GetAncestorInterface(SvPb::SVToolObjectType);
					SvOi::IObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
					if (nullptr != pToolSupplier && nullptr != pToolClient && pToolSupplier != pToolClient)
					{
						ObjectDependencies.insert({pToolSupplier->getObjectId(), pToolClient->getObjectId()});
					}
				}
			}
		}
		if (nullptr != fileName)
		{
			SvCl::ObjectGraph<uint32_t, SvOl::JoinType> OutputGraph(ObjectDependencies, SvOl::JoinType::Owner);
			SvOl::ObjectNameLookup NameLookup;
			OutputGraph.saveGraphDot(fileName, NameLookup);
		}
	}

	void DependencyManager::getToolDependency(SvOi::SvObjectIdSetInserter Inserter, const std::set<uint32_t>& rSourceSet) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		Dependencies ObjectDependencies;

		VertexSet childrenSet = getAllDependents(rSourceSet, SvOl::JoinType::Owner);
		getDependents(childrenSet, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType::Dependent, false);

		for (auto const& rEntry : ObjectDependencies)
		{
			SvOi::IObjectClass* pClient = SVObjectManagerClass::Instance().GetObject(rEntry.second);
			if (nullptr != pClient)
			{
				SvOi::IObjectClass* pTool = (SvPb::SVToolObjectType == pClient->GetObjectType()) ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
				if (nullptr != pTool)
				{
					// If tool not in Source add it
					if (rSourceSet.end() == rSourceSet.find(pTool->getObjectId()))
					{
						Inserter = pTool->getObjectId();
					}
				}
			}
		}
	}

	void DependencyManager::getObjectDependency(SvOi::StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		std::vector<Dependency> DependencyVector;
		Dependencies ObjectDependencies;

		VertexSet childrenSet = getAllDependents(rSourceSet, SvOl::JoinType::Owner);

		if (SvOi::ToolDependencyEnum::Client == (SvOi::ToolDependencyEnum::Client & ToolDependency))
		{
			getDependents(childrenSet, std::inserter(ObjectDependencies, ObjectDependencies.end()),SvOl::JoinType::Dependent, false);
		}
		if (SvOi::ToolDependencyEnum::Supplier == (SvOi::ToolDependencyEnum::Supplier & ToolDependency))
		{
			// Here the pairs need to be reversed
			Dependencies ToolSuppliers;
			getDependents(childrenSet, std::inserter(ToolSuppliers, ToolSuppliers.end()), SvOl::JoinType::Dependent, true);
			for (const auto& rDependency : ToolSuppliers)
			{
				ObjectDependencies.insert({rDependency.second, rDependency.first});
			}
		}

		//! This filters dependencies which are dependent on the same tool
		std::copy(ObjectDependencies.begin(), ObjectDependencies.end(), std::back_inserter(DependencyVector));

			//! First sort the supplier then the clients
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(true));
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(false));

		ObjectDependencies.clear();
		std::vector<Dependency>::const_iterator IterDependency(DependencyVector.begin());
		for (; DependencyVector.end() != IterDependency; ++IterDependency)
		{
			SvOi::IObjectClass* pSupplier = SvOi::getObject(IterDependency->first);
			SvOi::IObjectClass* pClient = SvOi::getObject(IterDependency->second);
			if (nullptr != pSupplier && nullptr != pClient)
			{
				SvOi::IObjectClass* pParent = SvOi::getObject(pSupplier->GetParentID());
				bool isParentToolset = (nullptr != pParent) && (SvPb::SVToolSetObjectType == pParent->GetObjectType());
				//To add also add the parent tool e.g. LoopTool if available
				std::string SupplierName = isParentToolset ? pSupplier->GetObjectNameToObjectType(SvPb::SVToolSetObjectType) : pSupplier->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				std::string ClientName = pClient->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);

				// cppcheck-suppress unreadVariable symbolName=Inserter ; cppCheck doesn't know back_insert_iterator
				Inserter = SvDef::StringPair(SupplierName, ClientName);
			}
		}
	}
	#pragma endregion Public Methods

	bool DependencyManager::DependencySort::operator()(const Dependency &rLhs, const Dependency &rRhs)
	{
		bool isSmaller = false;
		uint32_t idLhs = m_SortRight ? rLhs.second : rLhs.first;
		uint32_t idRhs = m_SortRight ? rRhs.second : rRhs.first;
		SvOi::IObjectClass* pLhs = SvOi::getObject(idLhs);
		SvOi::IObjectClass* pRhs = SvOi::getObject(idRhs);
		if (nullptr != pLhs && nullptr != pRhs)
		{
			bool isSupplier = pLhs->GetObjectType() == SvPb::SVToolObjectType;
			SvOi::ITool* pToolLhs = dynamic_cast<SvOi::ITool*> (isSupplier ? pLhs : pLhs->GetAncestorInterface(SvPb::SVToolObjectType));
			isSupplier = pRhs->GetObjectType() == SvPb::SVToolObjectType;
			SvOi::ITool* pToolRhs = dynamic_cast<SvOi::ITool*> (isSupplier ? pRhs : pRhs->GetAncestorInterface(SvPb::SVToolObjectType));
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
} //namespace SvOl

#pragma region IDependencyManager
void SvOi::getToolDependency( StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/, LPCTSTR fileName /*= nullptr*/)
{
	SvOl::DependencyManager::Instance().updateVertexIndex();
	SvOl::DependencyManager::Instance().getToolDependency(Inserter, rSourceSet, nameToObjectType, ToolDependency, fileName);
}

void SvOi::getToolDependency(SvObjectIdSetInserter Inserter, const std::set<uint32_t>& rSourceSet)
{
	SvOl::DependencyManager::Instance().updateVertexIndex();
	SvOl::DependencyManager::Instance().getToolDependency(Inserter, rSourceSet);
}

void SvOi::getObjectDependency(SvOi::StringPairInserter Inserter, const std::set<uint32_t>& rSourceSet, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/)
{
	SvOl::DependencyManager::Instance().updateVertexIndex();
	SvOl::DependencyManager::Instance().getObjectDependency(Inserter, rSourceSet, ToolDependency);
}
#pragma endregion IDependencyManager


