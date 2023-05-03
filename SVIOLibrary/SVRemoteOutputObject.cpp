//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/IObjectWriter.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVRemoteOutputObject, SvPb::RemoteOutputObjectId);

SVRemoteOutputObject::SVRemoteOutputObject()
{
	LocalInitialize();
}

std::string SVRemoteOutputObject::GetInputValueObjectName()
{
	// Use the ID to get an object.
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(m_valueObjectID);
	if( nullptr != dynamic_cast<SvOi::IValueObject*> (pObject) )
	{	// Get the object name from the object pointer.
		m_strObjectName = pObject->GetCompleteName();
	}
	return m_strObjectName;
}

std::string SVRemoteOutputObject::GetGroupID( ) const
{
	return m_strGroupID;
}

void SVRemoteOutputObject::Persist(SvOi::IObjectWriter& rWriter ) const
{
	_variant_t svVariant = convertObjectIdToVariant(getObjectId());
	rWriter.WriteAttribute( SvXml::CTAG_UNIQUE_REFERENCE_ID, svVariant );
	svVariant.Clear();

	svVariant = convertObjectIdToVariant(m_valueObjectID);
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_OUTPUT_INPUT_OBJECT_ID, svVariant );
	svVariant.Clear();

	svVariant = _variant_t(m_strGroupID.c_str());
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_GROUP_ID, svVariant );
	svVariant.Clear();

	svVariant = _variant_t(m_strObjectName.c_str());
	rWriter.WriteAttribute( SvXml::CTAG_REMOTE_OUTPUT_NAME, svVariant );
	svVariant.Clear();
}

bool SVRemoteOutputObject::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	_variant_t svVariant;
	bool bOk = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiParent, svVariant );
	if ( bOk )
	{
		bOk = SVObjectManagerClass::Instance().ChangeUniqueObjectID( this, calcObjectId(svVariant) );

		if( false == bOk )
		{
			OutputDebugString( "Remote Output Unable to set RemoteOutputObject Unique Id\n");
		}
	}

	if (bOk)
	{
		bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_OUTPUT_INPUT_OBJECT_ID, htiParent, svVariant);
		if (!bOk)
		{
			return false;
		}
		m_valueObjectID = calcObjectId(svVariant);
	}

	bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_GROUP_ID, htiParent, svVariant);
	if (!bOk)
	{
		return false;
	}
	
	m_strGroupID = SvUl::createStdString(svVariant);

	bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_OUTPUT_NAME, htiParent, svVariant);
	if (!bOk)
	{
		return false;
	}
	
	m_strObjectName = SvUl::createStdString(svVariant);
	
	return true;
}

void SVRemoteOutputObject::SetValueObjectID(uint32_t objectID, DWORD objectIDIndex /*= 0*/)
{
	if (0 == objectIDIndex)
	{
		m_valueObjectID = objectID;
	}
}

uint32_t SVRemoteOutputObject::GetValueObjectID(DWORD objectIDIndex /*= 0*/) const
{
	if (0 == objectIDIndex)
	{
		return m_valueObjectID;
	}
	return SvDef::InvalidObjectId;
}

HRESULT SVRemoteOutputObject::SetGroupID( const std::string& p_strGroupID )
{
	m_strGroupID = p_strGroupID;
	return S_OK;
}

void SVRemoteOutputObject::LocalInitialize()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVRemoteOutputObjectType;
}
