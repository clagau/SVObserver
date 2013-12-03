//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManager
//* .File Name       : $Workfile:   SVGigeCameraManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeCameraManager.h"

#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVGigeCameraManager::SVGigeCameraManager()
: m_OrderedCamaras()
{
	GetCameraOrder( m_OrderedCamaras );
	GetCameraOrder(m_iniCameras);
}

HRESULT SVGigeCameraManager::Refresh()
{
	// Refresh will get information from the system and update the
	// SVGigeCurrentCameraArray.
	// 

	HRESULT hr = S_OK;

	SVDigitizerProcessingClass::Instance().ScanForCameras();
	SVDigitizerProcessingClass::Instance().ConnectDevices();
	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

SVGigeCameraManager& SVGigeCameraManager::Instance()
{
	static SVGigeCameraManager l_Instance;

	return l_Instance;
}

const SVGigeCameraStructSet& SVGigeCameraManager::GetCameraOrder() const
{
	return m_OrderedCamaras;
}

const SVGigeCameraStructSet& SVGigeCameraManager::GetOriginalCameraIniOrder() const
{
	return m_iniCameras;
}

void SVGigeCameraManager::UpdateCameraIniList()
{
	GetCameraOrder(m_iniCameras);
}

HRESULT SVGigeCameraManager::UpdateCameraOrder( const SVGigeCameraStructSet& rSVCameraList, bool ignoreHandles )
{
	HRESULT l_Status = S_OK;

	int iSize = rSVCameraList.GetSize();

	// Write Camera IP Address to Camera Mapping
	for ( int x = 0; x < iSize; x++ )
	{
		CString strKey;
		CString l_Number;
		if( (rSVCameraList[x].m_ulHandle != NULL) || ignoreHandles )
		{
			l_Number = rSVCameraList[ x ].strIPAddress;
		}

		strKey.Format("Matrox_GIGE.Dig_%d", x);
		WritePrivateProfileString("Camera Mapping", strKey, l_Number, "c:\\SVObserver\\Bin\\SVIM.ini");
	}

	// Write Num Cameras to Camera Mapping
	CString sTmp;
	sTmp.Format( "%d", iSize ); 

	WritePrivateProfileString( "Camera Mapping",
		"GigeCameraCount",
		sTmp,
		"c:\\SVObserver\\Bin\\SVIM.ini" );

	SVConnectedCameraMap l_ConnectedCameras;

	GetConnectedCameras( l_ConnectedCameras );

	return OrderCameras( l_ConnectedCameras );
}

HRESULT SVGigeCameraManager::UpdateConnectedCameras( const SVGigeCameraStructSet& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	SVConnectedCameraMap l_ConnectedCameras;

	for( size_t i = 0; i < rSVCameraList.size(); ++i )
	{
		const SVGigeCameraStruct& l_rCameraInfo = rSVCameraList[ i ];
		if( ! l_rCameraInfo.strIPAddress.IsEmpty() )
		{
			l_ConnectedCameras[ l_rCameraInfo.strIPAddress ] = l_rCameraInfo;
		}
	}

	return OrderCameras( l_ConnectedCameras );
}

void SVGigeCameraManager::GetCameraOrder( SVGigeCameraStructSet& rSVCameraList ) const
{
	int iSize = GetPrivateProfileInt( "Camera Mapping",
		"GigeCameraCount",
		1,
		"c:\\SVObserver\\Bin\\SVIM.ini" );

	rSVCameraList.clear();

	for ( int x = 0; x < iSize; x++ )
	{
		SVGigeCameraStruct SVcs;

		CString strKey;
		char szGetBuf[128];

		strKey.Format( "Matrox_GIGE.Dig_%d", x );
		GetPrivateProfileString( "Camera Mapping", strKey, "", szGetBuf, 128, "c:\\SVObserver\\Bin\\SVIM.ini" );
		SVcs.strIPAddress = szGetBuf;

		SVcs.iPosition = x;
		SVcs.eChangeType = SVGigeCameraStruct::SVNoChange;
		
		rSVCameraList.Add( SVcs );
	}
}

void SVGigeCameraManager::GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const
{
	rSVCameraMap.clear();

	for( size_t i = 0; i < m_OrderedCamaras.size(); ++i )
	{
		const SVGigeCameraStruct& l_rCameraInfo = m_OrderedCamaras[ i ];
		if( ! l_rCameraInfo.strIPAddress.IsEmpty() )
		{
			rSVCameraMap[ l_rCameraInfo.strIPAddress ] = l_rCameraInfo;
		}
	}
}

HRESULT SVGigeCameraManager::OrderCameras( SVConnectedCameraMap& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	GetCameraOrder( m_OrderedCamaras );

	size_t l_Size = std::max< size_t >( m_OrderedCamaras.size(), rSVCameraList.size() );

	m_OrderedCamaras.resize( l_Size );

	for( size_t i = 0; i < l_Size; ++i )
	{
		SVGigeCameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.HasIPAddress() )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.find( l_rCamera.strIPAddress );

			if( l_Iter != rSVCameraList.end() )
			{
				l_rCamera = l_Iter->second;
				l_rCamera.eChangeType = SVGigeCameraStructInterface::SVNoChange;

				rSVCameraList.erase( l_Iter );
			}
			else
			{
				l_rCamera.strModelName.Empty();
				l_rCamera.strVendorName.Empty();
				l_rCamera.eChangeType = SVGigeCameraStructInterface::SVCameraRemoved;
				l_rCamera.m_ulHandle = NULL;
			}
		}
		l_rCamera.iPosition = static_cast<int>(i);
	}

	for( size_t i = 0; i < l_Size && !( rSVCameraList.empty() ); ++i )
	{
		SVGigeCameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.m_ulHandle == NULL )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.begin();

			if( l_Iter != rSVCameraList.end() )
			{

				if( l_rCamera.HasIPAddress() )
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SVGigeCameraStructInterface::SVReplacedCamera;
				}
				else
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SVGigeCameraStructInterface::SVNewCamera;
				}


				rSVCameraList.erase( l_Iter );
			}
		}

		l_rCamera.iPosition = static_cast<int>(i);
	}

	return l_Status;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGigeCameraManager.cpp_v  $
 * 
 *    Rev 1.3   01 Oct 2013 14:19:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2013 09:11:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Jun 2013 12:17:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new methods GetOriginalCameraIniOrder & UpdateCameraIniList, new member variable m_iniCameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2013 15:19:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 May 2013 10:19:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed all methods to use IPAddress instead of serail number. Changed GetCameraOrder to default number of devices from 3 to 1 and to read IPAddress
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:42:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Jan 2013 14:45:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  804
 * SCR Title:  Fix Camera Manager Cancel function
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter to ignore handles for use in the cancel button of the camera manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Mar 2011 16:00:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Nov 2010 13:35:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Mar 2010 13:46:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix a bug with a removed camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Mar 2010 11:55:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to fix issues with changing cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 12:29:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Oct 2009 18:51:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:47:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
