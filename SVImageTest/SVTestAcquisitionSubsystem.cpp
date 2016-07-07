// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionSubsystem.cpp
// * .File Name       : $Workfile:   SVTestAcquisitionSubsystem.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVTestAcquisitionClass.h"
#include "SVOMFCLibrary\SVDeviceParams.h"

SVTestAcquisitionSubsystem::SVTestAcquisitionSubsystem()
{
}

SVTestAcquisitionSubsystem::~SVTestAcquisitionSubsystem()
{
	Destroy();
}

bool SVTestAcquisitionSubsystem::Create(const CString& csDigitizerDLL)
{
	bool l_bOk = S_OK == m_svDigitizers.Open( csDigitizerDLL );

	return l_bOk;
}

void SVTestAcquisitionSubsystem::Destroy()
{
	m_svDigitizers.Close();
}

SVTestAcquisitionClass* SVTestAcquisitionSubsystem::GetAcquisitionDevice( long selectedCamera )
{
	unsigned long l_ulHandle = 0;

	m_svDigitizers.GetHandle( &l_ulHandle, selectedCamera );
		
	return new SVTestAcquisitionClass( *this, l_ulHandle );
}

