//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveRecord.h"
#include "SVArchiveTool.h"
#include "SVGlobal.h"
#include "ArchiveToolHelper.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVArchiveImageThreadClass.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#pragma endregion Includes

#pragma region Constructor
SVArchiveRecord::SVArchiveRecord()
{
	m_lCountImages = 0L;
	m_pArchiveTool = nullptr;

	m_lLastIndex = m_lCountImages;
	m_lMaxIndex = 0;
	m_eArchiveMethod = SVArchiveInvalidMethod;
}

SVArchiveRecord::~SVArchiveRecord()
{
	m_FileNames.clear();
	m_ImageStoreVector.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
void SVArchiveRecord::InitArchiveRecord(SVArchiveTool* p_pToolArchive, SVObjectReference p_refObject)
{
	ASSERT(p_pToolArchive);

	m_pArchiveTool = p_pToolArchive;
	m_eArchiveMethod = m_pArchiveTool->m_eArchiveMethod;

	m_svObjectReference = p_refObject;

	if (dynamic_cast <SVImageClass*> (m_svObjectReference.getObject()))
	{
		BuildFileName();
		BuildArchiveImageFilePaths();
	}
}

void SVArchiveRecord::BuildFileName()
{
	ASSERT(m_svObjectReference.getObject());
	m_FileNameImage = m_ImageObjectName = m_svObjectReference.getObject()->GetCompleteName();
	SvUl::searchAndReplace(m_FileNameImage, _T("."), _T("_"));
}

void SVArchiveRecord::BuildArchiveImageFilePaths()
{
	assert(0 < m_FileNameImage.size());
	assert(m_pArchiveTool);
	DWORD		dwMaxImages;
	m_FileNames.clear();

	SVFileNameClass svFileName;
	std::string PathRoot;
	if (m_pArchiveTool->isImagePathUsingKeywords())
	{
		m_pArchiveTool->getTranslatedImagePath(PathRoot);
	}
	else
	{
		m_pArchiveTool->GetImageArchivePath(PathRoot);
	}

	svFileName.SetPathName(PathRoot.c_str());


	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	for (DWORD i = 0; i < dwMaxImages; i++)
	{
		std::string FileName = SvUl::Format(_T("%s__%06.6ld.bmp"), m_FileNameImage.c_str(), i + 1);
		svFileName.SetFileName(FileName.c_str());
		m_FileNames.push_back(svFileName.GetFullFileName());
	}
}

HRESULT SVArchiveRecord::GetNextFileName(std::string& rImageFile)
{
	HRESULT	hr = S_OK;

	DWORD		dwMaxImages = 0;
	DWORD		dwStopAtMaxCount = 0;
	m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
	m_pArchiveTool->m_dwArchiveStopAtMaxImages.GetValue(dwStopAtMaxCount);

	if (m_lCountImages >= (long)dwMaxImages)
	{
		if (dwStopAtMaxCount)
		{
			return -1902;
		}

		m_lCountImages = 0;        // reset to overwrite existing image file.
	}

	m_lLastIndex = m_lCountImages;
	m_lMaxIndex = std::max(m_lMaxIndex, m_lLastIndex);

	m_lCountImages++;
	if (m_FileNames.size() > m_lLastIndex)
	{
		rImageFile = m_FileNames[m_lLastIndex];
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

void SVArchiveRecord::ConnectInputObject()
{
	if (GUID_NULL != m_svObjectReference.Guid())
	{
		//
		// Get a pointer to the object based on the guid.
		//
		SVObjectClass* pObject = nullptr;

		try
		{
			pObject = SVObjectManagerClass::Instance().GetObject(m_svObjectReference.Guid());

			if (nullptr != pObject)
			{
				if (nullptr == dynamic_cast<SvOi::IValueObject*> (pObject) &&
					nullptr == dynamic_cast<SVImageClass*>(pObject))
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

	if (GUID_NULL != m_svObjectReference.Guid())
	{
		ASSERT(m_pArchiveTool);

		SvOl::SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject(m_pArchiveTool);
		InObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;

		bool rc = SVObjectManagerClass::Instance().ConnectObjectInput(m_svObjectReference.Guid(), &InObjectInfo);

		ASSERT(rc);
	}
}

void SVArchiveRecord::DisconnectInputObject()
{
	if (GUID_NULL != m_svObjectReference.Guid())
	{
		SvOl::SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject(m_pArchiveTool);
		InObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;

		SVObjectManagerClass::Instance().DisconnectObjectInput(m_svObjectReference.Guid(), &InObjectInfo);
	}
}

HRESULT SVArchiveRecord::QueueImage(SvTrc::IImagePtr& rImage, const std::string& rFileName)
{
	assert(nullptr != rImage && !rImage->isEmpty());
	HRESULT Result = S_OK;

	if (m_eArchiveMethod == SVArchiveAsynchronous)
	{
		// the QueueImage function will copy the buffer, so pass in the original here
		SVArchiveImageThreadClass::BufferInfo info(rImage, rFileName, m_ImageInfo, this);
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
HRESULT SVArchiveRecord::AllocateBuffers(long lBufferNumber, BufferStructCountMap& rBufferMap)
{
	HRESULT hr = S_FALSE;

	m_lMaxIndex = 0;
	SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_svObjectReference.getObject());
	ASSERT(pImage);
	if (pImage)
	{
		m_ImageInfo = pImage->GetImageInfo();
		SVMatroxBufferCreateStruct bufferStruct;
		HRESULT Result = SVImageProcessingClass::FillBufferStructFromInfo(m_ImageInfo, bufferStruct);
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

HRESULT SVArchiveRecord::WriteImage(const SvTrc::ITriggerRecordRPtr pTriggerRecord)
{
	HRESULT hr = S_OK;
	bool bOk;

	SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_svObjectReference.getObject());
	bOk = nullptr != pImage;

	if (bOk)
	{
		//
		// An image in our list is archivable.
		//
		//
		// Create a file and convert the image to a .bmp type 
		// file.
		//
		SvTrc::IImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord, true);
		bOk = (nullptr != pImageBuffer && !pImageBuffer->isEmpty());
		if (bOk)
		{
			std::string ImageFile;

			//
			// Write the MIL image to a file in BMP form.
			//
			hr = GetNextFileName(ImageFile);
			if (hr & 0xc000)
			{
				bOk = FALSE;
			}
			else
			{
				bOk = TRUE;
			}

			if (bOk)
			{
				if (m_eArchiveMethod == SVArchiveSynchronous)
				{
					try
					{

						SVMatroxBufferInterface::Export(pImageBuffer->getHandle()->GetBuffer(), ImageFile, SVFileBitmap);

					}
					catch (CException&)
					{
						hr = E_FAIL;
					}
					catch (...)
					{
						hr = E_FAIL;
					}
				}
				else	// SVArchiveGoOffline or SVArchiveAsynchronous
				{
					QueueImage(pImageBuffer, ImageFile);
				}
			}
		}
	}


	return hr;
}
/*static*/HRESULT SVArchiveRecord::WriteImage(SVMatroxBuffer& milBuffer, const std::string& rFileName)
{
	HRESULT Result = S_OK;

	if (!milBuffer.empty())
	{
		try
		{
			Result = SVMatroxBufferInterface::Export(milBuffer, rFileName, SVFileBitmap);

		}
		catch (CException&)
		{
			Result = E_FAIL;
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


SVImageClass* SVArchiveRecord::GetImage()
{
	SVImageClass* l_psvImage = nullptr;

	if (nullptr != m_svObjectReference.getObject())
	{
		GUID guid = m_svObjectReference.Guid();

		l_psvImage = dynamic_cast<SVImageClass*>(SVObjectManagerClass::Instance().GetObject(guid));
	}

	return l_psvImage;
}
#pragma endregion Public Methods