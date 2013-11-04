//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraDataInputObject
//* .File Name       : $Workfile:   SVCameraDataInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:44:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVCameraDataInputObject.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

SVCameraDataInputObject::SVCameraDataInputObject(LPCSTR strObjectName)
: SVInputObject(strObjectName)
{
}

SVCameraDataInputObject::SVCameraDataInputObject(SVObjectClass *pOwner, int StringResourceID)
: SVInputObject(pOwner, StringResourceID)
{
}

SVCameraDataInputObject::~SVCameraDataInputObject()
{
	Destroy();
}

HRESULT SVCameraDataInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;
	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraDataInputObject.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:44:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jan 2013 10:59:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
