//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxAcquisitionClass
//* .File Name       : $Workfile:   SVMatroxAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:30:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxAcquisitionClass.h"

#include "SVAcquisitionImageBufferHandleStruct.h"
#include "ModelessDebug.h"
#include "SVImageProcessingClass.h"
#include "SVDisplayImageProcessingClass.h"
#include "SVMatroxDeviceManager.h"
#include <SVMessage.h>
#include "SVDCamDriver.h"
#include "SVDCamDragonflyDriver.h"
#include "SVDCamStandardDriver.h"
#include "SV1394CameraFileReader.h"
#include <SVTemplate.h>
#include <SVException.h>
#include <SVMessage.h>
#include "SVConfigurationObject.h"	// EB 20040109 TEMP HACK for Strobe Start Frame
#include "DataManager.h"

#include "SVObserver.h"
#include "SVDCamFactoryRegistrar.h"



enum MatroxAsyncAcquireMethod
{
	MatroxAcquireMethodQueue,
	MatroxAcquireMethodSingleGrab,
	MatroxAcquireMethodBlockedGrab,
};

enum FakeStartFrameVerifyMethod
{
	AllowOutstandingTriggers,
	RunningAverageTimeout,
	CalculatedLatency,
};

namespace	// unnamed namespace for file scope variables
{
	MatroxAsyncAcquireMethod f_eAcquireMethod = MatroxAcquireMethodQueue;	// new default
	int f_iNumOutstandingTriggersAllowed = 2;	// EB 20031205
	bool f_bContinuouslyNakOnOverTrigger = true;
	//20031205: The decision was made to allow 2 outstanding T's to allow for timing variations.
	// This is only (hopefully) until the strobe SF is fully implemented in the SVIM hardware.

	FakeStartFrameVerifyMethod f_eVerifyMethod = CalculatedLatency;	// EB 20040127 
}

namespace	// file scope
{
	__int64 f_iClockFrequency=1;
}

__int64 TicksToMicroseconds(__int64 iTicks)
{
	return iTicks / (f_iClockFrequency/1000000);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxAcquisitionClass::SVMatroxAcquisitionClass()
{
	m_iBandSize = 0;
	mbAreBuffersCreated = false;
	//mbIsStarted = false;
	m_pDeviceManager = NULL;
	m_pCameraDriver = NULL;
	mbTempOnline = false;
	mbDontReallocDigFormat = false;
	m_AcquisitionBufferQueue.Create();
	m_lGrabStartFrameLock=0;
	m_lCalculatedLatency=0;
	m_lOutstandingGrabRequests=0;
	m_bStartedGrabRequests=false;
	m_iLastTrigger=0;
	m_iLastEndFrame=0;

	f_eAcquireMethod = (MatroxAsyncAcquireMethod) AfxGetApp()->GetProfileInt(_T("Debug"), _T("1394AcquireMethod"), f_eAcquireMethod);
	//AfxGetApp()->WriteProfileInt(_T("Debug"), _T("1394AcquireMethod"), f_eAcquireMethod);

	f_eVerifyMethod = (FakeStartFrameVerifyMethod) AfxGetApp()->GetProfileInt(_T("Debug"), _T("1394SFMethod"), f_eVerifyMethod);
	//AfxGetApp()->WriteProfileInt(_T("Debug"), _T("1394SFMethod"), f_eVerifyMethod);

	f_iNumOutstandingTriggersAllowed = AfxGetApp()->GetProfileInt(_T("Debug"), _T("NumOutstandingTriggers"), f_iNumOutstandingTriggersAllowed);
	//AfxGetApp()->WriteProfileInt(_T("Debug"), _T("NumOutstandingTriggers"), f_iNumOutstandingTriggersAllowed);

	f_bContinuouslyNakOnOverTrigger = AfxGetApp()->GetProfileInt(_T("Debug"), _T("NakOnOverTrigger"), f_bContinuouslyNakOnOverTrigger ? 1 : 0) ? true : false;
	//AfxGetApp()->WriteProfileInt(_T("Debug"), _T("NakOnOverTrigger"), f_bContinuouslyNakOnOverTrigger ? 1 : 0);

	switch ( f_eVerifyMethod )
	{
		case RunningAverageTimeout:
		case CalculatedLatency:
			f_iNumOutstandingTriggersAllowed = 2;
			f_bContinuouslyNakOnOverTrigger = false;
			break;
	}

	::QueryPerformanceFrequency( (LARGE_INTEGER*) &f_iClockFrequency );
}

SVMatroxAcquisitionClass::~SVMatroxAcquisitionClass()
{
	HRESULT hr = Destroy();
}

bool SVMatroxAcquisitionClass::IsValid()
{
	bool bOk = m_Device.milDig != M_NULL;
	bOk = SVAcquisitionClass::IsValid() && bOk;

	if (   m_Device.ParameterExists( DeviceParamVendorId )
		&& m_DeviceParams.ParameterExists( DeviceParamVendorId ))
	{
//		bOk = bOk &&
//			StringValue(m_Device.Parameter( DeviceParamVendorId )) == StringValue(m_DeviceParams.Parameter( DeviceParamVendorId ));
	}

	return bOk;
}

bool SVMatroxAcquisitionClass::IsStarted()
{
	return SVAcquisitionClass::IsStarted();
}

bool SVMatroxAcquisitionClass::IsValidBoard()
{
	return m_pDeviceManager->IsValidDigitizerSubsystem();
}

HRESULT SVMatroxAcquisitionClass::Create( unsigned long ulSize )
{
	return SVAcquisitionClass::Create( ulSize );
}

HRESULT SVMatroxAcquisitionClass::Destroy()
{
	HRESULT hr = DestroyLocal();
	HRESULT hr2 = SVAcquisitionClass::Destroy();
	if (hr == S_OK)
		hr = hr2;
	return hr;
}

HRESULT SVMatroxAcquisitionClass::DestroyLocal()
{
	HRESULT hr = S_OK;
	
	//if ( m_bStarted )
		Stop();
	//if ( m_bBuffersCreated )
		DestroyBuffers();
	//if ( m_bFilesLoaded )
		UnloadFiles();
	ResetLightReference();
	//free cam
	//free vic
	// AcqReset

	if ( m_pCameraDriver )
	{
		delete m_pCameraDriver;
		m_pCameraDriver = NULL;
	}
	
	return hr;
}

HRESULT SVMatroxAcquisitionClass::LoadFiles( SVFileNameArrayClass& rArray )
{
	m_DeviceParams.Clear();

	HRESULT hr = SVAcquisitionClass::LoadFiles( rArray );
	if ( hr == S_OK )
	{
		ASSERT( mFiles.GetSize() == 1 );	// only one file
		if ( mFiles.GetSize() == 1 )
		{
			CString sFile = mFiles[0].GetFullFileName();

			SV1394CameraFileInfoStruct info;
			info.sFilename = sFile;
			info.bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;
			hr = ReadCameraFileImpl( info, m_CameraFileDeviceParams );

			if ( m_pCameraDriver )
			{
				delete m_pCameraDriver;
			}

			try
			{
				m_pCameraDriver = TheDCamDriverFactory::Instance().New( info.sCameraType );
			}
			catch ( std::runtime_error& )
			{
			}

			ASSERT( m_pCameraDriver );
			if ( m_pCameraDriver )
			{
				m_DCamAcqProxy.SetOwner( this );
				m_pCameraDriver->SetOwner( &m_DCamAcqProxy );
			}

			if ( hr == SVMSG_INCORRECT_CHECKSUM )
			{
			}
			mbDontReallocDigFormat = true;
			SetDeviceParameters( m_CameraFileDeviceParams );
			mbDontReallocDigFormat = false;
			GetCameraImageInfo( &msvImageInfo );
		}
	}
	return hr;
}

HRESULT SVMatroxAcquisitionClass::UnloadFiles()
{
	HRESULT hr = SVAcquisitionClass::UnloadFiles( );
	return hr;
}

HRESULT SVMatroxAcquisitionClass::ReadCameraFile( const CString& sFilename, SVDeviceParamCollection& rParams )
{
	SV1394CameraFileInfoStruct info;
	info.sFilename = sFilename;
	info.bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;
	HRESULT hr = ReadCameraFileImpl( info, rParams );

	return hr;
}

/*static*/ HRESULT SVMatroxAcquisitionClass::ReadCameraFileImpl( SV1394CameraFileInfoStruct& p_rInfo, SVDeviceParamCollection& p_rParams )
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

/*static*/ HRESULT SVMatroxAcquisitionClass::SetParameterDefaults( SVDeviceParamCollection& rParams )
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

HRESULT SVMatroxAcquisitionClass::CreateLightReference( int iBands, int iBrightness, int iContrast )
{
	HRESULT hr = S_FALSE;
	hr = SVAcquisitionClass::CreateLightReference( iBands, iBrightness, iContrast );
	
	if ( hr == S_OK )
	{
		DWORD dwDefaultBrightness = 0;	// not used
		for ( int i = 0; hr == S_OK && i < iBands; i++)
		{
			//if ( (hr = CreateLightReferenceBand( i, 1 )) == S_OK )
			if ( (hr = CreateLightReferenceBand( i, 0 )) == S_OK )
			{
			}
		}
		if ( hr != S_OK )
		{
			mLightReference.DeleteAll();
		}
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::ResetLightReference()
{
	//mLightReference.DeleteAll();
	return SVAcquisitionClass::ResetLightReference();
}


/*
HRESULT SVMatroxAcquisitionClass::GetCurrentLightReferenceValue( unsigned long ulType, long& rlValue )
{
	rlValue = 0;
	return E_NOTIMPL;
}
*/

HRESULT SVMatroxAcquisitionClass::GetMaxLightReferenceValue( unsigned long ulType, long& rlValue )
{
	HRESULT hr = S_OK;

	SVDeviceParamEnum eParam = (SVDeviceParamEnum) ulType;
	const SVLongValueDeviceParam* pParam = m_CameraFileDeviceParams.Parameter( eParam ).DerivedValue( pParam );
	if ( pParam )
		rlValue = pParam->info.max;
	else
	{
		rlValue = 0;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::GetMinLightReferenceValue( unsigned long ulType, long& rlValue )
{
	HRESULT hr = S_OK;

	const SVDeviceParamEnum eParam = (SVDeviceParamEnum) ulType;
	const SVLongValueDeviceParam* pParam = m_CameraFileDeviceParams.Parameter( eParam ).DerivedValue( pParam );
	if ( pParam )
		rlValue = pParam->info.min;
	else
	{
		rlValue = 0;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::GetLightReferenceValueStep( unsigned long ulType, unsigned long& rulValue )
{
	HRESULT hr = S_OK;
	rulValue = 1;
	return hr;
}

HRESULT SVMatroxAcquisitionClass::SetLightReferenceImpl( SVLightReference& rLR )
{
	HRESULT hr = S_OK;

	for ( int iBand = 0; iBand < BandMaxSize(); iBand++ )
	{
		for ( int iAttribute=0; iAttribute < rLR.Band(iBand).NumAttributes(); iAttribute++)
		{
			SVLightReferenceAttributeInfo& rAttribute = rLR.Band( iBand ).Attribute( iAttribute );
			SVDeviceParamEnum eParam = (SVDeviceParamEnum) rAttribute.dwType;
			SVLongValueDeviceParam* pParam = m_DeviceParams.GetParameter( eParam ).DerivedValue( pParam );
			if ( pParam )
			{
				pParam->lValue = rAttribute.lValue;
				SVDeviceParamWrapper w = pParam;
				InitializeDevice(w);
			}
		}// end for ( int iAttribute=0; iAttribute < rLR.Band(iBand).NumAttributes(); iAttribute++)
	}// end for ( int iBand = 0; iBand < BandMaxSize(); iBand++ )

	return hr;
}


HRESULT SVMatroxAcquisitionClass::CreateLut( const SVLutInfo& info )
{
	SVLutInfo lutinfo( info );

	// temp
	lutinfo.SetBands(1);
	lutinfo.SetBandSize(1);

	Lut().Create( lutinfo );

	return S_OK;
}

HRESULT SVMatroxAcquisitionClass::DestroyLut( )
{
	HRESULT hr = S_OK;

	Lut().Destroy();
	
	return hr;
}


HRESULT SVMatroxAcquisitionClass::SetLutImpl( const SVLut& lut )
{
	HRESULT hr = S_OK;

	SVLutDeviceParam lutparam;
	lutparam.lut = lut;
	SVDeviceParamWrapper w = lutparam;
	if (  CameraMatchesCameraFile() )
	{
		hr = m_pCameraDriver->SetParameter( w );
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT hr = S_OK;

	lut = Lut();

	return hr;
}


/*static*/ void SVMatroxAcquisitionClass::NeutralLutToRawLut(const SVLut& lut, CByteArray& aRawBytes)
{
}

/*static*/ void SVMatroxAcquisitionClass::RawLutToNeutralLut(const CByteArray& aRawBytes, SVLut& lut)
{
}


HRESULT SVMatroxAcquisitionClass::GetImageInfo(SVImageInfoClass* pImageInfo)
{
	HRESULT hrOk = SV_FALSE;
	
	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
		
		hrOk = S_OK;
	}
	
	return hrOk;
}


HRESULT SVMatroxAcquisitionClass::CreateBuffers( SVImageProcessingClass* pImaging,
                                                 SVImageInfoClass ImageInfo,
                                                 unsigned long ulSize )
{
	HRESULT hr = S_OK;
	TRACE(_T("***SVMatroxAcquisitionClass::CreateBuffers\n"));

	if ( !mbAreBuffersCreated )
	{
		hr = SVAcquisitionClass::CreateBuffers( pImaging, ImageInfo, ulSize );

		if ( hr == S_OK )
		{
			ulSize += f_iNumOutstandingTriggersAllowed + 1;
			BSTR bsName = mcsDeviceName.AllocSysString();
			hr = TheSVDataManager.CreateManagedIndexArray( bsName, ulSize, &mlDataManagerHandle );
			::SysFreeString( bsName );
		}

		if ( hr == S_OK )
		{
			int iMax = msvAcquisitionBufferArray.GetImageDepth();
			for ( int i = 0; i < iMax && hr == S_OK; i++ )
			{
				SVAcquisitionImageBufferHandleStruct* pImageHandle = NULL;
				msvAcquisitionBufferArray.GetImageHandle( i, pImageHandle );

				ASSERT( pImageHandle );
				ASSERT( pImageHandle->MIL != M_NULL );

				if ( pImageHandle != NULL )
				{
					hr = UnlockBuffer( pImageHandle );
				}
				else
				{
					hr = SV_FALSE;
				}
			}// end for ( int i = 0; i < iMax && hr == S_OK; i++ )

			if ( hr == S_OK )
			{
				mbAreBuffersCreated = true;
			}

		}// end if ( hr == S_OK )
	}// end if ( !mbAreBuffersCreated )
	
	TRACE(_T("***SVMatroxAcquisitionClass::CreateBuffers exit\n"));
	return hr;
}

HRESULT SVMatroxAcquisitionClass::DestroyBuffers()
{
	HRESULT hr = S_OK;
	TRACE(_T("***SVMatroxAcquisitionClass::DestroyBuffers\n"));

	if ( mbAreBuffersCreated )
	{

		hr = Stop();
		hr = S_OK;

		/*
		int iMax = msvAcquisitionBufferArray.GetImageDepth();
		for ( int i = 0; i < iMax && hr == S_OK; i++ )
		{
			SVAcquisitionImageBufferHandleStruct* pImageHandle = NULL;
			
			msvAcquisitionBufferArray.GetImageHandle( i, pImageHandle );
			
			if ( pImageHandle != NULL )
			{
				if ( pImageHandle->MIL != M_NULL )
				{
					MbufFree( pImageHandle->MIL );
					pImageHandle->MIL = NULL;
				}
			}
		}
		*/

		if ( m_Device.milDig != M_NULL )
		{
			//MdigControl( m_Device.milDig, M_GRAB_ABORT, M_DEFAULT ); done in ::Stop

			// why is this done here and not ::Stop? (this is how it's done in Coreco)
			MdigControl( m_Device.milDig, M_GRAB_TRIGGER, M_DISABLE );	// do in all cases (not really necessary if camera is not hardware triggerable)

			// do only if a grab queued!
			if ( MdigInquire( m_Device.milDig, M_GRAB_IN_PROGRESS, M_NULL ) == M_YES )
			{
				TRACE(_T("Begin MdigGrabWait\n"));
				MdigGrabWait( m_Device.milDig, M_GRAB_END );	// wait for current grab to finish
				TRACE(_T("End MdigGrabWait\n"));
			}
			else
			{
				TRACE(_T("Skipped MdigGrabWait; no grabs in progress\n"));
			}

			/*
			long lHookType;
			lHookType = M_GRAB_FRAMESTART + M_UNHOOK;
			MdigHookFunction( m_Device.milDig, lHookType, StartFrameCallback, NULL );
			lHookType = M_GRAB_START + M_UNHOOK;
			MdigHookFunction( m_Device.milDig, lHookType, StartFrameCallback, NULL );
			lHookType = M_GRAB_FRAME_END + M_UNHOOK;
			MdigHookFunction( m_Device.milDig, lHookType, EndFrameCallback, NULL );
			lHookType = M_GRAB_END + M_UNHOOK;
			MdigHookFunction( m_Device.milDig, lHookType, EndFrameCallback, NULL );
			*/
		}

		hr = SVAcquisitionClass::DestroyBuffers();

		mbAreBuffersCreated = false;

	}

	TRACE(_T("***SVMatroxAcquisitionClass::DestroyBuffers exit\n"));
	return hr;
}

HRESULT SVMatroxAcquisitionClass::Start()	// GoOnline
{
	HRESULT hr = S_OK;
	TRACE(_T("\n***SVMatroxAcquisitionClass::Start\n\n"));
	if ( !IsStarted() )
	{
		mbGoingOffline = false;
		m_lOutstandingTriggerCount = 0;
		m_lOutstandingGrabRequests = 0;
		m_bStopProcessing = false;
		m_iLastTrigger=0;
		m_iLastEndFrame=0;

		DWORD bLog = AfxGetApp()->GetProfileInt(_T("Debug"), _T("LogTSFEF"), FALSE);
		AfxGetApp()->WriteProfileInt(_T("Debug"), _T("LogTSFEF"), bLog );
		if ( bLog )
		{
			CString sName;
			sName.Format(_T("c:\\temp\\TSFEF_%s.log"), DeviceName());
			m_LogFile.Open(sName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite );
			CString s;
			s.Format(_T("%I64u, , ,  Frequency\n"), f_iClockFrequency);
			//m_LogFile.WriteString(s);
		}

		hr = SVAcquisitionClass::Start();

		if ( hr == S_OK )
		{
			if ( !CameraMatchesCameraFile() )
			{
				hr = S_FALSE;
			}
		}

		if ( hr == S_OK && m_Device.milDig != M_NULL )
		{
			void* pUserParam = this;
			long lHookType;
			lHookType = M_GRAB_START;
			MdigHookFunction( m_Device.milDig, lHookType, StartGrabCallback, pUserParam );
			lHookType = M_GRAB_FRAME_START;
			MdigHookFunction( m_Device.milDig, lHookType, StartFrameCallback, pUserParam );
			lHookType = M_GRAB_FRAME_END;
			MdigHookFunction( m_Device.milDig, lHookType, EndFrameCallback, pUserParam );
			lHookType = M_GRAB_END;
			MdigHookFunction( m_Device.milDig, lHookType, EndGrabCallback, pUserParam );
		}

		if ( hr == S_OK )
		{
			switch ( f_eVerifyMethod )
			{
				case RunningAverageTimeout:
					m_ratInfo.Clear();
					break;

				case CalculatedLatency:
				{
					const SVCameraFormatsDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
					const SVCameraFormat& option = pParam->options.find( pParam->strValue )->second;
					if ( !option.bVariableROI )
					{
						if ( option.lMinTransferTime != 0 )
						{
							m_lCalculatedLatency = option.lMinTransferTime;
						}
						else
						{
							int iPercent = AfxGetApp()->GetProfileInt(_T("Debug"), _T("SFLatencyPercent"), 80 );
							m_lCalculatedLatency = ((1000000 / option.lFrameRate) * iPercent) / 100;	// calculate frame rate in 탎
						}
					}
					else	// format 7
					{
						if ( option.lMaxTransferTime != 0 )
						{
							// take optimized x/y into account; assume linear speedup
							double dMultiplier = 1.0;
							if ( option.bOptimizedTransferRate_X )
								dMultiplier *= ((double) option.lWidth / (double) option.lWidthMax);
							if ( option.bOptimizedTransferRate_Y )
								dMultiplier *= ((double) option.lHeight / (double) option.lHeightMax);

							double dTransferTime = (double) option.lMaxTransferTime * dMultiplier;
							dTransferTime =	max( dTransferTime, (double) option.lMinTransferTime );
							m_lCalculatedLatency = static_cast<long>(dTransferTime);
						}
						else
						{
							// cannot calculate latency; what to do???
							hr = S_FALSE;	// FOR NOW
						}
					}
					break;
				}
			}

			m_AcquisitionBufferQueue.RemoveAll();
			m_Device.bRealStartFrame = false;	// TEMP EB20040109 - for now, default to this

			SVDCamTriggerSourceEnum eTriggerSource = (SVDCamTriggerSourceEnum) LongValue(m_DeviceParams.Parameter( DeviceParamAcquisitionTriggerType ));
			switch ( eTriggerSource )
			{
				case HardwareTrigger:
					//m_Device.bRealStartFrame = true;	// TEMP EB20040109 - for now, assume if we have hardware trigger, we have real (strobe) start frame
					TheSVObserverApp.m_svIOConfig.GetSVIMStrobeStartFrameActive( DigNumber(), m_Device.bRealStartFrame );	// only if enabled in SVIM.ini; EB 20040120
					// fall through...
				case SoftwareAsyncTrigger:
				{
					m_Device.bHardwareTrigger = true;

					// EB 20040109 HACK FUNCTION
					// tell the configuration object to tell the strobe start frames to go online
					if ( m_Device.bRealStartFrame )
						TheSVObserverApp.m_pConfiguration->StartAcqStrobeInputs();

					m_pCameraDriver->GoOnline();

					// activate camera's trigger sensing ... this should go after the QueueGrabs but MIL seems to handle it improperly if there is no trigger signal
					MdigControl( m_Device.milDig, M_GRAB_TRIGGER, M_ENABLE );

					PrimeTheQueue();

					m_bStartedGrabRequests = true;

				}// end case HardwareTrigger, etc.
				break;

				case SoftwareGrabTrigger:
				default:
				{
					m_Device.bHardwareTrigger = false;
					MdigControl( m_Device.milDig, M_GRAB_TRIGGER, M_DISABLE );
					break;
				}
			}// end switch ( eTriggerSource )
		}// end if ( hr == S_OK )
	}// end if ( !IsStarted() )
	
	TRACE(_T("***SVMatroxAcquisitionClass::Start exit\n"));
	return hr;
}

long SVMatroxAcquisitionClass::PrimeTheQueue()
{
	long lQueued=0;
	long lRequested=1;
	// Queue up MdigGrabs
	switch ( f_eAcquireMethod )
	{
		case MatroxAcquireMethodQueue:
		{
			// if m_Device.bRealStartFrame, it doesn't hurt to have extras queued (not necessary, either)
			lRequested = max( 2, f_iNumOutstandingTriggersAllowed );
			break;
		}

		case MatroxAcquireMethodSingleGrab:
		{
			lRequested = 1;
			break;
		}
		
		case MatroxAcquireMethodBlockedGrab:
		{
			lRequested = 1;
			break;
		}
	}// end switch (f_eAcquireMethod)

	lQueued = QueueGrabs( lRequested );

	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{
			CString s;
			s.Format(_T("%I64u, Prime the Queue; requested=%d, actual=%d\n"), TicksToMicroseconds(iTimer), lRequested, lQueued );
			m_LogFile.WriteString(s);
			TRACE(s);
		}
	}

	return lQueued;
}

HRESULT SVMatroxAcquisitionClass::Stop()	// GoOffline
{
	HRESULT hr = S_OK;
	TRACE(_T("\n***SVMatroxAcquisitionClass::Stop\n\n"));
	
	if ( IsStarted() )
	{
		mbGoingOffline = true;
		m_bStartedGrabRequests=false;
		m_lOutstandingGrabRequests=0;

		if ( m_Device.bHardwareTrigger )
		{
			//MdigControl( m_Device.milDig, M_GRAB_TRIGGER, M_DISABLE );	// do in all cases (not really necessary if camera is not hardware triggerable)


			// do we need to release all the queued buffers??
		}

		m_pCameraDriver->GoOffline();

		// EB 20040109 HACK FUNCTION
		// tell the configuration object to tell the strobe start frames to go offline
		TheSVObserverApp.m_pConfiguration->StopAcqStrobeInputs();

		TRACE(_T("Begin GRAB_ABORT\n"));
		MdigControl(m_Device.milDig, M_GRAB_ABORT, M_DEFAULT); // available on 1394 with beta DLL that Matrox sent
		TRACE(_T("End GRAB_ABORT\n"));
		long lMilError=0;
		lMilError = SVMatroxAppManager::GetStatusCode( _T(__FILE__), __LINE__ );
    if( lMilError != M_NULL_ERROR )
		{
			ASSERT(FALSE);
		}

		m_pCameraDriver->GoOffline();



		long lHookType;
		lHookType = M_GRAB_FRAME_START + M_UNHOOK;
		MdigHookFunction( m_Device.milDig, lHookType, StartFrameCallback, NULL );
		lHookType = M_GRAB_START + M_UNHOOK;
		MdigHookFunction( m_Device.milDig, lHookType, StartGrabCallback, NULL );
		lHookType = M_GRAB_FRAME_END + M_UNHOOK;
		MdigHookFunction( m_Device.milDig, lHookType, EndFrameCallback, NULL );
		lHookType = M_GRAB_END + M_UNHOOK;
		MdigHookFunction( m_Device.milDig, lHookType, EndGrabCallback, NULL );


		// clean up queued buffers; prevent circle buffer leaks
		SVAcquisitionImageBufferHandleStruct* pAcqImage = NULL;
		do
		{
			m_AcquisitionBufferQueue.RemoveHead( &pAcqImage ); // remove image from queue
		
		} while ( pAcqImage != NULL );

		/*
		while ( m_AcquisitionBufferQueue.GetCount() > 0 )
		{
			pAcqImage = m_AcquisitionBufferQueue.RemoveHead(); // remove image from queue
			
			// for now do a dragonfly-specific thing until GRAB_ABORT is supported
			HRESULT hrTrigger = m_pCameraDriver->SoftwareTrigger();

			MdigGrabWait( m_Device.milDig, M_GRAB_END );	// wait for current grab to finish...
			//::Sleep( 125 );	//??
		}
		*/

//		// prevent camera from processing more hardware triggers
//		InitializeDevice( m_DeviceParams.Parameter( DeviceParamAcquisitionTriggerType ) );

		//MdigControl( m_Device.milDig, M_GRAB_TRIGGER_SOURCE, M_DEFAULT );

		hr = SVAcquisitionClass::Stop();

		if ( m_LogFile.m_hFile != CFile::hFileNull )
		{
			m_LogFile.Close();
		}
	}

	TRACE(_T("***SVMatroxAcquisitionClass::Stop exit\n"));
	return hr;
}

long SVMatroxAcquisitionClass::QueueGrabs(long p_lNumToQueue /* = -1*/)
{
	long lNumToQueue=p_lNumToQueue;
	long lQueued=0;

	SVAcquisitionImageObjectClass* pBuffer = GetCircleBuffer();
	if ( IsStarted() && pBuffer != NULL && !mbGoingOffline )	// don't queue more grabs if we're going offline
	{
		long lIndex;
		HRESULT hrIndex;

		// if lNumToQueue is not specified, queues up all available
		while ( lNumToQueue-- != 0 && (hrIndex = GetNextIndex( lIndex )) == S_OK)
		{
			lQueued++;
			SVAcquisitionImageBufferHandleStruct* pImageHandle = NULL;
			BOOL bResult = pBuffer->GetImageHandle( lIndex, pImageHandle );
			if ( pImageHandle )
			{
				pImageHandle->mlDMIndex = lIndex;

				// store for StartFrame / EndFrame use
				m_AcquisitionBufferQueue.AddTail( pImageHandle );

				if ( m_LogFile.m_hFile != CFile::hFileNull )
				{
					__int64 iTimer=0;
					if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
					{
						CString s;
						s.Format(_T("%I64u, , ,  . Grab request\n"), TicksToMicroseconds(iTimer) );
						m_LogFile.WriteString(s);
						TRACE(s);
						s.Format(_T("MdigGrab MIL_ID = %d\n"), pImageHandle->milGrab);
						m_LogFile.WriteString(s);
						TRACE(s);
					}
				}

				//MbufHookFunction( pImageHandle->milGrab, M_MODIFIED_BUFFER + M_UNHOOK, GrabBufferFillCallback, this );

				//MbufClear( pImageHandle->milGrab, 0.0 );	// Is this necessary?

				//MbufHookFunction( pImageHandle->milGrab, M_MODIFIED_BUFFER, GrabBufferFillCallback, this );

				::InterlockedIncrement( const_cast<LPLONG> (&m_lOutstandingGrabRequests) );
				MdigGrab( m_Device.milDig, pImageHandle->milGrab );

				// check error!
				if ( SVMatroxAppManager::GetError() != M_NULL_ERROR )
				{
					if ( m_LogFile.m_hFile != CFile::hFileNull )
					{
						__int64 iTimer=0;
						if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
						{
							CString s;
							s.Format(_T("%I64u, Grab failed: %s\n"), TicksToMicroseconds(iTimer), SVMatroxAppManager::GetErrorString() );
							m_LogFile.WriteString(s);
							TRACE(s);
						}
					}
					else
					{
						#ifdef _DEBUG
							__int64 iTimer=0;
							if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
							{
								CString s;
								s.Format(_T("%I64u, Grab failed: %s\n"), TicksToMicroseconds(iTimer), SVMatroxAppManager::GetErrorString() );
								TRACE(s);
							}
						#endif
					}
				}

				if ( f_eAcquireMethod == MatroxAcquireMethodSingleGrab )
				{
					::InterlockedExchange(const_cast<LPLONG> (&m_lGrabStartFrameLock), 1);
				}
			}
		}// end while ( (hrIndex = GetNextIndex( lIndex )) == S_OK )

		if ( m_LogFile.m_hFile != CFile::hFileNull )
		{
			__int64 iTimer=0;
			if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
			{
				CString s;
				s.Format(_T("%I64u, QueueGrabs: %d requested, %d queued, hrIndex=%x\n"), TicksToMicroseconds(iTimer), p_lNumToQueue, lQueued, hrIndex );
				m_LogFile.WriteString(s);
				TRACE(s);
			}
		}

	}// end if ( pBuffer != NULL )

	return lQueued;
}


// StartProcess is called from the PPQ in response to a trigger callback
HRESULT SVMatroxAcquisitionClass::StartProcess( SVORequestClass* pRequest )	// TRIGGER
{
	HRESULT hr = S_OK;
	
	long lNumOutstandingTriggers = ::InterlockedIncrement( const_cast<LPLONG> (&m_lOutstandingTriggerCount) );
	
	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{
			if ( m_iLastTrigger == 0 )	// prime the timestamps
			{
				m_iLastTrigger = iTimer;
				m_iLastEndFrame = m_iLastTrigger;
			}
			SVProductInfoStruct* pProduct=NULL;
			pRequest->GetExtraData((void**) &pProduct);
			long lTriggerCount = pProduct != NULL ? pProduct->oTriggerInfo.lTriggerCount : 0;
			CString s;
			s.Format(_T("%I64u, %I64u, ,  _ Trigger; count=%d\n"),
				TicksToMicroseconds(iTimer),
				TicksToMicroseconds(iTimer - m_iLastTrigger),
				lTriggerCount );
			m_iLastTrigger = iTimer;
			m_LogFile.WriteString(s);
			TRACE(s);
		}
	}

	hr = SVAcquisitionClass::StartProcess( pRequest );
	
	if ( TheSVObserverApp.mpcmdStateViewer != NULL )
	{
		TheSVObserverApp.mpcmdStateViewer->AddString("%s - Start - %s", 
			DeviceName(), 
			"SVMatroxAcquisitionClass::StartProcess");
	}

	SVProductInfoStruct* pProduct = NULL;
	SVVirtualCamera* pCamera = NULL;
	
	pRequest->GetOwner((void**) &pCamera);
	pRequest->GetExtraData((void**) &pProduct);

	if ( f_eVerifyMethod == RunningAverageTimeout )
	{
		__int64 iTimestamp=0;
		pRequest->GetStartTick( iTimestamp );
		// queue timestamp
		m_ratInfo.vecTriggerTimestampQueue.push_back(iTimestamp);
	}

	ASSERT( pCamera != NULL );
	ASSERT( pProduct != NULL );


	if ( pCamera != NULL && pProduct != NULL )
	{
		if ( m_Device.bHardwareTrigger )
		{
			// we have to fake a start frame call from StartProcess (here)
			// because the StartFrame callback is meaningless on a 1394
			// (it happens immediately after a MdigGrab regardless of when the trigger occurs)
			// so if we queue grabs (we need to for hardware triggering),
			// we get no useful info from StartFrame

			// EB 20030918:
			// We are now faking the start frame when we get the end frame. This is because we
			// want to generate 2 NAKs in the T T EF scenario. We are unable at this time to reliably
			// determine which T the EF goes with.

			// EB 20031205:
			// The decision has been made to allow two outstanding triggers. In this scenario, the
			// fake start frame needs to happen at the time of the trigger, not the end frame

			// EB 20040120:
			// we are now using the RunningAverageTimeout method

			// EB 20040127:
			// we are now using the CalculatedDistance method

			switch ( f_eAcquireMethod )
			{
				case MatroxAcquireMethodQueue:
				{
					// retrieve current value (not intended to actually exchange values)
					if ( m_bStartedGrabRequests )
					{
						long lOutstandingGrabs = ::InterlockedCompareExchange( const_cast<LPLONG> (&m_lOutstandingGrabRequests), 0, 0xBAADF00D);
						if ( lOutstandingGrabs == 0 )
						{
							PrimeTheQueue();
						}
					}

					switch ( f_eVerifyMethod )
					{
						case AllowOutstandingTriggers:
						{
							if (  f_iNumOutstandingTriggersAllowed > 1 )
							{
								if ( !f_bContinuouslyNakOnOverTrigger && (lNumOutstandingTriggers > f_iNumOutstandingTriggersAllowed) )
								{
									// we have no room to queue more; ignore this trigger
									// this results in Tsf Tsf T EF Tsf
									lNumOutstandingTriggers = ::InterlockedDecrement( const_cast<LPLONG> (&m_lOutstandingTriggerCount) );
									TRACE("IGNORE TRIGGER\n");

								}
								else
								{
									if ( !m_Device.bRealStartFrame )	// only fake if we are not getting real SF (EB 20031205)
									{
										HRESULT hrStartFrame = OnStartFrame();
									}
								}
							}// end if (  f_iNumOutstandingTriggersAllowed > 1 )
						}// end case AllowOutstandingTriggers:
						break;

						case RunningAverageTimeout:
						{
							if (lNumOutstandingTriggers > f_iNumOutstandingTriggersAllowed)
							{
								// we have no room to queue more; ignore this trigger
								// this results in Tsf Tsf T EF Tsf
								lNumOutstandingTriggers = ::InterlockedDecrement( const_cast<LPLONG> (&m_lOutstandingTriggerCount) );
								TRACE(("IGNORE TRIGGER (outstanding triggers=%d)\n"), lNumOutstandingTriggers );
							}
						}// end case RunningAverageTimeout:
						break;

						case CalculatedLatency:
						{
							/*
							if (lNumOutstandingTriggers > f_iNumOutstandingTriggersAllowed)
							{
								// we have no room to queue more; ignore this trigger
								// this results in Tsf Tsf T EF Tsf
								lNumOutstandingTriggers = ::InterlockedDecrement( const_cast<LPLONG> (&m_lOutstandingTriggerCount) );
								TRACE(("IGNORE TRIGGER (outstanding triggers=%d)\n"), lNumOutstandingTriggers );
							}
							//*/
						}// end case CalculatedDistance:
						break;

					}// end switch ( f_eVerifyMethod )

				}// end case MatroxAcquireMethodQueue:

			}// end switch ( f_eAcquireMethod )

		}// end if ( m_Device.bHardwareTrigger )

		else	// software trigger
		{
			// software trigger; we will use StartProcess as the initiating event for a request for a grab
			// the StartFrame will occur immediately after this MdigGrab
			long lQueued = QueueGrabs(1);
		}// end if ( m_Device.bHardwareTrigger ) else

	}// end if ( pCamera != NULL && pProduct != NULL )

	return hr;
}


/*static*/HRESULT SVMatroxAcquisitionClass::StartGrabCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser)
{
	HRESULT hr = S_OK;
	SVMatroxAcquisitionClass* pThis = reinterpret_cast <SVMatroxAcquisitionClass*> (pUser);

	if ( pThis->m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{
			CString s;
			s.Format(_T("%I64u, , ,  -- GrabStart callback\n"), TicksToMicroseconds(iTimer) );
			pThis->m_LogFile.WriteString(s);
			TRACE(s);
		}
	}


	if ( ! pThis->m_Device.bHardwareTrigger )	// StartGrabCallback has no relevant meaning with a hardware trigger
	{
		hr = pThis->OnStartFrame();
	}

	return hr;
}

/*static*/HRESULT SVMatroxAcquisitionClass::StartFrameCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser)
{
	HRESULT hr = S_OK;
	SVMatroxAcquisitionClass* pThis = reinterpret_cast <SVMatroxAcquisitionClass*> (pUser);

	/*
	if ( pThis->m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{
			CString s;
			s.Format(_T("%I64u, , ,  -- StartFrame callback\n"), TicksToMicroseconds(iTimer) );
			pThis->m_LogFile.WriteString(s);
			TRACE(s);
		}
	}
	*/


	if ( pThis->m_Device.bHardwareTrigger )
	{
		//hr = pThis->OnStartFrame();	// don't call this from here
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::OnStartFrame(__int64 p_iTimestamp/*=0*/)
{
	HRESULT hr = S_OK;

	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{
			CString s;
			//s.Format(_T("%I64u, , ,  | Fake StartFrame\n"), iTimer);
			//m_LogFile.WriteString(s);
		}
	}

	if ( m_bStopProcessing )
	{
		CString s(_T("OnStartFrame::STOP PROCESSING flag is set\n"));
		if ( m_LogFile.m_hFile != CFile::hFileNull )
			m_LogFile.WriteString(s);
		TRACE(s);
		return hr;
	}

	if ( !IsOnline() )
		return hr;

	//ASSERT( ! m_AcquisitionBufferQueue.IsEmpty() );

	bool bPendingGrab = false;
	switch ( f_eAcquireMethod )
	{
		case MatroxAcquireMethodQueue:
			bPendingGrab = m_AcquisitionBufferQueue.GetCount() > 0;
			break;
		case MatroxAcquireMethodSingleGrab:
			bPendingGrab = ( ::InterlockedExchange(const_cast<LPLONG> (&m_lGrabStartFrameLock), 0) == 1 );	// if pending grab and no other SF yet
			break;
		case MatroxAcquireMethodBlockedGrab:
			bPendingGrab = true;
			break;
	}

	if ( bPendingGrab )
	{
		SVAcquisitionImageBufferHandleStruct* pAcqImage = NULL;
		m_AcquisitionBufferQueue.GetHead( &pAcqImage );	// look at head but don't remove until EndFrame

		if ( pAcqImage != NULL )
		{
			BOOL bResult = pAcqImage->NotifyStartFrame();
			
			SVODeviceClass* pDevice = NULL;
			GetStartDevice( &pDevice );

			if ( pDevice != NULL && pAcqImage != NULL )
			{
				SVOResponseClass* pResponse = NULL;
				hr = pDevice->GetNextResponse( &pResponse );
				#ifdef _DEBUG
				{
					unsigned __int64 iTimer=0;
					if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
					{
						const unsigned __int64 iDisplayRes = 1000000;	// display in microseconds
						unsigned __int64 iFreq=1;
						::QueryPerformanceFrequency( (LARGE_INTEGER*) &iFreq );
						long lIndex=-1;
						long lDMIndex = -1;
						if ( pResponse )
						{
							pResponse->GetIndex(lIndex);
							lDMIndex = pResponse->mlDMIndex;
						}
						//TRACE(_T("(%I64d 탎, this=%X) SF Response Index=%d DMIndex=%d\n"), (iTimer * iDisplayRes) / iFreq, this, lIndex, lDMIndex );
					}
				}
				#endif
				if ( hr == S_OK && pResponse != NULL )
				{
					hr = pResponse->Reset();

					if ( pResponse->SetIndex( pAcqImage->mlIndex ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pResponse->SetIsValid( TRUE ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pResponse->SetExtraData( 0 ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pResponse->SetIsComplete( TRUE ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( p_iTimestamp != 0 )
					{
						pResponse->SetStartTick( p_iTimestamp );
					}
					else if ( m_Device.bRealStartFrame )
					{
						// add delay to SF time to push it past the Trigger timestamp.
						// the T and the SF are in a race condition and randomly come in before each other.

						unsigned __int64 iFreq=1;
						::QueryPerformanceFrequency( (LARGE_INTEGER*) &iFreq );
						__int64 iTimestamp;
						pResponse->GetStartTick(iTimestamp);
						iTimestamp += (iFreq / ((1000/250) * 1000));	// add 250 탎ec
						pResponse->SetStartTick(iTimestamp);
					}

					if ( pDevice->Notify( pResponse ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( m_LogFile.m_hFile != CFile::hFileNull )
					{
						unsigned __int64 iTimer=0;
						if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
						{
							const unsigned __int64 iDisplayRes = 1000000;	// display in microseconds
							unsigned __int64 iFreq=1;
							::QueryPerformanceFrequency( (LARGE_INTEGER*) &iFreq );
							long lIndex=-1;
							long lDMIndex=-1;
							if ( pResponse )
							{
								pResponse->GetIndex(lIndex);
								lDMIndex = pResponse->mlDMIndex;
							}
							CString s;
							s.Format(_T("%I64u, %d, ,   StartFrame; Index\n"), TicksToMicroseconds(iTimer), lIndex);
							m_LogFile.WriteString(s);
							TRACE(s);
							//TRACE(_T("(%I64d 탎, this=%X) EF Response Index=%d DMIndex=%d\n"), (iTimer * iDisplayRes) / iFreq, pThis, lIndex, lDMIndex );
						}
					}
				}// end if ( hr == S_OK && pResponse != NULL )
			}// end if ( pDevice != NULL && pAcqImage != NULL )
		}// end if ( pAcqImage != NULL )
		else
		{
			hr = S_FALSE;
		}
	}
	else
	{
		// no grab pending; ignore trigger/SF
	}

	if ( m_Device.bHardwareTrigger )
	{
		// keep grab queue full;
		// QueueGrabs has a test to see if we're offline, so we don't get stuck in an infinite loop
		long lQueued=0;
		switch ( f_eAcquireMethod )
		{
			case MatroxAcquireMethodQueue:
			case MatroxAcquireMethodSingleGrab:
				break;
			
			case MatroxAcquireMethodBlockedGrab:
				lQueued = QueueGrabs(1);
				break;
		}// end switch (f_eAcquireMethod)
	}

	return hr;
}

/*static*/HRESULT SVMatroxAcquisitionClass::EndFrameCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser)
{
	SVMatroxAcquisitionClass* pThis = reinterpret_cast <SVMatroxAcquisitionClass*> (pUser);

#ifdef _DEBUG
	__int64 iTimer=0;
	if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
	{
		CString s;
		s.Format(_T("%I64u, , ,  -- EndFrame callback\n"), TicksToMicroseconds(iTimer) );
		if ( pThis->m_LogFile.m_hFile != CFile::hFileNull )
			pThis->m_LogFile.WriteString(s);
		TRACE(s);
	}
#endif

	return S_OK;
}

/*static*/HRESULT SVMatroxAcquisitionClass::EndGrabCallback(long lHookType, MIL_ID milEventId, void MPTYPE* pUser)
{
	SVMatroxAcquisitionClass* pThis = reinterpret_cast <SVMatroxAcquisitionClass*> (pUser);

#ifdef _DEBUG
	__int64 iTimer=0;
	if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
	{
		CString s;
		s.Format(_T("%I64u, , ,  -- EndGrab callback\n"), TicksToMicroseconds(iTimer) );
		if ( pThis->m_LogFile.m_hFile != CFile::hFileNull )
			pThis->m_LogFile.WriteString(s);
		TRACE(s);
	}
#endif

	return pThis->OnEndFrame();
}

HRESULT SVMatroxAcquisitionClass::OnEndFrame()
{
	HRESULT hr = S_OK;

	// EB 20030918
	long lOutstandingTriggers;
	switch ( f_eAcquireMethod )
	{
		case MatroxAcquireMethodQueue:
			// retrieve current value (not intended to actually exchange values)
			lOutstandingTriggers = ::InterlockedCompareExchange( const_cast<LPLONG> (&m_lOutstandingTriggerCount), 0, 0xBAADF00D);
			::InterlockedDecrement( const_cast<LPLONG> (&m_lOutstandingGrabRequests) );
			break;


		case MatroxAcquireMethodBlockedGrab:
		case MatroxAcquireMethodSingleGrab:
			// get count the number of outstanding triggers, then reset to 0
			lOutstandingTriggers = ::InterlockedExchange( const_cast<LPLONG> (&m_lOutstandingTriggerCount), 0);
			break;
		
	}

	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{
			CString s;
			s.Format(_T("%I64u, %I64u, %I64u,  * EndFrame/EndGrab handler\n"),
				TicksToMicroseconds(iTimer),
				TicksToMicroseconds(iTimer - m_iLastEndFrame),
				TicksToMicroseconds(iTimer - m_iLastTrigger) );
			m_LogFile.WriteString(s);
			TRACE(s);
			m_iLastEndFrame = iTimer;
		}
	}


	bool bProcess = false;
	switch ( f_eAcquireMethod )
	{
		case MatroxAcquireMethodQueue:
			{
				bProcess = !mbGoingOffline;
				if ( !m_Device.bRealStartFrame && f_eVerifyMethod == AllowOutstandingTriggers )
				{
					if ( lOutstandingTriggers > f_iNumOutstandingTriggersAllowed )
					{
						bProcess = false;
						m_bStopProcessing = true;
					}
				}
			}
			break;
		case MatroxAcquireMethodSingleGrab:
			bProcess = (lOutstandingTriggers == 1);// Only if there is one and only one outstanding trigger will we process the end frame
			break;
		case MatroxAcquireMethodBlockedGrab:
			bProcess = !mbGoingOffline;
			break;
	}// end switch (f_eAcquireMethod)

	if ( bProcess )
	{
		SVAcquisitionImageBufferHandleStruct* pAcqImage = NULL;
		__int64 iTimestamp = 0;

		bool bSendFakeStartFrame = !m_Device.bRealStartFrame;// only fake if we are not getting real SF (EB 20031205)
		switch ( f_eAcquireMethod )
		{
			case MatroxAcquireMethodQueue:
			{
				switch ( f_eVerifyMethod )
				{
					case AllowOutstandingTriggers:
						bSendFakeStartFrame = bSendFakeStartFrame && (f_iNumOutstandingTriggersAllowed == 1);// EB 20031205
						break;

					case RunningAverageTimeout:
					{
						bool bNak = true;
						TRACE(_T("Outstanding Triggers=%d\n"), lOutstandingTriggers);
						if (   lOutstandingTriggers <= f_iNumOutstandingTriggersAllowed
							&& lOutstandingTriggers > 0 )
						{
							__int64 iCurrentTime = 0;
							::QueryPerformanceCounter( (LARGE_INTEGER*) &iCurrentTime );

							__int64 iAverage = ComputeNextRunningAverage( iCurrentTime );
							if ( m_LogFile.m_hFile != CFile::hFileNull )
							{
								CString s;
								s.Format(_T("             SF offset (RA)= %I64u\n"), iAverage );
								m_LogFile.WriteString(s);
								TRACE(s);
							}

							iAverage = iAverage * (f_iClockFrequency / 1000000);
							iTimestamp = iCurrentTime - ((iAverage * 14) / 20);	// 70%

							bNak = false;
						}

						if ( bNak )
							bSendFakeStartFrame = false;
						else
							{} //leave bSendFakeStartFrame as it is.

					}// end case RunningAverageTimeout:
					break;

					case CalculatedLatency:
					{
						bool bNak = true;
						TRACE(_T("Outstanding Triggers=%d\n"), lOutstandingTriggers);
						/*
						if (   lOutstandingTriggers <= f_iNumOutstandingTriggersAllowed
							&& lOutstandingTriggers > 0 )
						//*/
						{
							__int64 iCurrentTime = 0;
							::QueryPerformanceCounter( (LARGE_INTEGER*) &iCurrentTime );

							__int64 iDistance = m_lCalculatedLatency;

							if ( m_LogFile.m_hFile != CFile::hFileNull )
							{
								CString s;
								s.Format(_T("             SF offset (CL)= %I64u\n"), iDistance );
								m_LogFile.WriteString(s);
								TRACE(s);
							}

							iDistance = iDistance * (f_iClockFrequency / 1000000);	// convert from microseconds to clock ticks
							//iDistance = (iDistance * 16) / 20;	// 80%
							iTimestamp = iCurrentTime - iDistance;


							bNak = false;
						}

						if ( bNak )
							bSendFakeStartFrame = false;
						else
							{} //leave bSendFakeStartFrame as it is.

					}
					break;
				}// end switch ( f_eVerifyMethod )

			}// end case MatroxAcquireMethodQueue:
			break;
		}// end switch ( f_eAcquireMethod )


		// EB 20030918:
		// generate a fake start frame here (only if only one outstanding trigger) rather than on the trigger.
		// This will allow us to NAK both triggers in the T T EF scenario.
		if ( bSendFakeStartFrame )	
		{
			HRESULT hrStartFrame = OnStartFrame( iTimestamp );
		}

		//TRACE("(%d) EndFrame - %d grab(s) on queue\n", ::GetTickCount(), pThis->m_AcquisitionBufferQueue.GetCount());

		//if ( !m_AcquisitionBufferQueue.IsEmpty() )
		//	pAcqImage = m_AcquisitionBufferQueue.RemoveHead(); // remove image from queue
		m_AcquisitionBufferQueue.RemoveHead( &pAcqImage ); // remove image from queue


		switch ( f_eAcquireMethod )
		{
			case MatroxAcquireMethodQueue:
				{
					if ( m_lOutstandingTriggerCount > 0 )
						::InterlockedDecrement( const_cast<LPLONG> (&m_lOutstandingTriggerCount) );
				}
				break;
		}

		if ( pAcqImage != NULL )
		{
			BOOL bResult = pAcqImage->NotifyEndFrame();
			
			SVODeviceClass* pDevice = NULL;
			GetEndDevice( &pDevice );

			if ( pDevice != NULL )
			{
				SVOResponseClass* pResponse = NULL;
				hr = pDevice->GetNextResponse( &pResponse );

				if ( hr == S_OK && pResponse != NULL )
				{
					hr = pResponse->Reset();

					pResponse->mlDMIndex = pAcqImage->mlDMIndex;

					if ( pResponse->SetIndex( pAcqImage->mlIndex ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pResponse->SetIsValid( TRUE ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pResponse->SetExtraData( 0 ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pResponse->SetIsComplete( TRUE ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( pDevice->Notify( pResponse ) != S_OK )
					{
						hr = S_FALSE;
					}

					if ( m_LogFile.m_hFile != CFile::hFileNull )
					{
						unsigned __int64 iTimer=0;
						if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
						{
							const unsigned __int64 iDisplayRes = 1000000;	// display in microseconds
							unsigned __int64 iFreq=1;
							::QueryPerformanceFrequency( (LARGE_INTEGER*) &iFreq );
							long lIndex=-1;
							long lDMIndex=-1;
							if ( pResponse )
							{
								pResponse->GetIndex(lIndex);
								lDMIndex = pResponse->mlDMIndex;
							}
							CString s;
							s.Format(_T("%I64u, %d, %d,   EF Index, DM index\n"), TicksToMicroseconds(iTimer), lIndex, lDMIndex);
							m_LogFile.WriteString(s);
							TRACE(s);
							//TRACE(_T("(%I64d 탎, this=%X) EF Response Index=%d DMIndex=%d\n"), (iTimer * iDisplayRes) / iFreq, pThis, lIndex, lDMIndex );
						}
					}
				}// end if ( hr == S_OK && pResponse != NULL )


			}// end if ( pDevice != NULL )

			if ( m_LogFile.m_hFile != CFile::hFileNull )
			{
				CString s;
				s.Format(_T("EndGrab MIL_ID = %d\n\n"), pAcqImage->milGrab );
				m_LogFile.WriteString(s);
				TRACE(s);
			}
		}// end if ( pAcqImage != NULL )
	}// end if ( bProcess )

	else	// EF received with no outstanding triggers or with multiple outstanding triggers (NAK all)
	{
		// start from scratch; empty queue
		SVAcquisitionImageBufferHandleStruct* pAcqImage = NULL;
		m_AcquisitionBufferQueue.RemoveHead( &pAcqImage ); // remove image from queue
		while ( pAcqImage != NULL )
		{
			pAcqImage->NotifyEndFrame();	// ??? do we want to do this?

			if ( m_LogFile.m_hFile != CFile::hFileNull )
			{
				CString s;
				s.Format(_T("NAK grab MIL_ID = %d\n"), pAcqImage->milGrab );
				m_LogFile.WriteString(s);
				TRACE(s);
			}

			m_AcquisitionBufferQueue.RemoveHead( &pAcqImage ); // remove image from queue
			switch ( f_eAcquireMethod )
			{
				case MatroxAcquireMethodQueue:
					::InterlockedDecrement( const_cast<LPLONG> (&m_lOutstandingTriggerCount) );
					break;
			}
		}
	}

	if ( m_bStopProcessing )
	{
		CString s(_T("OnEndFrame::STOP PROCESSING flag is set\n"));
		if ( m_LogFile.m_hFile != CFile::hFileNull )
			m_LogFile.WriteString(s);
		TRACE(s);
		return hr;
	}

	if ( m_Device.bHardwareTrigger )
	{
		// keep grab queue full;
		// QueueGrabs has a test to see if we're offline, so we don't get stuck in an infinite loop
		long lQueued=0;
		switch ( f_eAcquireMethod )
		{
			case MatroxAcquireMethodQueue:
			case MatroxAcquireMethodSingleGrab:
				{
					long lOutstandingGrabs = ::InterlockedCompareExchange( const_cast<LPLONG> (&m_lOutstandingGrabRequests), 0, 0xBAADF00D);
					if ( lOutstandingGrabs == 0 )
						lQueued = PrimeTheQueue();
					else
						lQueued = QueueGrabs( 1 );
				}
				break;
			
			case MatroxAcquireMethodBlockedGrab:
				break;
		}// end switch (f_eAcquireMethod)
	}


	return hr;
}


HRESULT SVMatroxAcquisitionClass::EndProcess( SVORequestClass* pRequest )
{
	HRESULT hr = S_FALSE;

	hr = SVAcquisitionClass::EndProcess( pRequest );

	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{

			long lIndex = -1, lDMIndex = -1;
			if ( pRequest )
			{
				pRequest->GetIndex( lIndex );
				lDMIndex = pRequest->mlDMIndex;
			}
			CString s;
			s.Format(_T("%I64u, %d, %d,  - EndProcess index, dm index\n"), TicksToMicroseconds(iTimer), lIndex, lDMIndex );
			m_iLastEndFrame = iTimer;
			m_LogFile.WriteString(s);
			TRACE(s);
		}
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::ExecuteCallback( SVOCallbackPtr pCallback,
	                             void *pvOwner, void *pvCaller,
	                             SVORequestClass *pRequest)
{
	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		__int64 iTimer=0;
		if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
		{

			long lIndex = -1, lDMIndex = -1;
			if ( pRequest )
			{
				pRequest->GetIndex( lIndex );
				lDMIndex = pRequest->mlDMIndex;
			}
			CString s;
			s.Format(_T("%I64u, %d, %d,  - ExecuteCallback index, dm index\n"), TicksToMicroseconds(iTimer), lIndex, lDMIndex );
			m_iLastEndFrame = iTimer;
			m_LogFile.WriteString(s);
			TRACE(s);
		}
	}

	HRESULT hr = SVAcquisitionClass::ExecuteCallback( pCallback, pvOwner, pvCaller, pRequest );
	return hr;
}


HRESULT SVMatroxAcquisitionClass::NotifyStartFrame( SVAcquisitionImageBufferHandleStruct* pHandle )
{
	HRESULT hr = SVAcquisitionClass::NotifyStartFrame( pHandle );
	return hr;
}

HRESULT SVMatroxAcquisitionClass::NotifyEndFrame( SVAcquisitionImageBufferHandleStruct* pHandle )
{
	HRESULT hr = SVAcquisitionClass::NotifyEndFrame( pHandle );
	return hr;
}

HRESULT SVMatroxAcquisitionClass::LockBuffer( SVAcquisitionImageBufferHandleStruct* pHandle )
{
	HRESULT hr = SVAcquisitionClass::LockBuffer( pHandle );
	return hr;
}

HRESULT SVMatroxAcquisitionClass::UnlockBuffer( SVAcquisitionImageBufferHandleStruct* pHandle )
{
	HRESULT hr = SVAcquisitionClass::UnlockBuffer( pHandle );
	return hr;
}

SVDeviceParamCollection& SVMatroxAcquisitionClass::HardwareCapabilities()
{
	return m_Device;
}


HRESULT SVMatroxAcquisitionClass::SetHardwareDeviceCapabilities( const SVDeviceParamCollection& rDeviceParamsBase )
{
	HRESULT hr = S_OK;
	try
	{
		// if this throws, it's because only a base SVDeviceParamCollection object was passed in.
		const SVMatroxDeviceParamCollection& rDeviceParams = dynamic_cast<const SVMatroxDeviceParamCollection&> ( rDeviceParamsBase );

		m_Device = rDeviceParams;
		m_Device.milDig = rDeviceParams.milDig;
		m_Device.milSystem = rDeviceParams.milSystem;

		//SetDeviceName( StringValue(rDeviceParams.Parameter(DeviceParamDeviceChannelName)) );
		SetDigName( StringValue(rDeviceParams.Parameter(DeviceParamDeviceName)) );
		SetDigNumber( LongValue(rDeviceParams.Parameter(DeviceParamDeviceDigNum)) );
		if (rDeviceParams.ParameterExists(DeviceParamSizeBands))
		{
			SetBandSize( LongValue(rDeviceParams.Parameter(DeviceParamSizeBands)) );
		}
		//SetBandMaxSize( m_Device.lBands );

		hr = GetCameraImageInfo( &msvImageInfo );
	}
	catch (std::bad_cast& )
	{
		ASSERT(FALSE);
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = SVAcquisitionClass::SetDeviceParameters( rDeviceParams );
	if ( hr == S_OK )
	{
		hr = InitializeDevice( rDeviceParams );
	}
	return hr;
}

HRESULT SVMatroxAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	HRESULT l_hrOk = S_OK;

	SVDeviceParamCollection l_HardwareParams;
	GetHardwareDeviceCapabilities(l_HardwareParams);

	// check for differences in vendor
	if (   StringValue(l_HardwareParams.Parameter( DeviceParamVendorId ))
		!= StringValue(rDeviceParams.Parameter( DeviceParamVendorId )) )
	{
		l_hrOk = S_FALSE;
	}
	// check for differences in model
	else if (   l_HardwareParams.ParameterExists( DeviceParamModelName )
		&& rDeviceParams.ParameterExists( DeviceParamModelName ) )
	{
		CString sHardwareModel = StringValue(l_HardwareParams.Parameter( DeviceParamModelName ));
		CString sModel = StringValue(rDeviceParams.Parameter( DeviceParamModelName ));
		if (   sHardwareModel != _T("") && sModel != _T("")
			&& sHardwareModel.CompareNoCase(sModel) != 0 )
		{
			l_hrOk = S_FALSE;
		}
	}

	return l_hrOk;
}

HRESULT SVMatroxAcquisitionClass::InitializeDevice( const SVDeviceParamWrapper& rwParam )
{
	HRESULT hr = S_OK;

	if (   m_Device.ParameterExists( DeviceParamVendorId )
		&& m_DeviceParams.ParameterExists( DeviceParamVendorId ))
	{
		if (   StringValue(m_Device.Parameter( DeviceParamVendorId ))
			!= StringValue(m_DeviceParams.Parameter( DeviceParamVendorId )) )
		{
			hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
		}

	}

	if ( hr == S_OK )
	{
		UpdateLightReferenceAttributes( rwParam );
		if ( ( m_Device.bValidDigitizer && CameraMatchesCameraFile() ) || (rwParam->Type() == DeviceParamCameraFormats) )
		{
			if ( rwParam.IsValid() )
			{
				if ( rwParam->Type() == DeviceParamCameraFormats )
				{
					hr = m_pCameraDriver->SetParameter( rwParam );
					// special case for CameraFormat... it resets the dig, so we need to send all of our params again
					InitializeDevice( m_DeviceParams );
				}
				else if ( m_Device.milDig != M_NULL )
				{
					hr = m_pCameraDriver->SetParameter( rwParam );
				}
			}
		}
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::InitializeDevice( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	if (   m_Device.ParameterExists( DeviceParamVendorId )
		&& m_DeviceParams.ParameterExists( DeviceParamVendorId ))
	{
		if (   StringValue(m_Device.Parameter( DeviceParamVendorId ))
			!= StringValue(m_DeviceParams.Parameter( DeviceParamVendorId )) )
		{
			hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
		}

	}

	UpdateLightReferenceAttributes( rDeviceParams );

	if ( hr == S_OK )
	{
		// always send this even if dig is NULL!
		// special case for CameraFormat... it resets the dig, so send it first
		if ( rDeviceParams.ParameterExists( DeviceParamCameraFormats ) )
		{
			const SVCameraFormatsDeviceParam* pParam = rDeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
			hr = m_pCameraDriver->SetParameter( *pParam );
		}

		if ( m_Device.bValidDigitizer && CameraMatchesCameraFile() )
		{
			if ( m_Device.milDig != M_NULL )
			{
				if ( m_pCameraDriver )
				{
					hr = m_pCameraDriver->SetParameters( rDeviceParams );
				}
				else
				{
					hr = S_OK;
				}
			}
		}// end if ( milDig != M_NULL )
	}

	return hr;
}

HRESULT SVMatroxAcquisitionClass::UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams )
{
	SVDeviceParamMap::const_iterator iter;
	for ( iter = rParams.mapParameters.begin(); iter != rParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		UpdateLightReferenceAttributes( rw );
	}
	return S_OK;
}

HRESULT SVMatroxAcquisitionClass::UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam )
{
	if ( rwParam.IsValid() )
	{
		switch ( rwParam->Type() )
		{
			case DeviceParamWhiteBalanceUB:
			case DeviceParamWhiteBalanceVR:
			{
				
				const SVLongValueDeviceParam* pParam = rwParam.DerivedValue( pParam );
				if ( mLightReference.NumBands() > 1 )
				{
					int iBand = rwParam->Type() == DeviceParamWhiteBalanceVR ? 0 : 2;
					UpdateLightReferenceAttributes( iBand, pParam );
				}
				
			}
			break;

			case DeviceParamBrightness:
			case DeviceParamGain:
			case DeviceParamExposure:
			case DeviceParamGamma:
			case DeviceParamHue:
			case DeviceParamSaturation:
			{
				// update LR
//				for (int iBand = 0; iBand < mLightReference.NumBands(); iBand++)
				for (int iBand = 0; iBand < 1; iBand++)	// we don't need duplicate data for each band; these are not specific to each band
				{
					const SVLongValueDeviceParam* pParam = rwParam.DerivedValue( pParam );
					UpdateLightReferenceAttributes( iBand, pParam );
				}
			}
		}
	}

	return S_OK;
}
HRESULT SVMatroxAcquisitionClass::GetCameraImageInfo(SVImageInfoClass* pImageInfo)
{

	ASSERT( pImageInfo );
	HRESULT hr = E_INVALIDARG;
	if ( pImageInfo != NULL )
	{
		UINT32 uiBufWidth = 640;
		UINT32 uiBufHeight = 480;
		UINT32 uiBufFormat = 0;
		UINT32 uiPixDepth = 8;
		UINT32 uiBandLink = 0;
		UINT32 uiNumberOfBands = 1;
		SVImageFormatEnum uiFormat = SVImageFormatUnknown;

		MIL_ID mil = m_Device.milDig;

		{

			SVCameraFormatsDeviceParam* pParam;
			pParam = m_DeviceParams.GetParameter( DeviceParamCameraFormats).DerivedValue( pParam );
			if ( pParam )
			{
				SVCameraFormat& rFormat = pParam->options[ pParam->strValue ];
				uiBufWidth = rFormat.lWidth;
				uiBufHeight = rFormat.lHeight;
				uiFormat = (SVImageFormatEnum) rFormat.eImageType;
			}

			switch ( uiFormat )
			{
				case SVImageFormatRGB888:
				case SVImageFormatRGB8888:
					uiPixDepth = 8;
					uiNumberOfBands = 3;
					break;
				case SVImageFormatMono8:
					uiPixDepth = 8;
					uiNumberOfBands = 1;
					break;
			}

			pImageInfo->SetProperty( SVImagePropertyFormat, uiFormat );
			pImageInfo->SetProperty( SVImagePropertyPixelDepth, uiPixDepth );
			pImageInfo->SetProperty( SVImagePropertyBandNumber, uiNumberOfBands );
			pImageInfo->SetProperty( SVImagePropertyBandLink, uiBandLink );

			pImageInfo->SetProperty( SVExtentPropertyOutputPositionPoint, 0 );
			pImageInfo->SetProperty( SVExtentPropertyWidth, uiBufWidth );
			pImageInfo->SetProperty( SVExtentPropertyHeight, uiBufHeight );

			hr = S_OK;
		}
	}// end if ( pImageInfo != NULL )

	return hr;
}

HRESULT SVMatroxAcquisitionClass::WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )
{
	return SVMatroxDeviceManager::WriteCameraRegister( m_Device.milSystem, m_Device.milDig, ulAddress, ulValue );
}

HRESULT SVMatroxAcquisitionClass::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return SVMatroxDeviceManager::ReadCameraRegister( m_Device.milSystem, m_Device.milDig, ulAddress, rulValue );
}

HRESULT SVMatroxAcquisitionClass::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return SVMatroxDeviceManager::WriteCameraRegisterBlock( m_Device.milSystem, m_Device.milDig, ulAddress, ulCount, paulValue );
}

HRESULT SVMatroxAcquisitionClass::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return SVMatroxDeviceManager::ReadCameraRegisterBlock( m_Device.milSystem, m_Device.milDig, ulAddress, ulCount, paulValue );
}


HRESULT SVMatroxAcquisitionClass::SetMatroxDeviceManagerPointer( SVMatroxDeviceManager* pDeviceManager )
{
	m_pDeviceManager = pDeviceManager;
	return S_OK;
}


HRESULT SVMatroxAcquisitionClass::CreateImageAcquisitionBuffer(SVImageInfoClass& rInfo,
                                                 SVAcquisitionImageBufferHandleStruct& rHandle )
{
	HRESULT hrOk = S_FALSE;
//	if( pAcqDevice != NULL )
	{
		int l_iFormat = SVImageFormatUnknown;
		int l_iPixelDepth = 0;
		int l_iBandNumber = 0;
		int l_iBandLink = 0;
		long l_lHeight = 0;
		long l_lWidth = 0;

		rInfo.GetProperty( SVExtentPropertyWidth, l_lWidth );
		rInfo.GetProperty( SVExtentPropertyHeight, l_lHeight );

		rInfo.GetProperty( SVImagePropertyFormat, l_iFormat );
		rInfo.GetProperty( SVImagePropertyBandNumber, l_iBandNumber );
		rInfo.GetProperty( SVImagePropertyBandLink, l_iBandLink );
		rInfo.GetProperty( SVImagePropertyPixelDepth, l_iPixelDepth );

		if( rHandle.milGrab == NULL &&
			l_lWidth > 0 && l_lHeight > 0 &&
			l_iPixelDepth > 0  && l_iBandNumber > 0 )
		{
			UINT32 uiFormat = 0;
			long lMilBufAttributes = M_IMAGE + M_PROC + M_GRAB + M_DIB + M_NON_PAGED;

			switch ( l_iFormat )
			{
				case SVImageFormatMono8:
				{
					// no change
					break;
				}
				case SVImageFormatRGB888:
				{
					lMilBufAttributes += M_BGR24;
					break;
				}
				case SVImageFormatRGB8888:
				{
					lMilBufAttributes += M_BGR32;
					break;
				}
				default:
				{
					// no change
					break;
				}
			}// end switch ( rInfo.muiFormat )

			SVMatroxDeviceParamCollection device;
			if ( m_pDeviceManager->GetDevice( DigName(), device ) == S_OK )
			{
				MIL_ID milBuffer = M_NULL;
				ASSERT( rHandle.milGrab == M_NULL );
				if ( rHandle.milGrab == M_NULL )
				{
					milBuffer = MbufAllocColor( device.milSystem,
					                            l_iBandNumber,
					                            l_lWidth,
					                            l_lHeight,
					                            l_iPixelDepth,
					                            lMilBufAttributes,
					                            M_NULL );
				}
				else
				{
					// should not get here!!!
					ASSERT( rHandle.milGrab == M_NULL );
				}

				long lError = SVMatroxAppManager::GetError();
				if ( lError != M_NULL )
				{
					CString sMsg = SVMatroxAppManager::GetErrorStringFull();
					TRACE(sMsg+_T("\n"));
					SVException e;
					//SETEXCEPTION1(e, SVMSG_SVO_18_OUT_OF_NP_MEMORY, sMsg);
					SETEXCEPTION5(e, SVMSG_SVO_18_OUT_OF_NP_MEMORY, -12341, sMsg);
				}

				hrOk = milBuffer != M_NULL ? S_OK : S_FALSE;
				if ( hrOk == S_OK )
				{
					UINT32 uiHostAddress = 0;
					// Get the buffer's host address...
					uiHostAddress = MbufInquire( milBuffer, M_HOST_ADDRESS, M_NULL );

					rHandle.PHostBuffer = (LPVOID) uiHostAddress;
					rHandle.milGrab = milBuffer;
					InitBuffer( rHandle, 0 );

					hrOk = mpImaging->CreateImageBufferOverlay( rInfo, rHandle, (void**) &uiHostAddress );
					if ( hrOk == S_OK )
					{

					}
					else
					{
						MbufFree( milBuffer );
					}
					
				}// end if ( bOk )

			}// end if ( m_DeviceManager.GetDevice( pAcqDevice->DigName(), device ) == S_OK )
			else
			{
				//bOk = SVBoardImageProcessingClass::CreateAcquisitionBuffer( pAcqDevice, rInfo, rHandle );
			}
		}// end if rInfo is OK
	}// end if( pAcqDevice != NULL )

	return hrOk;
}

HRESULT SVMatroxAcquisitionClass::DestroyImageAcquisitionBuffer( SVAcquisitionImageBufferHandleStruct& rHandle )
{
	HRESULT hrOk = S_OK;
	// Free the image buffer
	hrOk = mpImaging->DisplaySubsystem()->DestroyImageBufferOverlay( rHandle );

	MbufFree( rHandle.milGrab );
	long lErrorCode( 0 );
	lErrorCode = SVMatroxAppManager::GetStatusCode( _T(__FILE__), __LINE__ );
	ASSERT( lErrorCode == 0 );

	rHandle.milGrab = M_NULL;

	if ( hrOk == S_OK )
		hrOk = rHandle.milGrab == M_NULL ? S_OK : S_FALSE;

	return hrOk;
}

HRESULT SVMatroxAcquisitionClass::CreateSingleGrabImageBuffer(SVImageInfoClass& rInfo, SVAcquisitionImageBufferHandleStruct& rHandle)
{
	return CreateImageAcquisitionBuffer(rInfo, rHandle);
}

HRESULT SVMatroxAcquisitionClass::DestroySingleGrabImageBuffer(SVAcquisitionImageBufferHandleStruct& rHandle)
{
	return DestroyImageAcquisitionBuffer(rHandle);
}

HRESULT SVMatroxAcquisitionClass::InitBuffer( SVImageBufferHandleStruct& rHandle, DWORD dwValue )
{
	HRESULT hrOk = S_FALSE;

	ASSERT( rHandle.milGrab != M_NULL );
	hrOk = rHandle.milGrab != NULL ? S_OK : S_FALSE;
	if ( hrOk == S_OK )
	{
		MbufClear( rHandle.milGrab, ( double ) dwValue );
	}

	return hrOk;
}

HRESULT SVMatroxAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )	// for parameters that are settable through MIL (all standard DCAM params)
{
	HRESULT hr = S_OK;

	MIL_ID milDig = m_Device.milDig;

	if ( m_Device.bValidDigitizer || (rw->Type() == DeviceParamCameraFormats) )
	{
		if ( milDig != NULL || (rw->Type() == DeviceParamCameraFormats) )
		{
			switch ( rw->Type() )
			{
				case DeviceParamAcquisitionTriggerType:
				{
					SVDCamTriggerSourceEnum eTriggerSource = (SVDCamTriggerSourceEnum) LongValue( rw );
					switch ( eTriggerSource )
					{
						case HardwareTrigger:
						case SoftwareAsyncTrigger:
						{
							if ( IsOnline() )
							{
								MdigControl( milDig, M_GRAB_TRIGGER, M_DISABLE );
								MdigControl( milDig, M_GRAB_TRIGGER_SOURCE, M_HARDWARE_PORT0 ); // use for hardware triggerable cameras
								MdigControl( milDig, M_GRAB_TIMEOUT, M_INFINITE );	// for triggered cameras

								switch ( f_eAcquireMethod )
								{
									case MatroxAcquireMethodQueue:
										MdigControl( milDig, M_GRAB_MODE, M_ASYNCHRONOUS_QUEUED );	// this is the one we want
										break;

									case MatroxAcquireMethodSingleGrab:
									case MatroxAcquireMethodBlockedGrab:
										MdigControl( milDig, M_GRAB_MODE, M_ASYNCHRONOUS );	// this is the one we want
										break;
								}// end switch (f_eAcquireMethod)
							}// end if ( IsOnline() )
							else
							{
							}
							break;
						}// end  SoftwareAsyncTrigger:

						case SoftwareGrabTrigger:
						default:
							MdigControl( milDig, M_GRAB_TRIGGER_SOURCE, M_DEFAULT ); // grab-initiated
							//MdigControl( milDig, M_GRAB_TIMEOUT, 0 );
							break;
					}// end switch ( eTriggerSource )
				break;
				}// end case DeviceParamAcquisitionTriggerType:

				case DeviceParamWhiteBalanceUB:
				case DeviceParamWhiteBalanceVR:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					/*
					if ( mLightReference.NumBands() > 1 )
					{
						int iBand = rw->Type() == DeviceParamWhiteBalanceVR ? 0 : 2;
						UpdateLightReferenceAttributes( iBand, pParam );
					}
					*/
					const SVLongValueDeviceParam* pHardwareParam = m_Device.Parameter( rw->Type() ).DerivedValue( pHardwareParam );
					if ( pHardwareParam )
					{
						// If we set this, it forces manual mode; use M_DEFAULT to revert to automatic mode
						MdigControl( milDig, pHardwareParam->info.vendorId, pParam->lValue );
					}
				}
				break;


				case DeviceParamBrightness:
				case DeviceParamGain:
				case DeviceParamExposure:
				case DeviceParamGamma:
				case DeviceParamHue:
				case DeviceParamSaturation:
				case DeviceParamSharpness:
				case DeviceParamShutter:
				{
					const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
					if ( pParam )
					{
						// need to get the hardware param to get the vendor id (MIL_ID)
						const SVLongValueDeviceParam* pHardwareParam = m_Device.Parameter( rw->Type() ).DerivedValue( pHardwareParam );
						if ( pHardwareParam )
						{
							// EB 2005 08 11
							// Write directly to the register to avoid lock-up issues
							WriteCameraRegister( pHardwareParam->info.vendorId, pParam->lValue );
						}

					}
					break;
				}


				case DeviceParamCameraFormats:
				{
					ASSERT( !IsOnline() );
					if ( !IsOnline() )
					{
						const SVCameraFormatsDeviceParam* pParam = rw.DerivedValue( pParam );

						CString sOldDeviceFormat;
						SVMatroxDeviceParamCollection device;
						SVCameraFormatsDeviceParam* pDeviceParam = NULL;
						if ( m_pDeviceManager->GetDevice( DigName(), device ) == S_OK )
						{
							pDeviceParam = device.GetParameter( DeviceParamCameraFormats ).DerivedValue( pDeviceParam );
							sOldDeviceFormat = pDeviceParam->strValue;
						}
						else
						{
							sOldDeviceFormat = m_PreviousCameraFormat.m_strName;
						}

						const SVCameraFormat& rcf = pParam->options.find(pParam->strValue)->second;
						if ( pParam->strValue != sOldDeviceFormat || (m_Device.milDig == NULL && m_Device.bValidDigitizer) )
						{
							// we need to deallocate and reallocate the camera!!
							// assume all child buffers are destroyed??!!
							m_Device.SetParameter( DeviceParamCameraFormats, *pParam );

							if (true || !mbDontReallocDigFormat || sOldDeviceFormat == _T("M_RS170"))	// temp always realloc
							//if (!mbDontReallocDigFormat || sOldDeviceFormat == _T("M_RS170"))
							{
								HRESULT hrRealloc = m_pDeviceManager->ReallocDigitizer( m_Device );
								milDig = m_Device.milDig;	// this gets reassigned on Realloc
							}

							if ( milDig != M_NULL )
							{
								if ( rcf.bVariableROI )
								{
									MdigControl( milDig, M_SOURCE_COMPENSATION, M_DISABLE );	// software cropping if no hardware cropping (for now)
								}
								else
								{
									MdigControl( milDig, M_SOURCE_COMPENSATION, M_ENABLE );	// software cropping if no hardware cropping (for now)
								}
							}

							if ( rcf.bColor )
							{
								SetBandSize( 3 );
								SetBandMaxSize( 3 );
							}
							else
							{
								SetBandSize( 1 );
								SetBandMaxSize( 1 );
							}

						}// end if ( pParam->strValue != sDeviceFormat )

						// make sure the device param has all the information contained in the camera file
						const SVCameraFormatsDeviceParam* pCFParam = m_CameraFileDeviceParams.Parameter(DeviceParamCameraFormats).DerivedValue( pCFParam );
						if ( pCFParam )
						{
							const SVCameraFormat& rcfCamFile = pCFParam->options.find( pParam->strValue )->second;
							if ( rcf.lHeightMax == 0 || rcf.lWidthMax == 0 )
							{
								SVCameraFormat& rcfModify = const_cast <SVCameraFormat&> (rcf);
								rcfModify.AssignConstantValues( rcfCamFile );
								/*
								rcfModify.lHeightMax = rcfCamFile.lHeightMax;
								rcfModify.lWidthMax = rcfCamFile.lWidthMax;
								rcfModify.lFrameRate = rcfCamFile.lFrameRate;
								rcfModify.lHPosStep = rcfCamFile.lHPosStep;
								rcfModify.lHStep = rcfCamFile.lHStep;
								rcfModify.lVPosStep = rcfCamFile.lVPosStep;
								rcfModify.lVStep = rcfCamFile.lVStep;
								rcfModify.eImageType = rcfCamFile.eImageType;
								rcfModify.bColor = rcfCamFile.bColor;
								*/
							}
						}					

						if (   m_PreviousCameraFormat.lHeight != rcf.lHeight
							|| m_PreviousCameraFormat.lWidth != rcf.lWidth
							|| m_PreviousCameraFormat.lVPos != rcf.lVPos
							|| m_PreviousCameraFormat.lHPos != rcf.lHPos
							)
						{
							// Do this for non-format 7 as well to ensure no software cropping
							// according to documentation, should set size before offset
							if ( milDig != M_NULL )
							{
								MdigControl( milDig, M_SOURCE_SIZE_X, rcf.lHStep  ); // set minimum size first
								MdigControl( milDig, M_SOURCE_SIZE_Y, rcf.lVStep );  // set minimum size first
								MdigControl( milDig, M_SOURCE_OFFSET_X, 0 ); // set to origin to keep setting width from giving error
								MdigControl( milDig, M_SOURCE_OFFSET_Y, 0 ); // set to origin to keep setting height from giving error
								MdigControl( milDig, M_SOURCE_SIZE_X, rcf.lWidth  ); // use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
								MdigControl( milDig, M_SOURCE_SIZE_Y, rcf.lHeight ); // use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
								MdigControl( milDig, M_SOURCE_OFFSET_X, rcf.lHPos ); // use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
								MdigControl( milDig, M_SOURCE_OFFSET_Y, rcf.lVPos ); // use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
							}
						}

						// refresh image info
						GetCameraImageInfo( &msvImageInfo );



						m_PreviousCameraFormat = pParam->options.find(pParam->strValue)->second;
					}
					else
					{
						hr = SV_FALSE;
					}

				break;
				}// end case DeviceParamCameraFormats:
				default:
				{
					hr = S_FALSE;
					break;
				}
			}// end switch ( rw->Type() )
		}// end if ( milDig != NULL )
	}// end if valid digitizer

	return hr;
}

HRESULT SVMatroxAcquisitionClass::UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam )
{
	HRESULT hr = S_FALSE;
	SVLightReferenceAttributeInfo* pAttribute = mLightReference.Band( iBand ).AttributeByType( (DWORD) pParam->Type() );
	const SVLongValueDeviceParam* pCFParam = m_CameraFileDeviceParams.Parameter(pParam->Type()).DerivedValue( pCFParam );
	if ( pAttribute )
	{
		pAttribute->lValue = pParam->lValue;
		if ( pCFParam )
		{
			pAttribute->lMax = pCFParam->info.max;
			pAttribute->lMin = pCFParam->info.min;
		}
	}
	else
	{
		SVLightReferenceAttributeInfo attribute;

		switch ( pParam->Type() )
		{
			case DeviceParamGamma:
				attribute.strName = pCFParam->VisualName();
				break;
			case DeviceParamBrightness:
				attribute.strName = pCFParam->VisualName();
				break;
			case DeviceParamGain:
				attribute.strName = _T("Contrast");
				break;
			case DeviceParamExposure:
				attribute.strName = pCFParam->VisualName();
				break;
			case DeviceParamWhiteBalanceUB:
			case DeviceParamWhiteBalanceVR:
				attribute.strName = _T("White Balance");
				break;
		}
		
		attribute.dwType = (DWORD) pParam->Type();
		attribute.lStep = 1;
		attribute.lValue = pParam->lValue;
		if ( pCFParam )
		{
			attribute.lMax = pCFParam->info.max;
			attribute.lMin = pCFParam->info.max;
		}
		mLightReference.Band(iBand).AddAttribute( attribute );
		hr = S_OK;
	}// end if pAttribute else

	return hr;
}

bool SVMatroxAcquisitionClass::IsOnline()
{
	bool l_bOk = SVAcquisitionClass::IsOnline();

	return (l_bOk && !mbGoingOffline) || mbTempOnline;
}

DWORD WINAPI SVMatroxAcquisitionClass::SingleGrabHelperFn(LPVOID lpParameter)
{
	SVMatroxAcquisitionClass* pThis = ((SingleGrabHelperStruct*) lpParameter)->pThis;
	SVAcquisitionImageBufferHandleStruct* pHandle = ((SingleGrabHelperStruct*) lpParameter)->pHandle;
	MdigGrab( pThis->m_Device.milDig, pHandle->milGrab );
	return 0;
}

HRESULT SVMatroxAcquisitionClass::SingleGrab( SVAcquisitionImageBufferHandleStruct* pHandle )
{
	ASSERT( pHandle != NULL );

	HRESULT hr = S_FALSE;

	if ( !IsOnline() && m_Device.milDig != M_NULL )
	{
		mbTempOnline = true;
		if ( m_pCameraDriver->CanSoftwareTrigger() )
		{
			MdigControl( m_Device.milDig, M_GRAB_MODE, M_SYNCHRONOUS );
			MdigControl( m_Device.milDig, M_GRAB_TRIGGER_SOURCE, M_HARDWARE_PORT0 );
			MdigControl( m_Device.milDig, M_GRAB_TRIGGER, M_ENABLE );


			// turn strobe on if enabled (online)
			SVDeviceParamWrapper w = m_DeviceParams.Parameter(DeviceParamOutputStrobePulse);
			InitializeDevice(w);

			// do the grab in another thread
			SingleGrabHelperStruct helperinfo;
			helperinfo.pThis = this;
			helperinfo.pHandle = pHandle;
			DWORD dwThreadId=0;
			HANDLE hThread=0;
			hThread = ::CreateThread(NULL, 0, SingleGrabHelperFn, &helperinfo, 0, &dwThreadId);

			m_pCameraDriver->SoftwareTrigger();
			::WaitForSingleObject(hThread, 2000);
			

			long lMilError = M_NULL_ERROR;
			lMilError = SVMatroxAppManager::GetStatusCode( _T(__FILE__), __LINE__ );
			if ( lMilError == M_NULL_ERROR )
			{
				hr = S_OK;
			}

			mbTempOnline = false;

			// turn strobe off (offline)
			InitializeDevice(w);

			MdigControl( m_Device.milDig, M_GRAB_TRIGGER, M_DISABLE );
		}// end if ( m_pCameraDriver->CanSoftwareTrigger() )
		else
		{
			MdigControl( m_Device.milDig, M_GRAB_MODE, M_SYNCHRONOUS );
			MdigControl( m_Device.milDig, M_GRAB_TRIGGER_SOURCE, M_DEFAULT ); // grab-initiated
			
			// turn strobe on if enabled (online)
			SVDeviceParamWrapper w = m_DeviceParams.Parameter(DeviceParamOutputStrobePulse);
			InitializeDevice(w);

			// do the grab
			MdigGrab( m_Device.milDig, pHandle->milGrab );

			long lMilError = M_NULL_ERROR;
			lMilError = SVMatroxAppManager::GetStatusCode( _T(__FILE__), __LINE__ );
			if ( lMilError == M_NULL_ERROR )
			{
				hr = S_OK;
			}

			// turn strobe off (offline)
			InitializeDevice(w);
		}// end if can software trigger else
		mbTempOnline = false;
	}

	return hr;
}


HRESULT SVMatroxAcquisitionClass::AllocDigitizer( SVDeviceParamCollection& p_rDevice )
{
	HRESULT hr = S_OK;
	try
	{
		SVMatroxDeviceParamCollection& rDevice = dynamic_cast <SVMatroxDeviceParamCollection&> (p_rDevice);
		//m_pDeviceManager->AllocDigitizer();

		/*
		// reconnect buffers
		SVInspectionProcess* pInspection = NULL;
		for ( long lInspection = 0; lInspection < aInspections.GetSize(); lInspection++)
		{
			aInspections.GetAt(lInspection)->ConnectToolSetMainImage();
		}
		*/
	}
	catch ( std::bad_cast& )
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVMatroxAcquisitionClass::DeallocDigitizer( SVDeviceParamCollection& p_rDevice )
{
	HRESULT hr = S_OK;
	try
	{
		SVMatroxDeviceParamCollection& rDevice = dynamic_cast <SVMatroxDeviceParamCollection&> (p_rDevice);
		/*
		// disconnect buffers attached to dig
		long lInspectionCount = 0;
		SVInspectionProcess* pInspection = NULL;

		pConfig->GetInspectionCount( lInspectionCount );

		for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )
		{
			bool bHasCamera = false;
			pConfig->GetInspection( lInspection, &pInspection );
			if ( pInspection != NULL )
			{
				bool bFoundCamera=false;
				for (long lPPQ = 0; lPPQ < pInspection->m_arPPQs.GetSize(); lPPQ++)
				{
					SVPPQObject* pPPQ = pInspection->m_arPPQs.GetAt(lPPQ);
					long lCameraCount=0;
					pPPQ->GetCameraCount( lCameraCount );
					SVVirtualCamera* pCamera = NULL;
					for ( long lCamera=0; lCamera < lCameraCount; lCamera++)
					{
						pPPQ->GetCamera(lCamera, pCamera);
						if ( pCamera->mpsvDevice == pMatroxDevice )
						{
							bFoundCamera = true;
						}
					}// end for ( long lCamera=0; lCamera < lCameraCount; lCamera++)
				}// end for (long lPPQ = 0; lPPQ < pInspection->m_arPPQs.GetSize(); lPPQ++)
				if ( bFoundCamera )
				{
					pInspection->DisconnectToolSetMainImage();
					aInspections.Add( pInspection );
				}// end if ( bFoundCamera )
			}// end if ( pInspection != NULL )
		}// end for ( long lInspection = 0; lInspection < lInspectionCount; lInspection++ )


		*/
		//m_pDeviceManager->FreeDigitizer();
	}
	catch ( std::bad_cast& )
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVMatroxAcquisitionClass::LoadLightReference( SVLightReference& rArray )
{
	HRESULT hr = S_FALSE;

	// Light Reference needs to be changed in the 1394 before this can work.
	// returning S_FALSE will be sufficient to prevent bad things from happening
	// LR really needs to be revamped in the entire system!
	/*
	SVLightReferenceDeviceParam* pParam = m_CameraFileDeviceParams.Parameter(DeviceParamLightReference).DerivedValue( pParam );
	if ( pParam )
	{
		rArray = pParam->lr;
		hr = S_OK;
	}
	*/

	return hr;
}

// SVSubscriberInterface
// temp hack
SVOCallbackPtr SVMatroxAcquisitionClass::GetCallbackFunction(const CString& sType) const
{
	if ( sType == _T("StrobeInput") )
	{
		return StrobeStartFrameCallback;
	}
	else
	{
		return NULL;
	}
}

HRESULT SVMatroxAcquisitionClass::StrobeStartFrameCallback( void* pvOwner, void* pvCaller, void* pvData )
{
	SVMatroxAcquisitionClass* pThis = static_cast <SVMatroxAcquisitionClass*>
									              ( reinterpret_cast <SVSubscriberInterface*> (pvOwner) );

	if ( pThis->m_Device.bRealStartFrame )
	{
		if ( pThis->m_LogFile.m_hFile != CFile::hFileNull )
		{
			__int64 iTimer=0;
			if( ::QueryPerformanceCounter( (LARGE_INTEGER*) &iTimer ) )
			{
				CString s;
				s.Format(_T("%I64u, , ,  -- StrobeStartFrame callback\n"), TicksToMicroseconds(iTimer) );
				pThis->m_LogFile.WriteString(s);
				TRACE(s);
			}
		}

		HRESULT hr = pThis->OnStartFrame();
	}

	return S_OK;
}

bool SVMatroxAcquisitionClass::CameraMatchesCameraFile()
{
	bool bOk = false;

	if (   m_Device.ParameterExists( DeviceParamVendorId )
		&& m_CameraFileDeviceParams.ParameterExists( DeviceParamVendorId ))
	{
		if (   StringValue(m_Device.Parameter( DeviceParamVendorId ))
			== StringValue(m_CameraFileDeviceParams.Parameter( DeviceParamVendorId )) )
		{
			bOk = true;
		}
	}

	return bOk;
}

long SVMatroxAcquisitionClass::ComputeNextRunningAverage(__int64 iCurrentTime)
{
	// EB 20040120
	// get timestamp of first outstanding trigger
	if (m_ratInfo.vecTriggerTimestampQueue.size() > 0 )
	{
		__int64 iTriggerTimestamp = m_ratInfo.vecTriggerTimestampQueue[0];
		m_ratInfo.vecTriggerTimestampQueue.erase(m_ratInfo.vecTriggerTimestampQueue.begin());

		long lTimeDiff = (long) (iCurrentTime - iTriggerTimestamp);

		long lOrigVal = m_ratInfo.vecLatencyQueue[ m_ratInfo.iLatencyQueueIndex ];
		m_ratInfo.vecLatencyQueue[ m_ratInfo.iLatencyQueueIndex ] = lTimeDiff;
		++m_ratInfo.iLatencyQueueIndex;
		m_ratInfo.iLatencyQueueIndex %= m_ratInfo.vecLatencyQueue.size();

		// calculate running average
		if ( lOrigVal == 0 )
			++m_ratInfo.lLatencyCount;

		m_ratInfo.iLatencySum += lTimeDiff;
		m_ratInfo.iLatencySum -= lOrigVal;
	}

	return (long) (m_ratInfo.iLatencySum / m_ratInfo.lLatencyCount);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:30:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   04 Oct 2012 10:45:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   21 Jun 2007 14:16:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   19 Feb 2007 16:38:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   19 Feb 2007 06:42:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVO 4.78 Beta 064 branch code into main line code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   29 Nov 2006 15:40:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CreateSingleGrabImageBuffer method
 * Added DestroySingleGrabImageBuffer method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   29 Nov 2006 14:23:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DCamAcquisition Proxy
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 * 
 *    Rev 1.64.1.0   15 Feb 2007 12:01:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   21 Jun 2006 15:10:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  565
 * SCR Title:  Fix 1394 default greyscale camera mode problem with color camera file
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for mono product and adjust 1394 camera file reader for color camera files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   08 Nov 2005 15:59:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   22 Aug 2005 09:18:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  508
 * SCR Title:  Fix bug when loading or creating a 1394 config without a camera attached
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   always process DeviceParamCameraFormats
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   11 Aug 2005 11:54:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use direct register writes for light reference parameters to avoid lockup issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   22 Feb 2005 11:00:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  472
 * SCR Title:  Fix problems with Basler cameras
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated default information based on correct inversion information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   17 Feb 2005 14:50:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   30 Jul 2004 13:22:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   reset log timings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   30 Jul 2004 11:46:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  441
 * SCR Title:  Fix Light Reference Dialog - White Balance
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed SetLightReferenceImpl to work with multiple bands
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   15 Jul 2004 09:22:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  436
 * SCR Title:  Update ISG camera handler
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Format7 set size / position
 * fixed first timestamp for logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   08 Jun 2004 10:48:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated comment
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   25 May 2004 08:29:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetParameterDefaults method to either update the two new paramters from existing information or set to default value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   01 Apr 2004 14:51:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  408
 * SCR Title:  Fix Go Online problem with ISG camera
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with reading of the camera.  now it will only read, before it read and reset the camera info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   18 Feb 2004 09:39:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed latency calculation to prevent overrun
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   18 Feb 2004 09:35:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with previous start frame / end frame logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   18 Feb 2004 08:39:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   moved data manager buffer allocation to derived classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   18 Feb 2004 07:32:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   automatically add extra buffer space
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   17 Feb 2004 16:58:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem of continuous NAKs when triggering faster than the inspection can handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   16 Feb 2004 15:48:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made registry entry invisible
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   16 Feb 2004 11:49:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed unnecessary MbufClear
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   03 Feb 2004 17:31:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added CalculatedLatency acquisition method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   22 Jan 2004 12:27:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with RunningAverageTimeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   21 Jan 2004 15:29:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added RunningAverageTimeout method of acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   20 Jan 2004 09:05:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed strobe connected start frame.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   16 Jan 2004 07:55:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for StrobeStartFrame
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   12 Jan 2004 10:46:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  404
 * SCR Title:  Change Camera Parameters that represent time to use standard units
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes in SetParameterDefaults( 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   18 Dec 2003 14:49:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed Camera Manager problem that reset Device Channel name to Ch_0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   18 Dec 2003 14:48:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed Light Reference problems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   10 Dec 2003 11:27:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed fake start frame logic, added optional continuous NAK
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   10 Dec 2003 08:22:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added continuous NAKs if TTT
 * fixed breakage of other acq methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   09 Dec 2003 07:59:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   08 Dec 2003 15:55:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed merge omission
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   08 Dec 2003 15:19:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed:
 * - problem with Light Reference tabs not being updated
 * - problem with loading config with different physical camera attached
 * - changed T T T handling
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   05 Dec 2003 14:38:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made numoutstandingtriggersallowed settable in the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   05 Dec 2003 14:34:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed logic to allow multiple outstanding Triggers (2 by default)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   21 Nov 2003 09:57:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed DeviceParamCameraStrobePolarity to DeviceParamIOStrobeInvert
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   19 Nov 2003 11:53:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed color stuff
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   17 Nov 2003 14:39:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Queued acquire method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   12 Nov 2003 09:56:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added checks on VendorId to determine hardware compatibility
 * fixed color format band info
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   04 Nov 2003 09:34:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   04 Nov 2003 09:24:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed LR name retrevial method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   03 Nov 2003 16:24:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changes to allow unplugging and plugging cameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   31 Oct 2003 08:24:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general enhancements; grab queue; block register transfer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   22 Oct 2003 11:32:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added debugging support for blocked grabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   22 Oct 2003 09:57:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for color image formats
 * added Event Log entry if not enough Non-paged memory.
 * added support for Hue, Saturation, Sharpness, WhiteBalance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Oct 2003 09:12:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed LoadLightReference to work correctly with 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   06 Oct 2003 10:54:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added some debugging and error handling code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   24 Sep 2003 07:49:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added TriggerCount logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 Sep 2003 14:59:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   gamma, strobe, and Lut changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   19 Sep 2003 10:37:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   write out registry setting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 Sep 2003 09:53:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   put camera reading into separate class (SV1394CameraFileReader)
 * changed acquisitoin logic to NAK both triggers on T T EF
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Sep 2003 09:10:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes;
 * fixed Trigger, SF, EF handling
 * fixed LightReference handling
 * added logging
 * added functions:
 * AllocDigitizer, DeallocDigitizer
 * SetParameterDefaults
 * ExecuteCallback
 * UpdateLightReferenceAttributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Aug 2003 14:02:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   call to SVTQueueClass::Create
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   29 Aug 2003 13:19:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   using SVTQueueObject instead of CList
 * fix problem on realloc digitizer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Aug 2003 10:26:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes;
 * changed from using a queue to single outstanding grab
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   12 Aug 2003 09:02:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added code to fix problem with setting ROI
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Aug 2003 13:03:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed CreateImageBuffer to CreateImageAcquisitionBuffer
 * changed DestroyImageBuffer to DestroyImageAcquisitionBuffer
 * added SingleGrab
 * changed SetDCAMParameters to SetStandardCameraParameters
 * other general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Aug 2003 12:45:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   31 Jul 2003 10:05:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed lImageType to eImageType as part of SVCameraFormat change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Jul 2003 10:52:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Format 7 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Jul 2003 09:00:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVCameraFormatsDeviceParam
 * moved camera-specific code to SVDCamDriver class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jul 2003 13:06:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   surrounded debug code with #ifdef _DEBUG
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2003 09:59:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Matrox 1394 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 11:16:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Feb 2003 08:12:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   commented out mulType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:57:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
