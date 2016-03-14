//******************************************************************************
//* COPYRIGHT (c) 2011 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageCopyUtility
//* .File Name       : $Workfile:   SVImageCopyUtility.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:22:28  $
//******************************************************************************
#ifndef INCL_SVIMAGECOPYUTILITY_H
#define INCL_SVIMAGECOPYUTILITY_H

#include "SVBitmapInfo.h"

class SVImageCopyUtility
{
public:
	virtual HRESULT Copy(const SVBitmapInfo& p_Info, const void* p_pSource) = 0;

protected:
	SVImageCopyUtility() {}
	virtual ~SVImageCopyUtility() {}
};

#endif

