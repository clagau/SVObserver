//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class for synchronize method call (extent update methods) with the inspection thread. 
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVImageLibrary/SVImageExtentClass.h"
#pragma endregion Includes

#pragma region Declarations
class SVObjectClass;

enum SVCommandExtentUpdaterModeEnum
{
	ExtentUpdaterMode_Undefined,
	ExtentUpdaterMode_SetImageExtent,
	ExtentUpdaterMode_SetImageExtentToFit,
};
#pragma endregion Declarations


class CommandInspectionExtentUpdater
	/// Class for synchronize method call (extent update methods) with the inspection thread.
{

public:
	
	/// a pointer to SVImageExtentClass is used to avoid unnecessary copies in case of default arguments 
	CommandInspectionExtentUpdater(uint32_t inspectionId, uint32_t toolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass* pImageExtent = nullptr);

	 ~CommandInspectionExtentUpdater();


#pragma region Public Methods
public:
	//************************************
	//! Execute the extent updater. Should be called from the inspection thread and only if empty == false.
	//! \returns HRESULT
	//************************************
	HRESULT operator()();

	//************************************
	//! Return if this class is empty (no parameter set yet.)
	//! \returns bool
	//************************************
	bool empty() const;
#pragma endregion Public Methods
private:

#pragma region Member Variables
private:
	uint32_t m_InspectionId;
	uint32_t m_ToolId;
	SVImageExtentClass m_ImageExtent;
	SVCommandExtentUpdaterModeEnum m_mode;
#pragma endregion Member Variables
};

