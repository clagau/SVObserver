//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraStructInterface
//* .File Name       : $Workfile:   SVGigeCameraStructInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:48:52  $
//******************************************************************************
#ifndef SVGIGECAMERASTRUCTINTERFACE_H
#define SVGIGECAMERASTRUCTINTERFACE_H

struct SVGigeCameraStructInterface
{
	enum CameraChangeTypeEnum
	{
		SVUndefined,
		SVNoChange,
		SVNewCamera,
		SVReplacedCamera,
		SVCameraRemoved
	};

	SVGigeCameraStructInterface();
	virtual ~SVGigeCameraStructInterface();
	
	virtual bool HasSerialNumber() const=0;
	virtual CString GetSerialNumber() const=0;
	virtual bool HasModelName() const=0;
	virtual CString GetModelName() const=0;

	virtual bool HasIPAddress() const=0;
	virtual CString GetIPAddress() const=0;

	CameraChangeTypeEnum eChangeType;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGigeCameraStructInterface.h_v  $
 * 
 *    Rev 1.0   12 Jun 2013 15:48:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 May 2013 10:19:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
