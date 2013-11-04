//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManager
//* .File Name       : $Workfile:   SV1394CameraManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:17:54  $
//******************************************************************************

#include "stdafx.h"
#include "SV1394CameraManager.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


SV1394CameraManager::SV1394CameraManager()
: m_OrderedCamaras()
{
	GetCameraOrder( m_OrderedCamaras );
}

HRESULT SV1394CameraManager::Refresh()
{
	// Refresh will get information from the system and update the
	// SV1394CurrentCameraArray.

	HRESULT hr = S_OK;

	SVDigitizerProcessingClass::Instance().ConnectDevices();
	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

SV1394CameraManager& SV1394CameraManager::Instance()
{
	static SV1394CameraManager l_Instance;

	return l_Instance;
}

const SV1394CameraStructSet& SV1394CameraManager::GetCameraOrder() const
{
	return m_OrderedCamaras;
}

HRESULT SV1394CameraManager::UpdateCameraOrder( const SV1394CameraStructSet& rSVCameraList, bool ignoreHandles )
{
	HRESULT l_Status = S_OK;

	int iSize = rSVCameraList.GetSize();

	// Write Camera serial numbers to Camera Mapping
	for ( int x = 0; x < iSize; x++ )
	{
		CString strKey;
		CString l_Number;
		
		if( x < iSize )
		{
			if( (rSVCameraList[ x ].m_ulHandle != NULL) || ignoreHandles )
			{
				l_Number = rSVCameraList[ x ].strSerialNum;
			}
		}

		strKey.Format("Matrox_1394.Dig_%d", x);
		WritePrivateProfileString("Camera Mapping", strKey, l_Number, "c:\\SVObserver\\Bin\\SVIM.ini");
	}

	// Write Num Cameras to Camera Mapping
	CString sTmp;
	sTmp.Format( "%d", iSize ); 
	WritePrivateProfileString( "Camera Mapping",
		"Count",
		sTmp,
		"c:\\SVObserver\\Bin\\SVIM.ini" );

	SVConnectedCameraMap l_ConnectedCameras;

	GetConnectedCameras( l_ConnectedCameras );

	return OrderCameras( l_ConnectedCameras );
}

HRESULT SV1394CameraManager::UpdateConnectedCameras( const SV1394CameraStructSet& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	SVConnectedCameraMap l_ConnectedCameras;

	for( size_t i = 0; i < rSVCameraList.size(); ++i )
	{
		const SV1394CameraStruct& l_rCameraInfo = rSVCameraList[ i ];
		if( ! l_rCameraInfo.strSerialNum.IsEmpty() )
		{
			l_ConnectedCameras[ l_rCameraInfo.strSerialNum ] = l_rCameraInfo;
		}
	}

	return OrderCameras( l_ConnectedCameras );
}

void SV1394CameraManager::GetCameraOrder( SV1394CameraStructSet& rSVCameraList ) const
{
	int iSize = GetPrivateProfileInt( "Camera Mapping",
		"Count",
		3,
		"c:\\SVObserver\\Bin\\SVIM.ini" );

	rSVCameraList.clear();

	for ( int x = 0; x < iSize; x++ )
	{
		SV1394CameraStruct SVcs;
		
		SVString strKey;
		char szGetBuf[128];

		strKey.Format( "Matrox_1394.Dig_%d", x );
		GetPrivateProfileString( "Camera Mapping", strKey.ToString(), "", szGetBuf, 128, "c:\\SVObserver\\Bin\\SVIM.ini" );
		SVcs.strSerialNum = szGetBuf;

		SVcs.iPosition = x;
		SVcs.eChangeType = SV1394CameraStruct::SVNoChange;
		
		rSVCameraList.Add( SVcs );
	}
}

void SV1394CameraManager::GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const
{
	rSVCameraMap.clear();

	for( size_t i = 0; i < m_OrderedCamaras.size(); ++i )
	{
		const SV1394CameraStruct& l_rCameraInfo = m_OrderedCamaras[ i ];
		if( ! l_rCameraInfo.strSerialNum.IsEmpty() )
		{
			rSVCameraMap[ l_rCameraInfo.strSerialNum ] = l_rCameraInfo;
		}
	}
}

HRESULT SV1394CameraManager::OrderCameras( SVConnectedCameraMap& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	GetCameraOrder( m_OrderedCamaras );

	size_t l_Size = std::max< size_t >( m_OrderedCamaras.size(), rSVCameraList.size() );

	m_OrderedCamaras.resize( l_Size );

	for( size_t i = 0; i < l_Size; ++i )
	{
		SV1394CameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.HasSerialNumber() )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.find( l_rCamera.strSerialNum );

			if( l_Iter != rSVCameraList.end() )
			{
				l_rCamera = l_Iter->second;
				l_rCamera.eChangeType = SVCameraStructInterface::SVNoChange;

				rSVCameraList.erase( l_Iter );
			}
			else
			{
				l_rCamera.strModelName.Empty();
				l_rCamera.strVendorName.Empty();
				l_rCamera.eChangeType = SVCameraStructInterface::SVCameraRemoved;
				l_rCamera.m_ulHandle = NULL;
			}
		}

		l_rCamera.iPosition = i;
	}

	for( size_t i = 0; i < l_Size && !( rSVCameraList.empty() ); ++i )
	{
		SV1394CameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.m_ulHandle == NULL )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.begin();

			if( l_Iter != rSVCameraList.end() )
			{
				if( l_rCamera.HasSerialNumber() )
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SVCameraStructInterface::SVReplacedCamera;
				}
				else
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SVCameraStructInterface::SVNewCamera;
				}

				rSVCameraList.erase( l_Iter );
			}
		}

		l_rCamera.iPosition = i;
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV1394CameraManager.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:17:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   07 Jan 2013 14:45:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  804
 * SCR Title:  Fix Camera Manager Cancel function
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter to ignore handles for use in the cancel button of the camera manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Mar 2011 15:52:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   08 Dec 2010 07:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   04 Nov 2010 13:18:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   03 Mar 2010 13:46:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix a bug with a removed camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   03 Mar 2010 11:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to use a different functionality to manage the camera updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Feb 2010 12:22:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   27 Oct 2009 18:51:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Jun 2009 11:00:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to move SV1394CameraStruct to it's own module(s)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2007 11:48:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Jul 2006 10:34:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated code to allow the Camera Disconnect method to execute if the Connect method throws an exception.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Nov 2005 15:34:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Oct 2004 09:09:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added functionality to work correctly with different number of max cameras (for hub systems)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jan 2004 09:57:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to make the model and vendor name available in the camera manager dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Nov 2003 09:36:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
