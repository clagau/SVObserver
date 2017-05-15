#pragma region Includes
#include "StdAfx.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVString.h"
#include "SharedImageStore.h"

#pragma endregion Includes

namespace SvSml
{
	const LPCTSTR ShareNames[] = {".Images.Reject", ".Images.Last"};
	SharedImageStore::SharedImageStore():m_pSharedMemoryObject(NULL), 
											m_pMappedRegion(NULL),
											m_slotCount(0),
											m_slotSize(0) 
	{
		;
	}
	SharedImageStore::~SharedImageStore()
	{
		CloseConnection();
	}
	void   SharedImageStore::CloseConnection()
	{
		if(m_pSharedMemoryObject)
		{
			delete m_pSharedMemoryObject;
			m_pSharedMemoryObject = nullptr;
		}
		if(m_pMappedRegion)
		{
			delete m_pMappedRegion;
			m_pMappedRegion = nullptr;
		}
	}

	bool SharedImageStore::CreateImageStore(const SVString &InspectionName, StoreType t, DWORD size, DWORD  slots)
	{
		m_InspectionName = InspectionName;
		m_ShareName.clear();
		m_ShareName+= InspectionName;
		if(t == reject)
			m_ShareName +=  ShareNames[0];
		else
			m_ShareName +=  ShareNames[1];

		m_slotCount = slots;
		m_slotSize = size;
		if(m_pSharedMemoryObject)
		{
			delete m_pSharedMemoryObject;
			m_pSharedMemoryObject = nullptr;
		}

		if(m_pMappedRegion)
		{
			delete m_pMappedRegion;
			m_pMappedRegion = nullptr;
		}
		//Delete all in the shared memory
		Destroy();
		
		m_pSharedMemoryObject =  new  bip::shared_memory_object(bip::create_only, m_ShareName.c_str(), bip::read_write );
		m_pSharedMemoryObject->truncate(m_slotCount* m_slotSize +  2* sizeof(DWORD) );
		m_pMappedRegion =  new bip::mapped_region(*m_pSharedMemoryObject, bip::read_write);
		
		DWORD *pSize  = (DWORD*)(m_pMappedRegion->get_address());
		*pSize = m_slotSize;
		DWORD* pSlot  =   pSize+1; 
		*pSlot = m_slotCount;
		return true;
	}
	bool SharedImageStore::OpenImageStore(const SVString &InspectionName, StoreType t)
	{
		m_InspectionName = InspectionName;
		m_ShareName.clear();
		m_ShareName+= InspectionName;
		if(t == reject)
		{
			m_ShareName +=  ShareNames[0];
		}
		else
		{
			m_ShareName +=  ShareNames[1];
		}
		if(m_pSharedMemoryObject)
		{
			delete m_pSharedMemoryObject;
			m_pSharedMemoryObject = nullptr;
		}

		if(m_pMappedRegion)
		{
			delete m_pMappedRegion;
			m_pMappedRegion = nullptr;
		}
		m_pSharedMemoryObject =  new  bip::shared_memory_object(bip::open_only, m_ShareName.c_str(), bip::read_only );
		if(!m_pSharedMemoryObject)
		{
			SVString msg =  SvUl_SF::Format("Create memory object %s failed",  m_ShareName.c_str() ) ;
			throw std::exception(msg.c_str());
		};

		m_pMappedRegion =  new bip::mapped_region(*m_pSharedMemoryObject, bip::read_only);
		if(!m_pMappedRegion)
		{
			SVString msg =  SvUl_SF::Format("Create mapped region for  %s failed",  m_ShareName.c_str() ) ;
			throw std::exception(msg.c_str());
		};
		size_t size = m_pMappedRegion->get_size();
		if(size < 4)
		{
			throw std::exception("incorrect size");
		}
		DWORD* pSize  = (DWORD*)( m_pMappedRegion->get_address());
		m_slotSize = *pSize;
		DWORD* pSlot  =pSize +1;
		m_slotCount = *pSlot; 
		if(size < m_slotCount * m_slotSize + 4)
		{
			throw std::exception("incorrect size");
		}
		return true;
	}

	bool SharedImageStore::Destroy()
	{
		
		CloseConnection();

		if(!m_ShareName.empty())
		{
			bip::shared_memory_object::remove(m_ShareName.c_str());
		}

		return false;
	}
	
	BYTE* SharedImageStore::GetPtr(DWORD slot, DWORD offset )
	{
		if(! m_pMappedRegion || slot >= m_slotCount || offset >= m_slotSize )
			return NULL;
		BYTE *res =  (BYTE*) m_pMappedRegion->get_address(); 
		if(res)
		{
			res += (slot* m_slotSize) + 2 * sizeof(DWORD) + offset; 
		}
		return res;
	}

	LPCTSTR SharedImageStore::GetShareName() const
	{
			return m_ShareName.c_str();
	}
	DWORD SharedImageStore::GetSlotCount() const 
	{
		return m_slotCount;
	}
	DWORD SharedImageStore::GetSlotSize() const
	{
		return m_slotSize;
	}
} //namespace SvSml
