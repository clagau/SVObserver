// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImagingDeviceParams
// * .File Name       : $Workfile:   SVImagingDeviceParams.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:51:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImagingDeviceParams.h"

namespace	// create unnamed namespace for the variables to live in
{
	//REGISTER_DEVICE_PARAM( DeviceParamCameraFiles,         SVCameraFilesDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamLightReference,      DeviceParamLightReference_String, SVLightReferenceDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamLut,                 DeviceParamLut_String,            SVLutDeviceParam );
	REGISTER_DEVICE_PARAM( DeviceParamCameraFormats,       DeviceParamCameraFormats_String,  SVCameraFormatsDeviceParam );
}// end unnamed namespace


SVLutDeviceParam::SVLutDeviceParam()
{
	m_eDataType = DeviceDataTypeComplex;
	Init(DeviceParamLut);
}

SVLutDeviceParam::SVLutDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam()
{
	m_eDataType = DeviceDataTypeComplex;
	Init(DeviceParamLightReference);
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
{
	m_eDataType = DeviceDataTypeComplex;
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam()
{
//	m_eDataType = DeviceDataTypeComplex;
	m_eDataType = DeviceDataTypeString;
	Init(DeviceParamCameraFormats);
}

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum)
: SVStringValueDeviceParam(typeEnum)
{
//	m_eDataType = DeviceDataTypeComplex;
	m_eDataType = DeviceDataTypeString;
}

/*
SVCameraFilesDeviceParam::SVCameraFilesDeviceParam()
{
	m_eDataType = DeviceDataTypeComplex;
	Init( DeviceParamCameraFiles );
}
*/

SVLutDeviceParam::SVLutDeviceParam( const SVLutDeviceParam& rhs ) 
: SVDeviceParam( rhs )
, lut( rhs.lut )
{
	m_eDataType = DeviceDataTypeComplex;
}

SVLightReferenceDeviceParam::SVLightReferenceDeviceParam( const SVLightReferenceDeviceParam& rhs) 
: SVDeviceParam( rhs )
, lr( rhs.lr )
{
	m_eDataType = DeviceDataTypeComplex;
}

/*
SVCameraFilesDeviceParam::SVCameraFilesDeviceParam( const SVCameraFilesDeviceParam& rhs ) : SVDeviceParam( rhs ), cf( rhs.cf )
{
	m_eDataType = DeviceDataTypeComplex;
}
*/

SVCameraFormatsDeviceParam::SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs) 
: SVStringValueDeviceParam( rhs )
, options( rhs.options )
{
//	m_eDataType = DeviceDataTypeComplex;
	m_eDataType = DeviceDataTypeString;
}

SVClonable* SVLutDeviceParam::CloneImpl() const
{
	return new SVLutDeviceParam( *this );
}

SVClonable* SVLightReferenceDeviceParam::CloneImpl() const
{
	return new SVLightReferenceDeviceParam( *this );
}

SVClonable* SVCameraFormatsDeviceParam::CloneImpl() const
{
	return new SVCameraFormatsDeviceParam( *this );
}

/*
SVClonable* SVCameraFilesDeviceParam::CloneImpl() const
{
	return new SVCameraFilesDeviceParam( *this );
}
*/

SVLutDeviceParam& SVLutDeviceParam::operator=(const SVLutDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		lut = rhs.lut;
	}
	return *this;
}

SVLightReferenceDeviceParam& SVLightReferenceDeviceParam::operator=(const SVLightReferenceDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		lr = rhs.lr;
	}
	return *this;
}

const SVCameraFormatsDeviceParam& SVCameraFormatsDeviceParam::operator=(const SVCameraFormatsDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		options = rhs.options;
		strValue = rhs.strValue;
	}
	return *this;
}

HRESULT SVCameraFormatsDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if ( const SVCameraFormatsDeviceParam* pParam = dynamic_cast<const SVCameraFormatsDeviceParam*>(pBaseParam) )
	{
		info = pParam->info;
		OptionsType::iterator iterUser;
		OptionsType::iterator iterCameraFile;
		for ( iterUser = options.begin(); iterUser != options.end(); ++iterUser )
		{
			SVCameraFormat& rcf = iterUser->second;
			iterCameraFile = const_cast<SVCameraFormatsDeviceParam *>(pParam)->options.find( iterUser->first );
			if ( iterCameraFile != pParam->options.end() )
			{
				SVCameraFormat& rcfCameraFile = iterCameraFile->second;
				rcf.AssignConstantValues( rcfCameraFile );
			}
		}
		//options = pParam->options;
		hr = S_OK;
	}
	return hr;
}

bool SVCameraFormatsDeviceParam::SupportsColor() const
{
	bool bSupportsColor = false;

	SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
	for ( iter = options.begin(); iter != options.end(); ++iter)
	{
		if ( iter->second.bColor )
		{
			bSupportsColor = true;
			break;
		}
	}// end for ( iter = pParam->options.begin(); iter != pParam->options.end(); iter++)
	return bSupportsColor;
}

#if defined(_DEBUG) || defined(_MINDEBUG)

SVLut& Lut(SVDeviceParamWrapper& w) 
{
	const SVLutDeviceParam* p = w.DerivedValue(p); 
	if (p==NULL) 
	{
		w = SVLutDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVLutDeviceParam*>(p)->lut;
}
SVLightReference& LR(SVDeviceParamWrapper& w) 
{
	const SVLightReferenceDeviceParam* p = w.DerivedValue(p); 
	if (p==NULL) 
	{
		w = SVLightReferenceDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVLightReferenceDeviceParam*>(p)->lr;
}

#endif	// #if defined(_DEBUG) || defined(_MINDEBUG)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImagingDeviceParams.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Apr 2013 13:48:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor(s) that take an arguemnt of type SVDeviceParamEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Oct 2012 09:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Jun 2007 11:09:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Oct 2005 08:07:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed the SVCameraFormatsDeviceParam::SetMetadata function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Oct 2005 08:06:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed the SVCameraFormatsDeviceParam::SetMetadata function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Aug 2005 13:16:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SetMetadata
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Aug 2005 13:15:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SetMetadata
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Feb 2004 17:15:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added string name to device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Nov 2003 09:23:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SupportsColor function to the CameraFormats param
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:06:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   split off from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:02:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   split off from SVDeviceParams in SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
