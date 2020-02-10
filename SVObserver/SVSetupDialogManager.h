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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>

#include "SVProtobuf/SVO-Enum.h"
#pragma endregion Includes

namespace SvPb
{
	enum ClassIdEnum;
}
class SVGUID;

class SVSetupDialogManager
{
public:
	static SVSetupDialogManager& Instance();

	virtual ~SVSetupDialogManager();

	HRESULT SetupDialog(SvPb::ClassIdEnum classId, const SVGUID& rObjectId, CWnd* pParentWnd );

protected:
	typedef boost::function<HRESULT (const SVGUID&, CWnd*)> SVSetupDialogFunction;
	typedef std::map< SvPb::ClassIdEnum, SVSetupDialogFunction > SVSetupDialogMap;

	SVSetupDialogManager();

	static HRESULT SVBarCodeAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVBlobAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVColorToolClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVHistogramAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVLinearAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVLuminanceAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVOCVAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVPatternAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* pParentWnd );
	static HRESULT SVPixelAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );
	static HRESULT SVResultClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd );

	SVSetupDialogMap m_SetupDialogs;

private:
	SVSetupDialogManager( const SVSetupDialogManager& p_rObject );
	const SVSetupDialogManager& operator=( const SVSetupDialogManager& p_rObject );
};

