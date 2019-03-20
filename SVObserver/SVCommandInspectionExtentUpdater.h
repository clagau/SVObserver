//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class for synchronize method call (extent update methods) with the inspection thread. 
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVUtilityLibrary/SVGUID.h"
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

class SVCommandInspectionExtentUpdater
	/// Class for synchronize method call (extent update methods) with the inspection thread.
{
	#pragma region Constructor
public:
	SVCommandInspectionExtentUpdater(const SVCommandInspectionExtentUpdater& rObject) = delete;
	/// a pointer to SVImageExtentClass is used to avoid unnecessary copies in case of default arguments 
	SVCommandInspectionExtentUpdater(const SVGUID& rInspectionId, const SVGUID& rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass* pImageExtent = nullptr );

	virtual ~SVCommandInspectionExtentUpdater();
#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	//! Execute the extent updater. Should be called from the inspection thread and only if empty == false.
	//! \returns HRESULT
	//************************************
	HRESULT Execute();

	//************************************
	//! Return if this class is empty (no parameter set yet.)
	//! \returns bool
	//************************************
	bool empty() const;
#pragma endregion Public Methods
private:

#pragma region Member Variables
private:
	SVGUID m_InspectionId;
	SVGUID m_ToolId;
	SVImageExtentClass m_ImageExtent;
	SVCommandExtentUpdaterModeEnum m_mode;
#pragma endregion Member Variables
};

typedef std::shared_ptr< SVCommandInspectionExtentUpdater > SVCommandInspectionSetImageExtentPtr;
