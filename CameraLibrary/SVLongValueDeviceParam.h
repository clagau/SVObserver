//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueDeviceParam
//* .File Name       : $Workfile:   SVLongValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   03 Dec 2014 19:41:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVDeviceParam.h"
#include "SVDeviceParams.h"
#pragma endregion Includes
#include "SVStatusLibrary/MessageManagerHelper.h"


class SVLongValueDeviceParam : public SVDeviceParam
{
public:
	long GetScaledMax() const;
	long GetScaledMin() const;
	void SetScaledValue(long lvalue);
	long GetScaledValue() const;
	_variant_t GetNormalizedValue() const;
	SVLongValueDeviceParam();
	explicit SVLongValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVLongValueDeviceParam(const SVLongValueDeviceParam& rhs);
	virtual ~SVLongValueDeviceParam();
	virtual SVClonable* CloneImpl() const override;

	// automatic conversions
	explicit SVLongValueDeviceParam(long rhs);	// not const long& so that we get automatic conversion from other integral types for free
	operator long() const;
	SVLongValueDeviceParam& operator=(long lVal);
	SVLongValueDeviceParam& operator=(const SVLongValueDeviceParam& rhs);
	virtual HRESULT GetValue(VARIANT& rv) const override;
	virtual HRESULT SetValue(const VARIANT& rv) override;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam) override;
	long lValue {0L};

	TDeviceParamInfo<long> info;
	typedef TDeviceParamInfo<long>::OptionType  OptionType;
	typedef TDeviceParamInfo<long>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew(SVDeviceParamEnum typeEnum) { return new SVLongValueDeviceParam(typeEnum); }
	DEFINE_VISITABLE()

private:
};


#if defined(_DEBUG)
	long& LongValue(SVDeviceParamWrapper& w);
	long LongValue(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(long l);
	SVDeviceParamWrapper DeviceParam(int i);
	SVDeviceParamWrapper DeviceParam(short i);
#else
	inline long& LongValue(SVDeviceParamWrapper& w) {SVLongValueDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVLongValueDeviceParam(); p = w.DerivedValue(p);} Log_Assert(p); return p->lValue;}
	inline long LongValue(const SVDeviceParamWrapper& w) {const SVLongValueDeviceParam* p = w.DerivedValue(p); if (p) return p->lValue; else {Log_Assert(false); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(long l) {return SVLongValueDeviceParam(l);}	// get conversions for free
	inline SVDeviceParamWrapper DeviceParam(int i) {return SVLongValueDeviceParam(i);}
	inline SVDeviceParamWrapper DeviceParam(short i) {return SVLongValueDeviceParam(i);}
#endif


