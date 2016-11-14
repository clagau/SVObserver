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

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

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
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVExternalToolObjectType;

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	SVExternalToolTask* l_pTask = new SVExternalToolTask;
	if ( nullptr != l_pTask )
	{
		Add( l_pTask );
	}


	addDefaultInputObjects();
}



SVStaticStringValueObjectClass* SVExternalTool::GetInputImageNames()
{
	return &m_svSourceImageNames;	
}

