//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MLProduct.h
/// All Rights Reserved 
//*****************************************************************************
/// class holds result data for getProduct or getReject values 
//******************************************************************************
#pragma once
#pragma region Includes
#include "MonitorEntry.h"
#include "TriggerRecordController\ITriggerRecordR.h"
#pragma endregion Includes

namespace SvSml
{
	typedef std::unique_ptr<std::string>  stringpointer;
	/// struct holds result data for getProduct or getReject values 
	struct MLProduct
	{
		MLProduct();
		void ResetProduct();

		int m_status;
		int m_trigger;
		int m_SlotManagerIndex; //<Datenmanager Index
		int m_slot;
		
		std::vector<MonitorEntryPointer> m_dataEntries;
		std::vector<MonitorEntryPointer> m_ImageEntries;
		std::vector<stringpointer> m_data; //< data values as string 
		std::map<int, SvTrc::ITriggerRecordRPtr> m_triggerRecordMap; //first is monitorId for inspection
	};
	typedef std::unique_ptr<MLProduct> pProd;
	typedef std::vector<pProd>  vecpProd;
}


