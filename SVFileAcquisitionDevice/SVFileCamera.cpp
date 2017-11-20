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

#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVFileSystemLibrary/SVFileSystemScanner.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

SVFileCamera::SVFileCamera()
: m_lIsStarted(0)
, m_index(-1)
, m_bAcquisitionTriggered(false)
, m_dispatcher(nullptr,SvTh::TriggerParameters())
{
	m_pBufferInterface = nullptr;
	m_StartTimeStamp = 0;
}

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

bool SVFileCamera::IsSingleFileMode() const
{
	return (m_fileData.mode == SingleFileMode);
}

void SVFileCamera::SetLoadingMode(SVFileAcquisitonLoadingModeEnum mode)
{
	m_fileData.mode = mode;
}

SVFileAcquisitonLoadingModeEnum SVFileCamera::GetLoadingMode() const
{
	return m_fileData.mode;
}

bool SVFileCamera::IsContinuousLoadMode() const
{
	return (m_fileData.mode == ContinuousMode);
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

	// close bitmap
	m_bitmap.Destroy();
}

bool SVFileCamera::IsRunning() const
{
	return m_thread.IsActive();
}

std::string SVFileCamera::GetNextFilename()
{
	std::string fileName;
	const SVFileInfo& fileInfo = m_loadSequence.GetNext();
	return fileInfo.filename;
}

HRESULT SVFileCamera::DoOneShot()
{
	return m_thread.Signal((void *)this);
}

HRESULT SVFileCamera::CopyImage(unsigned char* pBuffer)
{
	HRESULT hr = S_FALSE;

	if (ValidImageFormatForCopy())
	{
		const unsigned char* pBits = m_bitmap.GetBits();
		if (pBits && pBuffer)
		{
			bool bInvert = false;
			// crop or fill for mismatch sizes
			SIZE imageSize = m_bitmap.GetSize();
			if (imageSize.cy > 0)
			{
				bInvert = true;
			}
			else
			{
				imageSize.cy = abs(imageSize.cy);
			}

			int srcBitDepth = m_bitmap.GetBitDepth();
			int dstBitDepth = 0;
			if (m_fileData.imageFormat == SvDef::SVImageFormatRGB8888)
			{
				dstBitDepth = 32;
			}
			else if (m_fileData.imageFormat == SvDef::SVImageFormatRGB888)
			{
				dstBitDepth = 24;
			}
			else //if (m_fileData.imageFormat == SvDef::SVImageFormatMono8)
			{
				dstBitDepth = 8;
			}

			if (dstBitDepth == 32 && srcBitDepth == 24)
			{
				hr = Copy24BitTo32BitImage(pBits, pBuffer, imageSize, bInvert);
			}
			else
			{
				hr = CopySameBitDepthImage(pBits, pBuffer, dstBitDepth, imageSize, bInvert);
			}
		}
	}
	return hr;
}

HRESULT SVFileCamera::CopySameBitDepthImage(const unsigned char* pSrcBuf, unsigned char* pDstBuf, int bitDepth, const SIZE& imageSize, bool bInvert)
{
	HRESULT hr = S_FALSE;

	if (pSrcBuf && pDstBuf)
	{
		// determine source and destintation stride
		int srcStride = ((((imageSize.cx * bitDepth) + 31 ) & ~31 ) >> 3 );				// align
		int dstStride = ((((m_fileData.imageSize.cx * bitDepth) + 31 ) & ~31 ) >> 3 );	// align
		
		//
		int width = min(imageSize.cx, m_fileData.imageSize.cx);
		if (bitDepth == 32)
		{
			width *= sizeof(RGBQUAD);
		}
		else if (bitDepth == 24)
		{
			width *= sizeof(RGBTRIPLE);
		}

		// copy row by row
		int numRows = min(imageSize.cy, m_fileData.imageSize.cy);

		for (int row = 0; row < numRows; row++)
		{
			const unsigned char* pSrc = pSrcBuf + (row * srcStride);
			long rowOffset = ((bInvert) ? (((numRows - 1) - row) * dstStride) : (row * dstStride));
			unsigned char* pDest = pDstBuf + rowOffset;
			
			memcpy(pDest, pSrc, width);

			// check for fill needed
			int remainder = dstStride - width;
			if (remainder > 0)
			{
				memset(pDest + width, 0, remainder);
			}
		}
		// fill remaining rows
		int rows = m_fileData.imageSize.cy - imageSize.cy;
		if (rows > 0)
		{
			unsigned char* pDest = pDstBuf + (imageSize.cy * dstStride);
			for (int row = 0; row < rows; row++)
			{
				memset(pDest, 0, dstStride);
				pDest += dstStride;
			}
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT SVFileCamera::Copy24BitTo32BitImage(const unsigned char* pSrcBuf, unsigned char* pDstBuf, const SIZE& imageSize, bool bInvert)
{
	HRESULT hr = S_FALSE;

	if (pSrcBuf && pDstBuf)
	{
		int srcBitDepth = 24;
		int dstBitDepth = 32;
		
		// determine source and destintation stride
		int srcStride = ((((imageSize.cx * srcBitDepth) + 31 ) & ~31 ) >> 3 );				// align
		int dstStride = ((((m_fileData.imageSize.cx * dstBitDepth) + 31 ) & ~31 ) >> 3 );	// align
				
		//
		int width = min(imageSize.cx, m_fileData.imageSize.cx);
		width *= sizeof(RGBQUAD);
		
		// copy row by row, column by column
		int numRows = min(imageSize.cy, m_fileData.imageSize.cy);
		int numCols = min(imageSize.cx, m_fileData.imageSize.cx);

		for (int row = 0; row < numRows; row++)
		{
			const unsigned char* pSrc = pSrcBuf + (row * srcStride);
			long rowOffset = ((bInvert) ? (((numRows - 1) - row) * dstStride) : (row * dstStride));
			unsigned char* pDest = pDstBuf + rowOffset;
		
			for (int col = 0; col < numCols; col++)
			{
				memcpy(pDest + (col * sizeof(RGBQUAD)), pSrc + (col * sizeof(RGBTRIPLE)), sizeof(RGBTRIPLE));
			}
			
			// check for fill needed
			int remainder = dstStride - width;
			if (remainder > 0)
			{
				memset(pDest + width, 0, remainder);
			}
		}
		// fill remaining rows
		int rows = m_fileData.imageSize.cy - imageSize.cy;
		if (rows > 0)
		{
			unsigned char* pDest = pDstBuf + (imageSize.cy * dstStride);
			for (int row = 0; row < rows; row++)
			{
				memset(pDest, 0, dstStride);
				pDest += dstStride;
			}
		}
		hr = S_OK;
	}
	return hr;
}

bool SVFileCamera::ValidImageFormatForCopy() const
{
	bool bRetVal = false;
	// check image formats
	// check for SvDef::SVImageFormatRGB8888 (32 bit color)
	// check for SvDef::SVImageFormatRGB888 (24 bit color)
	// check for SvDef::SVImageFormatMono8 (8 bit grayscale)
	int bitDepth = m_bitmap.GetBitDepth();
	if (bitDepth == 32 && m_fileData.imageFormat == SvDef::SVImageFormatRGB8888)
	{
		bRetVal = true;
	}
	else if (bitDepth == 24 && m_fileData.imageFormat == SvDef::SVImageFormatRGB888)
	{
		bRetVal = true;
	}
	// allow 24 to 32 bit conversion
	else if (bitDepth == 24 && m_fileData.imageFormat == SvDef::SVImageFormatRGB8888)
	{
		bRetVal = true;
	}
	else if (bitDepth == 8 && m_fileData.imageFormat == SvDef::SVImageFormatMono8)
	{
		// check for gray scale palette?
		bRetVal = true;
	}
	return bRetVal;
}

void SVFileCamera::OnAPCEvent( ULONG_PTR data )
{
	SVFileCamera* pCamera = (SVFileCamera *)data;
	std::string filename = pCamera->GetNextFilename();

	// Load file
	if (!filename.empty() && S_OK == SVImageFileLoader::Load(filename.c_str(), pCamera->m_bitmap))
	{
		// fire StartFrame event
		pCamera->m_startFrameEvent.Fire(pCamera->m_index);
	}
	else
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
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_IMAGE_LOAD_ERROR, id, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
}

void SVFileCamera::OnThreadEvent( bool& p_WaitForEvents )
{
	// check file Load status ?
	if (m_bitmap.GetBitmapHandle())
	{
		// fire EndFrame event
		m_endFrameEvent.Fire(m_index);
	}
}

bool SVFileCamera::IsAcquisitionTriggered() const
{
	return m_bAcquisitionTriggered;
}

void SVFileCamera::SetAcquisitionTriggered(bool bAcquisitionTriggered)
{
	m_bAcquisitionTriggered = bAcquisitionTriggered;
}

bool SVFileCamera::GetLineState() const
{
	return m_lineState;
}

void SVFileCamera::SetLineState(bool bState)
{
	m_lineState = bState;
}

const SvTh::TriggerDispatcher& SVFileCamera::GetTriggerDispatcher() const
{
	return m_dispatcher;
}

void SVFileCamera::SetTriggerDispatcher(const SvTh::TriggerDispatcher& rDispatcher)
{
	m_dispatcher = rDispatcher;
}

void SVFileCamera::ClearTriggerCallback()
{
	m_dispatcher.clear();
}

