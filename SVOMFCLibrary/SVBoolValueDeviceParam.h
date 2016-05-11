//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueDeviceParam
//* .File Name       : $Workfile:   SVBoolValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:59:26  $
//******************************************************************************

#pragma once

#include "SVDeviceParams.h"

class SVBoolValueDeviceParam : public SVDeviceParam
{
public:
	SVBoolValueDeviceParam();
	SVBoolValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVBoolValueDeviceParam( const SVBoolValueDeviceParam& rhs );
	virtual SVClonable* CloneImpl() const;

	// automatic conversions
	SVBoolValueDeviceParam(bool rhs);	// not const bool& so that we get automatic conversion from other integral types for free
	operator bool() const;
	SVBoolValueDeviceParam& operator=(const SVBoolValueDeviceParam& rhs);
	SVBoolValueDeviceParam& operator=(bool bVal);
	virtual HRESULT GetValue(VARIANT& rv) const;
	virtual HRESULT SetValue(const VARIANT& rv);

	bool bValue;

	TDeviceParamInfo<bool> info;
	typedef TDeviceOption<bool> OptionType;
	typedef std::vector<OptionType> OptionsType;
	
	virtual HRESULT SetMetadata(const SVDeviceParam* pParam);

	typedef TDeviceParamInfo<bool>::OptionType  OptionType;
	typedef TDeviceParamInfo<bool>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew() { return new SVBoolValueDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	bool& BoolValue(SVDeviceParamWrapper& w);
	bool BoolValue(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(bool b);
#else
	inline bool& BoolValue(SVDeviceParamWrapper& w) {SVBoolValueDeviceParam* p = w.DerivedValue(p); if (nullptr == p) {w = SVBoolValueDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->bValue;}
	inline bool BoolValue(const SVDeviceParamWrapper& w) {const SVBoolValueDeviceParam* p = w.DerivedValue(p); if (p) return p->bValue; else {ASSERT(FALSE); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(bool b) {return SVBoolValueDeviceParam(b);}
#endif

