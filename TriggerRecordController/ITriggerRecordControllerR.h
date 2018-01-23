//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordControllerR.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecordController in read-modus.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ITriggerRecordR.h"
#pragma endregion Includes

namespace SvPB
{
	class ImageList;
}

namespace SvTRC
{
	class ITriggerRecordControllerR
	{
	public:
		/// Get the last TRId. if no trigger record used yet it return -1;
		/// \returns int
		virtual int getLastTRId() = 0;

		/// Return a list of the image definitions.
		/// \returns const SvPB::ImageList&
		virtual const SvPB::ImageList& GetImageDefList() = 0;

		/// Create a trigger record instance from trigger record with trigger record ID, if it exist.
		/// \param trId [in] Trigger record ID
		/// \returns SvTRC::ITriggerRecordRPtr 
		virtual ITriggerRecordRPtr CreateTriggerRecordObject(int trId) = 0;
	};

	/// Get an instance of the TR controller in read modus.
	/// \returns ITriggerRecordControllerR&
	ITriggerRecordControllerR& getTriggerRecordControllerRInstance();
} // namespace SvTRC
