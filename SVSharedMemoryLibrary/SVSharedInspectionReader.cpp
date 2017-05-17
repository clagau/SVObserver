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

namespace SvSml
{
	SVSharedInspectionReader::SVSharedInspectionReader()
	: m_pSharedLastInspectedCache(nullptr)
	, m_pSharedRejectCache(nullptr)
	, m_bOpened(false)
	{
	}

	SVSharedInspectionReader::~SVSharedInspectionReader()
	{
		Close();
	}

	bool SVSharedInspectionReader::Open(const SVString& name)
	{
		bool bRetVal = false;
		try
		{
			m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
			m_pManagedSharedMemory = std::shared_ptr<bip::managed_shared_memory>(new bip::managed_shared_memory(bip::open_only, m_ShareName.c_str()));

			// get pointers to last_inspected/reject segments
			m_pSharedLastInspectedCache = m_pManagedSharedMemory->find<SVSharedLastInspectedCache>(SVSharedConfiguration::GetLastInspectedName().c_str()).first;
			m_pSharedRejectCache = m_pManagedSharedMemory->find<SVSharedRejectCache>(SVSharedConfiguration::GetRejectsName().c_str()).first;
			
			bool ok = m_RejectImages.OpenImageStore(name.c_str(), SharedImageStore::reject);
			ok= ok && m_ProductImages.OpenImageStore(name.c_str(), SharedImageStore::last);
			if (ok && m_pSharedLastInspectedCache && m_pSharedRejectCache)
			{
				bRetVal = true;
				m_bOpened = true;
			}
		}
		catch (bip::interprocess_exception& e)
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
			if (m_pManagedSharedMemory)
			{
				m_pManagedSharedMemory.reset();
			}
		}
		catch(bip::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
		}
		m_pSharedLastInspectedCache = nullptr;
		m_pSharedRejectCache = nullptr;

		m_RejectImages.CloseConnection();
		m_ProductImages.CloseConnection();
		m_bOpened = false;
	}

	bool SVSharedInspectionReader::IsOpen() const
	{
		return m_bOpened;
	}

	// get Last Inspected Run data
	SVSharedData& SVSharedInspectionReader::GetInspectedSlot(long index)
	{
		return m_pSharedLastInspectedCache->data[ index ];
	}

	// get Reject Run data
	SVSharedData& SVSharedInspectionReader::GetRejectSlot(long index)
	{
		return m_pSharedRejectCache->data[ index ];
	}
} //namespace SvSml
