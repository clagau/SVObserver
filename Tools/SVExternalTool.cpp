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

#pragma region Includes
#include "stdafx.h"
#include "SVExternalTool.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "Operators/SVExternalToolTask.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SVToolClass* SVExternalTool::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	return nullptr;
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

	SvOp::SVExternalToolTask* l_pTask = new SvOp::SVExternalToolTask;
	if ( nullptr != l_pTask )
	{
		Add( l_pTask );
	}

	removeEmbeddedExtents();
	addDefaultInputObjects();
}



SvVol::SVStringValueObjectClass* SVExternalTool::GetInputImageNames()
{
	return &m_SourceImageNames;	
}

} //namespace SvTo
