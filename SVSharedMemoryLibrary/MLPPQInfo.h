//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLPPQInfo.h
/// All Rights Reserved 
//*****************************************************************************
/// struct with PPQ informations for MLCpyContainer
//******************************************************************************
#pragma once

namespace SvSml
{
	/// struct with PPQ informations for MLCpyContainer
	struct MLPPQInfo
	{
		MLPPQInfo() = delete;
		MLPPQInfo(int LastSlot, int RejectSlot)
			:NumLastSlot(LastSlot),
			NumRejectSlot(RejectSlot),
			SlotManagerIndex(-1)
		{
			;
		};
		int NumLastSlot;  //<Number of slots for last products 
		int NumRejectSlot; //<Numbetr of slots for rejects 
		int SlotManagerIndex; //<Index in m_ManagmentStore
	
		static const  int NumRejectSizeDelta = 5;
	};
	typedef std::unique_ptr<MLPPQInfo> MLPPQInfoPointer;
	typedef  std::map<std::string, MLPPQInfoPointer>  MLPPQInfoMap; //< map PPQName  MLPPQInfoPointer

}

