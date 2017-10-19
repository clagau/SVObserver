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
#pragma region Constructor
public:
	SVDigitalInputObject( LPCSTR strObjectName );
	SVDigitalInputObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDIGITALINPUTOBJECT );

	virtual ~SVDigitalInputObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual bool Create() override;
	virtual bool Destroy() override;

	virtual HRESULT Read( _variant_t& p_rValue ) override;

	bool Force( bool bForce, bool bForcedValue );
	bool Invert( bool bInvert );

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
	//! Note that these member variables are mirrored in SVIODigitalStruct
	//! Take care that these stay synchronous
	//@WARNING [gra][7.40][07.11.2016] Try to remove the mirrored values in SVIODigitalStruct and keep only one copy of the data
	// Values for this digital input
	long m_lChannel;
	bool m_bForced;
	bool m_bInverted;
	bool m_bLastValue;
	bool m_bDefaultValue;
	bool m_bForcedValue;
#pragma endregion Member Variables
};
