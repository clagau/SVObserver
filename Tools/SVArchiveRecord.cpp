//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveRecord.h"
#include "SVArchiveImageThreadClass.h"
#include "SVArchiveTool.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Constructor
#pragma endregion Constructor

#pragma region Public Methods
void SVArchiveRecord::InitArchiveRecord(SVArchiveTool* pArchiveTool, SVObjectReference rObject)
{
	assert(pArchiveTool);

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
					m_formatString = _T("%I64d");
				}
				else
				{
					m_formatString = _T("");
				}
			}
		}
	}

}

void SVArchiveRecord::BuildImageFileName()
{
	assert(m_svObjectReference.getObject());
	m_ImageFileName = m_ImageObjectName = m_svObjectReference.getObject()->GetCompleteName();
	SvUl::searchAndReplace(m_ImageFileName, _T("."), _T("_"));
}

void SVArchiveRecord::BuildDefaultImageFilePaths()
{
	assert(0 < m_ImageFileName.size());
	assert(m_pArchiveTool);
	DWORD		dwMaxImages;
	m_FileNames.clear();

	m_ImagePathRoot = m_pArchiveTool->getCurrentImagePathRoot();

	SVFileNameClass svFileName;
	svFileName.SetPathName(m_ImagePathRoot.c_str());

	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	for (DWORD i = 0; i < dwMaxImages; i++)
	{
		std::string FileName = SvUl::Format(_T("%s__%06ld.bmp"), m_ImageFileName.c_str(), i + 1);
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

	SVFileNameClass svFileName;
	rImageFile = m_FileNames[m_lLastIndex];

	return S_OK;
}



HRESULT SVArchiveRecord::GetNextAlternativeImageFilePath(std::string& rImageFile, std::string& rImageDirectoryPath)
{
	if (false == CountImages())
	{
		return  -1902;
	}
	SVFileNameClass svFileName;
	
	rImageDirectoryPath = m_pArchiveTool->alternativeImageDirectory(m_ImagePathRoot);
	//std::filesystem::create_directories(imageDirectoryPath);
	svFileName.SetPathName(rImageDirectoryPath.c_str());
	svFileName.SetFileName(m_pArchiveTool->getNextImageFileName().c_str());
	rImageFile = (svFileName.GetFullFileName());

	return S_OK;
}

HRESULT SVArchiveRecord::QueueImage(SvOi::ITRCImagePtr& rImage, const std::string& rFileName, const std::string& rImageDirectoryPath)
{
	assert(nullptr != rImage && !rImage->isEmpty());
	HRESULT Result = S_OK;

	if (m_eArchiveMethod == SVArchiveAsynchronous)
	{
		// the QueueImage function will copy the buffer, so pass in the original here
		SVArchiveImageThreadClass::BufferInfo info(rImage, rFileName,rImageDirectoryPath, m_ImageInfo, m_toolPos);
		SVArchiveImageThreadClass::Instance().QueueImage(info);
	}
	else
	{
		if (0 <= m_lLastIndex && m_ImageStoreVector.size() > m_lLastIndex)
		{
			m_ImageStoreVector[m_lLastIndex] = rImage;
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

// right now called if method == SVArchiveGoOffline or SVArchiveAsynchronous
HRESULT SVArchiveRecord::AllocateBuffers(long lBufferNumber, BufferStructCountMap& rBufferMap, int toolPos)
{
	m_toolPos = toolPos;

	HRESULT hr = S_FALSE;
	m_lMaxIndex = 0;
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getFinalObject());
	//It must more add-on buffer for the parent image not for the child image.
	while (nullptr != pImage && pImage->isChildImageInTRC())
	{
		pImage = pImage->GetParentImage();
	}
	assert(pImage);
	if (pImage)
	{
		m_ImageInfo = pImage->GetImageInfo();
		SVMatroxBufferCreateStruct bufferStruct;
		HRESULT Result = SvIe::SVImageProcessingClass::FillBufferStructFromInfo(m_ImageInfo, bufferStruct);
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
			if (SVArchiveAsynchronous != m_eArchiveMethod)
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
			WriteImageToFile(m_ImageStoreVector[i]->getHandle()->GetBuffer(), m_FileNames[i]);
			m_ImageStoreVector[i] = nullptr;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

HRESULT SVArchiveRecord::WriteImage(const SvOi::ITriggerRecordR* pTriggerRecord)
{
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getFinalObject());

	if (nullptr == pImage)
	{
		return S_OK; //nothing to do
	}

	//
	// An image in our list is archivable.
	//
	//
	// Create a file and convert the image to a .bmp type 
	// file.
	//
	SvOi::ITRCImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord, true);
	if (nullptr == pImageBuffer || pImageBuffer->isEmpty())
	{
		return S_OK; //nothing to do
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
	
	//std::filesystem::create_directories(imageDirectoryPath);

	if (hr & 0xc000) //@TODO[Arvid] What does this magic number mean?
	{
		return hr;
	}

	if (m_eArchiveMethod == SVArchiveSynchronous)
	{
		try
		{
			if (useAlternativeImagePath)
			{
				std::filesystem::create_directories(imageDirectoryPath);
			}
			SVMatroxBufferInterface::Export(pImageBuffer->getHandle()->GetBuffer(), ImageFilePath, SVFileBitmap);
		}
		catch (...)
		{
			return E_FAIL;
		}
	}
	else	// SVArchiveGoOffline or SVArchiveAsynchronous
	{
		QueueImage(pImageBuffer, ImageFilePath, imageDirectoryPath);
	}

	return S_OK;
}


/*static*/ HRESULT SVArchiveRecord::WriteImageToFile(const SVMatroxBuffer& milBuffer, const std::string& rFileName)
{
	HRESULT Result = S_OK;

	if (!milBuffer.empty())
	{
		try
		{
			Result = SVMatroxBufferInterface::Export(milBuffer, rFileName, SVFileBitmap);
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
	if (pTool)
	{
		m_eArchiveMethod = pTool->m_eArchiveMethod;
	}
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
