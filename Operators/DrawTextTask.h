//*****************************************************************************
/// \copyright (c) 2022,2022 by Seidenader Maschinenbau GmbH
/// \file DrawTextTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a text by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawTextTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawTextTask(LPCTSTR ObjectName);
	DrawTextTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_TEXT);

	virtual ~DrawTextTask();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	SvVol::SVBoolValueObjectClass m_IsBGTransparentObject;
	bool m_isTransparent = false;
	SvVol::SVByteValueObjectClass  m_BGColor1Object;
	SvVol::SVByteValueObjectClass  m_BGColor2Object;
	SvVol::SVByteValueObjectClass  m_BGColor3Object;
	MIL_DOUBLE m_bgColor = 0;

	SvVol::LinkedValue       m_xValue;
	SvVol::LinkedValue       m_yValue;
	SvVol::LinkedValue       m_text;
	SvVol::SVEnumerateValueObjectClass m_fontSizeEnumObject;
	SvVol::LinkedValue       m_fontScaleX;
	SvVol::LinkedValue       m_fontScaleY;
#pragma endregion Member Variables
};

} //namespace SvOp
