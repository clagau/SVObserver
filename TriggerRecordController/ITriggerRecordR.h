//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ITriggerRecordR.h
/// All Rights Reserved
//*****************************************************************************
/// The interface-class for the TriggerRecord in read-modus.
//******************************************************************************
#pragma once
#include "stdafx.h"
#include "IImage.h"
//Moved to precompiled header: #include <memory>

namespace SvTrc
{
	struct TriggerData
	{
		int m_TriggerId = 0;
	};

	class ITriggerRecordR
	{
	public:
		/// Return the unique id of this trigger record.
		/// \returns int
		virtual int getId() const = 0;

		/// Return the trigger data.
		/// \returns const TriggerData&
		virtual const TriggerData& getTriggerData() const = 0;

		/// Return an pointer to a image instance (only in read only modus). ATTENTION: Use this instance only as long as this TriggerRecord-instance existing.
		/// \param imageId [in] GUID of the image.
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr getImage(const GUID& imageId) const = 0;

		/// Return an pointer to a image instance (only in read only modus). ATTENTION: Use this instance only as long as this TriggerRecord-instance existing.
		/// \param pos [in] Position in this triggerRecord.
		/// \returns SvTrc::IImagePtr
		virtual IImagePtr getImage(int pos) const = 0;
	};

	typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;
} //namespace SvTrc
