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
	inline SVDeviceParamWrapper DeviceParam(const SVString& s) {return SVStringValueDeviceParam(s.ToString());}
	inline SVDeviceParamWrapper DeviceParam(const CString& s) {return SVStringValueDeviceParam(s);}
#endif

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVStringValueDeviceParam.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:14:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Apr 2013 13:37:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor that takes an argument of type SVDeviceParamEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Feb 2011 13:37:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed release build issues with string conversion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Feb 2011 09:49:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 08:51:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin (Moved from SVDeviceParams)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

