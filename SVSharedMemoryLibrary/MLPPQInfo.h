//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLPPQInfo.h
/// All Rights Reserved 
//*****************************************************************************
/// struct with PPQ informations for MLCpyContainer
//******************************************************************************
#pragma once
#include "SVObserver\SVMonitorList.h"
namespace SvSml
{
	/// struct with PPQ informations for MLCpyContainer
	struct MLPPQInfo
	{
		MLPPQInfo::MLPPQInfo():DataSlotSize(NumProductSlot), RejectSize(0), SlotManagerIndex(-1)
		{; };
		DWORD DataSlotSize;
		DWORD RejectSize;
		DWORD SlotManagerIndex; //Index in m_ManagmentStore
		static  const int NumProductSlot = 32;
		static const  int NumRejectSizeDelta = 5;
	};
	typedef std::unique_ptr<MLPPQInfo> MLPPQInfoPointer;
	typedef  std::map<SVString, MLPPQInfoPointer>  MLPPQInfoMap; //< map PPQName  MLPPQInfoPointer

}

