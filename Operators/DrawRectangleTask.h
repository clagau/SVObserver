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
#include "SVValueObjectLibrary/LinkedValue.h"
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
	SvVol::LinkedValue       m_leftValue;
	SvVol::LinkedValue       m_topValue;
	SvVol::LinkedValue       m_width;
	SvVol::LinkedValue       m_height;
#pragma endregion Member Variables
};

} //namespace SvOp
