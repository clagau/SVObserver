//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestAcquisitionSubsystem.cpp
//* .File Name       : $Workfile:   SVTestGigeAcquisitionSubsystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:13:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTestGigeAcquisitionSubsystem.h"
#include "SVTestGigeAcquisitionClass.h"
#pragma endregion Includes

SVTestGigeAcquisitionSubsystem::SVTestGigeAcquisitionSubsystem()
: SVTestAcquisitionSubsystem()
{
}

SVTestGigeAcquisitionSubsystem::~SVTestGigeAcquisitionSubsystem()
{
}

SVTestAcquisitionClass* SVTestGigeAcquisitionSubsystem::GetAcquisitionDevice(long selectedCamera)
{
	unsigned long digitizerHandle = m_svDigitizers.GetHandle(selectedCamera);
		
	return new SVTestGigeAcquisitionClass(*this, digitizerHandle);
}

