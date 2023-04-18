//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \file SVArchiveRecord.cpp
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveRecord.h"
#include "SVArchiveImageThreadClass.h"
#include "SVArchiveTool.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IValueObject.h"
#include "FilesystemUtilities/FileHelper.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#pragma endregion Includes


namespace SvTo
{

#pragma region Public Methods
void SVArchiveRecord::InitArchiveRecord(SVArchiveTool* pArchiveTool, SVObjectReference rObject)
{
	Log_Assert(pArchiveTool);

	SetArchiveTool(pArchiveTool);

	m_svObjectReference = rObject;

	auto* pObject = m_svObjectReference.getFinalObject();

	if (nullptr != dynamic_cast <SvIe::SVImageClass*> (pObject))
	{
		BuildImageFileName();
		BuildDefaultImageFilePaths();
	}
	else
	{
		auto pValueObject = dynamic_cast <SvOi::IValueObject*> (pObject);
		if (nullptr != pValueObject)
		{
			DWORD setDisplayWidths;
			m_pArchiveTool->m_bvoFormatResults.GetValue(setDisplayWidths);

			if (setDisplayWidths)
			{
				DWORD totalWidth;
				m_pArchiveTool->m_dwArchiveResultsMinimumNumberOfCharacters.GetValue(totalWidth);
				DWORD decimals;
				m_pArchiveTool->m_dwArchiveResultsNumberOfDecimals.GetValue(decimals);
				m_formatString = pValueObject->getFixedWidthFormatString(totalWidth, decimals);
			}
			else
			{
				if (rObject.getObject()->GetObjectSubType() == SvPb::SVTimerValueObjectType)
				{
					m_formatString = _T("{:d}");
				}
				else
				{
					m_formatString = _T("{}");
				}
			}
		}
	}
}

void SVArchiveRecord::BuildImageFileName()
{
	Log_Assert(m_svObjectReference.getObject());
	m_ImageFileName = m_ImageObjectName = m_svObjectReference.getObject()->GetCompleteName();
	SvUl::searchAndReplace(m_ImageFileName, _T("."), _T("_"));
}

void SVArchiveRecord::BuildDefaultImageFilePaths()
{
	Log_Assert(0 < m_ImageFileName.size());
	Log_Assert(m_pArchiveTool);
	DWORD		dwMaxImages;
	m_FileNames.clear();

	m_ImagePathRoot = m_pArchiveTool->getCurrentImagePathRoot();

	SvFs::FileHelper svFileName;
	svFileName.SetPathName(m_ImagePathRoot.c_str());

	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	for (DWORD i = 0; i < dwMaxImages; i++)
	{
		std::string FileName = std::format(_T("{}__{:06d}{}"), m_ImageFileName, i + 1, firstImageFileNameExtension(imageFileFormat()));
		svFileName.SetFileName(FileName.c_str());
		m_FileNames.push_back(svFileName.GetFullFileName());
	}
}

bool SVArchiveRecord::CountImages()
{
	DWORD		dwMaxImages = 0;
	DWORD		dwStopAtMaxCount = 0;

	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	m_pArchiveTool->m_dwArchiveStopAtMaxImages.GetValue(dwStopAtMaxCount);


	if (m_lCountImages >= (long)dwMaxImages)
	{
		if (dwStopAtMaxCount)
		{
			return false;
		}

		m_lCountImages = 0; // reset to overwrite existing image file.
	}

	m_lLastIndex = m_lCountImages;
	m_lMaxIndex = std::max(m_lMaxIndex, m_lLastIndex);

	m_lCountImages++;

	return true;
}

HRESULT SVArchiveRecord::GetNextImageFilePath(std::string& rImageFile)
{
	if (false == CountImages())
	{
		return  -1902;
	}
	

	if (m_FileNames.size() <= m_lLastIndex)
	{
		return E_FAIL;
	}

	SvFs::FileHelper svFileName;
	rImageFile = m_FileNames[m_lLastIndex];

	return S_OK;
}


HRESULT SVArchiveRecord::GetNextAlternativeImageFilePath(std::string& rImageFile, std::string& rImageDirectoryPath)
{
	if (false == CountImages())
	{
		return  -1902;
	}
	SvFs::FileHelper svFileName;
	
	rImageDirectoryPath = m_pArchiveTool->alternativeImageDirectory(m_ImagePathRoot);
	//std::filesystem::create_directories(imageDirectoryPath);
	svFileName.SetPathName(rImageDirectoryPath.c_str());
	svFileName.SetFileName(m_pArchiveTool->getNextImageFileName().c_str());
	rImageFile = (svFileName.GetFullFileName());

	return S_OK;
}

long SVArchiveRecord::QueueImage(SvOi::ITRCImagePtr& rImage, const std::string& rFileName, const std::string& rImageDirectoryPath)
{
	Log_Assert(nullptr != rImage && !rImage->isEmpty());

	if (archiveMode() == SvDef::ArchiveMode::asynchronous)
	{
		// the QueueImage function will copy the buffer, so pass in the original here
		SVArchiveImageThreadClass::BufferInfo info(rImage, rFileName, imageFileFormat(), rImageDirectoryPath, m_ImageInfo, m_toolPos);
		return SVArchiveImageThreadClass::Instance().QueueImage(info);
	}

	if (0 <= m_lLastIndex && m_ImageStoreVector.size() > m_lLastIndex)
	{
		m_ImageStoreVector[m_lLastIndex] = rImage;
		return m_lLastIndex+1;
	}

	return -1;
}

// right now called if method == SvDef::ArchiveMode::goOffline or SvDef::ArchiveMode::asynchronous
HRESULT SVArchiveRecord::AllocateBuffers(long lBufferNumber, BufferStructCountMap& rBufferMap, int toolPos)
{
	m_toolPos = toolPos;

	HRESULT hr = S_FALSE;
	m_lMaxIndex = 0;
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getFinalObject());
	//It must more add-on buffer for the parent image not for the child image.
	while (nullptr != pImage && (SvIe::SVImageClass::BufferType::TRCBuffer != pImage->getBufferType() && SvIe::SVImageClass::BufferType::Undefined != pImage->getBufferType()))
		//@TODO [Arvid][10.30][17.4.2023] this is a bit hacky. It might be better to ensure that the bufferType cannot be undefined (e.g. by calling a Reset if it is)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE_RECORD)
		::OutputDebugString(std::format(_T("\t'{}'\n\t\t-> "), pImage->getInfoString(true)).c_str());
#endif
		pImage = pImage->GetParentImage();
#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE_RECORD)
		if (nullptr != pImage)
		{
			::OutputDebugString(std::format(_T("'{}\n"), pImage->getInfoString()).c_str());
	}
#endif
	}
	Log_Assert(pImage);
	if (pImage)
	{
		m_ImageInfo = pImage->GetImageInfo();

		SVMatroxBufferCreateStruct bufferStruct;
		HRESULT Result = SvIe::SVImageProcessingClass::FillBufferStructFromInfo(m_ImageInfo, bufferStruct);

#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE_RECORD)
		::OutputDebugString(std::format(_T("allocate for {}: "), pImage->getInfoString(true)).c_str());
#endif

		if (S_OK == Result)
		{
			auto iter = rBufferMap.find(bufferStruct);
			if (rBufferMap.end() == iter)
			{
				rBufferMap[bufferStruct] = 1;
			}
			else
			{
				iter->second += 1;
			}

			// now create buffer if reserve OK
			if (SvDef::ArchiveMode::asynchronous != archiveMode())
			{
				m_ImageStoreVector.clear();
				m_ImageStoreVector.resize(lBufferNumber);
			}
		}
		hr = S_OK;
	}

	return hr;
}

HRESULT SVArchiveRecord::WriteImageQueue()
{
	HRESULT hr = S_OK;

	long count = std::min(m_lMaxIndex + 1, static_cast<long>(std::min(m_ImageStoreVector.size(), m_FileNames.size())));

	for (long i = 0; i < count; i++)
	{
		if (nullptr != m_ImageStoreVector[i] && !m_ImageStoreVector[i]->isEmpty())
		{
			WriteImageToFile(m_ImageStoreVector[i]->getHandle()->GetBuffer(), m_FileNames[i], imageFileFormat());
			m_ImageStoreVector[i] = nullptr;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

long SVArchiveRecord::WriteArchiveImage(const SvOi::ITriggerRecordR* pTriggerRecord)
{
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getFinalObject());

	if (nullptr == pImage)
	{
		return 0; //nothing to do
	}

	//
	// An image in our list is archivable.
	//
	//
	// Create a file and convert the image to a .bmp or .png type
	// file.
	//
	SvOi::ITRCImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord, archiveMode() != SvDef::ArchiveMode::synchronous);
	if (nullptr == pImageBuffer || pImageBuffer->isEmpty())
	{
		return 0; //nothing to do
	}

	std::string ImageFilePath;

	//
	// Write the MIL image to a file in BMP form.
	//
	BOOL useAlternativeImagePath = FALSE;
	m_pArchiveTool->m_useAlternativeImagePath.GetValue(useAlternativeImagePath);
	HRESULT hr {S_OK};
	std::string imageDirectoryPath;
	if (!useAlternativeImagePath)
	{
		hr = GetNextImageFilePath(ImageFilePath);
	}
	else
	{
		hr = GetNextAlternativeImageFilePath(ImageFilePath, imageDirectoryPath);
	}
	
	if (hr & 0xc000) //@TODO[Arvid] What does this magic number mean?
	{
		return -1;
	}

	if (archiveMode() == SvDef::ArchiveMode::synchronous)
	{
		try
		{
			if (useAlternativeImagePath)
			{
				std::filesystem::create_directories(imageDirectoryPath);
			}
			SVMatroxBufferInterface::Export(pImageBuffer->getHandle()->GetBuffer(), ImageFilePath, imageFileFormat());
		}
		catch (...)
		{
			return -1;
		}
		return 0;
	}
	// SvDef::ArchiveMode::goOffline or SvDef::ArchiveMode::asynchronous
	return QueueImage(pImageBuffer, ImageFilePath, imageDirectoryPath);
}


/*static*/ HRESULT SVArchiveRecord::WriteImageToFile(const SVMatroxBuffer& milBuffer, const std::string& rFileName, ImageFileFormat fileFormat)
{
	HRESULT Result = S_OK;

	if (!milBuffer.empty())
	{
		try
		{
			Result = SVMatroxBufferInterface::Export(milBuffer, rFileName, fileFormat);
		}
		catch (...)
		{
			Result = E_FAIL;
		}
	}// end if ( bOk )

	return Result;
}

void SVArchiveRecord::SetArchiveTool(SVArchiveTool* pTool)
{
	m_pArchiveTool = pTool;
}


SvDef::ArchiveMode SVArchiveRecord::archiveMode() const
{
	return (nullptr != m_pArchiveTool) ? m_pArchiveTool->m_archiveMode : SvDef::ArchiveMode::invalid;
}


ImageFileFormat  SVArchiveRecord::imageFileFormat() const
{
	return (nullptr != m_pArchiveTool) ? m_pArchiveTool->m_imageFileFormat : ImageFileFormat::invalid; 
}


SvIe::SVImageClass* SVArchiveRecord::GetImage()
{
	SvIe::SVImageClass* pImage = nullptr;

	if (nullptr != m_svObjectReference.getObject())
	{
		pImage = dynamic_cast<SvIe::SVImageClass*> (m_svObjectReference.getFinalObject());
	}

	return pImage;
}
#pragma endregion Public Methods

} //namespace SvTo
