//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionClass
//* .File Name       : $Workfile:   SVAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Mar 2014 18:02:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>

#include "SVAcquisitionClass.h"
#include "TriggerHandling/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMessage/SVMessage.h"
#include "SVTimerLibrary/SVClock.h"
#include "TriggerInformation/SVAcquisitionConstructParams.h"
#include "SVDigitizerProcessingClass.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "Definitions/StringTypeDef.h"
#include "TriggerRecordController/ITriggerRecordControllerRW.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVAcquisitionClass::SVAcquisitionClass(const SvTi::SVAcquisitionConstructParams& p_rParams)
	: SVODataDeviceClass(p_rParams.m_DigitizerName.c_str())
	, m_LUTAndLRSet(p_rParams.m_LUTAndLRSet)
	, m_rDigitizerProc(SVDigitizerProcessingClass::Instance())
{
	UuidCreateSequential(&m_guid);
	mbIsBufferCreated = false;
	mbTempOnline = false;
	m_LastImage = nullptr;
	mulSize = 0;

	mlStartFrameIndex = -1;

	m_hDigitizer = 0;

	msvImageInfo.SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));

	SetDigName(p_rParams.m_DigitizerName.c_str());

	SetDigNumber(p_rParams.m_DigNumber);
	SetChannel(p_rParams.m_Channel);
	SetBandSize(p_rParams.m_BandSize);
	SetBandMaxSize(p_rParams.m_BandMaxSize);

	if (m_LUTAndLRSet)
	{
		CreateLightReference(p_rParams.m_LRBandSize);
		CreateLut(p_rParams.m_LUTInfo);
	}
}

SVAcquisitionClass::~SVAcquisitionClass()
{
	if (nullptr != m_LastImage)
	{
		::SysFreeString(m_LastImage);

		m_LastImage = nullptr;
	}
}

bool SVAcquisitionClass::IsValid() const
{
	return (SVODataDeviceClass::IsValid());
}

bool SVAcquisitionClass::IsStarted() const
{
	return (SVODataDeviceClass::IsStarted());
}

void SVAcquisitionClass::ClearDevice()
{
	if (nullptr != m_LastImage)
	{
		::SysFreeString(m_LastImage);

		m_LastImage = nullptr;
	}

	m_DeviceParams.Clear();
	m_CameraFileDeviceParams.Clear();

	mLightReference.Reset();

	for (long l = static_cast<long> (mFiles.size() - 1); -1 < l; l--)
	{
		SVFileNameManagerClass::Instance().RemoveItem(&(mFiles[l]));
	}

	m_SingleGrabHandle.reset();

	SVODataDeviceClass::ClearDevice();
}

HRESULT SVAcquisitionClass::Create(unsigned long ulSize)
{
	HRESULT l_Status = SVODataDeviceClass::Create(ulSize);

	return l_Status;
}

HRESULT SVAcquisitionClass::Destroy()
{
	DestroyLut();

	HRESULT hrOk = SVODataDeviceClass::Destroy();

	HRESULT l_Status = DestroyBuffers();

	if (S_OK == hrOk)
	{
		hrOk = l_Status;
	}

	for (long l = static_cast<long> (mFiles.size() - 1); -1 < l; l--)
	{
		SVFileNameManagerClass::Instance().RemoveItem(&(mFiles[l]));
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::Start()
{
	HRESULT hrOk = SVODataDeviceClass::Start();

	mlStartFrameIndex = -1;

	if (S_OK == hrOk)
	{
		hrOk = StartDigitizer();
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::Stop()
{
	HRESULT hr = S_OK;
	HRESULT l_Temp = S_OK;

	if (IsDigitizerSubsystemValid())
	{
		l_Temp = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->Stop(m_hDigitizer);

		if (S_OK == hr)
		{
			hr = l_Temp;
		}

		l_Temp = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->UnregisterBufferInterface(m_hDigitizer);

		if (S_OK == hr)
		{
			hr = l_Temp;
		}
	}
	else
	{
		if (S_OK == hr)
		{
			hr = E_FAIL;
		}
	}

	l_Temp = SVODataDeviceClass::Stop();

	if (S_OK == hr)
	{
		hr = l_Temp;
	}

	return hr;
}

HRESULT SVAcquisitionClass::Reset()
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = SVODataDeviceClass::Reset();

	if (S_OK == l_Status)
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::CreateBuffers(SVImageInfoClass IInfo, unsigned long ulSize)
{
	HRESULT Result = SVImageProcessingClass::FillBufferStructFromInfo(IInfo, m_bufferStruct);
	if (S_OK == Result)
	{
		Result = DestroyBuffers();
		try
		{
			bool mustRestartStart = !m_rTRController.isResetStarted();
			if (mustRestartStart)
			{
				m_rTRController.startResetTriggerRecordStructure();
			}

			m_rTRController.removeAllImageBuffer(m_guid);
			m_rTRController.addImageBuffer(m_guid, m_bufferStruct, ulSize);

			if (mustRestartStart)
			{
				m_rTRController.finishResetTriggerRecordStructure();
			}

		}
		catch (const SvStl::MessageContainer& rExp)
		{
			//This is the topmost catch for MessageContainer exceptions
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(rExp.getMessage());
			Result = E_FAIL;
		}
	}

	if (S_OK == Result)
	{
		mulSize = ulSize + 3;
		msvImageInfo = IInfo;

		if (IsDigitizerSubsystemValid())
		{
			Result = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->CreateBuffers(m_hDigitizer, ulSize + 3);
		}
	}

	if (S_OK == Result)
	{
		mbIsBufferCreated = true;
	}
	else
	{
		DestroyBuffers();
	}

	return Result;
}

HRESULT SVAcquisitionClass::DestroyBuffers()
{
	HRESULT hrOk = S_OK;

	mulSize = 0;

	mlStartFrameIndex = -1;

	if (IsDigitizerSubsystemValid())
	{
		if (S_OK != m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->DestroyBuffers(m_hDigitizer))
		{
			hrOk = S_FALSE;
		}
	}

	mbIsBufferCreated = false;

	return hrOk;
}

HRESULT SVAcquisitionClass::LoadFiles(SVFileNameArrayClass &rArray)
{
	HRESULT Result = S_OK;
	bool LogOnly(false);

	long l(0);

	for (l = static_cast<long> (mFiles.size() - 1); -1 < l; l--)
	{
		SVFileNameManagerClass::Instance().RemoveItem(&(mFiles[l]));
	}

	mFiles = rArray;
	m_CameraFileDeviceParams.Clear();

	for (l = 0; S_OK == Result && l < static_cast<long> (mFiles.size()); l++)
	{
		if (!SVFileNameManagerClass::Instance().AddItem(&(mFiles[l])))
		{
			LogOnly = SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD);
			if (LogOnly)
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_74_LOAD_FILE, mFiles[l].GetFullFileName().c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25047_LoadFileFailed);
			}
			else
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				if (IDYES == Exception.setMessage(SVMSG_SVO_74_LOAD_FILE, mFiles[l].GetFullFileName().c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25047_LoadFileFailed, GUID_NULL, MB_YESNO))
				{
					//All other missing files will only be logged
					LogOnly = true;
				}
				else
				{
					//This stops loading without any further messages
					return SVMSG_SVO_IGNORE_EXCEPTION;
				}
			}
		}
	}

	rArray = mFiles;
	return Result;
}

HRESULT SVAcquisitionClass::CreateLightReference(int iBands)
{
	assert(iBands > 0);

	HRESULT hrOk = S_FALSE;

	mLightReference.DeleteAll();

	if (iBands > 0)
	{
		bool bResult = mLightReference.Create(iBands);

		if (bResult)
		{
			hrOk = S_OK;
		}
		else
		{
			hrOk = S_FALSE;
		}
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::LoadLightReference(SVLightReference& rLR)
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetLightReference(SVLightReference& rLR) const
{
	rLR = mLightReference;
	return S_OK;
}

HRESULT SVAcquisitionClass::SetLightReference(SVLightReference& rLR, int iWhichBand /* = -1 */)
{
	if (iWhichBand >= 0 && mLightReference.IsSameDimensions(rLR))
	{
		mLightReference.SetBandData(rLR, iWhichBand);
	}
	else
	{
		mLightReference = rLR;
	}

	SetLightReferenceImpl(mLightReference);

	return S_OK;
}

HRESULT SVAcquisitionClass::CreateLightReferenceBand(int iBand, int iAttributes)
{
	HRESULT hrOk = S_FALSE;

	if (iBand < mLightReference.NumBands() && mLightReference.m_aLightReferenceBand.size() > 0)
	{
		mLightReference.m_aLightReferenceBand[iBand].Create(iAttributes);
		hrOk = S_OK;
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::GetMaxLightReferenceValue(unsigned long ulType, long &rlValue) const
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetMinLightReferenceValue(unsigned long ulType, long &rlValue) const
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetLightReferenceValueStep(unsigned long ulType, unsigned long &rulValue) const
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::SetStandardCameraParameter(const SVDeviceParamWrapper& rwParam)
{
	return E_NOTIMPL;
}

bool SVAcquisitionClass::IsOnline() const
{
	return IsStarted();
}

HRESULT SVAcquisitionClass::SetLightReferenceImpl(SVLightReference& rLR)
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetImageInfo(SVImageInfoClass *pImageInfo) const
{
	HRESULT hrOk = S_FALSE;

	if (nullptr != pImageInfo)
	{
		*pImageInfo = msvImageInfo;
	}
	return hrOk;
}

HRESULT SVAcquisitionClass::UnloadFiles()
{
	HRESULT hrOk = S_OK;

	for (long l = static_cast<long> (mFiles.size() - 1); -1 < l; l--)
	{
		SVFileNameManagerClass::Instance().RemoveItem(&(mFiles[l]));
	}

	mFiles.clear();

	return hrOk;
}

HRESULT SVAcquisitionClass::ResetLightReference()
{
	HRESULT hrOk = S_OK;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetFileNameArraySize(long &rlSize) const
{
	HRESULT hrOk = S_OK;

	rlSize = static_cast<long> (mFiles.size());

	return hrOk;
}

HRESULT SVAcquisitionClass::GetFileName(long lIndex, SVFileNameClass &rFileName) const
{
	HRESULT hrOk = S_FALSE;

	if (0 <= lIndex && lIndex < static_cast<long> (mFiles.size()))
	{
		hrOk = S_OK;

		rFileName = mFiles[lIndex];
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::GetLut(SVLut& lut)
{
	SVLut lutdata;
	HRESULT hr = GetLutImpl(lutdata);
	if (S_OK == hr)
	{
		Lut().CopyNoTransform(lutdata);
		if (nullptr == Lut().Info().GetTransform())
		{
			Lut().SetTransform(SVDefaultLutTransform());
			Lut().SetTransformOperation(SVLutTransformOperationNormal());
			Lut().Transform();
		}
	}
	lut = Lut();
	return S_OK;
}

HRESULT SVAcquisitionClass::GetLutImpl(SVLut& lut)
{
	return S_FALSE;
}

HRESULT SVAcquisitionClass::SetLut(const SVLutBand& lutBand)
{
	SVLut lut;
	GetLut(lut);
	lut.CopyBandData(lutBand);
	return SetLut(lut);
}

HRESULT SVAcquisitionClass::SetLut(const SVLut& lut, int iBand)
{
	HRESULT hr = S_FALSE;

	bool bSuccess = true;

	const SVLutTransformOperation* pTransform = lut.GetTransformOperation();
	if (bSuccess && pTransform)
	{
		bSuccess &= Lut().SetTransformOperation(*pTransform);
	}// end if

	long lSize = lut.NumBands();
	if (lSize > 0)
	{
		bool bBandsEqual = true;
		for (long l = 1; l < lSize && bBandsEqual; l++)
		{
			bBandsEqual = bBandsEqual && lut(l) == lut(0);
		}
		if (bBandsEqual)
		{
			pTransform = lut(0).GetTransformOperation();
			Lut().SetTransformOperation(*pTransform);
		}

		bSuccess = Lut().CopyBandData(lut, iBand);
	}

	if (bSuccess && IsValidBoard())
	{
		if (iBand == -1)
		{
			hr = SetLutImpl(Lut());
		}
		else
		{
			hr = SetLutImpl(Lut());	// Lut() already has modified band data
		}
	}
	else
	{
		hr = bSuccess ? S_OK : S_FALSE;
	}

	return hr;
}

HRESULT SVAcquisitionClass::SetLutImpl(const SVLut& lut)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::CreateLut(const SVLutInfo& info)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::DestroyLut()
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::ResetLut()
{
	Lut().SetTransformOperation(SVLutTransformOperationNormal());
	Lut().Transform();
	return S_OK;
}

SVLut& SVAcquisitionClass::Lut()
{
	SVLutDeviceParam* pParam = nullptr;
	HRESULT hr = m_DeviceParams.GetParameter(DeviceParamLut, pParam);
	if (S_OK == hr)
	{
		return pParam->lut;
	}

	SVLutDeviceParam lutparam;
	m_DeviceParams.SetParameter(DeviceParamLut, lutparam);
	hr = m_DeviceParams.GetParameter(DeviceParamLut, pParam);
	assert(S_OK == hr && nullptr != pParam);
	return pParam->lut;
}

bool SVAcquisitionClass::IsValidBoard() const
{
	return false;
}

HRESULT SVAcquisitionClass::SetNumberOfBands(int NumberOfBands)
{
	HRESULT Result(S_OK);

	SetBandMaxSize(NumberOfBands);
	SetBandSize(NumberOfBands);

	if (m_LUTAndLRSet)
	{
		CreateLightReference(NumberOfBands);
	}

	return Result;
}

HRESULT SVAcquisitionClass::GetDeviceParameters(SVDeviceParamCollection& rDeviceParams)
{
	rDeviceParams = m_DeviceParams;
	return S_OK;
}

HRESULT SVAcquisitionClass::SetDeviceParameters(const SVDeviceParamCollection& rDeviceParams)
{
	HRESULT hr = S_OK;

	// don't do a straight assignment ( m_DeviceParams = rDeviceParams )
	// only copy those parameters that are specified.
	SVDeviceParamMap::const_iterator iter;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;

		if (nullptr != ((const SVDeviceParam*)w))
		{
			m_DeviceParams.GetParameter(iter->first) = w;
		}
	}

	return hr;
}

HRESULT SVAcquisitionClass::IsValidCameraFileParameters(SVDeviceParamCollection& rDeviceParams)
{
	return E_NOTIMPL;	// derived class needs to override this
}


HRESULT SVAcquisitionClass::GetCameraFileParameters(SVDeviceParamCollection& rDeviceParams)
{
	rDeviceParams = m_CameraFileDeviceParams;
	return S_OK;
}

HRESULT SVAcquisitionClass::GetHardwareDeviceCapabilities(SVDeviceParamCollection& rDeviceParams)
{
	rDeviceParams = HardwareCapabilities();
	return S_OK;
}

SVDeviceParamCollection& SVAcquisitionClass::HardwareCapabilities()
{
	return m_DeviceParams;
}

HRESULT SVAcquisitionClass::ReadCameraFile(const std::string&, SVDeviceParamCollection& rParams)
{
	return E_NOTIMPL;
}

bool SVAcquisitionClass::IsDigitizerSubsystemValid() const
{
	return m_rDigitizerProc.IsValidDigitizerSubsystem(m_DigName.c_str());
}

DWORD WINAPI SVAcquisitionClass::SingleGrabHelperFn(LPVOID lpParameter)
{
	SVAcquisitionClass* pAcq = reinterpret_cast<SVAcquisitionClass*>(lpParameter);
	while (pAcq->mbTempOnline)
	{
		Sleep(0);
	}
	return 0;
}

HRESULT SVAcquisitionClass::SingleGrab(SvOi::SVImageBufferHandlePtr p_SingleGrabHandle)
{
	HRESULT l_Status = S_OK;

	if (!IsOnline())
	{
		m_SingleGrabHandle = p_SingleGrabHandle;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// do nothing, just a stub
void SVAcquisitionClass::ClearDeviceIdentifier()
{
}

unsigned long SVAcquisitionClass::GetBufferWidth() const
{
	unsigned long l_Width = 0;
	RECT l_Rect;

	if (S_OK == msvImageInfo.GetOutputRectangle(l_Rect))
	{
		l_Width = l_Rect.right;
	}

	return l_Width;
}

unsigned long SVAcquisitionClass::GetBufferHeight() const
{
	unsigned long l_Height = 0;
	RECT l_Rect;

	if (S_OK == msvImageInfo.GetOutputRectangle(l_Rect))
	{
		l_Height = l_Rect.bottom;
	}

	return l_Height;
}

int SVAcquisitionClass::GetBufferFormat() const
{
	int l_Format = SvDef::SVImageFormatUnknown;

	msvImageInfo.GetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyFormat, l_Format);

	return l_Format;
}

SvTrc::IImagePtr SVAcquisitionClass::GetNextBuffer()
{
	return m_rTRController.getImageBuffer(m_bufferStruct);
}

HRESULT SVAcquisitionClass::UpdateWithCompletedBuffer(const SvTrc::IImagePtr& rImage, const SvTl::SVTimeStamp StartTick, const SvTl::SVTimeStamp StopTick)
{
	if (nullptr != m_SingleGrabHandle)
	{
		if (nullptr != rImage && nullptr != rImage->getHandle() && !(rImage->getHandle()->empty()) && !(m_SingleGrabHandle->empty()))
		{
			SVMatroxBufferInterface::CopyBuffer(m_SingleGrabHandle->GetBuffer(), rImage->getHandle()->GetBuffer());
		}
		//This only resets this smart pointer as another smart pointer is still alive
		m_SingleGrabHandle.reset();

		this->mbTempOnline = false;
	}

	SVODataResponseClass l_Response;
	l_Response.setImage(rImage);
	l_Response.setStartTime(StartTick);
	l_Response.setEndTime(StopTick);
	l_Response.setIsValid(true);
	l_Response.setIsComplete(true);

	return Notify(l_Response);
}

HRESULT SVAcquisitionClass::StartDigitizer()
{
	HRESULT hrOk = S_OK;

	if (IsDigitizerSubsystemValid())
	{
		hrOk = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->RegisterBufferInterface(m_hDigitizer, this);

		if (S_OK == hrOk)
		{
			hrOk = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->Start(m_hDigitizer);
		}
	}
	else
	{
		hrOk = E_FAIL;
	}

	return hrOk;
}

} //namespace SvIe
