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
#pragma region Constructor
public:
	SVDigitalOutputObject( LPCSTR strObjectName );
	SVDigitalOutputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDIGITALOUTPUTOBJECT );

	virtual ~SVDigitalOutputObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool Create() override;
	virtual bool Destroy() override;

	virtual HRESULT Write( const _variant_t& rValue ) override;
	virtual HRESULT Reset() override;
	virtual bool IsCombined() const override;
	virtual bool GetCombinedValue() const override;

	bool Force( bool bForce, bool bForcedValue );
	bool Invert( bool bInvert );
	void Combine( bool bCombine, bool bCombineACK );

	bool IsForced() const;
	bool GetForcedValue() const;
	bool IsInverted() const;
	bool GetValue() const;

	void SetChannel( long lChannel );
	long GetChannel() const;

	/// Update the GUID to a fix GUID depend of a position (must between 0 and 0x100).
	/// \param position [in]
	void updateGuid(int position);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
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
#pragma endregion Member Variables
};
