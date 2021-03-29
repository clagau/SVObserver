//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDevcieExports
//* .File Name       : $Workfile:   SVFileAcquisitionDevice.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Dec 2014 12:16:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVFileAcquisitionDevice.h"
#include "SVFileAcquisitionDeviceParamEnum.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

SVFileAcquisitionDevice::~SVFileAcquisitionDevice()
{
	Destroy(true);
}

HRESULT SVFileAcquisitionDevice::Create()
{
	if (::InterlockedIncrement(&m_lRefCount) == 1)
	{
		// Allocate Max File Cameras ?
		for (int i = 0; i < MaxFileCameras; i++)
		{
			std::string camName{ "File.Dig_" };
			camName += std::to_string(i);
			m_cameras[i].SetName(camName);
		}
	}
	return S_OK;
}

HRESULT SVFileAcquisitionDevice::Destroy(bool p_bClose)
{
	if (p_bClose || ::InterlockedDecrement(&m_lRefCount) <= 0)
	{
		::InterlockedExchange(&m_lRefCount, 0);
	}
	return S_OK;
}

unsigned long SVFileAcquisitionDevice::CameraGetCount() const
{
	return MaxFileCameras;
}

_variant_t SVFileAcquisitionDevice::CameraGetName(unsigned long cameraIndex) const
{
	_variant_t result;

	if (cameraIndex < MaxFileCameras)
	{
		result.SetString(m_cameras[cameraIndex].GetName().c_str());
	}
	return result;
}

unsigned long SVFileAcquisitionDevice::CameraGetWidth(unsigned long cameraIndex) const
{
	unsigned long result{ 0UL };

	if (cameraIndex < MaxFileCameras)
	{
		result = m_cameras[cameraIndex].GetImageSize().cx;
	}
	return result;
}

unsigned long SVFileAcquisitionDevice::CameraGetHeight(unsigned long cameraIndex) const
{
	unsigned long result{ 0UL };

	if (cameraIndex < MaxFileCameras)
	{
		result = m_cameras[cameraIndex].GetImageSize().cy;
	}
	return result;
}

int SVFileAcquisitionDevice::CameraGetFormat(unsigned long cameraIndex) const
{
	int result{ 0 };

	if (cameraIndex < MaxFileCameras)
	{
		switch (m_cameras[cameraIndex].GetImageFormat())
		{
		case SvDef::SVImageFormatBGR888:
		case SvDef::SVImageFormatBGR888X:
		{
			result = SvDef::SVImageFormatBGR888X;
			break;
		}
		default:
		{
			result = m_cameras[cameraIndex].GetImageFormat();
			break;
		}
		}
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::CameraBufferCreateAll(unsigned long cameraIndex)
{
	if (MaxFileCameras <= cameraIndex)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT SVFileAcquisitionDevice::CameraBufferDestroyAll(unsigned long cameraIndex)
{
	if (MaxFileCameras <= cameraIndex)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT SVFileAcquisitionDevice::CameraRegisterBufferInterface(unsigned long cameraIndex, SVAcquisitionBufferInterface* pInterface)
{
	if (cameraIndex < MaxFileCameras)
	{
		m_cameras[cameraIndex].m_pBufferInterface = pInterface;
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT SVFileAcquisitionDevice::CameraUnregisterBufferInterface(unsigned long cameraIndex)
{
	if (cameraIndex < MaxFileCameras)
	{
		m_cameras[cameraIndex].m_pBufferInterface = nullptr;
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT SVFileAcquisitionDevice::CameraStart(unsigned long cameraIndex)
{
	HRESULT result{ E_FAIL };

	if (cameraIndex < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[cameraIndex];
		if (::InterlockedExchange(&(rCamera.m_lIsStarted), 1) == 0)
		{
			// Check if already Running
			if (!rCamera.IsRunning())
			{
				SVFileCamera::EventHandler startHandler = boost::bind(&SVFileAcquisitionDevice::CameraProcessStartFrame, this, boost::arg<1>());
				SVFileCamera::EventHandler endHandler = boost::bind(&SVFileAcquisitionDevice::CameraProcessEndFrame, this, boost::arg<1>());

				result = rCamera.Start(startHandler, endHandler, cameraIndex);
			}

			if (S_OK != result)
			{
				::InterlockedExchange(&(rCamera.m_lIsStarted), 0);
			}
		}
		else
		{
			// Camera is already started.
			result = S_OK;
		}
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::CameraStop(unsigned long cameraIndex)
{
	if (cameraIndex < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[cameraIndex];
		if (::InterlockedExchange(&(rCamera.m_lIsStarted), 0) == 1)
		{
			// Stop File Loader Thread
			rCamera.Stop();
		}
	}
	return S_OK;
}

_variant_t SVFileAcquisitionDevice::CameraGetParameterName(unsigned long cameraIndex, int parameterID) const
{
	_variant_t result;

	if (cameraIndex < MaxFileCameras)
	{
		if (0 <= parameterID && parameterID < SVFileAcquisitionDeviceParamEnum::SVFileAcquisitionParameterSize)
		{
			result.SetString(SVFileAcquisitionParameterNames[parameterID]);
		}
	}
	return result;
}

_variant_t SVFileAcquisitionDevice::CameraGetParameter(unsigned long cameraIndex, int parameterID) const
{
	_variant_t result;

	if (cameraIndex < MaxFileCameras)
	{
		const SVFileCamera& rCamera = m_cameras[cameraIndex];

		switch (parameterID)
		{
		case SVFileAcquisitionParameterImageFormat:
		{
			result.vt = VT_I4;
			result.lVal = rCamera.GetImageFormat();
			break;
		}
		case SVFileAcquisitionParameterImageWidth:
		{
			result.vt = VT_I4;
			result.lVal = rCamera.GetImageSize().cx;
			break;
		}

		case SVFileAcquisitionParameterImageHeight:
		{
			result.vt = VT_I4;
			result.lVal = rCamera.GetImageSize().cy;
			break;
		}

		case SVFileAcquisitionParameterFilename:
		{
			result.SetString(rCamera.GetFileName());
			break;
		}

		case SVFileAcquisitionParameterDirectory:
		{
			result.SetString(rCamera.GetDirectory());
			break;
		}

		case SVFileAcquisitionParameterLoadingMode:
		{
			result.vt = VT_I4;
			result.lVal = rCamera.GetLoadingMode();
			break;
		}
		case SVFileAcquisitionParameterMaxPreloadFileNumber:
		{
			result = rCamera.GetMaxPreLoadFilenumber();
			break;
		}
		case SVFileAcquisitionParameterPreloadTimeDelay:
		{
			result = rCamera.GetPreloadTimeDelay();
			break;
		}

		default:
		{
			break;
		}
		}
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::CameraSetParameter(unsigned long cameraIndex, int parameterID, const _variant_t& rValue)
{
	HRESULT result{ E_FAIL };

	if (cameraIndex < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[cameraIndex];

		switch (parameterID)
		{
		case SVFileAcquisitionParameterImageFormat:
		{
			rCamera.SetImageFormat(static_cast<SvDef::SVImageFormatEnum>(rValue.lVal));
			result = S_OK;
			break;
		}

		case SVFileAcquisitionParameterImageWidth:
		{
			SIZE size = rCamera.GetImageSize();
			size.cx = rValue;
			rCamera.SetImageSize(size);
			result = S_OK;
			break;
		}

		case SVFileAcquisitionParameterImageHeight:
		{
			SIZE size = rCamera.GetImageSize();
			size.cy = rValue;
			rCamera.SetImageSize(size);
			result = S_OK;
			break;
		}

		case SVFileAcquisitionParameterFilename:
		{
			rCamera.SetFileName(SvUl::createStdString(rValue));
			result = S_OK;
			break;
		}

		case SVFileAcquisitionParameterDirectory:
		{
			rCamera.SetDirectory(SvUl::createStdString(rValue));
			result = S_OK;
			break;
		}

		case SVFileAcquisitionParameterLoadingMode:
		{
			SVFileAcquisitonLoadingModeEnum value = static_cast<SVFileAcquisitonLoadingModeEnum>(rValue.lVal);
			if (value == SingleFileMode || value == ContinuousMode || value == SingleIterationMode)
			{
				rCamera.SetLoadingMode(value);
				result = S_OK;
			}
			break;
		}

		case SVFileAcquisitionParameterMaxPreloadFileNumber:
		{
			rCamera.SetMaxPreLoadFilenumber(rValue.operator int());
			result = S_OK;
			break;
		}
		case SVFileAcquisitionParameterPreloadTimeDelay:
		{
			rCamera.SetPreloadTimeDelay(rValue.operator int());
			result = S_OK;
			break;
		}

		default:
		{
			break;
		}
		}
	}
	return result;
}

_variant_t SVFileAcquisitionDevice::CameraGetParameterList(unsigned long cameraIndex) const
{
	_variant_t result;

	if (cameraIndex < MaxFileCameras)
	{
		SAFEARRAYBOUND arrayBound[1];
		arrayBound[0].cElements = SVFileAcquisitionParameterSize;
		arrayBound[0].lLbound = 0;

		result.vt = VT_ARRAY | VT_I4;
		result.parray = ::SafeArrayCreate(VT_I4, 1, arrayBound);

		for (long i = 0; i < SVFileAcquisitionParameterSize; i++)
		{
			::SafeArrayPutElement(result.parray, &i, reinterpret_cast<void*> (&i));
		}
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::InternalTriggerEnable(unsigned long)
{
	return S_OK;
}

HRESULT SVFileAcquisitionDevice::InternalTrigger(unsigned long cameraIndex)
{
	return FireOneShot(cameraIndex);
}

HRESULT SVFileAcquisitionDevice::CameraProcessStartFrame(unsigned long cameraIndex)
{
	HRESULT result{ S_OK };

	if (cameraIndex < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[cameraIndex];

		if (1 == rCamera.m_lIsStarted && nullptr != rCamera.m_pBufferInterface)
		{
			rCamera.m_StartTimeStamp = SvUl::GetTimeStamp();
		}
		else
		{
			result = E_FAIL;
		}
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::CameraProcessEndFrame(unsigned long cameraIndex)
{
	HRESULT result{ S_OK };

	if (cameraIndex < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[cameraIndex];
		if (rCamera.m_lIsStarted && nullptr != rCamera.m_pBufferInterface)
		{
			SvOi::ITRCImagePtr pImage = rCamera.m_pBufferInterface->GetNextBuffer();

			if (nullptr != pImage && nullptr != pImage->getHandle())
			{
				result = rCamera.CopyImage(pImage.get());

				if (S_OK != result)
				{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
					std::string outputText{ "FileAcquisition::CopyImage - Error in Format\r\n" };
					::OutputDebugString(outputText.c_str());
#endif

					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_IMAGE_FORMAT_ERROR, SvStl::Tid_FileAcquisition_FormatError, SvStl::SourceFileParams(StdMessageParams));
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(rCamera.GetName() + " : " + rCamera.GetFileName());
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_IMAGE_FORMAT_ERROR, SvStl::Tid_CameraEndFrame_GetNextBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				result = E_FAIL;
			}

			//Send this command also if buffer failed to trigger the PPQ-Thread to give it a change for cleanup.
			rCamera.m_pBufferInterface->UpdateWithCompletedBuffer(pImage, rCamera.m_StartTimeStamp, SvUl::GetTimeStamp());
		}
#if defined (TRACE_THEM_ALL) || defined (TRACE_ACQDEVICE)
		else
		{
			wsprintf(l_szbuf, "IsStarted = %s", (rCamera.m_lIsStarted) ? "TRUE" : "FALSE");
			TRACE("%s\n", l_szbuf);
		}
#endif
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::FireOneShot(unsigned long cameraIndex)
{
	HRESULT result{ E_FAIL };

	if (cameraIndex < MaxFileCameras)
	{
		result = m_cameras[cameraIndex].DoOneShot();
	}
	return result;
}
