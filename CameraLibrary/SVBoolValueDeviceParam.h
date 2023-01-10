//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueDeviceParam
//* .File Name       : $Workfile:   SVBoolValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:59:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDeviceParam.h"
#include "SVDeviceParams.h"
#pragma endregion Includes
#include "SVStatusLibrary/MessageManagerHelper.h"

class SVBoolValueDeviceParam : public SVDeviceParam
{
public:
	SVBoolValueDeviceParam();
	explicit SVBoolValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVBoolValueDeviceParam( const SVBoolValueDeviceParam& rhs );
	virtual SVClonable* CloneImpl() const override;

	// automatic conversions
	explicit SVBoolValueDeviceParam(bool rhs);	// not const bool& so that we get automatic conversion from other integral types for free
	operator bool() const;
	SVBoolValueDeviceParam& operator=(const SVBoolValueDeviceParam& rhs);
	SVBoolValueDeviceParam& operator=(bool bVal);
	virtual HRESULT GetValue(VARIANT& rv) const override;
	virtual HRESULT SetValue(const VARIANT& rv) override;

	bool bValue {false};

	TDeviceParamInfo<bool> info;
	typedef TDeviceOption<bool> OptionType;
	typedef std::vector<OptionType> OptionsType;
	
	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;

	typedef TDeviceParamInfo<bool>::OptionType  OptionType;
	typedef TDeviceParamInfo<bool>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVBoolValueDeviceParam(typeEnum); }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG)
	bool& BoolValue(SVDeviceParamWrapper& w);
	bool BoolValue(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(bool b);
#else
	inline bool& BoolValue(SVDeviceParamWrapper& w) {SVBoolValueDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVBoolValueDeviceParam(); p = w.DerivedValue(p);} Log_Assert(p); return p->bValue;}
	inline bool BoolValue(const SVDeviceParamWrapper& w) {const SVBoolValueDeviceParam* p = w.DerivedValue(p); if (p) return p->bValue; else {Log_Assert(false); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(bool b) {return SVBoolValueDeviceParam(b);}
#endif

