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

#include "stdafx.h"
#include "SVFileCamera.h"
#include <boost/bind.hpp>
#include "SVHBitmapUtilitiesLibrary/SVImageFileLoader.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVFileSystemLibrary/SVFileSystemScanner.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"

SVFileCamera::SVFileCamera()
: m_lIsStarted(0)
, m_index(-1)
, m_bAcquisitionTriggered(false)
{
	m_pBufferInterface = NULL;
	m_StartTimeStamp = 0;
}

const SVString& SVFileCamera::GetName() const
{
	return m_name;
}

void SVFileCamera::SetName(const SVString& name)
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

SVImageFormatEnum SVFileCamera::GetImageFormat() const
{
	return m_fileData.imageFormat;
}

void SVFileCamera::SetImageFormat(SVImageFormatEnum format)
{
	m_fileData.imageFormat = format;
}

LPCTSTR SVFileCamera::GetDirectory() const
{
	return m_fileData.directory.c_str();
}

void SVFileCamera::SetDirectory(const SVString& dir)
{
	m_fileData.directory = dir;
}

LPCTSTR SVFileCamera::GetFileName() const
{
	return m_fileData.fileName.c_str();
}

void SVFileCamera::SetFileName(const SVString& fileName)
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
		SVFileSystemScanner<Insertor>::ScanForFiles(m_fileData.directory.c_str(), "*.bmp", Insertor(m_fileList, m_fileList.end()));
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

SVString SVFileCamera::GetNextFilename()
{
	SVString fileName;
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
			if (m_fileData.imageFormat == SVImageFormatRGB8888)
			{
				dstBitDepth = 32;
			}
			else if (m_fileData.imageFormat == SVImageFormatRGB888)
			{
				dstBitDepth = 24;
			}
			else //if (m_fileData.imageFormat == SVImageFormatMono8)
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
	// check for SVImageFormatRGB8888 (32 bit color)
	// check for SVImageFormatRGB888 (24 bit color)
	// check for SVImageFormatMono8 (8 bit grayscale)
	int bitDepth = m_bitmap.GetBitDepth();
	if (bitDepth == 32 && m_fileData.imageFormat == SVImageFormatRGB8888)
	{
		bRetVal = true;
	}
	else if (bitDepth == 24 && m_fileData.imageFormat == SVImageFormatRGB888)
	{
		bRetVal = true;
	}
	// allow 24 to 32 bit conversion
	else if (bitDepth == 24 && m_fileData.imageFormat == SVImageFormatRGB8888)
	{
		bRetVal = true;
	}
	else if (bitDepth == 8 && m_fileData.imageFormat == SVImageFormatMono8)
	{
		// check for gray scale palette?
		bRetVal = true;
	}
	return bRetVal;
}

void SVFileCamera::OnAPCEvent( ULONG_PTR data )
{
	SVFileCamera* pCamera = (SVFileCamera *)data;
	SVString filename = pCamera->GetNextFilename();

	// Load file
	if (!filename.empty() && SVImageFileLoader::Load(filename.c_str(), pCamera->m_bitmap) == S_OK)
	{
		// fire StartFrame event
		pCamera->m_startFrameEvent.Fire(pCamera->m_index);
	}
	else
	{
		// add to event log
		CString l_csBuf;
		if (filename.empty())
		{
			l_csBuf.Format(_T("E: FileAcquisition::No Image File to Load, possible Invalid or Empty Directory %s"), pCamera->GetDirectory());
		}
		else
		{
			l_csBuf.Format(_T("E: FileAcquisition::Unable to Load Image File %s"), filename.c_str());
		}
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_IMAGE_LOAD_ERROR, l_csBuf, StdMessageParams );
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

const SVCallbackStruct& SVFileCamera::GetTriggerCallback() const
{
	return m_triggerCallback;
}

void SVFileCamera::SetTriggerCallback(SVCallbackStruct& callback)
{
	m_triggerCallback = callback;
}

void SVFileCamera::ClearTriggerCallback()
{
	m_triggerCallback.m_pCallback = NULL;
	m_triggerCallback.m_pOwner = NULL;
	m_triggerCallback.m_pData = NULL;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVFileAcquisitionDevice\SVFileCamera.cpp_v  $
 * 
 *    Rev 1.2   01 Dec 2014 12:16:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Thread Manager Settings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 06:14:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 09:28:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jan 2013 09:47:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jan 2010 14:52:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Sep 2009 08:45:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jul 2009 15:00:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Dec 2008 15:23:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Oct 2008 13:22:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to log exception to event log if unable to load file or Directory is Invalid or Empty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2008 16:11:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to send tag to Thread for use in creation failures
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:58:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/