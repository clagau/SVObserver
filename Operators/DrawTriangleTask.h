//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// \file DrawTriangleTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a triangle use by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawTriangleTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawTriangleTask(LPCTSTR ObjectName);
	DrawTriangleTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_TRIANGLE);

	virtual ~DrawTriangleTask();
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
	SvVol::LinkedValue       m_x1Value;
	SvVol::LinkedValue       m_y1Value;
	SvVol::LinkedValue       m_x2Value;
	SvVol::LinkedValue       m_y2Value;
	SvVol::LinkedValue       m_x3Value;
	SvVol::LinkedValue       m_y3Value;
#pragma endregion Member Variables
};

} //namespace SvOp
