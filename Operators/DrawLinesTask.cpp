//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawLinesTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw lines use by DrawTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DrawLinesTask.h"
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

SV_IMPLEMENT_CLASS(DrawLinesTask, SvPb::DrawLineClassId);


#pragma region Constructor
DrawLinesTask::DrawLinesTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName, {gNormalAreaPair, gInfinityLinesAreaPair}, 6)
{
	Initialize();
}

DrawLinesTask::DrawLinesTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID, {gNormalAreaPair, gInfinityLinesAreaPair}, 6)
{
	Initialize();
}

DrawLinesTask::~DrawLinesTask()
{}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawLinesTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		std::vector<MIL_DOUBLE> x1Points;
		std::vector<MIL_DOUBLE> y1Points;
		m_x1Values.getValues(x1Points);
		m_y1Values.getValues(y1Points);
		std::vector<MIL_DOUBLE> x2Points;
		std::vector<MIL_DOUBLE> y2Points;
		m_x2Values.getValues(x2Points);
		m_y2Values.getValues(y2Points);
		auto numbers = std::min({x1Points.size(), y1Points.size(), x2Points.size(), y2Points.size()});
		if (0 < numbers)
		{
			switch (drawArea)
			{
				case DrawArea::NormalArea:
					MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), numbers, x1Points.data(), y1Points.data(), x2Points.data(), y2Points.data(), M_LINE_LIST);
					break;
				case DrawArea::InfiniteArea:
					MgraLines(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), numbers, x1Points.data(), y1Points.data(), x2Points.data(), y2Points.data(), M_INFINITE_LINES);
					break;
			}
		}
		return true;
	}
	else
	{
		Log_Assert(false);
		return false;
	}
}
#pragma endregion Protected Methods

#pragma region Private Methods
void DrawLinesTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawLineTaskType;

	BuildEmbeddedObjectList();
}

void DrawLinesTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_x1Values, SvPb::X1EId, IDS_OBJECTNAME_X_POINTS, true, SvOi::SVResetItemOwner, true);
	m_x1Values.SetDefaultValue(SvUl::vectorToSafeArray<double>({0.}), true);
	RegisterEmbeddedObject(&m_y1Values, SvPb::Y1EId, IDS_OBJECTNAME_Y_POINTS, true, SvOi::SVResetItemOwner, true);
	m_y1Values.SetDefaultValue(SvUl::vectorToSafeArray<double>({0.}), true);
	RegisterEmbeddedObject(&m_x2Values, SvPb::X2EId, IDS_OBJECTNAME_X2_POINTS, true, SvOi::SVResetItemOwner, true);
	m_x2Values.SetDefaultValue(SvUl::vectorToSafeArray<double>({10.}), true);
	RegisterEmbeddedObject(&m_y2Values, SvPb::Y2EId, IDS_OBJECTNAME_Y2_POINTS, true, SvOi::SVResetItemOwner, true);
	m_y2Values.SetDefaultValue(SvUl::vectorToSafeArray<double>({50.}), true);
}
#pragma endregion Private Methods
} //namespace SvOp
