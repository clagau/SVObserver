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

#pragma region Includes
#include "ISVCancel.h"
#pragma endregion Includes

class SVPropertySheetCancelImpl : public ISVCancel
{
public:
	virtual ~SVPropertySheetCancelImpl();
	virtual bool CanCancel() override;
	virtual HRESULT GetCancelData(SVCancelData*& ppData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;
};

