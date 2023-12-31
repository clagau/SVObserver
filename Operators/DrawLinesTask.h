//*****************************************************************************
/// \copyright (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawLinesTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw lines use by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawLinesTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawLinesTask(LPCTSTR ObjectName);
	DrawLinesTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_LINES);

	virtual ~DrawLinesTask();
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
	SvVol::LinkedValue       m_x1Values;
	SvVol::LinkedValue       m_y1Values;
	SvVol::LinkedValue       m_x2Values;
	SvVol::LinkedValue       m_y2Values;
#pragma endregion Member Variables
};

} //namespace SvOp
