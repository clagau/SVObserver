//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuiExtentUpdater
//* .File Name       : $Workfile:   SVGuiExtentUpdater.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Feb 2015 03:00:46  $
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVCommandInspectionExtentUpdater.h"
#pragma endregion Includes

namespace SvIe
{
class SVTaskObjectClass;
}

class SVGuiExtentUpdater
{
public:
	static HRESULT SetImageExtent(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	static HRESULT SetImageExtentToParent(SvIe::SVTaskObjectClass* pTaskObject, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	static HRESULT SetImageExtentToFit(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	
	//************************************
	//! Update the size of the Image in pTool 
	//! and if forward size is true 	all Images which are dependent from this Image
	//! \param p_pTaskObject [in]
	//! \param ForwardSize [in]
	//! \returns HRESULT
	//************************************
	static HRESULT ForwardSizeAndPosition(SvIe::SVTaskObjectClass* pTaskObject, SVCommandExtentResetModeEnum resetMode  ) ;

private:
	static const int TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS = 120000;
};
