//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawPointsTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw points use by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawPointsTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawPointsTask(LPCTSTR ObjectName);
	DrawPointsTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_POINTS);

	virtual ~DrawPointsTask();
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
