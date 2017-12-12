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
	ExtentUpdaterMode_SetImageExtentToParent,
	ExtentUpdaterMode_SetImageExtentToFit,
	ExtentUpdaterMode_ForwardExtent

};

//! the resetmode determine what tools are reseted after changing the extents
enum SVCommandExtentResetModeEnum
{
		ResetMode_Tool =0,
		ResetMode_ToolList,
		ResetMode_Inspection,
};

#pragma endregion Declarations

class SVCommandInspectionExtentUpdater
	/// Class for synchronize method call (extent update methods) with the inspection thread.
{
	#pragma region Constructor
public:
	SVCommandInspectionExtentUpdater(const SVCommandInspectionExtentUpdater& rObject);
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

	//************************************
	//!  The resetmode determine what tools are reseted after changing the extents
	//! \param  [in] ResetInspection
	//************************************
	void SetResetInspection(SVCommandExtentResetModeEnum);


#pragma endregion Public Methods
private:
	//************************************
	//! calls ResetAllObject for pObject, if PObject Uses SizeAdjust
	//! \param pObject [in]
	//! \returns 1 if no error occurs 
	//************************************
	static int ResetToolSizeAdjustTool(SVObjectClass* pObject);


#pragma region Member Variables
private:
	SVGUID m_InspectionId;
	SVGUID m_ToolId;
	SVImageExtentClass m_ImageExtent;
	SVCommandExtentUpdaterModeEnum m_mode;
	/// the resetmode determine what tools are reseted after changing the extents
	SVCommandExtentResetModeEnum m_ResetMode;
#pragma endregion Member Variables
};

typedef std::shared_ptr< SVCommandInspectionExtentUpdater > SVCommandInspectionSetImageExtentPtr;
