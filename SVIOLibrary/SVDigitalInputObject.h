//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVInputObject.h"
#include "SVOResource/resource.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#pragma endregion Includes

class SVDigitalInputObject : public SVInputObject
{
#pragma region Constructor
public:
	explicit SVDigitalInputObject( LPCSTR strObjectName );
	SVDigitalInputObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDIGITALINPUTOBJECT );

	virtual ~SVDigitalInputObject() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;

	virtual HRESULT Read( _variant_t& rValue ) const override;
	virtual long GetChannel() const override { return m_Channel; }
	virtual SVObjectClass* GetValueObject() override { return &m_ValueObject; }

	bool Force( bool bForce, bool bForcedValue );
	bool Invert( bool bInvert );

	bool IsForced() const;
	bool GetForcedValue() const;
	bool IsInverted() const;
	bool GetValue() const;

	void SetChannel( long channel ) { m_Channel = channel; }
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	long m_Channel{-1L};
	SvVol::SVBoolValueObjectClass m_ValueObject {};
#pragma endregion Member Variables
};
