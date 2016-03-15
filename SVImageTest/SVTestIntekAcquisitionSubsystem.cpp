// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekAcquisitionSubsystem.cpp
// * .File Name       : $Workfile:   SVTestIntekAcquisitionSubsystem.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVTestIntekAcquisitionSubsystem.h"
#include "SVTestIntekAcquisitionClass.h"

SVTestIntekAcquisitionSubsystem::SVTestIntekAcquisitionSubsystem()
: SVTestAcquisitionSubsystem()
{
}

SVTestIntekAcquisitionSubsystem::~SVTestIntekAcquisitionSubsystem()
{
}

SVTestAcquisitionClass* SVTestIntekAcquisitionSubsystem::GetAcquisitionDevice( long selectedCamera )
{
	unsigned long l_ulHandle = 0;

	m_svDigitizers.GetHandle( &l_ulHandle, selectedCamera );
		
	return new SVTestIntekAcquisitionClass( *this, l_ulHandle );
}

