//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileInfoComparator
//* .File Name       : $Workfile:   SVFileInfoComparator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:48  $
//******************************************************************************
#ifndef SVFILEINFOCOMPARATOR_H
#define SVFILEINFOCOMPARATOR_H

#include <functional>
struct SVFileInfo;

class SVFileInfoComparator : public std::binary_function<SVFileInfo, SVFileInfo, bool>
{
public:
	bool operator()(const SVFileInfo& info1, const SVFileInfo& info2) const;
};

#endif

