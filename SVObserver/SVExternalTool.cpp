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

SVExternalTool::SVExternalTool( SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass( POwner, StringResourceID )
{
	Initialize();
}

SVExternalTool::~SVExternalTool()
{ 
}

bool SVExternalTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVExternalTool::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	return nullptr;
}

HRESULT SVExternalTool::GetImageExtent(SVImageExtentClass& rImageExtent)
{
	return E_FAIL;
}

HRESULT SVExternalTool::UpdateImageWithExtent()
{
	return S_OK;
}

void SVExternalTool::Initialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVExternalToolObjectType;

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

