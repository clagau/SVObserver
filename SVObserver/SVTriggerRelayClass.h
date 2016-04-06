//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerRelayClass
//* .File Name       : $Workfile:   SVTriggerRelayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:51:54  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#pragma endregion Includes

template<typename TriggerHandler>
class SVTriggerRelayClass
{
private:
	SVIOTriggerLoadLibraryClass* m_pTriggerDLL;
	unsigned long m_ulTriggerHandle;
	TriggerHandler m_triggerHandler;
	
public:
	SVTriggerRelayClass(); 
	~SVTriggerRelayClass();
	
	HRESULT RegisterTriggerRelay(SVIOTriggerLoadLibraryClass* pTriggerDLL, unsigned long ulHandle, TriggerHandler& rTriggerHandler);
	HRESULT UnregisterTriggerRelay();
};

#include "SVTriggerRelayClass.inl"

