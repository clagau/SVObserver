//*****************************************************************************
/// \copyright (c) 2022,2022 by Seidenader Maschinenbau GmbH
/// \file DrawPolygonTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw a polygon or a polyline use by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawPolygonTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawPolygonTask(LPCTSTR ObjectName);
	DrawPolygonTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_POLYGON);

	virtual ~DrawPolygonTask();
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
	SvVol::LinkedValue       m_xValues;
	SvVol::LinkedValue       m_yValues;
#pragma endregion Member Variables
};

} //namespace SvOp
