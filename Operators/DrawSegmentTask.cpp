//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// \file DrawSegmentTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a segment of an oval use by DrawTool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawSegmentTask.h"
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

SV_IMPLEMENT_CLASS(DrawSegmentTask, SvPb::DrawSegmentClassId);


#pragma region Constructor
DrawSegmentTask::DrawSegmentTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName, {gInsideAreaPair, gOutsideAreaPair, gContourAreaPair, gSectorAreaPair}, 1)
{
	Initialize();
}

DrawSegmentTask::DrawSegmentTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID, {gInsideAreaPair, gOutsideAreaPair, gContourAreaPair, gSectorAreaPair}, 1)
{
	Initialize();
}

DrawSegmentTask::~DrawSegmentTask()
{}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawSegmentTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea)
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
		double startAngle = 0;
		m_startAngle.getValue(startAngle);
		double stopAngle = 0;
		m_stopAngle.getValue(stopAngle);
		double xAxisAngle = 0;
		m_XAxisAngle.getValue(xAxisAngle);

		switch (drawArea)
		{
			case DrawArea::InsideArea:
			case DrawArea::OutsideArea:
				MgraArcAngle(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), centerX, centerY, width / 2, height / 2, startAngle, stopAngle, xAxisAngle, M_FILLED);
				break;
			case DrawArea::ContourArea:
				MgraArcAngle(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), centerX, centerY, width / 2, height / 2, startAngle, stopAngle, xAxisAngle, M_CONTOUR);
				break;
			case DrawArea::SectorArea:
				MgraArcAngle(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), centerX, centerY, width / 2, height / 2, startAngle, stopAngle, xAxisAngle, M_SECTOR);
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
void DrawSegmentTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawSegmentTaskType;

	BuildEmbeddedObjectList();
}

void DrawSegmentTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_centerX, SvPb::CenterXEId, IDS_OBJECTNAME_CENTER_X, true, SvOi::SVResetItemOwner);
	m_centerX.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_centerY, SvPb::CenterYEId, IDS_OBJECTNAME_CENTER_Y, true, SvOi::SVResetItemOwner);
	m_centerY.SetDefaultValue(_variant_t(50L), true);
	RegisterEmbeddedObject(&m_width, SvPb::WidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, true, SvOi::SVResetItemOwner);
	m_width.SetDefaultValue(_variant_t(80L), true);
	RegisterEmbeddedObject(&m_height, SvPb::HeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, true, SvOi::SVResetItemOwner);
	m_height.SetDefaultValue(_variant_t(80L), true);
	RegisterEmbeddedObject(&m_startAngle, SvPb::StartAngleEId, IDS_OBJECTNAME_START_ANGLE, true, SvOi::SVResetItemOwner);
	m_startAngle.SetDefaultValue(_variant_t(0.), true);
	RegisterEmbeddedObject(&m_stopAngle, SvPb::EndAngleEId, IDS_OBJECTNAME_END_ANGLE, true, SvOi::SVResetItemOwner);
	m_stopAngle.SetDefaultValue(_variant_t(360.), true);
	RegisterEmbeddedObject(&m_XAxisAngle, SvPb::XAxisAngleEId, IDS_OBJECTNAME_XAXIS_ANGLE, true, SvOi::SVResetItemOwner);
	m_XAxisAngle.SetDefaultValue(_variant_t(0.), true);
}
#pragma endregion Private Methods
} //namespace SvOp
