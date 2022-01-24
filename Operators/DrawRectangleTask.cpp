//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// \file DrawRectangleTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a rectangle use by DrawTool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawRectangleTask.h"
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

SV_IMPLEMENT_CLASS(DrawRectangleTask, SvPb::DrawRectangleClassId);


#pragma region Constructor
DrawRectangleTask::DrawRectangleTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName)
{
	Initialize();
}

DrawRectangleTask::DrawRectangleTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID)
{
	Initialize();
}

DrawRectangleTask::~DrawRectangleTask()
{
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawRectangleTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		double left = 0;
		m_leftValue.getValue(left);
		double top = 0;
		m_topValue.getValue(top);
		double width = 0;
		m_width.getValue(width);
		double height = 0;
		m_height.getValue(height);
		switch (drawArea)
		{
			case DrawArea::InsideArea:
			case DrawArea::OutsideArea:
				MgraRectFill(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), left, top, left + width - 1, top + height - 1);
				break;
			case DrawArea::ContourArea:
				MgraRect(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), left, top, left + width - 1, top + height - 1);
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
void DrawRectangleTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawRectangleTaskType;

	BuildEmbeddedObjectList();
}

void DrawRectangleTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_leftValue, SvPb::LeftEId, IDS_OBJECTNAME_EXTENT_LEFT, true, SvOi::SVResetItemOwner);
	m_leftValue.SetDefaultValue(_variant_t(10L), true);
	RegisterEmbeddedObject(&m_topValue, SvPb::TopEId, IDS_OBJECTNAME_EXTENT_TOP, true, SvOi::SVResetItemOwner);
	m_topValue.SetDefaultValue(_variant_t(10L), true);
	RegisterEmbeddedObject(&m_width, SvPb::WidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, true, SvOi::SVResetItemOwner);
	m_width.SetDefaultValue(_variant_t(80L), true);
	RegisterEmbeddedObject(&m_height, SvPb::HeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, true, SvOi::SVResetItemOwner);
	m_height.SetDefaultValue(_variant_t(80L), true);
}
#pragma endregion Private Methods
} //namespace SvOp
