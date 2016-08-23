//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionInitiator
//* .File Name       : $Workfile:   SVAcquisitionInitiator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:24:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVAcquisitionInitiator.h"

namespace Seidenader { namespace TriggerHandling {

	SVAcquisitionInitiator::SVAcquisitionInitiator()
	{
	}

	SVAcquisitionInitiator::SVAcquisitionInitiator(const SVAcquisitionInitiatorList& initiatorList)
	: m_initiatorList(initiatorList)
	{
	}

	void SVAcquisitionInitiator::Add(SVDigitizerLoadLibraryClass* pAcqDLL, unsigned long handle)
	{
		// check if Digitizer Acquisition DLL is present
		SVAcquisitionInitiatorList::iterator it = m_initiatorList.find(pAcqDLL);
		if (it != m_initiatorList.end())
		{
			// find the handle in the list
			SVAcquisitionInitiatorHandleList& handleList = it->second;
			SVAcquisitionInitiatorHandleList::iterator handleIt;
			bool bFound = false;
			for (handleIt = handleList.begin(); !bFound && handleIt != handleList.end(); ++handleIt)
			{
				if ((*handleIt) == handle)
				{
					bFound = true;
				}
			}
			if (!bFound)
			{
				handleList.push_back(handle);
			}
		}
		else
		{
			SVAcquisitionInitiatorHandleList handleList;
			handleList.push_back(handle);
			m_initiatorList.insert(std::make_pair(pAcqDLL, handleList));
		}
	}

	HRESULT SVAcquisitionInitiator::RegisterCallback(const SvTh::TriggerDispatcher& rDispatcher)
	{
		HRESULT hr = S_FALSE;
		if (m_initiatorList.size())
		{
			hr = S_OK;
			SVAcquisitionInitiatorList::iterator it;
			for (it = m_initiatorList.begin(); S_OK == hr && it != m_initiatorList.end(); ++it)
			{
				SVDigitizerLoadLibraryClass* pDLL = it->first;
				const SVAcquisitionInitiatorHandleList& handleList = it->second;
			
				if (handleList.size())
				{
					// register first one or just last one?
					hr = pDLL->InternalTriggerRegister(handleList[0], rDispatcher);
				}
			}
		}
		return hr;
	}

	HRESULT SVAcquisitionInitiator::UnRegisterCallback(const SvTh::TriggerDispatcher& rDispatcher)
	{
		HRESULT hr = S_FALSE;
		if (m_initiatorList.size())
		{
			hr = S_OK;
			SVAcquisitionInitiatorList::iterator it;
			for (it = m_initiatorList.begin(); S_OK == hr && it != m_initiatorList.end(); ++it)
			{
				SVDigitizerLoadLibraryClass* pDLL = it->first;
				const SVAcquisitionInitiatorHandleList& handleList = it->second;
			
				if (handleList.size())
				{
					// register first one or just last one?
					hr = pDLL->InternalTriggerUnregister(handleList[0], rDispatcher);
				}
			}
		}
		return hr;
	}

	HRESULT SVAcquisitionInitiator::UnRegisterAllCallbacks()
	{
		HRESULT hr = S_FALSE;
		if (m_initiatorList.size())
		{
			hr = S_OK;
			SVAcquisitionInitiatorList::iterator it;
			for (it = m_initiatorList.begin(); S_OK == hr && it != m_initiatorList.end(); ++it)
			{
				SVDigitizerLoadLibraryClass* pDLL = it->first;
				const SVAcquisitionInitiatorHandleList& handleList = it->second;
			
				if (handleList.size())
				{
					hr = pDLL->InternalTriggerUnregisterAll(handleList[0]);
				}
			}
		}
		return hr;
	}

	HRESULT SVAcquisitionInitiator::EnableInternalTrigger()
	{
		HRESULT hr = S_FALSE;
		if (m_initiatorList.size())
		{
			hr = S_OK;
			SVAcquisitionInitiatorList::iterator it;
			for (it = m_initiatorList.begin(); S_OK == hr && it != m_initiatorList.end(); ++it)
			{
				SVDigitizerLoadLibraryClass* pDLL = it->first;
				const SVAcquisitionInitiatorHandleList& handleList = it->second;
				SVAcquisitionInitiatorHandleList::const_iterator handleIt;
				for (handleIt = handleList.begin(); S_OK == hr && handleIt != handleList.end(); ++handleIt)
				{
					hr = pDLL->InternalTriggerEnable((*handleIt));
				}
			}
		}
		return hr;
	}

	HRESULT SVAcquisitionInitiator::Exec()
	{
		HRESULT hr = S_FALSE;
		if (m_initiatorList.size())
		{
			hr = S_OK;
			SVAcquisitionInitiatorList::iterator it;
			for (it = m_initiatorList.begin(); S_OK == hr && it != m_initiatorList.end(); ++it)
			{
				SVDigitizerLoadLibraryClass* pDLL = it->first;
				const SVAcquisitionInitiatorHandleList& handleList = it->second;
				SVAcquisitionInitiatorHandleList::const_iterator handleIt;
				for (handleIt = handleList.begin(); S_OK == hr && handleIt != handleList.end(); ++handleIt)
				{
					hr = pDLL->InternalTrigger((*handleIt));
				}
			}
		}
		return hr;
	}

} /* namespace TriggerHandling */ } /* namespace Seidenader */
