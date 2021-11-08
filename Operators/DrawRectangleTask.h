//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// \file DrawRectangleTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a rectangle use by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawRectangleTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawRectangleTask(LPCTSTR ObjectName);
	DrawRectangleTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_RECTANGLE);

	virtual ~DrawRectangleTask();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	virtual bool Run(SvOi::SVImageBufferHandlePtr pImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void Initialize();

	/// Build the embedded object list for this tool.
	void BuildEmbeddedObjectList();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	//@TODO[MZA][10.20][07.10.2021] SvVol::SVEnumerateValueObjectClass  m_drawArea;
	SvVol::LinkedValue       m_leftValue;
	SvVol::LinkedValue       m_topValue;
	SvVol::LinkedValue       m_width;
	SvVol::LinkedValue       m_height;

	SvVol::SVByteValueObjectClass  m_Color1Object;
	SvVol::SVByteValueObjectClass  m_Color2Object;
	SvVol::SVByteValueObjectClass  m_Color3Object;

	MIL_ID m_graphicContextId = M_NULL;
#pragma endregion Member Variables
};

} //namespace SvOp
