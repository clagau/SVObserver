//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTimerValueObjectClass
//* .File Name       : $Workfile:   SVTimerValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:43:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVInt64ValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVTimerValueObjectClass : public SVInt64ValueObjectClass  
{
	SV_DECLARE_CLASS

public:
	explicit SVTimerValueObjectClass(LPCTSTR ObjectName);
	SVTimerValueObjectClass( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINT64VALUEOBJECT );
	virtual ~SVTimerValueObjectClass();

	void setStandardFormatString();

	void Start();
	bool Stop();

private:
	void LocalInitialize();

	double m_Start;  // Time Stamp in Milliseconds
};

} //namespace SvVol
