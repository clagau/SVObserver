//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileInfoComparator
//* .File Name       : $Workfile:   SVFileInfoComparator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:44  $
//******************************************************************************
#include "stdafx.h"
#include "SVFileInfoComparator.h"
#include "SVFileInfo.h"

bool SVFileInfoComparator::operator()(const SVFileInfo& info1, const SVFileInfo& info2) const
{
	return info1.filename < info2.filename;
}

