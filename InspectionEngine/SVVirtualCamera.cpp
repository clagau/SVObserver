//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVirtualCamera
//* .File Name       : $Workfile:   SVVirtualCamera.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVVirtualCamera.h"
#include "SVDigitizerProcessingClass.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "Definitions/GlobalConst.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVOLibrary/SVLut.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#include "Triggering/SVTriggerClass.h"
#include "SVUtilityLibrary/AcquisitionName.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVVirtualCamera::~SVVirtualCamera()
{
	DestroyLocal();
}

bool SVVirtualCamera::GetImageInfo(SVImageInfoClass *pImageInfo)
{
	bool bOk = false;

	if(nullptr != m_pCurrentDevice)
	{
		bOk = S_OK == m_pCurrentDevice->GetImageInfo( pImageInfo );
		if ( nullptr != pImageInfo )
		{
			bOk = S_OK == pImageInfo->SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, mlBandLink );
		}
	}

	return bOk;
}

HRESULT SVVirtualCamera::RefreshObject( const SVObjectClass* const, RefreshObjectType Type)
{
	HRESULT Result = S_OK;
	
	//The camera parameters only need to be refreshed when type is PreRefresh
	if( PreRefresh == Type )
	{
		Result = updateCameraParameters();
	}
	
	return Result;
}

bool SVVirtualCamera::Create( LPCTSTR DeviceName )
{
	bool bOk = true;

	if(nullptr != m_pCurrentDevice )
	{
		bOk = Destroy();
	}

	SVDigitizerProcessingClass::Instance().SetDigitizerColor( DeviceName, m_IsColor );
	m_pMainDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DeviceName );
	m_pCurrentDevice = m_pMainDevice;

	if (false == m_bFileAcquisition)
	{
		std::string fileDeviceName = SvUl::getAcquisitionName(m_CameraID, true);
		m_pFileDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(fileDeviceName.c_str());
	}

	bOk = (nullptr != m_pCurrentDevice) && bOk;

	if ( bOk )
	{
		SetBandLink( m_pCurrentDevice->Channel() );

		m_ObjectTypeInfo.m_ObjectType = SvPb::SVVirtualCameraType;

		createCameraParameters();
		updateCameraParameters();
	}

	return bOk;
}

bool SVVirtualCamera::Destroy()
{
	return DestroyLocal();
}

HRESULT SVVirtualCamera::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	rpObject = nullptr;
	HRESULT l_Status = SVObjectClass::GetChildObject( rpObject, rNameInfo, Index );

	if( S_OK != l_Status )
	{
		if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName() )
		{
			SvVol::BasicValueObjectPtr pBasicValueObject = m_CameraValues.getValueObject( rNameInfo.GetObjectArrayName( Index + 1 ).c_str() );

			if(nullptr != pBasicValueObject)
			{
				rpObject = dynamic_cast<SVObjectClass*> (pBasicValueObject.get());
				l_Status = (nullptr != rpObject) ? S_OK : S_FALSE;
			}
		}
	}

	return l_Status;
}

bool SVVirtualCamera::CanGoOnline() const
{
	return m_pCurrentDevice && m_pCurrentDevice->IsValid();
}// end CanGoOnline

bool SVVirtualCamera::GoOnline()
{
	return (nullptr != m_pCurrentDevice) && ( S_OK == m_pCurrentDevice->Start() );
}

bool SVVirtualCamera::GoOffline()
{
	bool result = (nullptr != m_pCurrentDevice) && (S_OK == m_pCurrentDevice->Stop());
	setAcquisitionDevice(false);
	return result;
}

bool SVVirtualCamera::RegisterCallback(ULONG_PTR pPPQ, PpqCameraCallBack pPpqCameraCallback)
{
	bool result {false};
	if (nullptr != m_pMainDevice)
	{
		result = (S_OK == m_pMainDevice->RegisterCallback(reinterpret_cast<ULONG_PTR> (this), pPPQ, pPpqCameraCallback));
	}
	if (nullptr != m_pFileDevice)
	{
		m_pFileDevice->RegisterCallback(reinterpret_cast<ULONG_PTR> (this), pPPQ, pPpqCameraCallback);
	}
	return result;
}

bool SVVirtualCamera::UnregisterCallback(ULONG_PTR pPPQ)
{
	bool result {false};
	if (nullptr != m_pMainDevice)
	{
		result = (S_OK == m_pMainDevice->UnregisterCallback(pPPQ));
	}
	if (nullptr != m_pFileDevice)
	{
		m_pFileDevice->UnregisterCallback(pPPQ);
	}
	return result;
}

void SVVirtualCamera::addNeededBuffer(uint32_t id, int neededBufferSize)
{
	m_neededBufferMap[id] = neededBufferSize;
	setNeededBuffer();
}

void SVVirtualCamera::removeNeededBufferEntry(uint32_t id)
{
	auto iter = m_neededBufferMap.find(id);
	if (m_neededBufferMap.end() != iter)
	{
		m_neededBufferMap.erase(iter);
		setNeededBuffer();
	}
}

void SVVirtualCamera::setAcquisitionDevice(bool fileAcquisition)
{
	if (false == m_bFileAcquisition)
	{
		m_pCurrentDevice = fileAcquisition ? m_pFileDevice : m_pMainDevice;
		if (fileAcquisition && nullptr != m_pFileDevice && nullptr != m_pMainDevice)
		{
			SVImageInfoClass imageInfo;
			m_pMainDevice->GetImageInfo(&imageInfo);
			m_pFileDevice->CreateBuffers(imageInfo);
		}
	}
}

HRESULT SVVirtualCamera::setParameterValue(int parameterID, _variant_t value)
{
	HRESULT result {E_FAIL};
	if (nullptr != m_pCurrentDevice)
	{
		SvTrig::SVDigitizerLoadLibraryClass* pAcqDLL = SvIe::SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(m_pCurrentDevice->DigName().c_str());
		if (nullptr != pAcqDLL)
		{
			result = pAcqDLL->ParameterSetValue(m_pCurrentDevice->m_hDigitizer, parameterID, value);
		}
	}
	return result;
}

void SVVirtualCamera::setNeededBuffer()
{
	const auto maxElement = std::max_element(m_neededBufferMap.begin(), m_neededBufferMap.end(), [](const auto& rEntry1, const auto& rEntry2) { return rEntry1.second < rEntry2.second; });
	if (m_neededBufferMap.end() != maxElement && nullptr != m_pCurrentDevice)
	{
		m_pCurrentDevice->setNeededBuffers(maxElement->second);
	}
}

bool SVVirtualCamera::DestroyLocal()
{
	bool bOk = GoOffline();

	bOk &= (S_OK == UnregisterCallback(0UL));

	m_pCurrentDevice = nullptr;
	m_pMainDevice = nullptr;
	m_pFileDevice = nullptr;
	mlBandLink = 0;

	return bOk;
}

HRESULT SVVirtualCamera::GetBand(int& riBand) const
{
	riBand = mlBandLink;
	return S_OK;
}

HRESULT SVVirtualCamera::GetBandSize(int& riBandSize) const
{
	HRESULT hr = S_OK;
	
	if (nullptr != m_pCurrentDevice )
	{
		riBandSize = m_pCurrentDevice->BandSize();
	}
	else
	{
		hr = S_FALSE;
	}
	
	return hr;
}

HRESULT SVVirtualCamera::SetLightReference( SVLightReference& rArray )
{
	HRESULT hr=S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		int iBandSize=0;
		GetBandSize(iBandSize);
		if (iBandSize == 1)
		{
			int iBand=-1;
			GetBand(iBand);
			hr = GetAcquisitionDevice()->SetLightReference(rArray, iBand);
		}
		else
		{
			hr = GetAcquisitionDevice()->SetLightReference(rArray);
		}
		updateCameraParameters();
	}
	
	return hr;
}

HRESULT SVVirtualCamera::GetLightReference( SVLightReference& rArray ) const
{
	HRESULT hr = S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		hr = GetAcquisitionDevice()->GetLightReference(rArray);
	}
	return hr;
}

HRESULT SVVirtualCamera::SetLut( const SVLut& lut )
{
	HRESULT hr=S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		int iBandSize=0;
		GetBandSize(iBandSize);
		if (iBandSize == 1)
		{
			int iBand=-1;
			GetBand(iBand);
			hr = GetAcquisitionDevice()->SetLut(lut, iBand);
		}
		else
		{
			hr = GetAcquisitionDevice()->SetLut( lut );
		}
	}
	
	return hr;
}

HRESULT SVVirtualCamera::GetLut( SVLut& lut ) const
{
	HRESULT hr=S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		int iBandSize=0;
		GetBandSize(iBandSize);
		if (iBandSize == 1)
		{
			int iBand=-1;
			GetBand(iBand);
			hr = GetAcquisitionDevice()->GetLut( lut );
		}
		else
		{
			hr = GetAcquisitionDevice()->GetLut( lut );
		}
	}
	
	return hr;
}

SvOi::ITRCImagePtr SVVirtualCamera::ReserveNextImageHandle(  ) const
{
	if(nullptr != m_pCurrentDevice)
	{
		return m_pCurrentDevice->GetNextBuffer();
	}

	return nullptr;
}

bool SVVirtualCamera::IsFileAcquisition() const
{
	bool isFileAcq = (nullptr != m_pCurrentDevice && nullptr != m_pFileDevice) ? m_pCurrentDevice == m_pFileDevice : false;
	return m_bFileAcquisition || isFileAcq;
}

void SVVirtualCamera::SetFileAcquisitionMode(bool bFileAcquisition)
{
	m_bFileAcquisition = bFileAcquisition;
	m_canExternalSoftwareTrigger = bFileAcquisition;
}

LPCTSTR SVVirtualCamera::GetImageFilename() const
{
	return m_imageFilename.c_str();
}

void SVVirtualCamera::SetImageFilename(const std::string& filename)
{
	m_imageFilename = filename;
}

LPCTSTR SVVirtualCamera::GetImageDirectoryName() const
{
	return m_imageDirectoryName.c_str();
}

void SVVirtualCamera::SetImageDirectoryName(const std::string& directoryName)
{
	m_imageDirectoryName = directoryName;
}

long SVVirtualCamera::GetFileLoadingMode() const
{
	return m_imageLoadingMode;
}

void SVVirtualCamera::SetFileLoadingMode(long mode)
{
	m_imageLoadingMode = mode;
}

bool SVVirtualCamera::IsFileImageSizeEditModeFileBased() const
{
	return m_bImageSizeEditModeFileBased;
}

void SVVirtualCamera::SetFileImageSizeEditModeFileBased(bool m_bFileBased)
{
	m_bImageSizeEditModeFileBased = m_bFileBased;
}

const SIZE& SVVirtualCamera::GetFileImageSize() const
{
	return m_imageSize;
}

void SVVirtualCamera::SetFileImageSize(const SIZE& size)
{
	m_imageSize = size;
}

long SVVirtualCamera::GetFileImageWidth() const
{
	return m_imageSize.cx;
}

void SVVirtualCamera::SetFileImageWidth(long width)
{
	m_imageSize.cx = width;
}

long SVVirtualCamera::GetFileImageHeight() const
{
	return m_imageSize.cy;
}

void SVVirtualCamera::SetFileImageHeight(long height)
{
	m_imageSize.cy = height;
}

void SVVirtualCamera::createCameraParameters()
{
	SvVol::BasicValueObjectPtr pValue;
	//Initialize the parameters and set their unique ID
	pValue = m_CameraValues.setValueObject( SvDef::FqnCameraSerialNumber, _T(""), this, SvPb::SVCameraObjectType );
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ObjectIdEnum::CameraBaseSerialNumberId + m_CameraID);

	pValue = m_CameraValues.setValueObject( SvDef::FqnCameraGain, 0L, this, SvPb::SVCameraObjectType );
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ObjectIdEnum::CameraBaseGainId + m_CameraID);

	pValue = m_CameraValues.setValueObject( SvDef::FqnCameraShutter, 0L, this, SvPb::SVCameraObjectType );
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), ObjectIdEnum::CameraBaseShutterId + m_CameraID);

	pValue = m_CameraValues.setValueObject(SvDef::FqnCameraRegPath, _T(""), this, SvPb::SVCameraObjectType);
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), ObjectIdEnum::CameraBaseRegPathId + m_CameraID);

	pValue = m_CameraValues.setValueObject(SvDef::FqnCameraRegFile, _T(""), this, SvPb::SVCameraObjectType);
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), ObjectIdEnum::CameraBaseRegFileId + m_CameraID);

	pValue = m_CameraValues.setValueObject(SvDef::FqnCameraFrameID, 0L, this, SvPb::SVCameraObjectType);
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), ObjectIdEnum::CameraBaseFrameIDId + m_CameraID);

	pValue = m_CameraValues.setValueObject(SvDef::FqnCameraTimestamp, 0UL, this, SvPb::SVCameraObjectType);
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), ObjectIdEnum::CameraBaseTimestampId + m_CameraID);

	pValue = m_CameraValues.setValueObject(SvDef::FqnCameraLineStatusAll, 0L, this, SvPb::SVCameraObjectType);
	SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValue.get(), ObjectIdEnum::CameraBaseLineStatusAllId + m_CameraID);
}

HRESULT SVVirtualCamera::updateCameraParameters()
{
	HRESULT Result = S_OK;

	if(nullptr == m_pCurrentDevice)
	{
		Result = E_FAIL;
		return Result;
	}

	SVDeviceParamCollection CameraParameters;
	Result = m_pCurrentDevice->GetDeviceParameters( CameraParameters );
	if(S_OK == Result)
	{
		SVDeviceParam* pDeviceParam = CameraParameters.GetParameter( DeviceParamSerialNumberString );
		if( nullptr != pDeviceParam )
		{
			variant_t SerialNumberValue;
			pDeviceParam->GetValue(SerialNumberValue.GetVARIANT());
			pDeviceParam = nullptr;
			m_CameraValues.setValueObject( SvDef::FqnCameraSerialNumber, SerialNumberValue, this, SvPb::SVCameraObjectType );
		}
		pDeviceParam = CameraParameters.GetParameter(DeviceParamGigeTriggerSource);
		if (nullptr != pDeviceParam)
		{
			variant_t triggerSource;
			pDeviceParam->GetValue(triggerSource.GetVARIANT());
			pDeviceParam = nullptr;
			if (_T("All Trigger") == SvUl::createStdString(triggerSource))
			{
				m_canExternalSoftwareTrigger = true;
			}
		}

		pDeviceParam = CameraParameters.GetParameter(DeviceParamGain);
		/*Result = */updateCameraLongParameter( SvDef::FqnCameraGain, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );

		pDeviceParam = CameraParameters.GetParameter( DeviceParamShutter );
		Result = updateCameraLongParameter( SvDef::FqnCameraShutter, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );
	}
	return Result;
}

HRESULT SVVirtualCamera::updateCameraLongParameter( LPCTSTR Name, const SVLongValueDeviceParam* pLongValueDeviceParam )
{
	HRESULT Result = E_FAIL;

	if( nullptr != pLongValueDeviceParam )
	{
		variant_t Value;
		Value = pLongValueDeviceParam->GetScaledValue();
		SvVol::BasicValueObjectPtr pValueObject = m_CameraValues.setValueObject( Name, Value, this, SvPb::SVCameraObjectType );
		if(nullptr != pValueObject)
		{
			pValueObject->SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute );
			Result = S_OK;
		}
	}

	return Result;
}

HRESULT SVVirtualCamera::updateDeviceParameters(SVDeviceParamCollection& rCameraParameters)
{
	HRESULT Result = E_FAIL;

	if(nullptr ==  m_pCurrentDevice)
	{
		return Result;
	}

	if( S_OK == m_pCurrentDevice->GetDeviceParameters( rCameraParameters ) )
	{
		SVDeviceParamCollection	ChangedCameraParameters;
		SVDeviceParam* pDeviceParam = rCameraParameters.GetParameter( DeviceParamGain );
		SvVol::BasicValueObjectPtr pValueObject;
		pValueObject = m_CameraValues.getValueObject( SvDef::FqnCameraGain );

		if(nullptr != pValueObject)
		{
			/*Result = */pValueObject->updateDeviceParameter( pDeviceParam );
			ChangedCameraParameters.SetParameter(pDeviceParam);
			Result = S_OK;
		}

		if( S_OK == Result )
		{
			pDeviceParam = rCameraParameters.GetParameter( DeviceParamShutter );
			pValueObject = m_CameraValues.getValueObject( SvDef::FqnCameraShutter );
			if(nullptr != pValueObject)
			{
				Result = pValueObject->updateDeviceParameter( pDeviceParam );
				ChangedCameraParameters.SetParameter(pDeviceParam);
			}
			else
			{
				Result = E_FAIL;
			}
		}

		if( S_OK == Result )
		{
			m_pCurrentDevice->SetDeviceParameters( ChangedCameraParameters );
		}
	}

	return Result;
}

void SVVirtualCamera::setTempImage(const SVMatroxBuffer pImage)
{
	std::lock_guard<std::mutex> guard(m_tmpImage_mutex);
	m_tmpImage.clear();
	if (S_OK == SVMatroxBufferInterface::Create(m_tmpImage, pImage))
	{
		SVMatroxBufferInterface::CopyBuffer(m_tmpImage, pImage);
	}
}

SVMatroxBuffer SVVirtualCamera::getTempImage()
{ 
	std::lock_guard<std::mutex> guard(m_tmpImage_mutex);
	return m_tmpImage;
}

SvVol::BasicValueObjectPtr SVVirtualCamera::getCameraValue(LPCTSTR pName)
{
	return m_CameraValues.getValueObject(pName);
}

} //namespace SvIe
