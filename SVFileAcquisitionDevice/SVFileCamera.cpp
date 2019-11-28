//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
//Moved to precompiled header: #include <boost/bind.hpp>

#include "Definitions/StringTypeDef.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVFileSystemLibrary/SVFileSystemScanner.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMessage/SVMessage.h"
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

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
		// Get File List
		SVFileSystemScanner<Insertor>::ScanForFiles(m_fileData.directory.c_str(), _T("*.bmp"), Insertor(m_fileList, m_fileList.end()));
	}
	else // Single File Mode
	{
		SVFileInfo info;
		info.filename = m_fileData.fileName;
		m_fileList.insert(info);
	}
		
	// init sequencer - everybody wraps except Single Iteration mode
	m_loadSequence.Init(m_fileList.begin(), m_fileList.end(), !IsSingleIterationLoadMode());

	// start loader thread
	hr = m_thread.Create(&SVFileCamera::OnAPCEvent, boost::bind(&SVFileCamera::OnThreadEvent, this, _1), m_name.c_str(), SVAffinityAcq);
	return hr;
}

void SVFileCamera::Stop()
{
	//stop loader thread
	m_thread.Destroy();

	if (M_NULL != m_image)
	{
		MbufFree(m_image);
		m_image = M_NULL;
	}
}

bool SVFileCamera::IsRunning() const
{
	return m_thread.IsActive();
}

std::string SVFileCamera::GetNextFilename()
{
	const SVFileInfo& fileInfo = m_loadSequence.GetNext();
	return fileInfo.filename;
}

HRESULT SVFileCamera::DoOneShot()
{
	return m_thread.Signal((void *)this);
}

HRESULT SVFileCamera::CopyImage(SvTrc::IImage* pImage)
{
	HRESULT hr = SVMatroxBufferInterface::CopyBuffer(pImage->getHandle()->GetBuffer(), m_image);
	return hr;
}

void SVFileCamera::OnAPCEvent( ULONG_PTR data )
{
	SVFileCamera* pCamera = reinterpret_cast<SVFileCamera*>(data);
	if(nullptr != pCamera)
	{
		std::string filename = pCamera->GetNextFilename();
		// fire StartFrame event
		pCamera->m_startFrameEvent.Fire(pCamera->m_index);
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
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
			Exception.setMessage( SVMSG_IMAGE_LOAD_ERROR, id, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
}

void SVFileCamera::OnThreadEvent( bool& p_WaitForEvents )
{
	// check file Load status ?
	if (M_NULL != m_image)
	{
		// fire EndFrame event
		m_endFrameEvent.Fire(m_index);
	}
}

HRESULT SVFileCamera::loadImage(std::string fileName)
{
	if (M_NULL != m_image)
	{
		MbufClear(m_image,0);
		m_image = MbufImport(fileName.c_str(), M_DEFAULT, M_LOAD, M_DEFAULT_HOST, &m_image);
	}
	else
	{
		m_image = MbufImport(fileName.c_str(), M_DEFAULT, M_RESTORE, M_DEFAULT_HOST, M_NULL);
	}
	
	if (M_NULL != m_image)
	{
		return S_OK;
	}
	return E_FAIL;
}

