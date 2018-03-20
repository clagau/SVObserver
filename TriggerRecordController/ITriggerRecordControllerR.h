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

namespace SvPb
{
	class ImageList;
	class InspectionList;
}

namespace SvTrc
{
	class ITriggerRecordControllerR
	{
	public:

		/// Get the list of GUID of the available inspections.
		/// \returns SvPb::InspectionList
		virtual const SvPb::InspectionList& getInspections() const = 0;

		/// Get the last TRId. if no trigger record used yet it return -1;
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns int
		virtual int getLastTRId(int inspectionPos) const= 0;

		/// Return a list of the image definitions.
		/// \param inspectionPos [in] ID of the inspection.
		/// \returns const SvPb::ImageList&
		virtual const SvPb::ImageList& GetImageDefList(int inspectionPos) = 0;

		/// Create a trigger record instance from trigger record with trigger record ID, if it exist.
		/// \param inspectionPos [in] ID of the inspection.
		/// \param trId [in] Trigger record ID
		/// \returns SvTrc::ITriggerRecordRPtr 
		virtual ITriggerRecordRPtr CreateTriggerRecordObject(int inspectionPos, int trId) = 0;
	};

	/// Get an instance of the TR controller in read modus.
	/// \returns ITriggerRecordControllerR&
	ITriggerRecordControllerR& getTriggerRecordControllerRInstance();
} // namespace SvTrc
