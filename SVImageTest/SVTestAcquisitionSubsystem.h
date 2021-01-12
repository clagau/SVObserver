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

#pragma region Includes
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#pragma endregion Includes

class SVTestAcquisitionClass;

class SVTestAcquisitionSubsystem
{

public:
	SvTh::SVDigitizerLoadLibraryClass m_svDigitizers;

public:
	SVTestAcquisitionSubsystem();
	virtual ~SVTestAcquisitionSubsystem();

	bool Create( LPCTSTR DLLname );
	void Destroy();

	virtual SVTestAcquisitionClass* GetAcquisitionDevice( long selectedCamera );

};

