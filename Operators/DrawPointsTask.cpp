//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawPointsTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw points use by DrawTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DrawPointsTask.h"
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

SV_IMPLEMENT_CLASS(DrawPointsTask, SvPb::DrawPointsClassId);


#pragma region Constructor
DrawPointsTask::DrawPointsTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName, {})
{
	Initialize();
}

DrawPointsTask::DrawPointsTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID, {})
{
	Initialize();
}

DrawPointsTask::~DrawPointsTask()
{}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawPointsTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		std::vector<MIL_DOUBLE> xPoints;
		std::vector<MIL_DOUBLE> yPoints;
		m_xValues.getValues(xPoints);
		m_yValues.getValues(yPoints);
		auto numbers = std::min(xPoints.size(), yPoints.size());
		if (0 < numbers)
		{
			MgraDots(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), numbers, xPoints.data(), yPoints.data(), M_DEFAULT);
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
void DrawPointsTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawPointsTaskType;

	BuildEmbeddedObjectList();
}

void DrawPointsTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_xValues, SvPb::X1EId, IDS_OBJECTNAME_X_POINTS, true, SvOi::SVResetItemOwner, true);
	m_xValues.SetDefaultValue(SvUl::vectorToSafeArray<double>({50.}), true);
	RegisterEmbeddedObject(&m_yValues, SvPb::Y1EId, IDS_OBJECTNAME_Y_POINTS, true, SvOi::SVResetItemOwner, true);
	m_yValues.SetDefaultValue(SvUl::vectorToSafeArray<double>({50.}), true);
}
#pragma endregion Private Methods
} //namespace SvOp
