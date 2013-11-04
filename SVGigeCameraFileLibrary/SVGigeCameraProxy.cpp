//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraProxy
//* .File Name       : $Workfile:   SVGigeCameraProxy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:43:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeCameraProxy.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVGigeCameraFileReader.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"

SVGigeCameraProxy::SVGigeCameraProxy()
{ 
}

SVGigeCameraProxy::~SVGigeCameraProxy()
{
}

HRESULT SVGigeCameraProxy::ReadCameraFileImpl( SVGigeCameraFileInfoStruct& p_rInfo, SVDeviceParamCollection& p_rParams )
{
	HRESULT hr = S_OK;

	CStdioFile file;
	if ( file.Open( p_rInfo.sFilename.ToString(), CFile::modeRead | CFile::shareDenyWrite ) )
	{
		file.Close();

		p_rParams.Clear();
		SetParameterDefaults( p_rParams );

		SVGigeCameraFileReader reader( p_rInfo );
		
		// Read the Gige Feature Overrides
		hr = reader.ReadGigeFeatureOverrides();

		if (hr == S_OK)
		{
			hr = reader.ReadParams( p_rParams );
		}
	}
	else
	{
		// couldn't open file
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVGigeCameraProxy::SetParameterDefaults( SVDeviceParamCollection& rParams )
{
	// set default values for newly added parameters that wouldn't be in old configs
	// and for parameters that must always exist

	// IO Strobe Edge
	// The normal camera implementation has the strobe signal active high.
	// We want the normal signal after the gate inversion to be low.
	// So by default we need to provide an inversion. On cameras where the polarity is selectable,
	// no inversion is necessary - just default to active low.
	if ( !rParams.ParameterExists( DeviceParamAcquisitionStrobeEdge ) )
	{
		bool l_bRising = true;

		if ( rParams.ParameterExists( DeviceParamGigeStrobeEdge ) )
		{
			const SVStringValueDeviceParam* pParam = rParams.Parameter( DeviceParamGigeStrobeEdge ).DerivedValue( pParam );
			CString l_Temp( _T( "rising" ) );
			l_bRising = l_Temp.CompareNoCase( pParam->strValue.c_str() ) == 0;
		}

		if ( rParams.ParameterExists( DeviceParamIOStrobeInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = rParams.Parameter( DeviceParamIOStrobeInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		// for Legacy Code in SVConfigurationObject and SVOConfigAssistantDlg
		// Create the BoolDeviceParam for DeviceParamAcquisitionStrobeEdge
		rParams.SetParameter( DeviceParamAcquisitionStrobeEdge, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamAcquisitionStrobeEdge )) );
		SVBoolValueDeviceParam* pEdge = rParams.GetParameter( DeviceParamAcquisitionStrobeEdge ).DerivedValue(pEdge);

		SVBoolValueDeviceParam::OptionType l_Option;

		l_Option.strDescription = _T("Rising Edge");
		l_Option.value = true;
		pEdge->info.options.push_back( l_Option );

		l_Option.strDescription = _T("Falling Edge");
		l_Option.value = false;
		pEdge->info.options.push_back( l_Option );

		pEdge->bValue = l_bRising; // default falling edge
		pEdge->SetDetailLevel( 99 );
	}

	// IO Trigger Edge
	if ( !rParams.ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
	{
		bool l_bRising = true;

		if ( rParams.ParameterExists( DeviceParamGigeTriggerEdge ) )
		{
			const SVStringValueDeviceParam* pParam = rParams.Parameter( DeviceParamGigeTriggerEdge ).DerivedValue( pParam );
			CString l_Temp( _T( "rising" ) );
			l_bRising = l_Temp.CompareNoCase( pParam->strValue.c_str() ) == 0;
		}

		if ( rParams.ParameterExists( DeviceParamIOTriggerInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = rParams.Parameter( DeviceParamIOTriggerInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		// for Legacy Code in SVConfigurationObject and SVOConfigAssistantDlg
		// Create the BoolDeviceParam for DeviceParamAcquisitionTriggerEdge
		rParams.SetParameter( DeviceParamAcquisitionTriggerEdge, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( DeviceParamAcquisitionTriggerEdge )) );
		SVBoolValueDeviceParam* pEdge = rParams.GetParameter( DeviceParamAcquisitionTriggerEdge ).DerivedValue(pEdge);

		SVBoolValueDeviceParam::OptionType l_Option;

		l_Option.strDescription = _T("Rising Edge");
		l_Option.value = true;
		pEdge->info.options.push_back( l_Option );

		l_Option.strDescription = _T("Falling Edge");
		l_Option.value = false;
		pEdge->info.options.push_back( l_Option );

		pEdge->bValue = l_bRising; // default falling edge
		pEdge->SetDetailLevel( 99 );
	}

	return S_OK;
}

// moved from SVAcquistionClass - only useful for SVImageTest...
HRESULT SVGigeCameraProxy::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, SVDeviceParamCollection& rDestDeviceParams )
{
	HRESULT hr = S_OK;

	// don't do a straight assignment ( rDestDeviceParams = rDeviceParams )
	// only copy those parameters that are specified.
	SVDeviceParamMap::const_iterator iter;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;
		if ( ((const SVDeviceParam*) w) != NULL )
		{
			rDestDeviceParams.GetParameter( iter->first ) = w;
		}
	}

	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVGigeCameraProxy.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:43:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 2012 08:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jul 2010 13:45:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to foce the trigger edge parameter and strobe edge parameter to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jun 2010 08:24:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 11:24:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:09:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 16:51:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
