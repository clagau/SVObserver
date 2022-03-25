//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MLProduct.h
/// All Rights Reserved 
//*****************************************************************************
/// class holds result data for getProduct or getReject values 
//******************************************************************************
#pragma once
#pragma region Includes
#include "MonitorEntry.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#pragma endregion Includes

namespace SvSml
{
	
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
		std::vector<std::unique_ptr<_variant_t>> m_dataV; //< data values as Variant 
		std::map<int, SvOi::ITriggerRecordRPtr> m_triggerRecordMap; //first is monitorId for inspection
	};
	typedef std::unique_ptr<MLProduct> pProd;
	typedef std::vector<pProd>  vecpProd;
}


