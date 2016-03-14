//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueDeviceParam
//* .File Name       : $Workfile:   SVStringValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:14:34  $
//******************************************************************************
#ifndef SVSTRINGVALUEDEVICEPARAM_H
#define SVSTRINGVALUEDEVICEPARAM_H

#include "SVDeviceParams.h"

class SVStringValueDeviceParam : public SVDeviceParam
{
public:
	SVStringValueDeviceParam();
	SVStringValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVStringValueDeviceParam(const SVStringValueDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;

	// automatic conversions
	SVStringValueDeviceParam(const CString& rhs);
	operator CString() const;
	SVStringValueDeviceParam& operator=(const SVStringValueDeviceParam& rhs);
	SVStringValueDeviceParam& operator=(const SVString& sVal);
	virtual HRESULT GetValue(VARIANT& rv) const;
	virtual HRESULT SetValue(const VARIANT& rv);

	SVString strValue;

	TDeviceParamInfo<SVString> info;
	
	virtual HRESULT SetMetadata(const SVDeviceParam* pParam );

	typedef TDeviceParamInfo<SVString>::OptionType  OptionType;
	typedef TDeviceParamInfo<SVString>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew() { return new SVStringValueDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVString& StringValue(SVDeviceParamWrapper& w);
	const SVString& StringValue(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(const SVString& s);
	SVDeviceParamWrapper DeviceParam(const CString& s);
#else
	inline SVString& StringValue(SVDeviceParamWrapper& w) {SVStringValueDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVStringValueDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->strValue;}
	inline const SVString& StringValue(const SVDeviceParamWrapper& w) {const SVStringValueDeviceParam* p = w.DerivedValue(p); if (p) return p->strValue; else {ASSERT(FALSE); static SVString s(""); return s;} }
	inline SVDeviceParamWrapper DeviceParam(const SVString& s) {return SVStringValueDeviceParam(s.c_str());}
	inline SVDeviceParamWrapper DeviceParam(const CString& s) {return SVStringValueDeviceParam(s);}
#endif

#endif

