//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDriverProxy.cpp
//* .File Name       : $Workfile:   SVDCamDriverProxy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCamDriverProxy.h"
#include "SVDCamDriver.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"

SVDCamDriverProxy::SVDCamDriverProxy()
: m_pCameraDriver( NULL ) 
{ 
}

SVDCamDriverProxy::~SVDCamDriverProxy()
{
	m_pCameraDriver = NULL; 
}

HRESULT SVDCamDriverProxy::ReadCameraFileImpl( SV1394CameraFileInfoStruct& p_rInfo, SVDeviceParamCollection& p_rParams )
{
	HRESULT hr = S_OK;

	CStdioFile file;
	if ( file.Open( p_rInfo.sFilename, CFile::modeRead | CFile::shareDenyWrite ) )
	{
		file.Close();

		p_rParams.Clear();
		SetParameterDefaults( p_rParams );

		SV1394CameraFileReader reader( p_rInfo );
		hr = reader.ReadParams( p_rParams );
	}
	else
	{
		// couldn't open file
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVDCamDriverProxy::SetParameterDefaults( SVDeviceParamCollection& rParams )
{
	// set default values for newly added parameters that wouldn't be in old configs
	// and for parameters that must always exist

	// IO Strobe Edge
	// The normal camera implementation has the strobe signal active high.
	// We want the normal signal after the gate inversion to be low.
	// So by default we need to provide an inversion. On cameras where the polarity is selectable (ISG LightWise),
	// no inversion is necessary - just default to active low.
	if ( ! rParams.ParameterExists( DeviceParamAcquisitionStrobeEdge ) )
	{
		bool l_bRising = false;

		if ( rParams.ParameterExists( DeviceParamIOStrobeInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = rParams.Parameter( DeviceParamIOStrobeInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		SVBoolValueDeviceParam l_Edge;
		SVBoolValueDeviceParam::OptionType l_Option;

		l_Option.strDescription = _T("Rising Edge");
		l_Option.value = true;
		l_Edge.info.options.push_back( l_Option );

		l_Option.strDescription = _T("Falling Edge");
		l_Option.value = false;
		l_Edge.info.options.push_back( l_Option );

		l_Edge.bValue = l_bRising; // default falling edge
		l_Edge.SetDetailLevel( 99 );

		rParams.SetParameter(DeviceParamAcquisitionStrobeEdge, l_Edge ); // must always have this parameter.
	}

	// IO Trigger Edge
	if ( ! rParams.ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
	{
		bool l_bRising = false;

		if ( rParams.ParameterExists( DeviceParamIOTriggerInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = rParams.Parameter( DeviceParamIOTriggerInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		SVBoolValueDeviceParam l_Edge;
		SVBoolValueDeviceParam::OptionType l_Option;

		l_Option.strDescription = _T("Rising Edge");
		l_Option.value = true;
		l_Edge.info.options.push_back( l_Option );

		l_Option.strDescription = _T("Falling Edge");
		l_Option.value = false;
		l_Edge.info.options.push_back( l_Option );

		l_Edge.bValue = l_bRising;	// default falling edge
		l_Edge.SetDetailLevel( 99 );

		rParams.SetParameter( DeviceParamAcquisitionTriggerEdge, l_Edge ); // must always have this parameter.
	}

	return S_OK;
}

// moved from SVAcquistionClass - only useful for SVImageTest...
HRESULT SVDCamDriverProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, SVDeviceParamCollection& rDestDeviceParams )
{
	HRESULT hr = S_OK;

	// don't do a straight assignment ( rDestDeviceParams = rDeviceParams )
	// only copy those parameters that are specified.
	SVDeviceParamMap::const_iterator iter;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;
		if ( ((const SVDeviceParam*) w) != NULL )
			rDestDeviceParams.GetParameter( iter->first ) = w;
	}

	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamDriverProxy.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2012 08:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Mar 2011 15:36:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construct functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 11:13:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jun 2009 09:46:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:56:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
