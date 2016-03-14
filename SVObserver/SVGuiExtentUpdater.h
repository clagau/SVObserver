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
#ifndef INCL_SVGUIEXTENTUPDATER_H
#define INCL_SVGUIEXTENTUPDATER_H

#include "SVTaskObject.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVCommandInspectionExtentUpdater.h"

class SVGuiExtentUpdater
{
public:
	static HRESULT SetImageExtent(SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	static HRESULT SetImageExtentToParent(SVTaskObjectClass* pTaskObject, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	static HRESULT SetImageExtentToFit(SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents, SVCommandExtentResetModeEnum resetMode = ResetMode_Tool);
	
	//************************************
	//! Update the size of the Image in pTool 
	//! and if forward size is true 	all Images which are dependent from this Image
	//! \param p_pTaskObject [in]
	//! \param ForwardSize [in]
	//! \returns HRESULT
	//************************************
	static HRESULT ForwardSizeAndPosition(SVTaskObjectClass* p_pTaskObject, SVCommandExtentResetModeEnum resetMode  ) ;

private:
	static const int TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS = 120000;
};

#endif

