//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   12 Aug 2014 12:50:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BasicValueObjects.h"
#include "SVObjectLibrary\SVObjectNameInfo.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
BasicValueObjects::BasicValueObjects()
{
}

BasicValueObjects::~BasicValueObjects()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BasicValueObjectPtr BasicValueObjects::getValueObject( LPCTSTR DottedName ) const
{
	BasicValueObjectPtr pValue( nullptr );
	vt_const_iterator ValueIter = findValueObject( DottedName );
	if ( m_Tree.end() != ValueIter )
	{
		pValue = *ValueIter;
	}
	return pValue;
}

void BasicValueObjects::getValueList( ValueVector& rObjectList, LPCTSTR DottedBranch,  UINT AttributesAllowedFilter ) const
{
	//Do not clear the reference object list as this method could be called multiple times
	vt_const_pre_order_iterator BranchStartIter( m_Tree.pre_order_begin() );
	vt_const_pre_order_iterator BranchEndIter( m_Tree.pre_order_end() );

	if( !SVString(DottedBranch).empty() )
	{
		vt_const_iterator ParentIter = findValueObject( DottedBranch );
		if( m_Tree.end() != ParentIter )
		{
			BranchStartIter = ParentIter.node()->pre_order_begin();
			BranchEndIter = ParentIter.node()->pre_order_end();
		}
	}

	vt_const_pre_order_iterator Iter( BranchStartIter );
	while( BranchEndIter != Iter )
	{
		BasicValueObjectPtr ObjPtr = *Iter;

		//Only leafs are of interest no nodes
		if( !Iter->empty() && !(*Iter)->isNode() )
		{
			if( ((*Iter)->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter )
			{
				rObjectList.push_back( (*Iter) );
			}
		}
		++Iter;
	}
}

HRESULT BasicValueObjects::deleteValueObject( LPCTSTR DottedName )
{
	HRESULT Result( S_FALSE );
	vt_iterator Iter( m_Tree.end() );

	SVObjectNameInfo ParsedName;
	SVString RootChildName;

	ParsedName.ParseObjectName( DottedName );
	if( 0 < ParsedName.m_NameArray.size() )
	{
		RootChildName = ParsedName.m_NameArray[0];
	}

	Iter = findValueObject( ParsedName );
	if( m_Tree.end() != Iter && !Iter->empty() )
	{
		Result = S_OK;
		Iter.node()->parent()->erase( Iter );

		//If the branch is a node and no other children then delete the node
		ParsedName.RemoveBottomName();
		Iter = findValueObject( ParsedName );
		while( m_Tree.end() != Iter && !Iter->empty()  )
		{
			bool isDeleteableEmptyNode = (*Iter)->isNode() && 0 == Iter.node()->size() && RootChildName != ParsedName.GetObjectName();
			if( isDeleteableEmptyNode )
			{
				Iter.node()->parent()->erase( Iter );
				ParsedName.RemoveBottomName();
				Iter = findValueObject( ParsedName );
			}
			else
			{
				Iter = m_Tree.end();
			}
		}
	}

	return Result;
}

#pragma endregion Public Methods

BasicValueObjects::vt_const_iterator BasicValueObjects::findValueObject( LPCTSTR DottedName ) const
{
	SVObjectNameInfo ParsedName;

	ParsedName.ParseObjectName( DottedName );

	return findValueObject( ParsedName );
}

BasicValueObjects::vt_const_iterator BasicValueObjects::findValueObject( const SVObjectNameInfo& rParsedName ) const
{
	vt_const_iterator Iter( m_Tree.end() );
	vt_const_iterator IterParent( m_Tree.end() );
	vt_const_iterator IterStart( m_Tree.begin() );
	vt_const_iterator IterEnd( m_Tree.end() );

	for( SVString::size_type Index(0); Index < rParsedName.m_NameArray.size(); ++Index )
	{
		SVString Name = rParsedName.m_NameArray[Index];
		Iter = findChildObject( IterStart, IterEnd, Name.c_str() );

		if( IterEnd != Iter &&  !Iter->empty() )
		{
			IterParent = Iter;
			IterStart = Iter.node()->begin();
			IterEnd = Iter.node()->end();
		}
		else
		{
			Iter = m_Tree.end();
			break;
		}
	}

	return Iter;
}

BasicValueObjects::vt_iterator BasicValueObjects::findValueObject( const SVObjectNameInfo& rParsedName )
{
	vt_iterator Iter( m_Tree.end() );
	vt_iterator IterParent( m_Tree.end() );
	vt_iterator IterStart( m_Tree.begin() );
	vt_iterator IterEnd( m_Tree.end() );

	for( SVString::size_type Index(0); Index < rParsedName.m_NameArray.size(); ++Index )
	{
		SVString Name = rParsedName.m_NameArray[Index];

		vt_const_iterator IterConst( IterEnd );
		IterConst = findChildObject( IterStart, IterEnd, Name.c_str() );

		if( IterEnd != IterConst && !IterConst->empty() )
		{
			Iter = IterStart;
			std::advance ( Iter, std::distance<vt_const_iterator>( IterStart, IterConst ) );
			IterParent = Iter;
			IterStart = Iter.node()->begin();
			IterEnd = Iter.node()->end();
		}
		else
		{
			Iter = m_Tree.end();
			break;
		}
	}

	return Iter;
}

BasicValueObjects::vt_const_iterator BasicValueObjects::findChildObject( const vt_const_iterator& rStartIter, const vt_const_iterator& rEndIter, LPCTSTR ChildName ) const
{
	vt_const_iterator Iter( rStartIter );
	SVString SearchName( ChildName );

	while( rEndIter != Iter )
	{
		if( !Iter->empty() && SearchName  == (*Iter)->GetName() )
		{
			break;
		}
		Iter++;
	}

	return Iter;
}

