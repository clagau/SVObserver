//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileInfoComparator
//* .File Name       : $Workfile:   SVFileInfoComparator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:48  $
//******************************************************************************

#pragma once

struct SVFileInfo;

struct SVFileInfoComparator
{
public:
	bool operator()(const SVFileInfo& rLhs, const SVFileInfo& rRhs) const
	{
		return rLhs.filename < rRhs.filename;
	}
};


