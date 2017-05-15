#include "StdAfx.h"
#include "SharedMemReader.h"

namespace SvSml
{
	SharedMemReader::SharedMemReader(void)
	{
	}


	SharedMemReader::~SharedMemReader(void)
	{
	}

	void SharedMemReader::Reload( DWORD version)
	{
		m_MLContainer.ReloadMonitorMap(m_monitorListReader,version);
		m_PPQSReaderMap.clear();
		m_ImageContainer.Clear();
		MonitorListCpyMap::const_iterator it = m_MLContainer.m_MonitorListCpyMap.begin();
		for(;it != m_MLContainer.m_MonitorListCpyMap.end();++it )
		{
			const SVString& rPPQName =  it->second->GetPPQname();
			m_PPQSReaderMap[rPPQName] = upSharedPPQReader(new SVSharedPPQReader);
			m_PPQSReaderMap[rPPQName]->Open(rPPQName);
		}
		///Reload m_PPQSReaderMap

		m_ImageContainer.OpenImageStores(m_MLContainer);
		m_ImageContainer.CreateSharedMatroxBuffer(m_MLContainer);
	}
	DWORD SharedMemReader::GetVersion() const
	{
		return m_MLContainer.GetVersion();
	}

	bool SharedMemReader::IsActiveMonitorList(const SVString& Monitorlistname ) const
	{
		return m_MLContainer.IsActiveMonitorList(Monitorlistname);
	}

	upSharedPPQReader&  SharedMemReader::GetPPQReaderPtr(const SVString& ppqname)
	{
		return m_PPQSReaderMap[ppqname];
	}
} //namespace SvSml
