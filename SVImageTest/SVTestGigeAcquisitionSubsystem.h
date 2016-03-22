//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeAcquisitionSubsystem.h
//* .File Name       : $Workfile:   SVTestGigeAcquisitionSubsystem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:13:24  $
//******************************************************************************

#pragma once

#include "SVTestAcquisitionSubsystem.h"

class SVTestGigeAcquisitionSubsystem : public SVTestAcquisitionSubsystem
{
public:
	SVTestGigeAcquisitionSubsystem();
	virtual ~SVTestGigeAcquisitionSubsystem();

	virtual SVTestAcquisitionClass* GetAcquisitionDevice( long selectedCamera );

};

