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
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SharedMemWriter.h"
#pragma endregion Includes

namespace SvSml
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
		
		if (!m_ShareName.empty())
		{
			bip::shared_memory_object::remove(m_ShareName.c_str());
		}
		// Do cleanup of previous image file filemapping
		RemovePreviousImageFiles();
	}

	static void RemoveAllFilesInDirectory(const SVString& directoryName)
	{
		WIN32_FIND_DATA fData;
		SVString searchpath = directoryName  + "*.bmp";
		HANDLE fHandle = FindFirstFile(searchpath.c_str(), &fData);
		if (fHandle != INVALID_HANDLE_VALUE)
		{
			int rc = true;
			do
			{
				SVString filename = directoryName + fData.cFileName;
				::DeleteFile(filename.c_str());
				rc = FindNextFile(fHandle, &fData);
			} while(rc);
			FindClose(fHandle);
		}
	}

	void SVSharedInspectionWriter::RemovePreviousImageFiles()
	{
		// Remove all files in the image directory
		SVString directoryName = SVSharedConfiguration::GetImageDirectoryName();
		directoryName += "\\";
		RemoveAllFilesInDirectory(directoryName);
		// Remove all files in the reject image directory
		directoryName = SVSharedConfiguration::GetRejectImageDirectoryName();
		directoryName += "\\";
		RemoveAllFilesInDirectory(directoryName);
	}

	// Create
	HRESULT SVSharedInspectionWriter::Create( const SVString& name, const GUID& guid, const SVSharedMemorySettings& rSettings, const long numProductSlots, const long numRejectSlots, const CreationInfo& rImagesInfo, const CreationInfo& rValuesInfo )
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
			m_pManagedSharedMemory =std::shared_ptr<bip::managed_shared_memory>(new bip::managed_shared_memory(bip::create_only, m_ShareName.c_str(), managedShareSize));

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
	catch (const bip::interprocess_exception& e)
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

	const SVString& SVSharedInspectionWriter::GetShareName() const
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

	
	DWORD SVSharedInspectionWriter::GetRejectCacheSize()
	{
		 return static_cast<DWORD>(m_pSharedRejectCache->data.size());
	}


	DWORD SVSharedInspectionWriter::GetLastInspectedCacheSize()
	{
		return static_cast<DWORD>(m_pSharedLastInspectedCache->data.size());
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
			
			//Copy only changing values instead of all  
			CopySharedValues( rReject.m_Values, rLastInspected.m_Values);
			CopySharedImages( rReject.m_Images, rLastInspected.m_Images );

			SVSharedImageVector::iterator Rej_it =   rReject.m_Images.begin();
			SVSharedImageVector::const_iterator LaI_it =   rLastInspected.m_Images.begin();


			for (;Rej_it != rReject.m_Images.end(), LaI_it !=  rLastInspected.m_Images.end(); ++Rej_it, ++LaI_it)
			{
				SVMatroxBuffer& rToBuffer = SharedMemWriter::Instance().GetImageBuffer(rejectIndex, SvSml::SharedImageStore::reject, Rej_it->m_ImageStoreIndex, Rej_it->m_ImageIndex); 
				SVMatroxBuffer& rFromBuffer=  SharedMemWriter::Instance().GetImageBuffer(index, SvSml::SharedImageStore::last, LaI_it->m_ImageStoreIndex, LaI_it->m_ImageIndex); 
				
				if(rToBuffer.empty()== false && rFromBuffer.empty() == false )
				{
					hr =  SVMatroxBufferInterface::CopyBuffer(rToBuffer,rFromBuffer);
				}

			}
		}
		catch (const std::exception& e)
		{
			SVSharedConfiguration::Log(e.what());
			hr = E_FAIL;
		}
		return hr;
	}
} //namespace SvSml

