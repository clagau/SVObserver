// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestAcquisitionSubsystem.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:42  $
// ******************************************************************************

#pragma once

#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"

class SVTestAcquisitionClass;

class SVTestAcquisitionSubsystem
{

public:
	SVDigitizerLoadLibraryClass m_svDigitizers;

public:
	SVTestAcquisitionSubsystem();
	virtual ~SVTestAcquisitionSubsystem();

	virtual bool Create(const CString& sDLLname);
	virtual void Destroy();

	virtual SVTestAcquisitionClass* GetAcquisitionDevice( long selectedCamera );

};

