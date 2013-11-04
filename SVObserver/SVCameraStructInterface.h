//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraStructInterface
//* .File Name       : $Workfile:   SVCameraStructInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:50:12  $
//******************************************************************************
#ifndef SVCAMERASTRUCTINTERFACE_H
#define SVCAMERASTRUCTINTERFACE_H

struct SVCameraStructInterface
{
	enum CameraChangeTypeEnum
	{
		SVUndefined,
		SVNoChange,
		SVNewCamera,
		SVReplacedCamera,
		SVCameraRemoved
	};

	SVCameraStructInterface();
	virtual ~SVCameraStructInterface();
	
	virtual bool HasSerialNumber() const=0;
	virtual CString GetSerialNumber() const=0;
	virtual bool HasModelName() const=0;
	virtual CString GetModelName() const=0;

	CameraChangeTypeEnum eChangeType;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraStructInterface.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:50:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 10:57:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
