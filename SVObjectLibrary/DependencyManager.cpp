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

	void DependencyManager::getToolDependency( SvOi::StringPairInserter Inserter, const SVGuidSet& rSourceSet, SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/ ) const
	{
		//! Note before calling this method the graph index must be updated this is done in the interface!
		SVGuidSet::const_iterator Iter( rSourceSet.begin() );
		for( ; rSourceSet.end() != Iter; ++Iter )
		{
			SVObjectClass* pSource = SVObjectManagerClass::Instance().GetObject( *Iter );
			if (nullptr != pSource)
			{
				Dependencies ObjectDependencies;

				if( SvOi::ToolDependencyEnum::Client == (SvOi::ToolDependencyEnum::Client & ToolDependency) )
				{
					getChildDependents( *Iter, std::inserter(ObjectDependencies, ObjectDependencies.end()), SvOl::JoinType::Owner, SvOl::JoinType::Dependent, false );
				}
				if( SvOi::ToolDependencyEnum::Supplier == (SvOi::ToolDependencyEnum::Supplier & ToolDependency) ) 
				{
					// Here the pairs need to be reversed
					Dependencies ToolSuppliers;
					getChildDependents( *Iter, std::inserter(ToolSuppliers, ToolSuppliers.end()), SvOl::JoinType::Owner, SvOl::JoinType::Dependent, true );
					Dependencies::const_iterator SupplierIter(ToolSuppliers.begin());
					for (; ToolSuppliers.end() != SupplierIter; ++SupplierIter)
					{
						ObjectDependencies.insert( Dependency( SupplierIter->second, SupplierIter->first) );
					}
				}


				std::vector<Dependency> DependencyVector;
				//! This filters dependencies which are dependent on the same tool
				std::copy_if(ObjectDependencies.begin(), ObjectDependencies.end(), std::back_inserter(DependencyVector), [](const Dependency &rDependency)
				{
					bool CopyItem( false );
					//! Check if same Tool
					SVObjectClass* pSupplier = SVObjectManagerClass::Instance().GetObject( rDependency.first );
					SVObjectClass* pClient = SVObjectManagerClass::Instance().GetObject( rDependency.second );
					if (nullptr != pSupplier && nullptr != pClient)
					{
						//Global constant objects don't have tools then use the Global constant object and check if main object is of type ToolObjectType
						bool isSupplier = pSupplier->GetObjectType() == SVGlobalConstantObjectType || pSupplier->GetObjectType() == SVToolObjectType;
						bool isClient = pClient->GetObjectType() == SVToolObjectType;
						SVObjectClass* pToolSupplier = isSupplier ? pSupplier : pSupplier->GetAncestor(SVToolObjectType);
						SVObjectClass* pToolClient = isClient ? pClient : pClient->GetAncestor(SVToolObjectType);
						if (nullptr != pToolSupplier && nullptr != pToolClient && pToolSupplier != pToolClient)
						{
							CopyItem = true;
						}
					}
					return CopyItem;
				});

				//! First sort the supplier then the clients
				std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(true));
				std::sort(DependencyVector.begin(), DependencyVector.end(), DependencySort(false));

				std::vector<Dependency>::const_iterator IterDependency(DependencyVector.begin());
				for( ; DependencyVector.end() != IterDependency; ++IterDependency )
				{
					SVObjectClass* pSupplier = SVObjectManagerClass::Instance().GetObject( IterDependency->first );
					SVObjectClass* pClient = SVObjectManagerClass::Instance().GetObject( IterDependency->second );
					if( nullptr != pSupplier && nullptr != pClient )
					{
						SVString SupplierName = pSupplier->GetCompleteObjectNameToObjectType( nullptr, nameToObjectType );
						SVString ClientName = pClient->GetCompleteObjectNameToObjectType( nullptr, nameToObjectType );

						Inserter = SVStringPair( SupplierName, ClientName );
					}
				}
			}
		}
	}
	#pragma endregion Public Methods
} //namespace SvOl

#pragma region IDependencyManager
void SvOi::getToolDependency( StringPairInserter Inserter, const SVGuidSet& rSourceSet, SVObjectTypeEnum nameToObjectType, SvOi::ToolDependencyEnum ToolDependency /*= SvOi::ToolDependencyEnum::Client*/ )
{
	SvOl::DependencyManager::Instance().updateVertexIndex();
	SvOl::DependencyManager::Instance().getToolDependency( Inserter, rSourceSet, nameToObjectType, ToolDependency );
}
#pragma endregion IDependencyManager
