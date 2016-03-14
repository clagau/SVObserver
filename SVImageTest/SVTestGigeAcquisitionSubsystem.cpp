//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestAcquisitionSubsystem.cpp
//* .File Name       : $Workfile:   SVTestGigeAcquisitionSubsystem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:13:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVTestGigeAcquisitionSubsystem.h"
#include "SVTestGigeAcquisitionClass.h"

SVTestGigeAcquisitionSubsystem::SVTestGigeAcquisitionSubsystem()
: SVTestAcquisitionSubsystem()
{
}

SVTestGigeAcquisitionSubsystem::~SVTestGigeAcquisitionSubsystem()
{
}

SVTestAcquisitionClass* SVTestGigeAcquisitionSubsystem::GetAcquisitionDevice( long selectedCamera )
{
	unsigned long l_ulHandle = 0;

	m_svDigitizers.GetHandle( &l_ulHandle, selectedCamera );
		
	return new SVTestGigeAcquisitionClass( *this, l_ulHandle );
}

