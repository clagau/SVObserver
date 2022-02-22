//*****************************************************************************
/// \copyright (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawOvalTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated to draw an oval use by DrawTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "DrawTask.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawOvalTask : public DrawTask
{
#pragma region Constructor
	SV_DECLARE_CLASS
public:
	explicit DrawOvalTask(LPCTSTR ObjectName);
	DrawOvalTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_OBJECTNAME_DRAW_OVAL);

	virtual ~DrawOvalTask();
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
	bool m_drawOutside = false;

	SvVol::LinkedValue       m_centerX;
	SvVol::LinkedValue       m_centerY;
	SvVol::LinkedValue       m_width;
	SvVol::LinkedValue       m_height;
#pragma endregion Member Variables
};

} //namespace SvOp
