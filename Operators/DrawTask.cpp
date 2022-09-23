//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a rectangle use by DrawTool
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DrawTask.h"
#include "Definitions/SVImageFormatEnum.h"
#include "InspectionEngine/SVImageProcessingClass.h"
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

#pragma region Constructor
DrawTask::DrawTask(LPCTSTR ObjectName, SvDef::NameValueVector drawAreaList, long defaultDrawArea)
	: SVTaskObjectClass(ObjectName)
{
	initialize(drawAreaList, defaultDrawArea);
}

DrawTask::DrawTask(SVObjectClass* POwner, int StringResourceID, SvDef::NameValueVector drawAreaList, long defaultDrawArea)
	: SVTaskObjectClass(POwner, StringResourceID)
{
	initialize(drawAreaList, defaultDrawArea);
}

DrawTask::~DrawTask()
{
	MgraFree(m_graphicContextId);
}
#pragma endregion Constructor

#pragma region Public Methods
bool DrawTask::Run(SvOi::SVImageBufferHandlePtr pImageHandle, SvIe::RunStatus&, SvStl::MessageContainerVector*)
{
	bool retValue {true};
	if (DrawArea::OutsideArea == m_drawArea)
	{
		auto pTmpImage = getTmpImage();
		if (nullptr != pTmpImage && false == pTmpImage->empty())
		{
			MbufClear(pTmpImage->GetBuffer().GetIdentifier(), getColor());
			MgraColor(M_DEFAULT, getTmpColor());
			retValue = drawImage(pTmpImage, m_drawArea);
			MbufCopyCond(pTmpImage->GetBuffer().GetIdentifier(), pImageHandle->GetBuffer().GetIdentifier(), pTmpImage->GetBuffer().GetIdentifier(), M_NOT_EQUAL, getTmpColor());
		}
	}
	else
	{
		retValue = drawImage(pImageHandle, m_drawArea);
	}

	return retValue;
}

bool DrawTask::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	long drawArea = 1;
	m_drawAreaObject.GetValue(drawArea);
	m_drawArea = static_cast<DrawArea>(drawArea);

	byte color1 = 0;
	m_Color1Object.GetValue(color1);
	if (isColorImage())
	{
		byte color2 = 0;
		m_Color2Object.GetValue(color2);
		byte color3 = 0;
		m_Color3Object.GetValue(color3);
		m_color = M_RGB888(color1, color2, color3);
		m_tmpColor = M_RGB888(color1+1, color2, color3);
	}
	else
	{
		m_color = color1;
		m_tmpColor = color1 + 1;
	}

	if (DrawArea::OutsideArea == m_drawArea)
	{
		SvIe::SVImageProcessingClass::CreateImageBuffer(m_imageInfo, m_pTmpImage);
		MgraControl(m_graphicContextId, M_COLOR, m_tmpColor);
	}
	else
	{
		m_pTmpImage = nullptr;
		MgraControl(m_graphicContextId, M_COLOR, m_color);
	}

	return Result;
}

bool DrawTask::isColorImage() const
{
	long value;
	m_imageInfo.GetImageProperty(SvDef::SVImagePropertyFormat, value);
	return (SvDef::SVImageFormatMono8 != value);
}
#pragma endregion Public Methods

#pragma region Private Methods
void DrawTask::initialize(const SvDef::NameValueVector& rDrawAreaList, long defaultDrawArea)
{
	MgraAlloc(M_DEFAULT_HOST, &m_graphicContextId);
	buildEmbeddedObjectList(rDrawAreaList, defaultDrawArea);
}

void DrawTask::buildEmbeddedObjectList(const SvDef::NameValueVector& rDrawAreaList, long defaultDrawArea)
{
	RegisterEmbeddedObject(&m_Color1Object, SvPb::Color1EId, IDS_COLOR1, false, SvOi::SVResetItemOwner);
	m_Color1Object.SetDefaultValue(0, false);
	RegisterEmbeddedObject(&m_Color2Object, SvPb::Color2EId, IDS_COLOR2, false, SvOi::SVResetItemOwner);
	m_Color2Object.SetDefaultValue(0, false);
	RegisterEmbeddedObject(&m_Color3Object, SvPb::Color3EId, IDS_COLOR3, false, SvOi::SVResetItemOwner);
	m_Color3Object.SetDefaultValue(0, false);

	if (false == rDrawAreaList.empty())
	{
		RegisterEmbeddedObject(&m_drawAreaObject, SvPb::DrawAreaEId, IDS_OBJECT_DRAW_AREA, false, SvOi::SVResetItemOwner);
		m_drawAreaObject.SetEnumTypes(rDrawAreaList);
		m_drawAreaObject.SetDefaultValue(defaultDrawArea);
	}
}
#pragma endregion Private Methods
} //namespace SvOp
