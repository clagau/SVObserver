//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawTextTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a text by DrawTool
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "DrawTextTask.h"
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

SV_IMPLEMENT_CLASS(DrawTextTask, SvPb::DrawTextClassId);


#pragma region Constructor
DrawTextTask::DrawTextTask(LPCTSTR ObjectName)
	: DrawTask(ObjectName, {gNormalAreaPair}, 1)
{
	Initialize();
}

DrawTextTask::DrawTextTask(SVObjectClass* POwner, int StringResourceID)
	: DrawTask(POwner, StringResourceID, {gNormalAreaPair}, 1)
{
	Initialize();
}

DrawTextTask::~DrawTextTask()
{
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Protected Methods
bool DrawTextTask::drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea)
{
	if (nullptr != pImageHandle && false == pImageHandle->empty())
	{
		double scaleX;
		double scaleY;
		m_fontScaleX.getValue(scaleX);
		m_fontScaleY.getValue(scaleY);
		MgraControl(getGraphicContextId(), M_FONT_X_SCALE, scaleX);
		MgraControl(getGraphicContextId(), M_FONT_Y_SCALE, scaleY);
		long fontSize;
		m_fontSizeEnumObject.GetValue(fontSize);
		MgraFont(getGraphicContextId(), fontSize);
		BOOL isTransparent = true;
		m_IsBGTransparentObject.GetValue(isTransparent);
		if (isTransparent)
		{
			MgraControl(getGraphicContextId(), M_BACKGROUND_MODE, M_TRANSPARENT);
		}
		else
		{
			MgraControl(getGraphicContextId(), M_BACKGROUND_MODE, M_OPAQUE);
			byte color1 = 0;
			m_BGColor1Object.GetValue(color1);
			MIL_DOUBLE bgColor = color1;
			if (isColorImage())
			{
				byte color2 = 0;
				m_BGColor2Object.GetValue(color2);
				byte color3 = 0;
				m_BGColor3Object.GetValue(color3);
				bgColor = M_RGB888(color1, color2, color3);
			}
			MgraBackColor(getGraphicContextId(), bgColor);
		}

		MIL_DOUBLE xPoint;
		MIL_DOUBLE yPoint;
		m_xValue.getValue(xPoint);
		m_yValue.getValue(yPoint);
		std::string text;
		m_text.getValue(text);
		MgraText(getGraphicContextId(), pImageHandle->GetBuffer().GetIdentifier(), xPoint, yPoint, text);
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
void DrawTextTask::Initialize()
{
	// Set up your type
	m_ObjectTypeInfo.m_ObjectType = SvPb::DrawTaskType;
	m_ObjectTypeInfo.m_SubType = SvPb::DrawTextTaskType;

	BuildEmbeddedObjectList();
}

SvOi::NameValueVector g_SizeEnumList = {{_T("Small"), 1}, {_T("Medium"), 2}, {_T("Large"), 3}};
void DrawTextTask::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_IsBGTransparentObject, SvPb::IsBackgroundTransparentEId, IDS_OBJECTNAME_IS_BACKGROUND_TRANSPARENT, false, SvOi::SVResetItemOwner);
	m_IsBGTransparentObject.SetDefaultValue(true, true);
	RegisterEmbeddedObject(&m_BGColor1Object, SvPb::BackgroundColor1EId, IDS_BACKGROUND_COLOR1, false, SvOi::SVResetItemOwner);
	m_BGColor1Object.SetDefaultValue(0, true);
	RegisterEmbeddedObject(&m_BGColor2Object, SvPb::BackgroundColor2EId, IDS_BACKGROUND_COLOR2, false, SvOi::SVResetItemOwner);
	m_BGColor2Object.SetDefaultValue(0, true);
	RegisterEmbeddedObject(&m_BGColor3Object, SvPb::BackgroundColor3EId, IDS_BACKGROUND_COLOR3, false, SvOi::SVResetItemOwner);
	m_BGColor3Object.SetDefaultValue(0, true);

	RegisterEmbeddedObject(&m_xValue, SvPb::X1EId, IDS_OBJECTNAME_X, true, SvOi::SVResetItemOwner);
	m_xValue.SetDefaultValue(10, true);
	RegisterEmbeddedObject(&m_yValue, SvPb::Y1EId, IDS_OBJECTNAME_Y, true, SvOi::SVResetItemOwner);
	m_yValue.SetDefaultValue(10, true);
	RegisterEmbeddedObject(&m_text, SvPb::TextEId, IDS_OBJECTNAME_DRAW_TEXT, true, SvOi::SVResetItemOwner);
	m_text.SetDefaultValue("", true);
	
	RegisterEmbeddedObject(&m_fontSizeEnumObject, SvPb::FontSizeEId, IDS_OBJECTNAME_FONTSIZE, true, SvOi::SVResetItemOwner);
	m_fontSizeEnumObject.SetEnumTypes(g_SizeEnumList);
	m_fontSizeEnumObject.SetDefaultValue(2);
	RegisterEmbeddedObject(&m_fontScaleX, SvPb::FontScaleXEId, IDS_OBJECTNAME_FONT_SCALEX, true, SvOi::SVResetItemOwner);
	m_fontScaleX.SetDefaultValue(1., true);
	RegisterEmbeddedObject(&m_fontScaleY, SvPb::FontScaleYEId, IDS_OBJECTNAME_FONT_SCALEY, true, SvOi::SVResetItemOwner);
	m_fontScaleY.SetDefaultValue(1., true);
}
#pragma endregion Private Methods
} //namespace SvOp
