/// \copyright COPYRIGHT (c) 2017, 2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerInformation.h
/// \brief Contains structs and classes containing information about triggers

#pragma once

#include <stdint.h>

/// Basic information on a trigger: channel, sequence number and the ID of the object it is associated with
struct TriggerInformation
{
	TriggerInformation() = default;
	virtual ~TriggerInformation() = default;
	TriggerInformation(uint8_t Channel, uint32_t ObjectId, int8_t SequenceCode, uint32_t triggerIndex, uint8_t triggersPerProduct, double triggerTimeStamp) : 
		m_Channel(Channel), m_ObjectId(ObjectId), m_SequenceCode(SequenceCode), m_triggerIndex{triggerIndex}, m_triggersPerProduct{triggersPerProduct}, m_triggerTimestamp{triggerTimeStamp}{}
	TriggerInformation(const TriggerInformation&) = default;
	TriggerInformation& operator=(const TriggerInformation&) = default;

	bool isValid() const { return 0 != m_ObjectId; } ///< a TriggerIdentifier  is valid if (and only if) it refers to a product with non-zero object ID

	uint8_t m_Channel {0};
	uint32_t m_ObjectId {0};
	int8_t m_SequenceCode {0};
	uint32_t m_triggerIndex {0};
	uint8_t m_triggersPerProduct {0};  ///< that many triggers should be contained in a complete product
	double m_triggerTimestamp {0.0};
};

///contains evaluation information on a product (comprising one or more triggers) that is required in SVObserver
struct Product : public TriggerInformation
{
	Product(const Product&) = default;
	Product() = default;
	virtual ~Product() = default;
	Product(const TriggerInformation& rTrigger) :
		TriggerInformation(rTrigger)
	{
	}

	Product& operator=(const Product&) = default;

	uint8_t m_InspectionResult {0};
	uint16_t m_outbits {0};
};
