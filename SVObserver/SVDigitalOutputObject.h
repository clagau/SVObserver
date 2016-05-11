//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOutputObject.h"
#pragma endregion Includes

class SVDigitalOutputObject : public SVOutputObject
{
public:
	SVDigitalOutputObject( LPCSTR strObjectName );
	SVDigitalOutputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDIGITALOUTPUTOBJECT );

	virtual ~SVDigitalOutputObject();

	virtual BOOL Create();
	virtual BOOL Destroy();

	virtual HRESULT Write( const _variant_t& p_rValue );
	virtual HRESULT Reset();

	BOOL Force( bool bForce, bool bForcedValue );
	BOOL Invert( bool bInvert );
	BOOL Combine( bool bCombine, bool bCombineACK );

	bool IsForced() const;
	bool GetForcedValue() const;
	bool IsInverted() const;
	bool IsCombined() const;
	bool GetCombinedValue() const;
	bool GetValue() const;

	BOOL SetChannel( long lChannel );
	long GetChannel() const;

private:
	// Values for this digital output
	long m_lChannel;
	bool m_bForced;
	bool m_bInverted;
	bool m_bCombined;
	bool m_bLastValue;
	bool m_bDefaultValue;
	bool m_bForcedValue;
	bool m_bCombinedACK;
};
