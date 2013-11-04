//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionInitiator
//* .File Name       : $Workfile:   SVFileAcquisitionInitiator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:35:46  $
//******************************************************************************
#include "stdafx.h"
#include "SVFileAcquisitionInitiator.h"

SVFileAcquisitionInitiator::SVFileAcquisitionInitiator()
{
}

SVFileAcquisitionInitiator::~SVFileAcquisitionInitiator()
{
}

SVFileAcquisitionInitiator::SVFileAcquisitionInitiator(SVAcquisitionInitiator& rAcquisitionInitiator)
: m_acquisitionInitiator(rAcquisitionInitiator)
{
}

void SVFileAcquisitionInitiator::Create( SVAcquisitionInitiator& rFunc )
{
	m_acquisitionInitiator = rFunc;
}

void SVFileAcquisitionInitiator::Destroy()
{
	m_acquisitionInitiator = SVAcquisitionInitiator();
}

HRESULT SVFileAcquisitionInitiator::FireAcquisitionTrigger()
{
	HRESULT hr = m_acquisitionInitiator.Exec();
	return hr;
}

HRESULT CALLBACK SVFileAcquisitionInitiator::TriggerCallback( void *p_pvOwner, void *p_pvData )
{
	HRESULT hrOk = S_OK;

	if ( p_pvOwner )
	{
		try
		{
			SVFileAcquisitionInitiator* pDevice = (SVFileAcquisitionInitiator *)p_pvOwner;
			hrOk = pDevice->FireAcquisitionTrigger();
		}
		catch ( ... )
		{
		}
	}
	return hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileAcquisitionInitiator.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:35:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Oct 2008 16:28:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/