//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLMaterialsTree
//* .File Name       : $Workfile:   SVXMLMaterialsTree.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:46:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvXml
{
	#pragma region Constructor
	SVXMLMaterialsTree::SVXMLMaterialsTree() :
	 m_Tree()
	,m_pCurrentParent( nullptr )
	,m_pCurrentLeaf( m_Tree.end() )
	{
	}

	SVXMLMaterialsTree::SVXMLMaterialsTree(  const SVMaterialsTree::SVTreeContainer& rTree ) :
	 m_Tree( rTree )
	,m_pCurrentParent( nullptr )
	,m_pCurrentLeaf( m_Tree.end() )
	{
	}

	SVXMLMaterialsTree::~SVXMLMaterialsTree()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	size_t SVXMLMaterialsTree::getCount() const
	{
		return SVMaterialsTree::getCount( m_Tree );
	}

	SVXMLMaterialsTree::SVBranchHandle SVXMLMaterialsTree::getRoot()
	{
		SVBranchHandle pResult( &m_Tree );

		return pResult;
	}

	bool SVXMLMaterialsTree::isRoot( const SVBranchHandle pBranch ) const
	{
		bool Result( false );

		if( nullptr != pBranch )
		{
			Result = pBranch->is_root();
		}

		return Result;
	}

	bool SVXMLMaterialsTree::hasBranches( const SVBranchHandle pParent )
	{
		bool Result( false );

		SVBranchHandle  pBranch( pParent );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		if( nullptr != pBranch )
		{
			if( setChildren( pBranch ) && 0 < m_ChildBranches.size() )
			{
				Result = true;
			}
		}

		return Result;
	}

	SVXMLMaterialsTree::SVBranchHandle SVXMLMaterialsTree::getParentBranch( const SVBranchHandle pChild )
	{
		SVBranchHandle pResult( nullptr );

		if( nullptr != pChild )
		{
			pResult = pChild->parent();
		}

		return pResult;
	}

	SVXMLMaterialsTree::SVBranchHandle SVXMLMaterialsTree::getFirstBranch( const SVBranchHandle pParent )
	{
		SVBranchHandle pResult( nullptr );

		SVBranchHandle pBranch( pParent );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		m_pCurrentLeaf = m_Tree.end();
		if( setChildren( pBranch ) )
		{
			SVBranches::iterator Iter( m_ChildBranches.begin() );
			if( m_ChildBranches.end() != Iter )
			{
				pResult = *Iter;
			}
		}

		return pResult;
	}

	SVXMLMaterialsTree::SVBranchHandle SVXMLMaterialsTree::getNextBranch( const SVBranchHandle pParent, const SVBranchHandle pChild )
	{
		SVBranchHandle pResult( nullptr );

		SVBranchHandle pBranch( pParent );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		m_pCurrentLeaf = m_Tree.end();
		if( setChildren( pBranch ) )
		{
			for( SVBranches::iterator Iter( m_ChildBranches.begin() ); nullptr == pResult && m_ChildBranches.end() != Iter; ++Iter )
			{
				//Find the current child branch
				if( pChild == *Iter )
				{
					++Iter;
					//Check if the next branch child is valid
					if( m_ChildBranches.end() != Iter )
					{
						pResult = *Iter;
					}
					else
					{
						//exit the loop no further branches found
						break;
					}
				}
			}
		}

		return pResult;
	}

	SVXMLMaterialsTree::SVBranchHandle SVXMLMaterialsTree::getPreviousBranch(const SVBranchHandle pParent, const SVBranchHandle pChild)
	{
		SVBranchHandle pBranch(pParent);
		if (nullptr == pBranch)
		{
			pBranch = getRoot();
		}

		m_pCurrentLeaf = m_Tree.end();
		if (setChildren(pBranch))
		{
			auto iter = std::find(m_ChildBranches.begin(), m_ChildBranches.end(), pChild);
			if (m_ChildBranches.end() != iter && m_ChildBranches.begin() != iter)
			{
				--iter;
				return *iter;
			}
		}

		return nullptr;
	}

	SVXMLMaterialsTree::SVBranchHandle SVXMLMaterialsTree::findBranch(const SVBranchHandle pParent, LPCTSTR Name)
	{
		SVBranchHandle pResult(nullptr);

		SVBranchHandle pBranch(pParent);

		if (nullptr == pBranch)
		{
			pBranch = getRoot();
		}

		m_pCurrentLeaf = m_Tree.end();
		if (setChildren(pBranch))
		{
			for (SVBranches::iterator Iter(m_ChildBranches.begin()); nullptr == pResult && m_ChildBranches.end() != Iter; ++Iter)
			{
				//Find the current child branch
				if ((*Iter)->get()->first == Name)
				{
					pResult = *Iter;
				}
			}
		}

		return pResult;
	}


	// cppcheck-suppress unusedFunction
	std::vector<SVXMLMaterialsTree::SVBranchHandle> SVXMLMaterialsTree::getSubbranches(const SVBranchHandle pParent)
	{
		SVBranchHandle pBranch(pParent);

		if (nullptr == pBranch)
		{
			pBranch = getRoot();
		}

		m_pCurrentLeaf = m_Tree.end();

		if (setChildren(pBranch))
		{
			return m_ChildBranches;
		}

		return {};
	}

	bool SVXMLMaterialsTree::isValidBranch( const SVBranchHandle pParent )
	{
		return (nullptr != pParent);
	}

	HRESULT SVXMLMaterialsTree::createBranch( const SVBranchHandle pParent, LPCTSTR Name, SVBranchHandle* ppBranch )
	{
		HRESULT Result( S_OK );

		SVBranchHandle pBranch( pParent );
		SVBranchHandle pNewBranch( nullptr );
		std::string BranchName( Name );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		if( nullptr != pBranch )
		{
			//When creating a branch the data pointer is set to nullptr
			SVMaterialsTree::iterator Iter = pBranch->insert( SVMaterialsTree::SVTreeElement( BranchName, SVMaterialDataPtr(nullptr) ) );
			if( pBranch->end() != Iter )
			{
				pNewBranch = Iter.node();

				if( nullptr == pNewBranch )
				{
					Result = E_FAIL;
				}
			}
		}
		else
		{
			Result = E_FAIL;
		}
	
		if( nullptr != ppBranch )
		{
			*ppBranch = pNewBranch;
		}

		return Result;
	}

	HRESULT SVXMLMaterialsTree::deleteBranch( SVBranchHandle& rpBranch )
	{
		HRESULT Result( S_OK );

		if( nullptr != rpBranch )
		{
			if( isRoot( rpBranch ) )
			{
				m_Tree.clear();
			}
			else
			{
				SVBranchHandle pParent( rpBranch->parent() );

				if( nullptr != pParent )
				{
					SVMaterialsTree::iterator Iter( pParent->begin() );

					for( ; Iter != pParent->end(); ++Iter )
					{
						if( Iter.node() == rpBranch )
						{
							pParent->erase( Iter );
							break;
						}
					}
				}
			}

			rpBranch = nullptr;
		}

		return Result;
	}

	std::string SVXMLMaterialsTree::getBranchName( const SVBranchHandle pBranch ) const
	{
		std::string Result;

		if( nullptr != pBranch )
		{
			if( isRoot( pBranch ) )
			{
				Result = _T( "Base" );
			}
			else
			{
				const SVMaterialsTree::SVTreeElement* pElement( pBranch->get() );

				if( nullptr != pElement )
				{
					Result = pElement->first;
				}
			}
		}

		return Result;
	}

	bool SVXMLMaterialsTree::hasLeaves( const SVBranchHandle pBranch )
	{
		bool Result( false );

		SVBranchHandle pParent = pBranch;

		if( nullptr == pParent )
		{
			pParent = getRoot();
		}

		if( nullptr != pParent )
		{
			if( setChildren( pParent ) && 0 < m_ChildLeaves.size() )
			{
				Result = true;
			}
		}

		return Result;
	}

	SVXMLMaterialsTree::SVLeafHandle SVXMLMaterialsTree::getFirstLeaf( const SVBranchHandle pParent )
	{
		SVLeafHandle pResult( m_Tree.end() );

		SVBranchHandle pBranch( pParent );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		if( nullptr != pBranch )
		{
			m_pCurrentLeaf = m_Tree.end();
			if( setChildren( pBranch ) )
			{
				SVLeaves::iterator Iter( m_ChildLeaves.begin() );
				if( m_ChildLeaves.end() != Iter )
				{
					pResult = *Iter;
				}
			}
		}

		return pResult;
	}

	SVXMLMaterialsTree::SVLeafHandle SVXMLMaterialsTree::getNextLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf )
	{
		SVLeafHandle pResult( m_Tree.end() );

		SVBranchHandle pBranch( pParent );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		if( nullptr != pBranch )
		{
			m_pCurrentLeaf = m_Tree.end();
			if( setChildren( pBranch ) )
			{
				for( SVLeaves::iterator Iter( m_ChildLeaves.begin() ); m_ChildLeaves.end() != Iter; ++Iter )
				{
					//Find the current child branch
					if( pLeaf == *Iter )
					{
						++Iter;
						//Check if the next branch child is valid
						if( m_ChildLeaves.end() != Iter )
						{
							m_pCurrentLeaf = *Iter;
							pResult = *Iter;
						}
						else
						{
							//exit the loop no further leaves found
							break;
						}
					}
				}
			}
		}

		return pResult;
	}

	SVXMLMaterialsTree::SVLeafHandle SVXMLMaterialsTree::findLeaf( const SVBranchHandle pParent, LPCTSTR Name )
	{
		SVLeafHandle pResult( m_Tree.end() );

		SVBranchHandle pBranch( pParent );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		if( nullptr != pBranch )
		{
			m_pCurrentLeaf = m_Tree.end();
			if( setChildren( pBranch ) )
			{
				for( SVLeaves::iterator Iter( m_ChildLeaves.begin() ); m_ChildLeaves.end() != Iter; ++Iter )
				{
					//Find the current child branch
					if( (*Iter)->first == Name )
					{
						m_pCurrentLeaf = *Iter;
						pResult = *Iter;
						break;
					}
				}
			}
		}

		return pResult;
	}

	bool SVXMLMaterialsTree::isValidLeaf( const SVBranchHandle pParent, const SVLeafHandle pChild )
	{
		bool Result( false );

		//if current leaf then is valid
		if( pChild == m_pCurrentLeaf && m_Tree.end() != m_pCurrentLeaf )
		{
			Result = true;
		}
		else
		{
			SVBranchHandle pBranch( pParent );

			if( nullptr == pBranch )
			{
				pBranch = getRoot();
			}

			if( nullptr != pBranch )
			{
				if( setChildren( pBranch ) )
				{
					for( SVLeaves::iterator Iter( m_ChildLeaves.begin() ); m_ChildLeaves.end() != Iter; ++Iter )
					{
						//Find the current child branch
						if( pChild == *Iter )
						{
							Result = true;
							break;
						}
					}
				}
			}
		}

		return Result;
	}

	HRESULT SVXMLMaterialsTree::createLeaf( const SVBranchHandle pParent, LPCTSTR Name, const _variant_t& rData, SVLeafHandle* ppLeaf )
	{
		HRESULT Result( S_OK );
		SVBranchHandle pBranch( pParent );
		std::string LeafName( Name );

		if( nullptr == pBranch )
		{
			pBranch = getRoot();
		}

		if( nullptr != pBranch )
		{
			SVLeafHandle pNewLeaf;
			SVMaterialDataPtr pMaterial{ new SVMaterialData(rData) };
			pNewLeaf = pBranch->insert( SVMaterialsTree::SVTreeElement( LeafName, pMaterial ) );

			if( pBranch->end() != pNewLeaf )
			{
				if( nullptr != ppLeaf )
				{
					*ppLeaf = pNewLeaf;
				}
			}
			else
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_FAIL;
		}

		return Result;
	}

	HRESULT SVXMLMaterialsTree::deleteLeaf( const SVLeafHandle pLeaf )
	{
		HRESULT Result( E_FAIL );

		if( m_Tree.end() != pLeaf )
		{
			SVBranchHandle pParent( pLeaf.node()->parent() );
			if ( nullptr != pParent )
			{
				pParent->erase( pLeaf );
			}
			Result = S_OK;
		}


		return Result;
	}

	std::string SVXMLMaterialsTree::getLeafName( const SVLeafHandle pLeaf ) const
	{
		std::string Result;

		if( m_Tree.end() != pLeaf )
		{
			Result = pLeaf->first;
		}

		return Result;
	}

	HRESULT SVXMLMaterialsTree::getLeafData( const SVLeafHandle pLeaf, _variant_t& rData ) const
	{
		HRESULT Result( E_FAIL );

		rData.Clear();
		if( m_Tree.end() != pLeaf )
		{
			::VariantInit( &rData.GetVARIANT() );
			::VariantCopy( &rData.GetVARIANT(), &pLeaf->second->GetVARIANT());
			Result = S_OK;
		}

		return Result;
	}

	HRESULT SVXMLMaterialsTree::getLeafData( const SVBranchHandle pParent, LPCTSTR Name, _variant_t& rData )
	{
		return getLeafData( findLeaf( pParent, Name ), rData );
	}

	HRESULT SVXMLMaterialsTree::setLeafData( const SVLeafHandle pLeaf, const _variant_t& rData )
	{
		HRESULT Result( E_FAIL );

		if( m_Tree.end() != pLeaf )
		{
			SVLeafHandle pSetData( pLeaf );
			if(nullptr != pSetData->second)
			{
				(*pSetData->second).clear();
				*pSetData->second = rData;
			}
			Result = S_OK;
		}

		return Result;
	}

	SvDef::StringVector SVXMLMaterialsTree::getLeafValues( const SVBranchHandle pParent, const std::string& rSearchName)
	{
		SvDef::StringVector result;

		SVLeafHandle pLeaf( getFirstLeaf( pParent ) );
		while( isValidLeaf( pParent, pLeaf ) )
		{
			std::string LeafName = getLeafName( pLeaf );
			if( rSearchName == LeafName )
			{
				_variant_t variantValue;
				getLeafData( pLeaf, variantValue);
				std::string value{ SvUl::createStdString(variantValue.GetVARIANT()) };
				//Only insert in vector if it does not exist
				if (result.cend() == std::find(result.cbegin(), result.cend(), value))
				{
					result.push_back(value);
				}
			}
			pLeaf = getNextLeaf( pParent, pLeaf);
		}

		SVBranchHandle pBranch( getFirstBranch( pParent ) );
		while( isValidBranch( pBranch ) )
		{
			SvDef::StringVector branchResult = getLeafValues( pBranch, rSearchName);
			for (const auto& rBranchValue : branchResult)
			{
				//Only insert in vector if it does not exist
				if (result.cend() == std::find(result.cbegin(), result.cend(), rBranchValue))
				{
					result.push_back(rBranchValue);
				}
			}
			pBranch = getNextBranch(pParent, pBranch);
		}

		return result;
	}
	#pragma endregion Public Methods

	#pragma region Private Methods
	bool SVXMLMaterialsTree::setChildren( const SVBranchHandle pParent ) const
	{
		bool Result( false );

		if( nullptr != pParent )
		{
			if( pParent == m_pCurrentParent )
			{
				Result = true;
			}
			else
			{
				m_pCurrentParent = nullptr;
				m_ChildBranches.clear();
				m_ChildLeaves.clear();
				for( SVMaterialsTree::iterator Iter( pParent->begin() ); pParent->end() != Iter; ++Iter )
				{
					//If smart pointer empty then it is a branch
					if( nullptr == Iter->second )
					{
						m_ChildBranches.push_back( Iter.node() );
					}
					else
					{
						m_ChildLeaves.push_back( Iter );
					}
				}
				Result = true;
				m_pCurrentParent = pParent;
			}
		}

		return Result;
	}
	#pragma endregion Private Methods
} //namespace SvXml


