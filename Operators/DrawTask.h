//*****************************************************************************
/// \copyright (c) 2021,2021 by Seidenader Maschinenbau GmbH
/// \file DrawTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Virtual class for DrawTask
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#pragma endregion Includes

namespace SvOp //< Operators
{

class DrawTask : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
public:
	explicit DrawTask(LPCTSTR ObjectName) : SVTaskObjectClass(ObjectName) {};
	DrawTask(SVObjectClass* POwner, int StringResourceID) : SVTaskObjectClass(POwner, StringResourceID) {};
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool Run(SvOi::SVImageBufferHandlePtr pImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) = 0;
	void setColorImageFlag(bool useColorImage) { m_useColorImage = useColorImage; };
#pragma endregion Public Methods

#pragma region Member Variables
protected:
	bool m_useColorImage = false;
#pragma endregion Member Variables
};

} //namespace SvOp