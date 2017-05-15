//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionInitiator
//* .File Name       : $Workfile:   SVAcquisitionInitiator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:22:54  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>

#include "TriggerBasics.h"
#include "SVTriggerClass.h"

#pragma endregion Includes

class SVDigitizerLoadLibraryClass;

namespace SvTh
{
	typedef std::vector<unsigned long> SVAcquisitionInitiatorHandleList;
	typedef std::map<SVDigitizerLoadLibraryClass*, SVAcquisitionInitiatorHandleList> SVAcquisitionInitiatorList;

	class SVAcquisitionInitiator
	{
	private:
		SVAcquisitionInitiatorList m_initiatorList;

	public:
		SVAcquisitionInitiator();
		SVAcquisitionInitiator(const SVAcquisitionInitiatorList& initatorList);
	
		void Add(SVDigitizerLoadLibraryClass* pAcqDLL, unsigned long handle);

		HRESULT EnableInternalTrigger();
		HRESULT Exec();
		HRESULT RegisterCallback(const SvTh::TriggerDispatcher& rDispatcher );
		HRESULT UnRegisterCallback(const SvTh::TriggerDispatcher& rDispatcher );
		HRESULT UnRegisterAllCallbacks();
	};
} //namespace SvTh
