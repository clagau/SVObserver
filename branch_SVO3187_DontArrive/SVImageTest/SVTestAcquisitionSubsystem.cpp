// ******************************************************************************
// * COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionSubsystem.cpp
// * .File Name       : $Workfile:   SVTestAcquisitionSubsystem.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:40  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVTestAcquisitionClass.h"
#pragma endregion Includes

SVTestAcquisitionSubsystem::SVTestAcquisitionSubsystem()
{
}

SVTestAcquisitionSubsystem::~SVTestAcquisitionSubsystem()
{
	Destroy();
}

bool SVTestAcquisitionSubsystem::Create( LPCTSTR DigitizerDLL)
{
	bool l_bOk = S_OK == m_svDigitizers.Open( DigitizerDLL );

	return l_bOk;
}

void SVTestAcquisitionSubsystem::Destroy()
{
	m_svDigitizers.Close();
}

SVTestAcquisitionClass* SVTestAcquisitionSubsystem::GetAcquisitionDevice( long selectedCamera )
{
	unsigned long digitizerHandle = m_svDigitizers.GetHandle(selectedCamera);
		
	return new SVTestAcquisitionClass(*this, digitizerHandle);
}

