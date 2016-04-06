//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionInitiator
//* .File Name       : $Workfile:   SVFileAcquisitionInitiator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:35:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVAcquisitionInitiator.h"
#pragma region Includes

class SVFileAcquisitionInitiator
{
private:
	SVAcquisitionInitiator m_acquisitionInitiator;

public:
	SVFileAcquisitionInitiator();
	SVFileAcquisitionInitiator(SVAcquisitionInitiator& rAcquisitionInitiator);
	~SVFileAcquisitionInitiator();

	void Create( SVAcquisitionInitiator& rFunc );
	void Destroy();

	HRESULT FireAcquisitionTrigger();

	static HRESULT CALLBACK TriggerCallback( void *p_pvOwner, void *p_pvData );
};
