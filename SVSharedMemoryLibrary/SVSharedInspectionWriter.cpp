//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspectionWriter
//* .File Name       : $Workfile:   SVSharedInspectionWriter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:42  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVSharedInspectionWriter.h"
#include "SVSharedConfiguration.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedInspectionWriter::SVSharedInspectionWriter()
		: m_pSharedLastInspectedCache(nullptr)
		, m_pSharedRejectCache(nullptr)
	{
	}

	SVSharedInspectionWriter::~SVSharedInspectionWriter()
	{
		Destroy();
	}

	void SVSharedInspectionWriter::Init()
	{
		SVString rejectImgDirectory = SVSharedConfiguration::GetRejectImageDirectoryName()  + "\\"; 
		memset(m_BufferRejectImageFileName,0, BUFFER_REJECT_IMAGE_FILENAME_LEN * sizeof(TCHAR) );
		_tcscpy_s(m_BufferRejectImageFileName,BUFFER_REJECT_IMAGE_FILENAME_LEN, rejectImgDirectory.c_str());
		m_SecondPtrRejectImageFileName = m_BufferRejectImageFileName + static_cast<int>(strlen(m_BufferRejectImageFileName));
		m_SecondPtrRejectImageFileNameLen = BUFFER_REJECT_IMAGE_FILENAME_LEN - static_cast<int>(strlen(m_BufferRejectImageFileName));
		
		// remove previous share
		if (!m_ShareName.empty())
		{
			boost::interprocess::shared_memory_object::remove(m_ShareName.c_str());
		}
		// Do cleanup of previous image file filemapping
		RemovePreviousImageFiles();
	}

	static void RemoveAllFilesInDirectory(const std::string& directoryName)
	{
		WIN32_FIND_DATA fData;
		std::string searchpath = directoryName  + "*.bmp";
		HANDLE fHandle = FindFirstFile(searchpath.c_str(), &fData);
		if (fHandle != INVALID_HANDLE_VALUE)
		{
			int rc = true;
			do
			{
				std::string filename = directoryName + fData.cFileName;
				::DeleteFile(filename.c_str());
				rc = FindNextFile(fHandle, &fData);
			} while(rc);
			FindClose(fHandle);
		}
	}

	void SVSharedInspectionWriter::RemovePreviousImageFiles()
	{
		// Remove all files in the image directory
		std::string directoryName = SVSharedConfiguration::GetImageDirectoryName();
		directoryName += "\\";
		RemoveAllFilesInDirectory(directoryName);

		// Remove all files in the reject image directory
		directoryName = SVSharedConfiguration::GetRejectImageDirectoryName();
		directoryName += "\\";
		RemoveAllFilesInDirectory(directoryName);
	}

	// Create
	HRESULT SVSharedInspectionWriter::Create( const std::string& name, const GUID& guid, const SVSharedMemorySettings& rSettings, const long numProductSlots, const long numRejectSlots, const CreationInfo& rImagesInfo, const CreationInfo& rValuesInfo )
	{
		HRESULT hr = S_OK;
		m_guid = guid;

		m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
		try
		{
			Init();

		// check for required size
		size_t requiredDataSize = (rValuesInfo.num_entries * sizeof(SVSharedValue)) + (rValuesInfo.names_size * rValuesInfo.num_entries);
		requiredDataSize += (rImagesInfo.num_entries * sizeof(SVSharedImage)) + (rImagesInfo.names_size * rImagesInfo.num_entries);
		size_t requiredSize = (numProductSlots * requiredDataSize) + (numRejectSlots * requiredDataSize);
		requiredSize += (numRejectSlots * requiredDataSize) + (numRejectSlots * requiredDataSize);
		size_t managedShareSize = rSettings.DataStoreSize() * statics::M;

		if (requiredSize < managedShareSize)
		{
			// Allocate new repositories
			m_pManagedSharedMemory = managed_shared_memory_shared_ptr(new boost::interprocess::managed_shared_memory(boost::interprocess::create_only, m_ShareName.c_str(), managedShareSize));

			// Allocate Last Inspected Cache
			SVSharedLastInspectedCacheAllocator salloc = m_pManagedSharedMemory->get_allocator<SVSharedLastInspectedCache>();
			m_pManagedSharedMemory->construct<SVSharedLastInspectedCache>(SVSharedConfiguration::GetLastInspectedName().c_str())(salloc, numProductSlots, rImagesInfo.num_entries, rValuesInfo.num_entries);

			// Allocate Reject Cache
			SVSharedRejectCacheAllocator ralloc = m_pManagedSharedMemory->get_allocator<SVSharedRejectCache>();
			m_pManagedSharedMemory->construct<SVSharedRejectCache>(SVSharedConfiguration::GetRejectsName().c_str())(ralloc, numRejectSlots, rImagesInfo.num_entries, rValuesInfo.num_entries);

			if (S_OK == hr)
			{
				// get pointers to last_inspected/rejects segments
				m_pSharedLastInspectedCache = m_pManagedSharedMemory->find<SVSharedLastInspectedCache>(SVSharedConfiguration::GetLastInspectedName().c_str()).first;
				m_pSharedRejectCache = m_pManagedSharedMemory->find<SVSharedRejectCache>(SVSharedConfiguration::GetRejectsName().c_str()).first;
			}
		}
		else
		{
			hr = E_INVALIDARG; // maybe E_OUTOFMEMORY ?
		}
	}
	catch (const boost::interprocess::interprocess_exception& e)
	{
		SVSharedConfiguration::Log(e.what());
		Destroy();
		hr = E_FAIL;
	}
	return hr;
}

	void SVSharedInspectionWriter::Destroy()
	{
		if (nullptr != m_pManagedSharedMemory.get())
		{
			ReleaseAll();
			if (m_pSharedRejectCache)
			{
				m_pManagedSharedMemory->destroy_ptr(m_pSharedRejectCache);
				m_pSharedRejectCache = nullptr;
			}
			if (m_pSharedLastInspectedCache)
			{
				m_pManagedSharedMemory->destroy_ptr(m_pSharedLastInspectedCache);
				m_pSharedLastInspectedCache = nullptr;
			}
			// release managed_shared_memory
			m_pManagedSharedMemory.reset();
		}
		// cleanup
		Init();
		m_ShareName.clear();
	}

	const std::string& SVSharedInspectionWriter::GetShareName() const
	{
		return m_ShareName;
	}

	const GUID & SVSharedInspectionWriter::GetGuid() const
	{
		return m_guid;
	}

	SVSharedData& SVSharedInspectionWriter::GetLastInspectedSlot(long idx)
	{
		if (idx >= 0 && idx < static_cast<long>(m_pSharedLastInspectedCache->data.size()))
		{
			return m_pSharedLastInspectedCache->data[idx];
		}
		throw (std::exception("GetLastInspectedSlot bad Index"));
	}

	const SVSharedData& SVSharedInspectionWriter::GetLastInspectedSlot(long idx) const
	{
		if (idx >= 0 && idx < static_cast<long>(m_pSharedLastInspectedCache->data.size()))
		{
			return m_pSharedLastInspectedCache->data[idx];
		}
		throw (std::exception("GetLastInspectedSlot bad Index"));
	}

	SVSharedData & SVSharedInspectionWriter::GetRejectSlot(long idx)
	{
		if (idx >= 0 && idx < static_cast<long>(m_pSharedRejectCache->data.size()))
		{
			return m_pSharedRejectCache->data[idx];
		}
		throw (std::exception("GetRejectSlot bad Index"));
	}

	// this should only be called when there are no clients attached
	void SVSharedInspectionWriter::ClearHeldLastInspected()
	{
		if (m_pSharedLastInspectedCache)
		{
			size_t size = m_pSharedLastInspectedCache->data.size();
			for (size_t i = 0; i < size;i++)
			{
				m_pSharedLastInspectedCache->data[i].m_Flags = ds::none;
			}
			m_pSharedLastInspectedCache->current_idx = -1;
		}
	}

	// this should only be called when there are no clients attached
	void SVSharedInspectionWriter::ClearHeldRejects()
	{
		if (m_pSharedRejectCache)
		{
			size_t size = m_pSharedRejectCache->data.size();
			for (size_t i = 0; i < size;i++)
			{
				m_pSharedRejectCache->data[i].m_Flags = ds::none;
			}
			m_pSharedRejectCache->current_idx = -1;
		}
	}

	// this should be called when closing  or the client disconnects
	void SVSharedInspectionWriter::ReleaseAll()
	{
		// free all resources
		ClearHeldLastInspected();
		ClearHeldRejects();
	}

	HRESULT SVSharedInspectionWriter::CopyLastInspectedToReject(long index, long rejectIndex)
	{
		HRESULT hr = S_OK;

		try
		{
			const SVSharedData& rLastInspected = GetLastInspectedSlot(index);
			SVSharedData& rReject = GetRejectSlot(rejectIndex);
			rReject.m_TriggerCount = rLastInspected.m_TriggerCount;
			rReject.m_Values = rLastInspected.m_Values; // Copy Entire Container
			rReject.m_Images = rLastInspected.m_Images; // Copy Entire Container

			for (SVSharedImageContainer::iterator it = rReject.m_Images.begin();it != rReject.m_Images.end();++it)
			{
				std::string lastInspectedImageFileName = it->m_Filename;
				// need to change image name and copy image
				std::string basename = it->m_ElementName.c_str();
				// What a horrible unsafe practice and programmer trap!
				// m_SecondPtrImageFileName points to m_BufferImageFileName, sort of
				// You must use m_BufferImageFileName and not m_SecondPtrImageFileName after calling SvSml::SVSharedImage::BuildImageFileName
				SvSml::SVSharedImage::BuildImageFileName(m_SecondPtrRejectImageFileName, m_SecondPtrRejectImageFileNameLen, basename.c_str(), rejectIndex, true, SVFileBitmap);
				it->SetFileName(m_BufferRejectImageFileName); // assign new name
				CopyFile(lastInspectedImageFileName.c_str(), m_BufferRejectImageFileName, false);
			}
		}
		catch (const std::exception& e)
		{
			SVSharedConfiguration::Log(e.what());
			hr = E_FAIL;
		}
		return hr;
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

