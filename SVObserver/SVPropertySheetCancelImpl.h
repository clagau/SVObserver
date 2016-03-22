// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPropertySheetCancelImpl
// * .File Name       : $Workfile:   SVPropertySheetCancelImpl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 14:38:12  $
// ******************************************************************************

#pragma once

#include "ISVCancel.h"

class SVPropertySheetCancelImpl : public ISVCancel
{
public:
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& ppData);
	virtual HRESULT SetCancelData(SVCancelData* pData);
};

