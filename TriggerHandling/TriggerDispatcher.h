//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .File Name       : $Workfile:   TriggerDispatcher.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:36  $
//******************************************************************************

#pragma once

namespace SvTh
{
enum TriggerDataEnum
{
	TimeStamp = 1,
	LineState,
	StartFrameTime,
	ObjectID,
	TriggerIndex,
	TriggerPerObjectID,
	OutputData,
};

//! This struct was introduced to facilitate the passing of further parameters 
//! (beyond 'Owner' and 'Data') - in particular in preparation for the SVPLCIO.dll.
typedef  std::map<int, _variant_t> IntVariantMap;

struct TriggerParameters
{
	void* m_pOwner{nullptr};
	IntVariantMap m_Data;
	explicit TriggerParameters(void* pOwner = nullptr) : m_pOwner(pOwner) {}
};

typedef HRESULT(CALLBACK *SVTriggerCallbackPtr)(const TriggerParameters& rTriggerData);

//! this adds a callback function pointer and further informational variables
//! (currently just one) to the information contained in TriggerParameters
//! Note: assignment and comparison operators ('=' and '=='), which had been implemented previously,
//! were removed because the defaults for both TriggerDispatcher and m_TriggerParameters seemed 
//! better (and safer)
class TriggerDispatcher
{
public:
	explicit TriggerDispatcher(const SVTriggerCallbackPtr callback, const TriggerParameters &tp);
	explicit TriggerDispatcher(const TriggerDispatcher& rRhs) = default;

	TriggerDispatcher& operator=(const TriggerDispatcher& rRhs) = default;

	bool operator==(const TriggerDispatcher &rRhs) const;

	virtual ~TriggerDispatcher() {}

	void clear();
	void DispatchIfPossible() const;
	void Dispatch() const;

	bool hasCallback() const { return (nullptr != m_pCallback); }

	const SVTriggerCallbackPtr getCallback() const { return m_pCallback; }
	const TriggerParameters &GetTriggerParameters() const { return m_TriggerParameters; }

	void SetData(const IntVariantMap& rData) { m_TriggerParameters.m_Data = rData; }

	bool m_IsStarted;

private:

	SVTriggerCallbackPtr m_pCallback;
	TriggerParameters m_TriggerParameters;
};
} //namespace SvTh
