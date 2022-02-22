//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawPolygonTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a polygon or a polyline use by DrawTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DrawPolygonTask.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(DrawPolygonTask, SvPb::DrawPolygonClassId);


#pragma region Constructor
DrawPolygonTask::DrawPolygonTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName, {gInsideAreaPair, gOutsideAreaPair, gContourAreaPair, gPolylineAreaPair}, 1)
{
	Initialize();
}

DrawPolygonTask::DrawPolygonTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID, {gInsideAreaPair, gOutsideAreaPair, gContourAreaPair, gPolylineAreaPair}, 1)
{
	Initialize();
}

DrawPolygonTask::~DrawPolygonTask()
{}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawPolygonTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		std::vector<MIL_DOUBLE> xPoints;
		std::vector<MIL_DOUBLE> yPoints;
		m_xValues.getValues(xPoints);
		m_yValues.getValues(yPoints);
		auto numbers = std::min(xPoints.size(), yPoints.size());
		if (1 < numbers)
		{
			switch (drawArea)
			{
				case DrawArea::InsideArea:
				case DrawArea::OutsideArea:
					MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), numbers, xPoints.data(), yPoints.data(), M_NULL, M_NULL, M_POLYGON | M_FILLED);
					break;
				case DrawArea::ContourArea:
					MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), numbers, xPoints.data(), yPoints.data(), M_NULL, M_NULL, M_POLYGON);
					break;
				case DrawArea::PolylineArea:
					MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), numbers, xPoints.data(), yPoints.data(), M_NULL, M_NULL, M_POLYLINE);
					break;
			}
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
void DrawPolygonTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawPolygonTaskType;

	BuildEmbeddedObjectList();
}

void DrawPolygonTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_xValues, SvPb::X1EId, IDS_OBJECTNAME_X_POINTS, true, SvOi::SVResetItemOwner);
	m_xValues.SetDefaultValue(SvUl::vectorToSafeArray<double>({0.,10.,50.}), true);
	RegisterEmbeddedObject(&m_yValues, SvPb::Y1EId, IDS_OBJECTNAME_Y_POINTS, true, SvOi::SVResetItemOwner);
	m_yValues.SetDefaultValue(SvUl::vectorToSafeArray<double>({0.,50.,50.}), true);
}
#pragma endregion Private Methods
} //namespace SvOp
