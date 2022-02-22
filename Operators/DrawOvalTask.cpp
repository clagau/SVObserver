//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
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
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawOvalTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea)
{ 
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		double centerX = 0;
		m_centerX.getValue(centerX);
		double centerY = 0;
		m_centerY.getValue(centerY);
		double width = 0;
		m_width.getValue(width);
		double height = 0;
		m_height.getValue(height);

		switch (drawArea)
		{
			case DrawArea::InsideArea:
			case DrawArea::OutsideArea:
				MgraArcFill(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), centerX, centerY, width / 2, height / 2, 0, 360);
				break;
			case DrawArea::ContourArea:
				MgraArc(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), centerX, centerY, width / 2, height / 2, 0, 360);
				break;
		}
		return true;
	}
	else
	{
		assert(false);
		return false;
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
void DrawOvalTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawOvalTaskType;

	BuildEmbeddedObjectList();
}

void DrawOvalTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_centerX, SvPb::CenterXEId, IDS_OBJECTNAME_CENTER_X, true, SvOi::SVResetItemOwner);
	m_centerX.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_centerY, SvPb::CenterYEId, IDS_OBJECTNAME_CENTER_Y, true, SvOi::SVResetItemOwner);
	m_centerY.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_width, SvPb::WidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, true, SvOi::SVResetItemOwner);
	m_width.SetDefaultValue(_variant_t(80L), true);
	RegisterEmbeddedObject(&m_height, SvPb::HeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, true, SvOi::SVResetItemOwner);
	m_height.SetDefaultValue(_variant_t(80L), true);
}
#pragma endregion Private Methods
} //namespace SvOp
