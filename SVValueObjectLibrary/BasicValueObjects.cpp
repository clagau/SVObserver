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
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "BasicValueObjects.h"
#include "SVObjectLibrary/SVObjectNameInfo.h"
#include "Definitions/GlobalConst.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const BasicValueObjects::DottedNameGuidMap BasicValueObjects::m_StaticUniqueIDMap = boost::assign::map_list_of<std::string, SVGUID>
	// Dotted name,												static unique ID
	( std::string( SvDef::FqnRoot ),									SVGUID( RootUidGuid ) )
	( std::string( SvDef::FqnGlobal ),								SVGUID( GlobalUidGuid ) )
	( std::string( SvDef::FqnEnvironment ),							SVGUID( EnvironmentUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModelNumber ),				SVGUID( EnvironmentModelNumberUidGuid ) )
	( std::string( SvDef::FqnEnvironmentWinKey ),					SVGUID( EnvironmentWinKeyUidGuid ) )
	( std::string( SvDef::FqnEnvironmentImageUpdate ),				SVGUID( EnvironmentImageUpdateUidGuid ) )
	( std::string( SvDef::FqnEnvironmentResultUpdate ),				SVGUID( EnvironmentResultUpdateUidGuid ) )
	( std::string( SvDef::FqnEnvironmentMode ),						SVGUID( EnvironmentModeUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModeIsRun ),					SVGUID( EnvironmentModeIsRunUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModeIsStop ),				SVGUID( EnvironmentModeIsStopUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModeIsRegressionTest ),		SVGUID( EnvironmentModeIsRegressionTestUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModeIsTest ),				SVGUID( EnvironmentModeIsTestUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModeIsEdit ),				SVGUID( EnvironmentModeIsEditUidGuid ) )
	( std::string( SvDef::FqnEnvironmentModeValue ),					SVGUID( EnvironmentModeValueUidGuid ) )
	;
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

	if( !std::string(DottedBranch).empty() )
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
	std::string RootChildName;

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

	for( std::string::size_type Index(0); Index < rParsedName.m_NameArray.size(); ++Index )
	{
		std::string Name = rParsedName.m_NameArray[Index];
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

	for( std::string::size_type Index(0); Index < rParsedName.m_NameArray.size(); ++Index )
	{
		std::string Name = rParsedName.m_NameArray[Index];

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
	std::string SearchName( ChildName );

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

