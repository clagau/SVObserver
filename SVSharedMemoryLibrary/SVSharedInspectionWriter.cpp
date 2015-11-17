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

#include "stdafx.h"
#include "SVSharedInspectionWriter.h"
#include "SVSharedConfiguration.h"

using namespace SeidenaderVision;

SVSharedInspectionWriter::SVSharedInspectionWriter()
: sh(nullptr)
, rsh(nullptr)
{
}

SVSharedInspectionWriter::~SVSharedInspectionWriter()
{
	Destroy();
}

void SVSharedInspectionWriter::Init()
{
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
HRESULT SVSharedInspectionWriter::Create( const std::string& name, const GUID & guid, const SVSharedMemorySettings& rSettings )
{
	HRESULT hr = S_OK;
	m_guid = guid;

	m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
	try
	{
		Init();

		// Allocate new repositories
		size_t managedShareSize = rSettings.DataStoreSize() * statics::M;
		shm = managed_shared_memory_shared_ptr(new boost::interprocess::managed_shared_memory(boost::interprocess::create_only, m_ShareName.c_str(), managedShareSize));

		// Allocate Last Inspected Cache
		SVSharedLastInspectedCacheAllocator salloc = shm->get_allocator<SVSharedLastInspectedCache>();
		shm->construct<SVSharedLastInspectedCache>(SVSharedConfiguration::GetLastInspectedName().c_str())(salloc, rSettings.NumProductSlots());

		// Allocate Reject Cache
		SVSharedRejectCacheAllocator ralloc = shm->get_allocator<SVSharedRejectCache>();
		shm->construct<SVSharedRejectCache>(SVSharedConfiguration::GetRejectsName().c_str())(ralloc, rSettings.NumRejectSlots());

		if (S_OK == hr)
		{
			// get pointers to last_inspected/rejects segments
			sh = shm->find<SVSharedLastInspectedCache>(SVSharedConfiguration::GetLastInspectedName().c_str()).first;
			rsh = shm->find<SVSharedRejectCache>(SVSharedConfiguration::GetRejectsName().c_str()).first;
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
	if (shm.get() != nullptr)
	{
		ReleaseAll();
		if (rsh)
		{
			shm->destroy_ptr(rsh);
			rsh = nullptr;
		}
		if (sh)
		{
			shm->destroy_ptr(sh);
			sh = nullptr;
		}
		// release managed_shared_memory
		shm.reset();
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
	if (idx >= 0 && idx < static_cast<long>(sh->data.size()))
	{
		return sh->data[idx];
	}
	throw (std::exception("GetLastInspectedSlot bad Index"));
}

const SVSharedData& SVSharedInspectionWriter::GetLastInspectedSlot(long idx) const
{
	if (idx >= 0 && idx < static_cast<long>(sh->data.size()))
	{
		return sh->data[idx];
	}
	throw (std::exception("GetLastInspectedSlot bad Index"));
}

SVSharedData & SVSharedInspectionWriter::GetRejectSlot(long idx)
{
	if (idx >= 0 && idx < static_cast<long>(rsh->data.size()))
	{
		return rsh->data[idx];
	}
	throw (std::exception("GetRejectSlot bad Index"));
}

// this should only be called when there are no clients attached
void SVSharedInspectionWriter::ClearHeldLastInspected()
{
	if (sh)
	{
		size_t size = sh->data.size();
		for (size_t i = 0; i < size;i++)
		{
			sh->data[i].m_Flags = ds::none;
		}
		sh->current_idx = -1;
	}
}

// this should only be called when there are no clients attached
void SVSharedInspectionWriter::ClearHeldRejects()
{
	if (rsh)
	{
		size_t size = rsh->data.size();
		for (size_t i = 0; i < size;i++)
		{
			rsh->data[i].m_Flags = ds::none;
		}
		rsh->current_idx = -1;
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
		

#ifdef TRACERUNREJECT 
		std::stringstream debugstream;
		debugstream << "Trigger Count: " <<  rLastInspected.m_TriggerCount <<  " Index: " <<   index << " Reject Index: " << rejectIndex << std::endl;
		::OutputDebugStringA(debugstream.str().c_str());
#endif 
		for (SVSharedValueMap::const_iterator it = rLastInspected.m_Values.begin();it != rLastInspected.m_Values.end();++it)
		{


			std::pair< SVSharedValueMap::iterator,bool>  mRet; 	
			mRet = rReject.m_Values.insert(SVSharedValuePair(it->first, it->second));
			if(false == mRet.second)
			{
				mRet.first->second = it->second;

			}

#ifdef TRACERUNREJECT 
			std::string  myStringIn = it->first.c_str();
			std::string myResultIn = it->second.m_Result.c_str();			
			
			std::string myStringOut =  mRet.first->first.c_str(); 
			std::string  myResultout =  mRet.first->second.m_Result.c_str();

			std::stringstream dstream;
			dstream <<  "Insert in SharedValueMap(  "  << myStringIn << " " << myResultIn <<  " ) ret " << mRet.second << std::endl;
			::OutputDebugStringA(dstream.str().c_str());

			if(myStringOut.compare(myStringIn)  || myResultout.compare(myResultIn) )
			{
				std::stringstream dstream;
				dstream << "ERROR: in SharedValueMap " << myStringOut <<  " : " << myResultout <<  std::endl;
				::OutputDebugStringA(dstream.str().c_str());

			}
#endif
			
		}
		for (SVSharedImageMap::const_iterator it = rLastInspected.m_Images.begin();it != rLastInspected.m_Images.end();++it)
		{
			// need to change images names and copy images
			img::type imgType = SVSharedImage::GetImageType(it->second.m_Filename.c_str());
			std::string basename = it->first.c_str();
			std::string rejectImgName = SVSharedConfiguration::GetRejectImageDirectoryName() + + "\\" + SVSharedImage::filename(basename, rejectIndex, imgType, true);
			SVSharedImage rejectImage(rejectImgName.c_str(), it->second.m_Status, it->second.m_Allocator);

			std::pair<SVSharedImageMap::iterator, bool> l_IterPair = rReject.m_Images.insert(SVSharedImagePair(it->first, rejectImage));
			if (l_IterPair.first != rReject.m_Images.end())
			{
				CopyFile(it->second.m_Filename.c_str(), rejectImage.m_Filename.c_str(), false);
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedInspectionWriter.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jan 2012 19:00:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Revised Create method to check for available disk space
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:40:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
