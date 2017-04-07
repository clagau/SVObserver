//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalTool
//* .File Name       : $Workfile:   SVExternalTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVExternalTool.h"
#include "SVExternalToolTask.h"

SV_IMPLEMENT_CLASS( SVExternalTool, SVExternalToolGuid );

SVExternalTool::SVExternalTool( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	Initialize();
}

SVExternalTool::~SVExternalTool()
{ 
}

BOOL SVExternalTool::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.SetObjectAttributesAllowed( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVExternalTool::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	return nullptr;
}

HRESULT SVExternalTool::GetImageExtent( SVImageExtentClass &p_rsvImageExtent )
{
	return S_FALSE;
}

HRESULT SVExternalTool::GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent )
{
	return S_FALSE;
}

HRESULT SVExternalTool::UpdateImageWithExtent( unsigned long p_Index )
{
	return S_OK;
}

void SVExternalTool::Initialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVExternalToolObjectType;

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	SVExternalToolTask* l_pTask = new SVExternalToolTask;
	if ( nullptr != l_pTask )
	{
		Add( l_pTask );
	}


	addDefaultInputObjects();
}



SVStringValueObjectClass* SVExternalTool::GetInputImageNames()
{
	return &m_SourceImageNames;	
}

