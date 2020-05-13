//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveRecord.h"
#include "ArchiveToolHelper.h"
#include "SVArchiveImageThreadClass.h"
#include "SVArchiveTool.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Constructor
#pragma endregion Constructor

#pragma region Public Methods
void SVArchiveRecord::InitArchiveRecord(SVArchiveTool* p_pToolArchive, SVObjectReference p_refObject)
{
	assert(p_pToolArchive);

	m_pArchiveTool = p_pToolArchive;
	m_eArchiveMethod = m_pArchiveTool->m_eArchiveMethod;

	m_svObjectReference = p_refObject;

	if(nullptr != dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getObject()))
	{
		BuildFileName();
		BuildArchiveImageFilePaths();
	}
}

void SVArchiveRecord::BuildFileName()
{
	assert(m_svObjectReference.getObject());
	m_FileNameImage = m_ImageObjectName = m_svObjectReference.getObject()->GetCompleteName();
	SvUl::searchAndReplace(m_FileNameImage, _T("."), _T("_"));
}

void SVArchiveRecord::BuildArchiveImageFilePaths()
{
	assert(0 < m_FileNameImage.size());
	assert(m_pArchiveTool);
	DWORD		dwMaxImages;
	m_FileNames.clear();

	if (m_pArchiveTool->isImagePathUsingKeywords())
	{
		m_pArchiveTool->getTranslatedImagePath(m_ImagePathRoot);
	}
	else
	{
		m_pArchiveTool->GetImageArchivePath(m_ImagePathRoot);
	}

	SVFileNameClass svFileName;
	svFileName.SetPathName(m_ImagePathRoot.c_str());

	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	for (DWORD i = 0; i < dwMaxImages; i++)
	{
		std::string FileName = SvUl::Format(_T("%s__%06ld.bmp"), m_FileNameImage.c_str(), i + 1);
		svFileName.SetFileName(FileName.c_str());
		m_FileNames.push_back(svFileName.GetFullFileName());
	}
}


HRESULT SVArchiveRecord::GetNextImageFilePath(std::string& rImageFile, bool useAlternativeImagePaths)
{
	DWORD		dwMaxImages = 0;
	DWORD		dwStopAtMaxCount = 0;
	DWORD		dwUseTriggerCountForImages = 0;

	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	m_pArchiveTool->m_dwArchiveStopAtMaxImages.GetValue(dwStopAtMaxCount);
	m_pArchiveTool->m_dwUseTriggerCountForImages.GetValue(dwUseTriggerCountForImages);

	if (m_lCountImages >= (long)dwMaxImages)
	{
		if (dwStopAtMaxCount)
		{
			return -1902; //@TODO[Arvid] What does this $%&*! magic number mean?
		}

		m_lCountImages = 0; // reset to overwrite existing image file.
	}

	m_lLastIndex = m_lCountImages;
	m_lMaxIndex = std::max(m_lMaxIndex, m_lLastIndex);

	m_lCountImages++;

	if (m_FileNames.size() <= m_lLastIndex)
	{
		return E_FAIL;
	}

	SVFileNameClass svFileName;
	if (useAlternativeImagePaths)
	{
		svFileName.SetPathName(m_pArchiveTool->getNextImageDirectory(m_ImagePathRoot).c_str());
		svFileName.SetFileName(m_pArchiveTool->getNextImageFileName(m_FileNameImage, useAlternativeImagePaths).c_str());
		m_FileNames[m_lLastIndex] = (svFileName.GetFullFileName()); 
	}
	else
	{
		if (dwUseTriggerCountForImages)
		{
			svFileName.SetPathName(m_pArchiveTool->getNextImageDirectory(m_ImagePathRoot).c_str());
			svFileName.SetFileName(m_pArchiveTool->getNextImageFileName(m_FileNameImage, useAlternativeImagePaths).c_str());
			m_FileNames[m_lLastIndex] = svFileName.GetFullFileName();
		}
		//else otherwise the file name already set elsewhere will be used
	}
	rImageFile = m_FileNames[m_lLastIndex];

	return S_OK;
}

void SVArchiveRecord::ConnectInputObject()
{
	if (SvDef::InvalidObjectId != m_svObjectReference.getObjectId())
	{
		//
		// Get a pointer to the object based on the id.
		//
		SVObjectClass* pObject = nullptr;

		try
		{
			pObject = SVObjectManagerClass::Instance().GetObject(m_svObjectReference.getObjectId());

			if (nullptr != pObject)
			{
				if (nullptr == dynamic_cast<SvOi::IValueObject*> (pObject) &&
					nullptr == dynamic_cast<SvIe::SVImageClass*>(pObject))
				{
					pObject = nullptr;
				}
			}
		}
		catch (...)
		{
			pObject = nullptr;
		}

		if (pObject != m_svObjectReference.getObject())
		{
			long lArrayIndex = m_svObjectReference.ArrayIndex();
			m_svObjectReference = pObject;
			m_svObjectReference.SetArrayIndex(lArrayIndex);
		}
	}

	if (SvDef::InvalidObjectId != m_svObjectReference.getObjectId())
	{
		assert(m_pArchiveTool);

		SvOl::SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject(m_pArchiveTool);
		InObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;

		bool rc = SVObjectManagerClass::Instance().ConnectObjectInput(m_svObjectReference.getObjectId(), &InObjectInfo);

		assert(rc);
	}
}

void SVArchiveRecord::DisconnectInputObject()
{
	if (SvDef::InvalidObjectId != m_svObjectReference.getObjectId())
	{
		SvOl::SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject(m_pArchiveTool);
		InObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;

		SVObjectManagerClass::Instance().DisconnectObjectInput(m_svObjectReference.getObjectId(), &InObjectInfo);
	}
}

HRESULT SVArchiveRecord::QueueImage(SvTrc::IImagePtr& rImage, const std::string& rFileName)
{
	assert(nullptr != rImage && !rImage->isEmpty());
	HRESULT Result = S_OK;

	if (m_eArchiveMethod == SVArchiveAsynchronous)
	{
		// the QueueImage function will copy the buffer, so pass in the original here
		SVArchiveImageThreadClass::BufferInfo info(rImage, rFileName, m_ImageInfo, m_toolPos);
		TheSVArchiveImageThreadClass().QueueImage(info);
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
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getObject());
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
			WriteImage(m_ImageStoreVector[i]->getHandle()->GetBuffer(), m_FileNames[i]);
			m_ImageStoreVector[i] = nullptr;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

HRESULT SVArchiveRecord::WriteImage(const SvTrc::ITriggerRecordR* pTriggerRecord)
{
	SvIe::SVImageClass* pImage = dynamic_cast <SvIe::SVImageClass*> (m_svObjectReference.getObject());
	
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
	SvTrc::IImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord, true);
	if (nullptr == pImageBuffer || pImageBuffer->isEmpty())
	{
		return S_OK; //nothing to do
	}

	std::string ImageFilePath;

	//
	// Write the MIL image to a file in BMP form.
	//
	BOOL useAlternativeImagePaths = FALSE;
	m_pArchiveTool->m_useAlternativeImagePaths.GetValue(useAlternativeImagePaths);

	HRESULT hr = GetNextImageFilePath(ImageFilePath, useAlternativeImagePaths==TRUE);

	if (hr & 0xc000) //@TODO[Arvid] What does this $%&*! magic number mean?
	{
		return hr;
	}

	if (m_eArchiveMethod == SVArchiveSynchronous)
	{
		try
		{
			SVMatroxBufferInterface::Export(pImageBuffer->getHandle()->GetBuffer(), ImageFilePath, SVFileBitmap);
		}
		catch (...)
		{
			return E_FAIL;
		}
	}
	else	// SVArchiveGoOffline or SVArchiveAsynchronous
	{
		QueueImage(pImageBuffer, ImageFilePath);
	}

	return S_OK;
}


/*static*/ HRESULT SVArchiveRecord::WriteImage(const SVMatroxBuffer& milBuffer, const std::string& rFileName)
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

void SVArchiveRecord::Init(SVArchiveTool* pTool)
{
	m_pArchiveTool = pTool;
	m_eArchiveMethod = pTool->m_eArchiveMethod;
}


SvIe::SVImageClass* SVArchiveRecord::GetImage()
{
	SvIe::SVImageClass* pImage = nullptr;

	if (nullptr != m_svObjectReference.getObject())
	{
		pImage = dynamic_cast<SvIe::SVImageClass*> (SvOi::getObject(m_svObjectReference.getObjectId()));
	}

	return pImage;
}
#pragma endregion Public Methods

} //namespace SvTo
