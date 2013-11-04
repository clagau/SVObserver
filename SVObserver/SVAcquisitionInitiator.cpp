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
#include "SVAcquisitionInitiator.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"

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

HRESULT SVAcquisitionInitiator::RegisterCallback(SVCallbackStruct& p_rCallback)
{
	HRESULT hr = S_FALSE;
	if (m_initiatorList.size())
	{
		hr = S_OK;
		SVAcquisitionInitiatorList::iterator it;
		for (it = m_initiatorList.begin(); hr == S_OK && it != m_initiatorList.end(); ++it)
		{
			SVDigitizerLoadLibraryClass* pDLL = it->first;
			const SVAcquisitionInitiatorHandleList& handleList = it->second;
			
			if (handleList.size())
			{
				// register first one or just last one?
				hr = pDLL->InternalTriggerRegister(handleList[0], p_rCallback);
			}
		}
	}
	return hr;
}

HRESULT SVAcquisitionInitiator::UnRegisterCallback(SVCallbackStruct& p_rCallback)
{
	HRESULT hr = S_FALSE;
	if (m_initiatorList.size())
	{
		hr = S_OK;
		SVAcquisitionInitiatorList::iterator it;
		for (it = m_initiatorList.begin(); hr == S_OK && it != m_initiatorList.end(); ++it)
		{
			SVDigitizerLoadLibraryClass* pDLL = it->first;
			const SVAcquisitionInitiatorHandleList& handleList = it->second;
			
			if (handleList.size())
			{
				// register first one or just last one?
				hr = pDLL->InternalTriggerUnregister(handleList[0], p_rCallback);
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
		for (it = m_initiatorList.begin(); hr == S_OK && it != m_initiatorList.end(); ++it)
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
		for (it = m_initiatorList.begin(); hr == S_OK && it != m_initiatorList.end(); ++it)
		{
			SVDigitizerLoadLibraryClass* pDLL = it->first;
			const SVAcquisitionInitiatorHandleList& handleList = it->second;
			SVAcquisitionInitiatorHandleList::const_iterator handleIt;
			for (handleIt = handleList.begin(); hr == S_OK && handleIt != handleList.end(); ++handleIt)
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
		for (it = m_initiatorList.begin(); hr == S_OK && it != m_initiatorList.end(); ++it)
		{
			SVDigitizerLoadLibraryClass* pDLL = it->first;
			const SVAcquisitionInitiatorHandleList& handleList = it->second;
			SVAcquisitionInitiatorHandleList::const_iterator handleIt;
			for (handleIt = handleList.begin(); hr == S_OK && handleIt != handleList.end(); ++handleIt)
			{
				hr = pDLL->InternalTrigger((*handleIt));
			}
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAcquisitionInitiator.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:24:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 14:55:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 11:19:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
