//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAcquisition
//* .File Name       : $Workfile:   SVToolAcquisition.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Jul 2014 12:44:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAcquisition.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVAcquisitionToolClass, SVAcquisitionToolClassGuid );

SVAcquisitionToolClass::SVAcquisitionToolClass( SVObjectClass* POwner, int StringResourceID )
					   :SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVAcquisitionToolClass::init()
{
	// Indentify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVToolAcquisitionObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &mainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	addDefaultInputObjects();
}


SVAcquisitionToolClass::~SVAcquisitionToolClass()
{
}


bool SVAcquisitionToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	if( bOk )
	{
		bOk = ( S_OK == SetImageExtent( mainImageObject.GetImageExtents() ) );
		mainImageObject.SetObjectAttributesAllowed( SvOi::SV_PUBLISH_RESULT_IMAGE, SvOi::SetAttributeType::AddAttribute );
	}

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	m_SourceImageNames.SetValue( mainImageObject.GetCompleteName());

	m_isCreated = bOk;

	return bOk;
}

bool SVAcquisitionToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	HRESULT l_Temp = SetImageExtent( mainImageObject.GetImageExtents() );

	if( S_OK != l_Temp )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetImageExtentFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	UpdateImageWithExtent();

	if (nullptr == mainImageObject.GetCamera())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoCameraToMainImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

bool SVAcquisitionToolClass::DoesObjectHaveExtents() const
{
	return false;
}

SVStringValueObjectClass* SVAcquisitionToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}


