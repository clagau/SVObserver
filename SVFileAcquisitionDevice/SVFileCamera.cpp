//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileCamera
//* .File Name       : $Workfile:   SVFileCamera.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Dec 2014 12:16:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFileCamera.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVImageFormatEnum.h"
#include "ObjectInterfaces/ITRCImage.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/FileHelper.h"
#pragma endregion Includes

//#define TRACE_LOADTIME true
#ifdef TRACE_LOADTIME
#include "SVUtilityLibrary/SVClock.h"
#endif 

const std::string& SVFileCamera::GetName() const
{
	return m_name;
}

void SVFileCamera::SetName(const std::string& name)
{
	m_name = name;
}

const SIZE& SVFileCamera::GetImageSize() const
{
	return m_fileData.imageSize;
}

void SVFileCamera::SetImageSize(const SIZE& size)
{
	m_fileData.imageSize = size;
}

SvDef::SVImageFormatEnum SVFileCamera::GetImageFormat() const
{
	return m_fileData.imageFormat;
}

void SVFileCamera::SetImageFormat(SvDef::SVImageFormatEnum format)
{
	m_fileData.imageFormat = format;
}

LPCTSTR SVFileCamera::GetDirectory() const
{
	return m_fileData.directory.c_str();
}

void SVFileCamera::SetDirectory(const std::string& dir)
{
	m_fileData.directory = dir;
}

LPCTSTR SVFileCamera::GetFileName() const
{
	return m_fileData.fileName.c_str();
}

void SVFileCamera::SetFileName(const std::string& fileName)
{
	m_fileData.fileName = fileName;
}

void SVFileCamera::SetLoadingMode(SVFileAcquisitonLoadingModeEnum mode)
{
	m_fileData.mode = mode;
}

SVFileAcquisitonLoadingModeEnum SVFileCamera::GetLoadingMode() const
{
	return m_fileData.mode;
}

bool SVFileCamera::IsSingleIterationLoadMode() const
{
	return (m_fileData.mode == SingleIterationMode);
}

HRESULT SVFileCamera::Start(const EventHandler& startFrameHandler, const EventHandler& endFrameHandler, unsigned long p_ulIndex)
{
	HRESULT hr = S_FALSE;

	m_index = p_ulIndex;
	m_startFrameEvent = startFrameHandler;
	m_endFrameEvent = endFrameHandler;

	m_fileList.clear();

	if (m_fileData.mode == ContinuousMode || m_fileData.mode == SingleIterationMode)
	{
		m_fileList = SvUl::getFileList(m_fileData.directory.c_str(), _T(".bmp"), false);
	}
	else // Single File Mode
	{
		m_fileList.push_back(m_fileData.fileName);
	}
	m_currentFileIter = m_fileList.begin();

	std::stringstream ss;
	ss << "SVFileCamera start filelist size for" <<  GetName().c_str()  << " : " << m_fileList.size(); 
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, ss.str().c_str(), SvStl::SourceFileParams(StdMessageParams));

	m_UsePreLoadImages = true;
	if (m_fileList.size() > m_MaxPreloadFileNumber)
	{
		m_UsePreLoadImages = false;
	}

	if (m_UsePreLoadImages)
	{
		m_imageList.resize(m_fileList.size(), M_NULL);
		int index{ 0 };
		for (auto& filename : m_fileList)
		{
			MIL_ID id = MbufImport(filename.c_str(), M_DEFAULT, M_RESTORE, M_DEFAULT_HOST, M_NULL);
			if (id != NULL)
			{
				m_imageList.at(index++) = id;
			}
			else
			{
				return E_FAIL;
			}
		}
		m_currentImageIter = m_imageList.begin();
	}

	hr = m_processThread.Create(&SVFileCamera::ProcessCallback, m_name.c_str());
	return hr;
}

void SVFileCamera::Stop()
{
	//stop loader thread
	m_processThread.Destroy();
	if (m_UsePreLoadImages)
	{
		for (auto& id : m_imageList)
		{
			if (M_NULL != id)
			{
				MbufFree(id);
				id = M_NULL;
			}
		}
	}
	else if (M_NULL != m_image)
	{
		MbufFree(m_image);
		m_image = M_NULL;
	}
}

bool SVFileCamera::IsRunning() const
{
	return m_processThread.IsActive();
}

std::string SVFileCamera::GetNextFilename()
{
	std::string result;
	if (m_acquisitionFile.empty())
	{
		if (m_fileList.end() == m_currentFileIter)
		{
			m_currentFileIter = IsSingleIterationLoadMode() ? m_fileList.end() : m_fileList.begin();
		}
		if (m_fileList.end() != m_currentFileIter)
		{
			result = *m_currentFileIter;
			++m_currentFileIter;
		}
	}
	else
	{
		result = m_acquisitionFile;
	}
	m_acquisitionFile.clear();
	return result;
}

MIL_ID SVFileCamera::GetNextImageId()
{
	MIL_ID result {0LL};
	if (m_imageList.end() == m_currentImageIter)
	{
		m_currentImageIter = IsSingleIterationLoadMode() ? m_imageList.end() : m_imageList.begin();
	}
	if (m_imageList.end() != m_currentImageIter)
	{
		result = *m_currentImageIter;
		++m_currentImageIter;
	}
	return result;
}

void SVFileCamera::DoOneShot(LPCTSTR pAcquisitionFile)
{
	{
		std::lock_guard<std::mutex> guard(m_fileCameraMutex);
		m_acquisitionFile = pAcquisitionFile;
	}
	m_processThread.Signal(reinterpret_cast<ULONG_PTR> (this));
}

HRESULT SVFileCamera::CopyImage(SvOi::ITRCImage* pImage)
{
	HRESULT hr = SVMatroxBufferInterface::CopyBuffer(pImage->getHandle()->GetBuffer(), m_image);
	return hr;
}

void __stdcall SVFileCamera::ProcessCallback(ULONG_PTR pCaller)
{
	SVFileCamera* pCamera = reinterpret_cast<SVFileCamera*> (pCaller);
	if (nullptr != pCamera)
	{
		std::string acuisitionFile = pCamera->getAcquisitionFile();
		// fire StartFrame event
		pCamera->m_startFrameEvent(pCamera->m_index);
#ifdef TRACE_LOADTIME
		double Duration = SvUl::GetTimeStamp();
#endif
		if (pCamera->m_UsePreLoadImages && acuisitionFile.empty())
		{
			pCamera->m_image = pCamera->GetNextImageId();
			if (pCamera->m_PreloadTimeDelay > 0)
			{
				Sleep(pCamera->m_PreloadTimeDelay );
			}
		}
		else
		{
			std::string filename = pCamera->GetNextFilename();
			// Load file
			if (filename.empty() || S_OK != pCamera->loadImage(filename))
			{
				// add to event log
				SvDef::StringVector msgList;
				SvStl::MessageTextEnum id = SvStl::Tid_Empty;
				if (filename.empty())
				{
					msgList.push_back(pCamera->GetDirectory());
					id = SvStl::Tid_FileCamera_NoImageFile;
				}
				else
				{
					msgList.push_back(filename);
					id = SvStl::Tid_FileCamera_LoadFailed;
				}
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_IMAGE_LOAD_ERROR, id, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		if (M_NULL != pCamera->m_image)
		{
			// fire EndFrame event
			pCamera->m_endFrameEvent(pCamera->m_index);
		}
#ifdef TRACE_LOADTIME
		Duration = SvUl::GetTimeStamp() -Duration;
		std::stringstream ss;
		ss << "Fileloading time(µ): " << Duration * 1000 << std::endl;
		OutputDebugString(ss.str().c_str());
#endif

	}
}

HRESULT SVFileCamera::loadImage(const std::string& rFileName)
{
	if (M_NULL != m_image)
	{
		MbufClear(m_image, 0);
		MbufImport(rFileName, M_DEFAULT, M_LOAD, M_NULL, &m_image);
	}
	else
	{
		m_image = MbufImport(rFileName, M_DEFAULT, M_RESTORE, M_DEFAULT_HOST, M_NULL);
	}

	if (M_NULL != m_image)
	{
		return S_OK;
	}
	return E_FAIL;
}

std::string SVFileCamera::getAcquisitionFile() const
{
	std::string result;
	{
		std::lock_guard<std::mutex> guard(m_fileCameraMutex);
		result = m_acquisitionFile;
	}
	return result;
}

