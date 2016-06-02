//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspectionReader
//* .File Name       : $Workfile:   SVSharedInspectionReader.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <intrin.h>
#include "SVSharedInspectionReader.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

#pragma intrinsic (_InterlockedDecrement16)
#pragma intrinsic (_InterlockedIncrement16)
#pragma intrinsic (_InterlockedCompareExchange)

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedInspectionReader::SVSharedInspectionReader()
	: sh(nullptr)
	, rsh(nullptr)
	, m_bOpened(false)
	{
	}

	SVSharedInspectionReader::~SVSharedInspectionReader()
	{
		Close();
	}

	bool SVSharedInspectionReader::Open(const std::string& name)
	{
		bool bRetVal = false;
		try
		{
			m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
			shm = DataSharedMemPtr(new boost::interprocess::managed_shared_memory(boost::interprocess::open_only, m_ShareName.c_str()));

			// get pointers to last_inspected/reject segments
			sh = shm->find<SVSharedLastInspectedCache>(SVSharedConfiguration::GetLastInspectedName().c_str()).first;
			rsh = shm->find<SVSharedRejectCache>(SVSharedConfiguration::GetRejectsName().c_str()).first;
			if (sh && rsh)
			{
				bRetVal = true;
				m_bOpened = true;
			}
		}
		catch (boost::interprocess::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
			m_bOpened = false;
		}
		return bRetVal;
	}

	void SVSharedInspectionReader::Close()
	{
		try
		{
			if (shm)
			{
				shm.reset();
			}
		}
		catch(boost::interprocess::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
		}
		sh = nullptr;
		rsh = nullptr;

		m_bOpened = false;
	}

	bool SVSharedInspectionReader::IsOpen() const
	{
		return m_bOpened;
	}

	// get Last Inspected Run data
	SVSharedData& SVSharedInspectionReader::GetInspectedSlot(long index)
	{
		return sh->data[ index ];
	}

	// get Reject Run data
	SVSharedData& SVSharedInspectionReader::GetRejectSlot(long index)
	{
		return rsh->data[ index ];
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
