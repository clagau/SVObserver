// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestIntekAcquisitionSubsystem.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:22  $
// ******************************************************************************

#pragma once

#include "SVTestAcquisitionSubsystem.h"

class SVTestIntekAcquisitionSubsystem : public SVTestAcquisitionSubsystem
{
public:
	SVTestIntekAcquisitionSubsystem();
	virtual ~SVTestIntekAcquisitionSubsystem();

	virtual SVTestAcquisitionClass* GetAcquisitionDevice( long selectedCamera );

};

