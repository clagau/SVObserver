//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueDeviceParam
//* .File Name       : $Workfile:   SVLongValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   03 Dec 2014 19:41:32  $
//******************************************************************************
#ifndef SVLONGVALUEDEVICEPARAM_H
#define SVLONGVALUEDEVICEPARAM_H

#include "SVDeviceParams.h"

class SVLongValueDeviceParam : public SVDeviceParam
{
public:
	long GetScaledMax() const;
	long GetScaledMin() const;
	void SetScaledValue(long lvalue);
	long GetScaledValue() const;
	_variant_t GetNormalizedValue() const;
	SVLongValueDeviceParam();
	SVLongValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVLongValueDeviceParam(const SVLongValueDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;

	// automatic conversions
	SVLongValueDeviceParam(long rhs);	// not const long& so that we get automatic conversion from other integral types for free
	operator long() const;
	SVLongValueDeviceParam& operator=(long lVal);
	SVLongValueDeviceParam& operator=(const SVLongValueDeviceParam& rhs);
	virtual HRESULT GetValue(VARIANT& rv) const;
	virtual HRESULT SetValue(const VARIANT& rv);

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam);
	long lValue;

	TDeviceParamInfo<long> info;
	typedef TDeviceParamInfo<long>::OptionType  OptionType;
	typedef TDeviceParamInfo<long>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew() { return new SVLongValueDeviceParam; }
	DEFINE_VISITABLE()

private:
	void init();
};


#if defined(_DEBUG) || defined(_MINDEBUG)
	long& LongValue(SVDeviceParamWrapper& w);
	long LongValue(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(long l);
	SVDeviceParamWrapper DeviceParam(int i);
	SVDeviceParamWrapper DeviceParam(short i);
#else
	inline long& LongValue(SVDeviceParamWrapper& w) {SVLongValueDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVLongValueDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->lValue;}
	inline long LongValue(const SVDeviceParamWrapper& w) {const SVLongValueDeviceParam* p = w.DerivedValue(p); if (p) return p->lValue; else {ASSERT(FALSE); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(long l) {return SVLongValueDeviceParam(l);}	// get conversions for free
	inline SVDeviceParamWrapper DeviceParam(int i) {return SVLongValueDeviceParam(i);}
	inline SVDeviceParamWrapper DeviceParam(short i) {return SVLongValueDeviceParam(i);}
#endif

#endif

