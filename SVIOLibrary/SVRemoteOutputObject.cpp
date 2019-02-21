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
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVRemoteOutputObject, SVRemoteOutputObjectGUID );

SVRemoteOutputObject::SVRemoteOutputObject()
: m_pValueObject(nullptr)
{
	LocalInitialize();
}

SVRemoteOutputObject::~SVRemoteOutputObject()
{
}

std::string SVRemoteOutputObject::GetInputValueObjectName()
{
	// Use the GUID to get an object.
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObjectA(m_InputObjectId);
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
	{	// Get the object name from the object pointer.
		m_strObjectName = pObject->GetCompleteName();
		m_pValueObject = pObject;
	}
	return m_strObjectName;
}

SVObjectClass* SVRemoteOutputObject::GetValueObject()
{
	return m_pValueObject;
}

std::string SVRemoteOutputObject::GetGroupID( ) const
{
	return m_strGroupID;
}


// Parameters >> Tree
bool SVRemoteOutputObject::GetParameters(SvXml::SVObjectXMLWriter& rWriter ) const
{
	_variant_t svVariant;

	svVariant = m_outObjectInfo.getUniqueObjectID().ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant );
	svVariant.Clear();

	svVariant = m_InputObjectId.ToVARIANT();
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, svVariant );
	svVariant.Clear();

	svVariant = _variant_t(m_strGroupID.c_str());
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_GROUP_ID, svVariant );
	svVariant.Clear();

	svVariant = _variant_t(m_strObjectName.c_str());
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_OUTPUT_NAME, svVariant );
	svVariant.Clear();

	return true;
}

// Sets parameters from Tree Control
bool SVRemoteOutputObject::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	bool bOk = false;
	_variant_t svVariant;

	bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiParent, svVariant );
	if ( bOk )
	{
		SVGUID UniqueID{ svVariant };

		bOk = SVObjectManagerClass::Instance().CloseUniqueObjectID( this );

		if( bOk )
		{
			m_outObjectInfo.GetObjectReference().setGuid(UniqueID);

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
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_REMOTE_OUTPUT_INPUT_OBJECT_GUID, htiParent, svVariant );
			if ( bOk )
			{
				m_InputObjectId = SVGUID( svVariant );
			}
		}

		if ( bOk )
		{
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_REMOTE_GROUP_ID, htiParent, svVariant );
			if ( bOk )
			{
				m_strGroupID = SvUl::createStdString(svVariant);
			}
		}

		if ( bOk )
		{
			bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_REMOTE_OUTPUT_NAME, htiParent, svVariant );
			if ( bOk )
			{
				m_strObjectName = SvUl::createStdString(svVariant);
			}
		}

	}
	return bOk;
}

HRESULT SVRemoteOutputObject::SetInputObject(SVObjectClass* pObject )
{
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
	{
		m_pValueObject = pObject;
		m_InputObjectId = pObject->GetUniqueObjectID();
	}
	else
	{
		m_pValueObject = nullptr;
		m_InputObjectId.clear();
	}

	return S_OK;
}

HRESULT SVRemoteOutputObject::SetInputObjectId( GUID ObjectId )
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( ObjectId );
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
	{
		m_InputObjectId = ObjectId;
		m_pValueObject = pObject;
	}
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

HRESULT SVRemoteOutputObject::SetGroupID( const std::string& p_strGroupID )
{
	m_strGroupID = p_strGroupID;
	return S_OK;
}

void SVRemoteOutputObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVRemoteOutputObjectType;
}