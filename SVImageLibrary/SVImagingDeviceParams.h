// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImagingDeviceParams
// * .File Name       : $Workfile:   SVImagingDeviceParams.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:51:32  $
// ******************************************************************************

#ifndef __SVIMAGINGDEVICEPARAMS_H__
#define __SVIMAGINGDEVICEPARAMS_H__

#include <map>
#include "SVLut.h"
#include "SVLightReference.h"
#include "SVCameraFormat.h"

#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"

// for now, still need to update the enum in SVDeviceParams.h

class SVLutDeviceParam : public SVDeviceParam
{
public:
	SVLutDeviceParam();
	SVLutDeviceParam(SVDeviceParamEnum typeEnum);
	SVLutDeviceParam(const SVLutDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	SVLutDeviceParam& operator = (const SVLutDeviceParam& rhs);

	SVLut lut;

	static SVDeviceParam* CreateNew() { return new SVLutDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVLut& Lut(SVDeviceParamWrapper& w);
#else
	inline SVLut& Lut(SVDeviceParamWrapper& w) {SVLutDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVLutDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->lut;}
#endif

class SVLightReferenceDeviceParam : public SVDeviceParam
{
public:
	SVLightReferenceDeviceParam();
	SVLightReferenceDeviceParam(SVDeviceParamEnum typeEnum);
	SVLightReferenceDeviceParam( const SVLightReferenceDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	SVLightReferenceDeviceParam& operator = (const SVLightReferenceDeviceParam& rhs);

	SVLightReference lr;

	static SVDeviceParam* CreateNew() { return new SVLightReferenceDeviceParam; }
	DEFINE_VISITABLE()
};

#if defined(_DEBUG) || defined (_MINDEBUG)
	SVLightReference& LR(SVDeviceParamWrapper& w);
#else
	inline SVLightReference& LR(SVDeviceParamWrapper& w) {SVLightReferenceDeviceParam* p = w.DerivedValue(p); if (p==NULL) {w = SVLightReferenceDeviceParam(); p = w.DerivedValue(p);} ASSERT(p); return p->lr;}
#endif


class SVCameraFormatsDeviceParam : public SVStringValueDeviceParam
{
public:
	SVCameraFormatsDeviceParam();
	SVCameraFormatsDeviceParam(SVDeviceParamEnum typeEnum);
	SVCameraFormatsDeviceParam(const SVCameraFormatsDeviceParam& rhs);
	virtual SVClonable* CloneImpl() const;
	const SVCameraFormatsDeviceParam& operator=(const SVCameraFormatsDeviceParam& rhs);
	bool SupportsColor() const;

	typedef std::map<SVString, SVCameraFormat> OptionsType;
	OptionsType options;

	virtual HRESULT SetMetadata(const SVDeviceParam* pParam);

	static SVDeviceParam* CreateNew() { return new SVCameraFormatsDeviceParam; }
	DEFINE_VISITABLE()
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImagingDeviceParams.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Apr 2013 13:48:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor(s) that take an arguemnt of type SVDeviceParamEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Feb 2011 09:38:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Jun 2009 09:50:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Jun 2007 11:09:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Aug 2005 13:15:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SetMetadata
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Aug 2005 13:15:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SetMetadata
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Nov 2003 09:23:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SupportsColor function to the CameraFormats param
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Aug 2003 10:02:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DEFINE_VISITABLE()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:06:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   split off from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:02:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   split off from SVDeviceParams in SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
