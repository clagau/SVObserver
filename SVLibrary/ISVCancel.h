// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ISVCancel
// * .File Name       : $Workfile:   ISVCancel.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:13:38  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#pragma endregion Includes

struct SVCancelData
{
	SVCancelData() {}
	virtual ~SVCancelData() {}
};

class ISVCancel
{
public:
	virtual ~ISVCancel() {}
	virtual HRESULT GetCancelData(SVCancelData*& rpData) = 0;
	virtual HRESULT SetCancelData(SVCancelData* pData) = 0;
};
