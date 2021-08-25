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
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

namespace SvPb
{
	enum ClassIdEnum;
}

class SVSetupDialogManager
{
public:
	SVSetupDialogManager(const SVSetupDialogManager&) = delete;
	SVSetupDialogManager(SVSetupDialogManager&&) = delete;
	virtual ~SVSetupDialogManager() = default;

	SVSetupDialogManager& operator=(const SVSetupDialogManager&) = delete;
	SVSetupDialogManager& operator=(SVSetupDialogManager&&) = delete;

	static SVSetupDialogManager& Instance();

	HRESULT SetupDialog(SvPb::ClassIdEnum classId, uint32_t objectId, CWnd* pParentWnd );


protected:
	SVSetupDialogManager() = default;

	static HRESULT SVBarCodeAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVBlobAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT BlobAnalyzer2SetupDialog(uint32_t objectId, CWnd* pParentWnd);
	static HRESULT SVColorToolClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT HistogramAnalyzerSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVLinearAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVLuminanceAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVOCVAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVPatternAnalyzerClassSetupDialog( uint32_t objectId, CWnd* pParentWnd );
	static HRESULT PixelAnalyzerClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );
	static HRESULT SVResultClassSetupDialog( uint32_t objectId, CWnd* PParentWnd );

private:

	using SVSetupDialogFunction = std::function<HRESULT(uint32_t, CWnd*)>;

	std::array<std::pair<SvPb::ClassIdEnum, SVSetupDialogFunction>, 25> m_SetupDialogs
	{
		std::make_pair(SvPb::LuminanceAnalyzerClassId, &SVSetupDialogManager::SVLuminanceAnalyzerClassSetupDialog),
		std::make_pair(SvPb::BarCodeAnalyzerClassId, &SVSetupDialogManager::SVBarCodeAnalyzerClassSetupDialog),
		std::make_pair(SvPb::BlobAnalyzerClassId, &SVSetupDialogManager::SVBlobAnalyzerClassSetupDialog),
		std::make_pair(SvPb::BlobAnalyzer2ClassId, &SVSetupDialogManager::BlobAnalyzer2SetupDialog),
		std::make_pair(SvPb::ColorToolClassId, &SVSetupDialogManager::SVColorToolClassSetupDialog),
		std::make_pair(SvPb::DPointXResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::DPointYResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::HistogramAnalyzerClassId, &SVSetupDialogManager::HistogramAnalyzerSetupDialog),
		std::make_pair(SvPb::LinearEdgeCountingLineAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::LinearEdgePositionLineAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::LinearMaximumBackgroundObjectLineAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::LinearMaximumForegroundObjectLineAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::LinearMaximumObjectLineAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::LinearPixelCountingLineAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::LinearMeasurementAnalyzerClassId, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog),
		std::make_pair(SvPb::OCVAnalyzerClassId, &SVSetupDialogManager::SVOCVAnalyzerClassSetupDialog),
		std::make_pair(SvPb::OCVAnalyzerResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::PatternAnalyzerClassId, &SVSetupDialogManager::SVPatternAnalyzerClassSetupDialog),
		std::make_pair(SvPb::PixelAnalyzerClassId, &SVSetupDialogManager::PixelAnalyzerClassSetupDialog),
		std::make_pair(SvPb::DoubleResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::LongResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::PointXResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::PointYResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::StringResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog),
		std::make_pair(SvPb::VariantResultClassId, &SVSetupDialogManager::SVResultClassSetupDialog)
	};
};

