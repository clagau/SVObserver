//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeParameterAccessor
//* .File Name       : $Workfile:   SVGigeParameterAccessor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:57:06  $
//******************************************************************************
#ifndef SVGIGEPARAMETERACCESSOR_H
#define SVGIGEPARAMETERACCESSOR_H

#include <comdef.h>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVGigeFeature.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"

struct SVGigeParameterAccessor
{
	typedef boost::function<HRESULT(SVMatroxDigitizerRef, const SVGigeFeature& rFeature, _variant_t&)> ParamGetter;
	typedef boost::function<HRESULT(SVMatroxDigitizerRef, const SVGigeFeature& rFeature, const _variant_t&)> ParamSetter;

	ParamGetter GetParam;
	ParamSetter SetParam;

	SVGigeFeature feature;

	// This Method uses the default Setter and getter
	SVGigeParameterAccessor(const SVGigeFeature& rFeature);

	// This Method is for overriding Setter and Getter
	SVGigeParameterAccessor(const SVGigeFeature& rFeature, const ParamGetter& getterFunc, const ParamSetter& setterFunc);
	
	SVGigeParameterAccessor(const SVGigeParameterAccessor& rAccessor);
	SVGigeParameterAccessor& operator=(const SVGigeParameterAccessor& rAccessor);

	// default Getter/Setter
	static HRESULT GetFeature(SVMatroxDigitizerRef digitizer, const SVGigeFeature& feature, _variant_t& value);
	static HRESULT SetFeature(SVMatroxDigitizerRef digitizer, const SVGigeFeature& feature, const _variant_t& value);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeParameterAccessor.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:57:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2012 09:01:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed extra whitespace.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:32:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:06:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
