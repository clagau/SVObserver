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
#ifndef SVBOOLVALUEDEVICEPARAM_H
#define SVBOOLVALUEDEVICEPARAM_H

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
	inline bool& BoolValue(SVDeviceParamWrapper& w) {SVBoolValueDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVBoolValueDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->bValue;}
	inline bool BoolValue(const SVDeviceParamWrapper& w) {const SVBoolValueDeviceParam* p = w.DerivedValue(p); if (p) return p->bValue; else {ASSERT(FALSE); return 0;} }
	inline SVDeviceParamWrapper DeviceParam(bool b) {return SVBoolValueDeviceParam(b);}
#endif

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVBoolValueDeviceParam.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:59:26   bWalter
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

