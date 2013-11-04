//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSetupDialogManager
//* .File Name       : $Workfile:   SVSetupDialogManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:02:58  $
//******************************************************************************

#ifndef SVSETUPDIALOGMANAGER_H
#define SVSETUPDIALOGMANAGER_H

#include <map>
#include <boost/config.hpp>
#include <boost/function.hpp>

#include "SVUtilityLibrary/SVGUID.h"

class SVSetupDialogManager
{
public:
	static SVSetupDialogManager& Instance();

	virtual ~SVSetupDialogManager();

	HRESULT SetupDialog( const SVGUID& p_rClassId, const SVGUID& p_rObjectId, CWnd* PParentWnd );

protected:
	typedef boost::function<HRESULT (const SVGUID&, CWnd*)> SVSetupDialogFunction;
	typedef std::map< SVGUID, SVSetupDialogFunction > SVSetupDialogMap;

	SVSetupDialogManager();

	static HRESULT SVNotImplemented( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVBarCodeAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVBlobAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVColorToolClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVEdgeCountingLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVEdgePositionLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVHistogramAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVLinearAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVLinearMeasurementLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVLineROIClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVLuminanceAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVMaximumBackgroundObjectLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVMaximumForegroundObjectLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVMaximumObjectLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVOCRAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVOCRGrayAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVOCVAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVPatternAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* pParentWnd );
	static HRESULT SVPixelAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVPixelCountingLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVResultClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );

	SVSetupDialogMap m_SetupDialogs;

private:
	SVSetupDialogManager( const SVSetupDialogManager& p_rObject );
	const SVSetupDialogManager& operator=( const SVSetupDialogManager& p_rObject );

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSetupDialogManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:02:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 17:35:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
