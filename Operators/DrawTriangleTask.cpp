//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawTriangleTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a triangle use by DrawTool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawTriangleTask.h"
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

SV_IMPLEMENT_CLASS(DrawTriangleTask, SvPb::DrawTriangleClassId);


#pragma region Constructor
DrawTriangleTask::DrawTriangleTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName)
{
	Initialize();
}

DrawTriangleTask::DrawTriangleTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID)
{
	Initialize();
}

DrawTriangleTask::~DrawTriangleTask()
{}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawTriangleTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		std::array<MIL_DOUBLE, 3> xPoints; 
		std::array<MIL_DOUBLE, 3> yPoints; 	
		m_x1Value.getValue(xPoints[0]);
		m_x2Value.getValue(xPoints[1]);
		m_x3Value.getValue(xPoints[2]);
		m_y1Value.getValue(yPoints[0]);
		m_y2Value.getValue(yPoints[1]);
		m_y3Value.getValue(yPoints[2]);
		switch (drawArea)
		{
			case DrawArea::InsideArea:
			case DrawArea::OutsideArea:
				MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), 3, xPoints.data(), yPoints.data(), M_NULL, M_NULL, M_POLYGON | M_FILLED);
				break;
			case DrawArea::ContourArea:
				MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), 3, xPoints.data(), yPoints.data(), M_NULL, M_NULL, M_POLYGON);
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
void DrawTriangleTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawTriangleTaskType;

	BuildEmbeddedObjectList();
}

void DrawTriangleTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_x1Value, SvPb::X1EId, IDS_OBJECTNAME_EXTENT_X1, true, SvOi::SVResetItemOwner, true);
	m_x1Value.SetDefaultValue(_variant_t(10L), true);
	RegisterEmbeddedObject(&m_y1Value, SvPb::Y1EId, IDS_OBJECTNAME_EXTENT_Y1, true, SvOi::SVResetItemOwner, true);
	m_y1Value.SetDefaultValue(_variant_t(10L), true);
	RegisterEmbeddedObject(&m_x2Value, SvPb::X2EId, IDS_OBJECTNAME_EXTENT_X2, true, SvOi::SVResetItemOwner, true);
	m_x2Value.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_y2Value, SvPb::Y2EId, IDS_OBJECTNAME_EXTENT_Y2, true, SvOi::SVResetItemOwner, true);
	m_y2Value.SetDefaultValue(_variant_t(10L), true);
	RegisterEmbeddedObject(&m_x3Value, SvPb::X3EId, IDS_OBJECTNAME_X3, true, SvOi::SVResetItemOwner, true);
	m_x3Value.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_y3Value, SvPb::Y3EId, IDS_OBJECTNAME_Y3, true, SvOi::SVResetItemOwner, true);
	m_y3Value.SetDefaultValue(_variant_t(50L), true);
}
#pragma endregion Private Methods
} //namespace SvOp
