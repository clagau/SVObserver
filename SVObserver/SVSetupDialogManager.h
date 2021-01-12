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

#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

namespace SvPb
{
	enum ClassIdEnum;
}

class SVSetupDialogManager
{
public:
	static SVSetupDialogManager& Instance();

	virtual ~SVSetupDialogManager();

	HRESULT SetupDialog(SvPb::ClassIdEnum classId, uint32_t objectId, CWnd* pParentWnd );

protected:
	typedef boost::function<HRESULT (uint32_t, CWnd*)> SVSetupDialogFunction;
	typedef std::unordered_map< SvPb::ClassIdEnum, SVSetupDialogFunction > SVSetupDialogMap;

	SVSetupDialogManager();

	static HRESULT SVBarCodeAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVBlobAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT BlobAnalyzer2SetupDialog(uint32_t objectId, CWnd* pParentWnd);
	static HRESULT SVColorToolClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVHistogramAnalyzerSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVLinearAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVLuminanceAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVOCVAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVPatternAnalyzerClassSetupDialog( uint32_t objectId, CWnd* pParentWnd );
	static HRESULT SVPixelAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVResultClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );

	SVSetupDialogMap m_SetupDialogs;

private:
	SVSetupDialogManager( const SVSetupDialogManager& p_rObject );
	const SVSetupDialogManager& operator=( const SVSetupDialogManager& p_rObject );
};

