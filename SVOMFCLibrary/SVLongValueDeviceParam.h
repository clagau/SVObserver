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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOMFCLibrary\SVLongValueDeviceParam.h_v  $
 * 
 *    Rev 1.1   03 Dec 2014 19:41:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  974
 * SCR Title:  Revise Gain handling for Baumer cameras (SVO-401)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetNormalizedValue method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:07:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Apr 2013 13:37:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor that takes an argument of type SVDeviceParamEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 08:50:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin (Moved from SVDeviceParams)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

