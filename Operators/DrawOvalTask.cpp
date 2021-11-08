//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// \file DrawOvalTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a rectangle use by DrawTool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawOvalTask.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"

#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(DrawOvalTask, SvPb::DrawOvalClassId);


#pragma region Constructor
DrawOvalTask::DrawOvalTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName)
{
	Initialize();
}

DrawOvalTask::DrawOvalTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID)
{
	Initialize();
}

DrawOvalTask::~DrawOvalTask()
{
	MgraFree(m_graphicContextId);
}
#pragma endregion Constructor

#pragma region Public Methods
bool DrawOvalTask::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	return l_bOk;
}

bool DrawOvalTask::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	byte color1 = 0;
	m_Color1Object.GetValue(color1);
	if (m_useColorImage)
	{
		byte color2 = 0;
		m_Color2Object.GetValue(color2);
		byte color3 = 0;
		m_Color3Object.GetValue(color3);
		MgraControl(m_graphicContextId, M_COLOR, M_RGB888(color1, color2, color3));
	}
	else
	{
		MgraControl(m_graphicContextId, M_COLOR, color1);
	}

	return Result;
}

bool DrawOvalTask::Run(SvOi::SVImageBufferHandlePtr pImageHandle, SvIe::RunStatus&, SvStl::MessageContainerVector*)
{ 
	double centerX = 0;
	m_centerX.getValue(centerX);
	double centerY = 0;
	m_centerY.getValue(centerY);
	double width = 0;
	m_width.getValue(width);
	double height = 0;
	m_height.getValue(height);
	MgraArcFill(m_graphicContextId, pImageHandle->GetBuffer().GetIdentifier(), centerX, centerY, width/2, height/2, 0, 360);
	return true;
}
#pragma endregion Public Methods

#pragma region Private Methods
void DrawOvalTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawOvalTaskType;

	MgraAlloc(M_DEFAULT_HOST, &m_graphicContextId);
	BuildEmbeddedObjectList();
}

void DrawOvalTask::BuildEmbeddedObjectList()
{
	//@TODO[MZA][10.20][07.10.2021] 
	/*
	RegisterEmbeddedObject(&m_drawArea, SvPb::DrawAreaEId, IDS_OBJECT_DRAW_AREA, false, SvOi::SVResetItemOwner);
	m_drawArea.SetEnumTypes({{_T("Inside"), 1}, {_T("Outside Shape"), 2}});
	m_drawArea.SetDefaultValue(2);*/

	RegisterEmbeddedObject(&m_centerX, SvPb::CenterXEId, IDS_OBJECTNAME_CENTER_X, true, SvOi::SVResetItemOwner);
	m_centerX.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_centerY, SvPb::CenterYEId, IDS_OBJECTNAME_CENTER_Y, true, SvOi::SVResetItemOwner);
	m_centerY.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_width, SvPb::WidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, true, SvOi::SVResetItemOwner);
	m_width.SetDefaultValue(_variant_t(80L), true);
	RegisterEmbeddedObject(&m_height, SvPb::HeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, true, SvOi::SVResetItemOwner);
	m_height.SetDefaultValue(_variant_t(80L), true);

	RegisterEmbeddedObject(&m_Color1Object, SvPb::Color1EId, IDS_COLOR1, false, SvOi::SVResetItemOwner);
	m_Color1Object.SetDefaultValue(0, false);
	RegisterEmbeddedObject(&m_Color2Object, SvPb::Color2EId, IDS_COLOR2, false, SvOi::SVResetItemOwner);
	m_Color2Object.SetDefaultValue(0, false);
	RegisterEmbeddedObject(&m_Color3Object, SvPb::Color3EId, IDS_COLOR3, false, SvOi::SVResetItemOwner);
	m_Color3Object.SetDefaultValue(0, false);
}
#pragma endregion Private Methods
} //namespace SvOp
