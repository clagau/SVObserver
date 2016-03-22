//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraDatabase
//* .File Name       : $Workfile:   SVGigeCameraDatabase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:48  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <comdef.h>
#include "SVUtilityLibrary/SVString.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"

typedef std::deque<_bstr_t> SVGigeEnumNameList;

class SVGigeCameraDatabase
{
	// Enum to String mapping
	typedef std::map<SVGigeParameterEnum, _bstr_t> SVGigeEnumToString;

private:
	static const SVGigeEnumToString m_enumToString;

	SVGigeCameraDatabase();
	~SVGigeCameraDatabase();

public:
	static long GetFeatureCount();
	static HRESULT GetFeatureNames(SVGigeEnumNameList& list);
};

