//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputObject
//* .File Name       : $Workfile:   SVRemoteOutputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:52:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comdef.h>
#include "SVRemoteOutputObject.h"
#include "SVObjectLibrary\SVClsIds.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVPPQObject.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVRemoteOutputObject, SVRemoteOutputObjectGUID );

SVRemoteOutputObject::SVRemoteOutputObject(void)
: m_pValueObject(nullptr)
{
}

SVRemoteOutputObject::~SVRemoteOutputObject(void)
{
}

SVString SVRemoteOutputObject::GetInputValueObjectName()
{
	// Use the GUID to get an object.
	SVValueObjectClass* l_pObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObjectA(m_InputObjectId));
	if( nullptr != l_pObject )
	{	// Get the object name from the object pointer.
		m_strObjectName = l_pObject->GetCompleteObjectName();
		m_pValueObject = l_pObject;
	}
	return m_strObjectName;
}

SVValueObjectClass* SVRemoteOutputObject::GetValueObject()
{
	return m_pValueObject;
}

SVString SVRemoteOutputObject::GetGroupID( ) const
{
	return m_strGroupID;
}


// Parameters >> Tree
bool SVRemoteOutputObject::GetParameters( SVObjectXMLWriter& rWriter ) const
{
	_variant_t svVariant;

	svVariant = SVGUID( m_outObjectInfo.UniqueObjectID ).ToVARIANT();
	rWriter.WriteAttribute( CTAG_UNIQUE_REFERENCE_ID, svVariant );
	svVariant.Clear();

	svVariant = SVGUID( m_InputObjectId ).ToVARIANT();
	rWriter.WriteAttribute( CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, svVariant );
	svVariant.Clear();

	svVariant = _variant_t(m_strGroupID.c_str());
	rWriter.WriteAttribute( CTAG_REMOTE_GROUP_ID, svVariant );
	svVariant.Clear();

	svVariant = _variant_t(m_strObjectName.c_str());
	rWriter.WriteAttribute( CTAG_REMOTE_OUTPUT_NAME, svVariant );
	svVariant.Clear();

	return true;
}

// Sets parameters from Tree Control
BOOL SVRemoteOutputObject::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;
	_variant_t svVariant;

	bOk = SVNavigateTree::GetItem( rTree, CTAG_UNIQUE_REFERENCE_ID, htiParent, svVariant );
	if ( bOk )
	{
		SVGUID ObjectID = svVariant;

		bOk = SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

		if( bOk )
		{
			m_outObjectInfo.UniqueObjectID = ObjectID;

			bOk = SVObjectManagerClass::Instance().OpenUniqueObjectID( this );
		}
		else
		{
			OutputDebugString( "Remote Output Unable to set RemoteOutputObject Unique Id\n");
		}
	}

	if( bOk )
	{
		if ( bOk )
		{
			bOk = SVNavigateTree::GetItem( rTree, CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, htiParent, svVariant );
			if ( bOk )
			{
				m_InputObjectId = SVGUID( svVariant );
			}
		}

		if ( bOk )
		{
			bOk = SVNavigateTree::GetItem( rTree, CTAG_REMOTE_GROUP_ID, htiParent, svVariant );
			if ( bOk )
			{
				m_strGroupID = SvUl_SF::createSVString(svVariant);
			}
		}

		if ( bOk )
		{
			bOk = SVNavigateTree::GetItem( rTree, CTAG_REMOTE_OUTPUT_NAME, htiParent, svVariant );
			if ( bOk )
			{
				m_strObjectName = SvUl_SF::createSVString(svVariant);
			}
		}

	}
	return bOk;
}


HRESULT SVRemoteOutputObject::SetInputObject(SVValueObjectClass* p_pObject )
{
	if( nullptr != p_pObject )
	{
		m_InputObjectId = p_pObject->GetUniqueObjectID();
	}
	else
	{
		m_InputObjectId.clear();
	}

	m_pValueObject = p_pObject;
	return S_OK;
}


HRESULT SVRemoteOutputObject::SetInputObjectId( GUID p_ObjectId )
{
	m_InputObjectId = p_ObjectId;
	m_pValueObject = dynamic_cast<SVValueObjectClass*>(SVObjectManagerClass::Instance().GetObjectA( p_ObjectId ));
	return S_OK;
}

HRESULT SVRemoteOutputObject::GetInputValueObjectGUID( GUID& p_rGUID ) const
{
	p_rGUID = m_InputObjectId;
	return S_OK;
}

SVGUID SVRemoteOutputObject::GetInputValueObjectGUID() const
{
	return m_InputObjectId;
}

HRESULT SVRemoteOutputObject::SetGroupID( const SVString& p_strGroupID )
{
	m_strGroupID = p_strGroupID;
	return S_OK;
}

