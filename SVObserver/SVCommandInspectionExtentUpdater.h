//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class for synchronize method call (extent update methods) with the inspection thread. 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#pragma endregion Includes

#pragma region Declarations
enum SVCommandExtentUpdaterModeEnum
{
	ExtentUpdaterMode_Undefined,
	ExtentUpdaterMode_SetImageExtent,
	ExtentUpdaterMode_SetImageExtentToParent,
	ExtentUpdaterMode_SetImageExtentToFit
};
#pragma endregion Declarations

class SVCommandInspectionExtentUpdater
	/// Class for synchronize method call (extent update methods) with the inspection thread.
{
	#pragma region Constructor
public:
	SVCommandInspectionExtentUpdater();
	SVCommandInspectionExtentUpdater(const SVCommandInspectionExtentUpdater& rObject);
	SVCommandInspectionExtentUpdater(const SVGUID& rInspectionId, const SVGUID& rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass& rImageExtent = SVImageExtentClass());

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

	//************************************
	//! Reset member parameter of this class.
	//! \returns void
	//************************************
	void clear();

	//************************************
	//! PLEASE ENTER A DESCRIPTION
	//! \param rInspectionId [in] GUID of the inspection.
	//! \param rToolId [in] GUID of the tool.
	//! \param mode [in] Mode in which the Execute should run.
	//! \param rImageExtent [in] Image extent parameter, can be empty if not used in this mode (e.g. ExtentUpdater_SetImageExtentToParent)
	//! \returns HRESULT
	//************************************
	HRESULT SetCommandData(const SVGUID& rInspectionId, const SVGUID& rToolId, SVCommandExtentUpdaterModeEnum mode, const SVImageExtentClass& rImageExtent = SVImageExtentClass());
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SVGUID m_InspectionId;
	SVGUID m_ToolId;
	SVImageExtentClass m_ImageExtent;
	SVCommandExtentUpdaterModeEnum m_mode;
#pragma endregion Member Variables
};

typedef SVSharedPtr< SVCommandInspectionExtentUpdater > SVCommandInspectionSetImageExtentPtr;
