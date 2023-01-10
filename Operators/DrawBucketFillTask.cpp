//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawBucketFillTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a bucket fill by DrawTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DrawBucketFillTask.h"
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

SV_IMPLEMENT_CLASS(DrawBucketFillTask, SvPb::DrawBucketFillClassId);


#pragma region Constructor
DrawBucketFillTask::DrawBucketFillTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName, {})
{
	Initialize();
}

DrawBucketFillTask::DrawBucketFillTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID, {})
{
	Initialize();
}

DrawBucketFillTask::~DrawBucketFillTask()
{}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawBucketFillTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		MIL_DOUBLE xPoint;
		MIL_DOUBLE yPoint;
		m_xValue.getValue(xPoint);
		m_yValue.getValue(yPoint);
		MgraFill(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), xPoint, yPoint);
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
void DrawBucketFillTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawBucketFillTaskType;

	BuildEmbeddedObjectList();
}

void DrawBucketFillTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_xValue, SvPb::X1EId, IDS_OBJECTNAME_X, true, SvOi::SVResetItemOwner, true);
	m_xValue.SetDefaultValue(10, true);
	RegisterEmbeddedObject(&m_yValue, SvPb::Y1EId, IDS_OBJECTNAME_Y, true, SvOi::SVResetItemOwner, true);
	m_yValue.SetDefaultValue(10, true);
}
#pragma endregion Private Methods
} //namespace SvOp
