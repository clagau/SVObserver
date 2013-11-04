//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVi64ValueDeviceParam
//* .File Name       : $Workfile:   SVi64ValueDeviceParam.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:07:02  $
//******************************************************************************
#ifndef SVI64VALUEDEVICEPARAM_H
#define SVI64VALUEDEVICEPARAM_H

#include "SVDeviceParams.h"

class SVi64ValueDeviceParam : public SVDeviceParam
{
public:
	SVi64ValueDeviceParam();
	SVi64ValueDeviceParam(SVDeviceParamEnum typeEnum);
	SVi64ValueDeviceParam(const SVi64ValueDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;

	// automatic conversions
	SVi64ValueDeviceParam(const __int64& rhs);
	SVi64ValueDeviceParam(const LARGE_INTEGER& rhs);
	operator __int64() const;
	SVi64ValueDeviceParam& operator=(const SVi64ValueDeviceParam& rhs);
	virtual HRESULT GetValue(VARIANT& rv) const;
	virtual HRESULT SetValue(const VARIANT& rv);

	__int64 iValue;

	TDeviceParamInfo<__int64> info;
	typedef TDeviceOption<__int64> OptionType;
	typedef std::vector<OptionType> OptionsType;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam);

	typedef TDeviceParamInfo<__int64>::OptionType  OptionType;
	typedef TDeviceParamInfo<__int64>::OptionsType OptionsType;

	static SVDeviceParam* CreateNew() { return new SVi64ValueDeviceParam; }
	DEFINE_VISITABLE()

private:
	void init();
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	__int64& i64Value(SVDeviceParamWrapper& w);
	__int64 i64Value(const SVDeviceParamWrapper& w);
	SVDeviceParamWrapper DeviceParam(__int64 i);
#else
	inline __int64& i64Value(SVDeviceParamWrapper& w) {SVi64ValueDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVi64ValueDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->iValue;}
	inline __int64 i64Value(const SVDeviceParamWrapper& w) {const SVi64ValueDeviceParam* p = w.DerivedValue(p); if (p) return p->iValue; else {ASSERT(FALSE); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(__int64 i) {return SVi64ValueDeviceParam(i);}
#endif

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVi64ValueDeviceParam.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:07:02   bWalter
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

