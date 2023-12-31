//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
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

SV_IMPLEMENT_CLASS( SVExternalTool, SvPb::ExternalToolClassId);

SVExternalTool::SVExternalTool( SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass(ToolExtType::NoHeightWidth, POwner, StringResourceID )
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
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

HRESULT SVExternalTool::UpdateImageWithExtent()
{
	return S_OK;
}

void SVExternalTool::Initialize()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVExternalToolObjectType;

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemToolAndDependent, false );

	SvOp::SVExternalToolTask* l_pTask = new SvOp::SVExternalToolTask;
	if ( nullptr != l_pTask )
	{
		Add( l_pTask );
	}
	if (m_pEmbeddedExtents)
	{
		m_pEmbeddedExtents->SetAttributes();
	}
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationNone);
	m_toolExtent.updateImageExtent(false);
}



SvVol::SVStringValueObjectClass* SVExternalTool::GetInputImageNames()
{
	return &m_SourceImageNames;	
}

std::vector<std::string> SVExternalTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 7> cToolAdjustNameList
	{
		_T("Select External DLL"),
		_T("Input Images"),
		_T("Input Values"),
		_T("Result Values"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

} //namespace SvTo
