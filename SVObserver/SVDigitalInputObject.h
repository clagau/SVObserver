//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVInputObject.h"
#pragma endregion Includes

class SVDigitalInputObject : public SVInputObject
{
public:
	SVDigitalInputObject( LPCSTR strObjectName );
	SVDigitalInputObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDIGITALINPUTOBJECT );

	virtual ~SVDigitalInputObject();

	virtual BOOL Create();
	virtual BOOL Destroy();

	virtual HRESULT Read( _variant_t& p_rValue );

	BOOL Force( bool bForce, bool bForcedValue );
	BOOL Invert( bool bInvert );

	bool IsForced() const;
	bool GetForcedValue() const;
	bool IsInverted() const;
	bool GetValue() const;

	BOOL SetChannel( long lChannel );
	long GetChannel() const;

private:
	// Values for this digital input
	long m_lChannel;
	bool m_bForced;
	bool m_bInverted;
	bool m_bLastValue;
	bool m_bDefaultValue;
	bool m_bForcedValue;
};
