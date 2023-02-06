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

	SvPb::GetDependencyResponse DependencyManager::getDependencyList(const std::set<uint32_t>& rSourceSet, SvPb::SVObjectTypeEnum nameToObjectType, SvPb::ToolDependencyEnum ToolDependency /*= SvPb::ToolDependencyEnum::Client*/, bool allDependecies /*false*/, LPCTSTR fileName /*= nullptr*/) const
	{	 
	
		//! Note: Before calling this method the graph index must be updated. This is done in the interface.
		std::vector<Dependency> DependencyVector;
		Dependencies ObjectDependencies;

		VertexSet childrenSet = getAllDependents(rSourceSet, SvOl::JoinType(SvOl::JoinType::Owner));

		if (SvPb::ToolDependencyEnum::Client == (SvPb::ToolDependencyEnum::Client & ToolDependency))
		{
			getDependents(childrenSet, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType(SvOl::JoinType::Dependent), false);
		}
		if (SvPb::ToolDependencyEnum::Supplier == (SvPb::ToolDependencyEnum::Supplier & ToolDependency))
		{
			// Here the pairs need to be reversed
			Dependencies ToolSuppliers;
			getDependents(childrenSet, std::inserter(ToolSuppliers, ToolSuppliers.end()), SvOl::JoinType(SvOl::JoinType::Dependent), true);
			for (const auto& rDependency : ToolSuppliers)
			{
				ObjectDependencies.insert({rDependency.second, rDependency.first});
			}
		}

		std::set<uint32_t> toolIdSet;
		for (auto& objectId : rSourceSet)
		{
			SvOi::IObjectClass* pObject = SvOi::getObject(objectId);
			SvOi::IObjectClass* parentTool = pObject->GetAncestorInterface(SvPb::SVToolObjectType);
			SvPb::SVObjectTypeEnum objectType = pObject->GetObjectType();
			
			if (nullptr != parentTool && objectType != SvPb::SVObjectTypeEnum::SVToolObjectType)
			{
				toolIdSet.insert(parentTool->getObjectId());
			}
			else
			{
				toolIdSet.insert(objectId);
			}
		}
		
		//! This filters dependencies which are dependent on the same tool
		std::copy_if(ObjectDependencies.begin(), ObjectDependencies.end(), std::back_inserter(DependencyVector), [&toolIdSet, allDependecies](const Dependency& rDependency)
		{
			//copies dependency when true is returned
			SvOi::IObjectClass* pSupplier = SvOi::getObject(rDependency.first);
			SvOi::IObjectClass* pClient = SvOi::getObject(rDependency.second);

			if (nullptr != pSupplier && nullptr != pClient)
			{
				//Basic value objects don't have tools check if main object is of type ToolObjectType
				bool isMainSupplier = pSupplier->GetObjectType() == SvPb::SVBasicValueObjectType || pSupplier->GetObjectType() == SvPb::SVToolObjectType;
				auto* pToolParent = pSupplier->GetAncestorInterface(SvPb::SVToolObjectType);
				bool isParentTool = (nullptr != pToolParent);
				SvPb::SVObjectTypeEnum supplierAncestorType = isParentTool ? SvPb::SVToolObjectType : SvPb::SVInspectionObjectType;
				SvOi::IObjectClass* pMainSupplier = pSupplier;
				if (false == isMainSupplier)
				{
					pMainSupplier = (isParentTool ? pToolParent : pSupplier->GetAncestorInterface(supplierAncestorType));
				}
				bool isToolClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
				SvOi::IObjectClass* pToolClient = isToolClient ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
				if (nullptr != pMainSupplier && nullptr != pToolClient)
				{
					std::set<uint32_t> supplierList;
					SvOi::IObjectClass* pParent {pMainSupplier};
					while (nullptr != pParent)
					{
						uint32_t objectID {pParent->getObjectId()};
						supplierList.insert(objectID);
						if (toolIdSet.end() != toolIdSet.find(objectID))
						{
							break;
						}
						pParent = pParent->GetAncestorInterface(SvPb::SVToolObjectType);
					}

					std::set<uint32_t> clientList;
					pParent = pToolClient;
					while (nullptr != pParent)
					{
						uint32_t objectID {pParent->getObjectId()};
						clientList.insert(objectID);
						if(toolIdSet.end() != toolIdSet.find(objectID))
						{
							break;
						}
						pParent = pParent->GetAncestorInterface(SvPb::SVToolObjectType);
					}
					std::vector<uint32_t> supplierInToolIdSet;
					std::set_intersection(supplierList.begin(), supplierList.end(), toolIdSet.begin(), toolIdSet.end(), std::back_inserter(supplierInToolIdSet));
					std::vector<uint32_t> clientInToolIdSet;
					std::set_intersection(clientList.begin(), clientList.end(), toolIdSet.begin(), toolIdSet.end(), std::back_inserter(clientInToolIdSet));

					bool isSupplierInToolIdSet = 0 < supplierInToolIdSet.size();
					bool isClientInToolIdSet = 0 < clientInToolIdSet.size();
					//If same Tool filter out directly
					if (allDependecies == false && isClientInToolIdSet && isSupplierInToolIdSet)
					{
						return false;
					}
					else if(isClientInToolIdSet || isSupplierInToolIdSet)
					{
						//Make sure client and supplier are not the same tool
						bool isNotSameTool = clientList.end() == clientList.find(pMainSupplier->getObjectId());
						if (isNotSameTool)
						{
							return true;
						}
					}
				}
			}
			return false;
		});

		//! First sort the supplier then the clients
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(true));
		std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(false));
		
		SvPb::GetDependencyResponse dependecyList;

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
				bool isParentInspection = (nullptr != pParent) && (SvPb::SVInspectionObjectType == pParent->GetObjectType());
				//To add also add the parent tool e.g. LoopTool if available
				std::string SupplierName = isParentToolset ? pSupplier->GetObjectNameToObjectType(SvPb::SVToolSetObjectType) : pSupplier->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				std::string ClientName = pClient->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
				if (SvPb::SVToolObjectType != nameToObjectType)
				{
					SupplierName = isParentToolset ? pSupplier->GetObjectNameToObjectType(SvPb::SVToolSetObjectType) : pSupplier->GetObjectNameToObjectType(nameToObjectType);
					ClientName = pClient->GetObjectNameToObjectType(nameToObjectType);
				}
				if (isParentInspection)
				{
					std::string inspectionName {pParent->GetName()};
					SvUl::searchAndReplace(SupplierName, inspectionName.c_str(), SvDef::FqnPPQVariables);
				}
				bool isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
				SvOi::IObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
				bool isClientInSourceSet {false};
				pParent = pToolClient;
				while (nullptr != pParent)
				{
					uint32_t objectID {pParent->getObjectId()};
					isClientInSourceSet = rSourceSet.end() != rSourceSet.find(objectID);
					if (isClientInSourceSet)
					{
						break;
					}
					pParent = pParent->GetAncestorInterface(SvPb::SVToolObjectType);
				}
				
				SvOi::IObjectClass* pTopToolSupplier = pSupplier->GetAncestorInterface(SvPb::SVToolObjectType, true);
				SvOi::IObjectClass* pTopToolClient = pClient->GetAncestorInterface(SvPb::SVToolObjectType, true);

				auto* pDependencyPair = dependecyList.add_dependencypair();
				pDependencyPair->mutable_supplier()->set_name(SupplierName);
				pDependencyPair->mutable_supplier()->set_objectid(IterDependency->first);
				if (nullptr != pTopToolSupplier)
				{
					pDependencyPair->mutable_supplier()->set_toolobjectid(pTopToolSupplier->getObjectId());
				}
				pDependencyPair->mutable_client()->set_name(ClientName);
				pDependencyPair->mutable_client()->set_objectid(IterDependency->second);
				if (nullptr != pTopToolClient)
				{
					pDependencyPair->mutable_client()->set_toolobjectid(pTopToolClient->getObjectId());
				}
				pDependencyPair->set_sourceisclient(isClientInSourceSet);


				// If the file name is not empty we want to save a tool dependency graph
				if (nullptr != fileName)
				{
					//Basic value objects don't have tools check if main object is of type ToolObjectType
					bool isSupplier = pSupplier->GetObjectType() == SvPb::SVBasicValueObjectType || pSupplier->GetObjectType() == SvPb::SVToolObjectType;
					isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
					SvOi::IObjectClass* pToolSupplier = isSupplier ? pSupplier : pSupplier->GetAncestorInterface(SvPb::SVToolObjectType);
					pToolClient = isClient ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
					if (nullptr != pToolSupplier && nullptr != pToolClient && pToolSupplier != pToolClient)
					{
						ObjectDependencies.insert({pToolSupplier->getObjectId(), pToolClient->getObjectId()});
					}
				}
			}
		}
		if (nullptr != fileName)
		{
			SvCl::ObjectGraph<uint32_t, SvOl::JoinType> OutputGraph(ObjectDependencies, SvOl::JoinType(SvOl::JoinType::Owner));
			SvOl::ObjectNameLookup NameLookup;
			OutputGraph.saveGraphDot(fileName, NameLookup);
		}

		return dependecyList;
	}

	SvPb::GetDependencyResponse DependencyManager::getObjectDependency(const std::set<uint32_t>& rSourceSet, SvPb::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		std::vector<Dependency> DependencyVector;
		Dependencies ObjectDependencies;

		VertexSet childrenSet = getAllDependents(rSourceSet, SvOl::JoinType(SvOl::JoinType::Owner));

		if (SvPb::ToolDependencyEnum::Client == (SvPb::ToolDependencyEnum::Client & ToolDependency))
		{
			getDependents(childrenSet, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType(SvOl::JoinType::Dependent), false);
		}
		if (SvPb::ToolDependencyEnum::Supplier == (SvPb::ToolDependencyEnum::Supplier & ToolDependency))
		{
			// Here the pairs need to be reversed
			Dependencies ToolSuppliers;
			getDependents(childrenSet, std::inserter(ToolSuppliers, ToolSuppliers.end()), SvOl::JoinType(SvOl::JoinType::Dependent), true);
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

		SvPb::GetDependencyResponse dependecyList;

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

				bool isClient = pClient->GetObjectType() == SvPb::SVToolObjectType;
				SvOi::IObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestorInterface(SvPb::SVToolObjectType);
				SvOi::IObjectClass* pTopToolClient = pClient->GetAncestorInterface(SvPb::SVToolObjectType, true);
				bool isClientInSourceSet = (pToolClient != nullptr) ? rSourceSet.end() != rSourceSet.find(pToolClient->getObjectId()) : false;
				isClientInSourceSet |= (pTopToolClient != nullptr) ? rSourceSet.end() != rSourceSet.find(pTopToolClient->getObjectId()) : false;

				SvOi::IObjectClass* pTopToolSupplier = pSupplier->GetAncestorInterface(SvPb::SVToolObjectType, true);

				auto* pDependencyPair = dependecyList.add_dependencypair();
				pDependencyPair->mutable_supplier()->set_name(SupplierName);
				pDependencyPair->mutable_supplier()->set_objectid(IterDependency->first);
				if (nullptr != pTopToolSupplier)
				{
					pDependencyPair->mutable_supplier()->set_toolobjectid(pTopToolSupplier->getObjectId());
				}
				pDependencyPair->mutable_client()->set_name(ClientName);
				pDependencyPair->mutable_client()->set_objectid(IterDependency->second);
				if (nullptr != pTopToolClient)
				{
					pDependencyPair->mutable_client()->set_toolobjectid(pTopToolClient->getObjectId());
				}
				

				pDependencyPair->set_sourceisclient(isClientInSourceSet);
			}
		}

		return dependecyList;
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