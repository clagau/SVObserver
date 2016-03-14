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

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVExternalTool::SVExternalTool( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	Initialize();
}


SVExternalTool::~SVExternalTool()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVExternalTool::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVExternalTool::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	return NULL;
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

DWORD_PTR SVExternalTool::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;
	
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		case SVMSGID_IMAGE_SOURCE_CHANGED:
		{
//			isCreated = FALSE;
		}
		break;
	}

	return ( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

void SVExternalTool::Initialize()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVExternalToolObjectType;

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	SVExternalToolTask* l_pTask = new SVExternalToolTask;
	if ( l_pTask != NULL )
	{
		Add( l_pTask );
	}


	addDefaultInputObjects();
}



SVStaticStringValueObjectClass* SVExternalTool::GetInputImageNames()
{
	return &m_svSourceImageNames;	
}

