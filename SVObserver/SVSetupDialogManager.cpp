//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSetupDialogManager
//* .File Name       : $Workfile:   SVSetupDialogManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Dec 2014 15:03:10  $
//******************************************************************************

#pragma region Inlcudes
#include "stdafx.h"
#include "SVSetupDialogManager.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "AnalyzerOperators/SVAnalyzer.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#include "SVOGui/SVLuminanceAnalyzerDlg.h"
#include "SVOGui/SVBarCodeProperties.h"
#include "AnalyzerOperators/SVBarCodeAnalyzerClass.h"
#include "Operators/SVBarCodeResult.h"
#include "AnalyzerOperators/SVBlobAnalyzer.h"
#include "SVOGui/SVBlobAnalyzerDialog.h"
#include "Operators/SVColorThreshold.h"
#include "Tools/SVColorTool.h"
#include "AnalyzerOperators/SVHistogramAnalyzer.h"
#include "SVHistogramAnalyzerSetup.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "AnalyzerOperators/SVLinearAnalyzerClass.h"
#include "Operators/SVLinearEdgeProcessingClass.h"
#include "SVMeasureAnalyzerAdjustmentSheet.h"
#include "AnalyzerOperators/SVOCVAnalyzer.h"
#include "Operators/SVOCVAnalyzerResult.h"
#include "SVOGui/SVOCVSheet.h"
#include "SVPatAdvancedPageClass.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVPatGeneralPageClass.h"
#include "SVOGui/SVPatternAnalyzerModelPage.h"
#include "AnalyzerOperators/SVPatternAnalyzerClass.h"
#include "AnalyzerOperators/SVPixelAnalyzer.h"
#include "SVPixelAnalyzerDlg.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "Operators/SVResult.h"
#include "SVTADlgColorThresholdSheet.h"
#include "Tools/SVTool.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "RangeXDialogClass.h"
#include "Operators/SVRange.h"
#include "SVToolSet.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Inlcudes

SVSetupDialogManager& SVSetupDialogManager::Instance()
{
	static SVSetupDialogManager l_Object;

	return l_Object;
}

SVSetupDialogManager::~SVSetupDialogManager()
{
}

HRESULT SVSetupDialogManager::SetupDialog(const SVGUID& rClassId, const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SVSetupDialogMap::iterator l_Iter = m_SetupDialogs.find(rClassId);

	if (l_Iter != m_SetupDialogs.end())
	{
		l_Status = (l_Iter->second)(rObjectId, pParentWnd);
	}
	else
	{
		l_Status = E_NOTIMPL;
	}

	return l_Status;
}

SVSetupDialogManager::SVSetupDialogManager()
	: m_SetupDialogs {
		{SVAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVImageAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLuminanceAnalyzerClassGuid, &SVSetupDialogManager::SVLuminanceAnalyzerClassSetupDialog},
		{SVArchiveToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVAutoThresholdEquationClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVBarCodeAnalyzerClassGuid, &SVSetupDialogManager::SVBarCodeAnalyzerClassSetupDialog},
		{SVBarCodeResultClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVBlobAnalyzerClassGuid, &SVSetupDialogManager::SVBlobAnalyzerClassSetupDialog},
		{SVBoolValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVByteValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVCharValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVColorThresholdClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVColorToolClassGuid, &SVSetupDialogManager::SVColorToolClassSetupDialog},
		{SVConditionalClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVCustomFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVCylindricalWarpToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVDataBufferClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVDoubleValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVDPointValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVDPointXResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVDPointYResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVDWordValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEnumerateValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEquationClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateTranslationXClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateTranslationYClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateRotationXClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateRotationYClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateRotationAngleClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateCenterXClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateCenterYClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateStartRadiusClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateEndRadiusClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateStartAngleClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateEndAngleClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateBetaClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateXClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEvaluateYClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVExternalToolGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVExternalToolTaskGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVFileNameValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVHistogramAnalyzerClassGuid, &SVSetupDialogManager::SVHistogramAnalyzerClassSetupDialog},
		{SVImageArithmeticClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVImageClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVImagePolarTransformClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVImageTransformClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVInspectionProcessGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVInt64ValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVIOControllerGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLinearAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearEdgeAProcessingClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLinearEdgeBProcessingClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLinearEdgeProcessingClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLinearImageOperatorListClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLinearEdgeCountingLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearEdgePositionLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearMaximumForegroundObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearMaximumObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearPixelCountingLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearMeasurementAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog},
		{SVLinearToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLongValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLowerThresholdEquationClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLUTEquationClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLUTOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVMathContainerClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVMathEquationClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVMathToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVObjectAppClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVOCVAnalyzerClassGuid, &SVSetupDialogManager::SVOCVAnalyzerClassSetupDialog},
		{SVOCVAnalyzerResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVOCVCharacterResultClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVBinaryOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVUnaryImageOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVThresholdClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVSmoothFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVSharpenFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVSharpen2FilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVHorizEdgeFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVVertEdgeFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEdgeDetectFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVEdgeDetect2FilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLaplacianEdgeFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLaplacianEdge2FilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVMaxMinusMinFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVErosionFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVDilationFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVOpeningFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVClosingFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVShapeMaskHelperClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVUserMaskOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVPatternAnalyzerClassGuid, &SVSetupDialogManager::SVPatternAnalyzerClassSetupDialog},
		{SVPerspectiveToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVPixelAnalyzerClassGuid, &SVSetupDialogManager::SVPixelAnalyzerClassSetupDialog},
		{SVPointValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVPolarTransformClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVPolarTransformationToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVRangeClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVRankingFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVRemoteOutputDataControllerGUID, &SVSetupDialogManager::SVNotImplemented},
		{SVRemoteOutputGroupGUID, &SVSetupDialogManager::SVNotImplemented},
		{SVRemoteOutputObjectGUID, &SVSetupDialogManager::SVNotImplemented},
		{SVResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVDoubleResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVLongResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVPointXResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVPointYResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVStringResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVRotateReferenceClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVSkeletonFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVStatisticsToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVStringValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVTaskObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVTaskObjectListClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVThickeningFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVThinningFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVTimerValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVAcquisitionToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVImageToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVLoadImageToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVToolSetClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVTransformClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVTransformationToolClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVStdImageOperatorListClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVInPlaceImageOperatorListClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVUpperThresholdEquationClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVVariantResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog},
		{SVVariantValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVWatershedFilterClassGuid, &SVSetupDialogManager::SVNotImplemented},
		{SVWindowToolClassGuid, &SVSetupDialogManager::SVNotImplemented}}
{
}

HRESULT SVSetupDialogManager::SVNotImplemented(const SVGUID& p_rObjectId, CWnd* PParentWnd)
{
	return E_NOTIMPL;
}

HRESULT SVSetupDialogManager::SVBarCodeAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::SVBarCodeAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVBarCodeAnalyzerClass*>(SVObjectManagerClass::Instance().GetObject(rObjectId));

	SVInspectionProcess* pInspection(nullptr);

	if (nullptr != pAnalyzer && nullptr != (pInspection = dynamic_cast<SVInspectionProcess*>(pAnalyzer->GetInspection())))
	{
		SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc(pInspection->GetUniqueObjectID());

		if (nullptr != pIPDoc)
		{
			pIPDoc->SetModifiedFlag();
		}

		SVBarCodeProperties dlgProp;
		int iResult;
		BOOL bUseSingle;
		std::string RegExp;
		BOOL bSaveInFile;
		std::string SingleFile;
		BOOL bUseMultiple;
		std::string MultiFile;

		SvOp::SVBarCodeResultClass *pResult = dynamic_cast<SvOp::SVBarCodeResultClass*>(pAnalyzer->GetResultObject());

		dlgProp.m_dlgBarCodeGeneral.SetBarCodeSearchSpeed(pAnalyzer->msv_dSpeed);
		dlgProp.m_dlgBarCodeGeneral.SetBarCodeType(pAnalyzer->msv_lBarCodeType);
		dlgProp.m_dlgBarCodeGeneral.SetOrientation(pAnalyzer->msv_dOrientation);
		dlgProp.m_dlgBarCodeGeneral.SetSkewNegative(pAnalyzer->msv_dSkewNegative);
		dlgProp.m_dlgBarCodeGeneral.SetSkewPositive(pAnalyzer->msv_dSkewPositive);
		dlgProp.m_dlgBarCodeGeneral.SetThreshold(pAnalyzer->msv_dThreshold);
		dlgProp.m_dlgBarCodeGeneral.SetForegroundColor(pAnalyzer->msv_dForegroundColor);
		dlgProp.m_dlgBarCodeGeneral.SetBarCodeStringSize(pAnalyzer->msv_dStringSize);
		dlgProp.m_dlgBarCodeGeneral.SetTimeout(pAnalyzer->msv_lBarcodeTimeout);

		// To support special DMCs May 2008.
		dlgProp.m_dlgBarCodeGeneral.SetBarcodeStringFormat(pAnalyzer->msv_eStringFormat);
		dlgProp.m_dlgBarCodeGeneral.SetBarcodeThresholdType(pAnalyzer->msv_lThresholdType);

		//for support for the Uneven grid step for DMCs
		dlgProp.m_dlgBarCodeGeneral.SetUnEvenGrid(pAnalyzer->msv_bUnEvenGrid);

		BOOL bTmp = FALSE;
		pAnalyzer->m_bWarnOnFailedRead.GetValue(bTmp);
		dlgProp.m_dlgBarCodeGeneral.SetWarnOnFail(bTmp);

		dlgProp.m_dlgBarCodeAttributes.SetEncoding(pAnalyzer->msv_dEncoding);
		dlgProp.m_dlgBarCodeAttributes.SetErrCorrection(pAnalyzer->msv_dErrorCorrection);

		pAnalyzer->LoadRegExpression();

		pResult->msv_bUseSingleMatchString.GetValue(bUseSingle);
		pAnalyzer->msv_szRegExpressionValue.GetValue(RegExp);
		pAnalyzer->msv_bSaveStringInFile.GetValue(bSaveInFile);
		pAnalyzer->msv_szStringFileName.GetValue(SingleFile);
		pResult->msv_bUseMatchStringFile.GetValue(bUseMultiple);
		pResult->msv_szMatchStringFileName.GetValue(MultiFile);

		dlgProp.m_dlgBarCodeStringMatch.SetUseSingle(bUseSingle);
		dlgProp.m_dlgBarCodeStringMatch.SetRegExpression(RegExp.c_str());
		dlgProp.m_dlgBarCodeStringMatch.SetSaveInFile(bSaveInFile);
		dlgProp.m_dlgBarCodeStringMatch.SetSingleFileName(SingleFile.c_str());
		dlgProp.m_dlgBarCodeStringMatch.SetUseMultiple(bUseMultiple);
		dlgProp.m_dlgBarCodeStringMatch.SetMultiFileName(MultiFile.c_str());

		dlgProp.m_dlgBarCodeDataMatrix.SetCellX(pAnalyzer->msv_dCellNumberX);
		dlgProp.m_dlgBarCodeDataMatrix.SetCellY(pAnalyzer->msv_dCellNumberY);
		dlgProp.m_dlgBarCodeDataMatrix.SetMinCellSize(pAnalyzer->msv_dCellMinSize);
		dlgProp.m_dlgBarCodeDataMatrix.SetMaxCellSize(pAnalyzer->msv_dCellMaxSize);

		//remove the apply button
		dlgProp.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		iResult = static_cast<int>(dlgProp.DoModal());

		if (iResult == IDOK)
		{
			bUseSingle = dlgProp.m_dlgBarCodeStringMatch.GetUseSingle();
			RegExp = dlgProp.m_dlgBarCodeStringMatch.GetRegExpression();
			bSaveInFile = dlgProp.m_dlgBarCodeStringMatch.GetSaveInFile();
			SingleFile = dlgProp.m_dlgBarCodeStringMatch.GetSingleFileName();
			bUseMultiple = dlgProp.m_dlgBarCodeStringMatch.GetUseMultiple();
			MultiFile = dlgProp.m_dlgBarCodeStringMatch.GetMultiFileName();

			//@TODO[gra][8.00][15.01.2018]: The data controller should be moved into the dialog
			typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
			Controller Values {SvOg::BoundValues{ pInspection->GetUniqueObjectID(), rObjectId }};
			Values.Init();

			Values.Set<long>(SVBCTypeObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetBarCodeType());
			Values.Set<double>(SVBCSpeedObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetBarCodeSearchSpeed());
			Values.Set<double>(SVBCOrientationObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetOrientation());
			Values.Set<double>(SVBCSkewNegativeObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetSkewNegative());
			Values.Set<double>(SVBCSkewPositiveObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetSkewPositive());
			Values.Set<double>(SVBCThresholdObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetThreshold());
			Values.Set<double>(SVBCForegroundColorObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetForegroundColor());
			Values.Set<double>(SVBCStringSizeObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetBarCodeStringSize());
			Values.Set<bool>(SVBCWarnOnFailedReadObjectGuid, dlgProp.m_dlgBarCodeGeneral.GetWarnedOnFail() ? true : false);
			Values.Set<long>(SVBCTimeoutGuid, dlgProp.m_dlgBarCodeGeneral.GetTimeout());
			Values.Set<CString>(SVBCStringFormatGuid, dlgProp.m_dlgBarCodeGeneral.GetBarcodeStringFormat());
			Values.Set<long>(SVBCThresholdTypeGuid, dlgProp.m_dlgBarCodeGeneral.GetBarcodeThresholdType());
			Values.Set<bool>(SVBCUnevenGridGuid, dlgProp.m_dlgBarCodeGeneral.GetUnEvenGrid() ? true : false);

			Values.Set<double>(SVBCEncodingObjectGuid, dlgProp.m_dlgBarCodeAttributes.GetEncoding());
			Values.Set<double>(SVBCErrorCorrectionObjectGuid, dlgProp.m_dlgBarCodeAttributes.GetErrorCorrection());
			Values.Set<CString>(SVRegExpressionObjectGuid, RegExp.c_str());
			Values.Set<bool>(SVBCSaveStringInFileObjectGuid, bSaveInFile ? true : false);
			Values.Set<CString>(SVBCStringFileNameObjectGuid, SingleFile.c_str());

			Values.Set<double>(SVBCCellNumberXObjectGuid, static_cast<double> (dlgProp.m_dlgBarCodeDataMatrix.GetCellX()));
			Values.Set<double>(SVBCCellNumberYObjectGuid, static_cast<double> (dlgProp.m_dlgBarCodeDataMatrix.GetCellY()));
			Values.Set<double>(SVBCCellMinSizeObjectGuid, static_cast<double> (dlgProp.m_dlgBarCodeDataMatrix.GetMinCellSize()));
			Values.Set<double>(SVBCCellMaxSizeObjectGuid, static_cast<double> (dlgProp.m_dlgBarCodeDataMatrix.GetMaxCellSize()));

			Values.Commit();

			Controller Result {SvOg::BoundValues{ pInspection->GetUniqueObjectID(), pResult->GetUniqueObjectID() }};
			Result.Init();

			Result.Set<bool>(SVBCUseSingleMatchStringGuid, bUseSingle ? true : false);
			Result.Set<bool>(SVBCUseMatchStringFileGuid, bUseMultiple ? true : false);
			Result.Set<CString>(SVBCMatchStringFileNameObjectGuid, MultiFile.c_str());
			Result.Commit();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVBlobAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::SVBlobAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVBlobAnalyzerClass*> (SVObjectManagerClass::Instance().GetObject(rObjectId));

	if (nullptr != pAnalyzer)
	{
		if (nullptr != pAnalyzer->GetInspection())
		{
			SVGUID InspectionID(pAnalyzer->GetInspection()->GetUniqueObjectID());
			SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc(InspectionID);

			if (nullptr != pIPDoc)
			{
				pIPDoc->SetModifiedFlag();

				SvOg::SVBlobAnalyzeFeatureDialogClass dlg(InspectionID, rObjectId, pParentWnd);

				if (IDOK == dlg.DoModal())
				{
					BOOL l_bIsFillBlob;
					pAnalyzer->m_bvoFillBlobs.GetValue(l_bIsFillBlob);
#if SV_DESIRED_MIL_VERSION == 0x0900
					SVMatroxBlobInterface::Set(pAnalyzer->m_ResultBufferID, SVEBlobSaveRuns, static_cast<long>(l_bIsFillBlob ? SVValueEnable : SVValueDisable));
#else
					SVMatroxBlobInterface::Set(pAnalyzer->m_BlobContextID, SVEBlobSaveRuns, static_cast<long>(l_bIsFillBlob ? SVValueEnable : SVValueDisable));
#endif
					pAnalyzer->CreateArray();


					// Rebuild results list/view
					// what about published results list ?
					pIPDoc->UpdateAllViews(nullptr);
				}
				else
				{
					l_Status = S_FALSE;
				}
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVColorToolClassSetupDialog(const SVGUID& p_rObjectId, CWnd* PParentWnd)
{
	HRESULT l_Status = E_FAIL;

	SvTo::SVColorToolClass* pTool = dynamic_cast<SvTo::SVColorToolClass*>(SVObjectManagerClass::Instance().GetObject(p_rObjectId));

	if (nullptr != pTool)
	{
		std::string Title = SvUl::LoadStdString(IDS_ADJUSTMENT_STRING);

		// Get Complete Name up to the tool level...
		Title = pTool->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType) + _T(" ") + Title;

		SVObjectClass* pInspection(pTool->GetInspection());

		if (nullptr != pInspection)
		{
			SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc(pInspection->GetUniqueObjectID());
			ASSERT(nullptr != pIPDoc);

			if (nullptr != pIPDoc)
			{
				// Create the Train Color Property Sheet
				SVTADlgColorThresholdSheet dlg(pTool, Title.c_str());
				SvOp::SVColorThresholdClass* pThreshold = dynamic_cast<SvOp::SVColorThresholdClass*> (pTool->GetAt(0));
				if (ID_OK == dlg.DoModal())
				{
					pIPDoc->SetModifiedFlag();
					l_Status = S_OK;
				}
				else
				{
					l_Status = S_FALSE;
				}
				pIPDoc->UpdateAllViews(nullptr);
			}
		}
	}
	return l_Status;
}

HRESULT SVSetupDialogManager::SVHistogramAnalyzerClassSetupDialog(const SVGUID& p_rObjectId, CWnd* PParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::SVHistogramAnalyzerClass* l_pAnalyzer = dynamic_cast<SvAo::SVHistogramAnalyzerClass*>(SVObjectManagerClass::Instance().GetObject(p_rObjectId));

	if (nullptr != l_pAnalyzer)
	{
		SVHistogramAnalyzerSetupClass dlg;
		dlg.m_pAnalyzer = l_pAnalyzer;
		dlg.m_histogram.SetScale(static_cast<histogram::scale>(l_pAnalyzer->m_scale));
		dlg.m_histogram.SetPeakThreshold(l_pAnalyzer->m_threshold);
		dlg.m_histogram.SetPeakDefault(l_pAnalyzer->m_default);
		dlg.m_histogram.SetDisplay(l_pAnalyzer->m_accumulate ? histogram::accumulated : histogram::normal);
		dlg.m_histogram.SetMinHeight(l_pAnalyzer->m_minHeight);
		dlg.m_histogram.SetValleyDefault(l_pAnalyzer->m_valleyDefault);
		dlg.m_histogram.SetValleyLow(l_pAnalyzer->m_lowValley);
		dlg.m_histogram.SetValleyHigh(l_pAnalyzer->m_highValley);
		dlg.m_histogram.SetFixedHeight(l_pAnalyzer->m_fixedHeight);
		dlg.m_histogram.SetHeightOption(l_pAnalyzer->m_dynamicHeight ? histogram::dynamic : histogram::fixed);
		dlg.m_histogram.Clip(l_pAnalyzer->m_rangeStart, l_pAnalyzer->m_rangeEnd);

		if (IDOK == dlg.DoModal())
		{
			l_pAnalyzer->m_rangeStart = dlg.m_histogram.GetLowClip();
			l_pAnalyzer->m_rangeEnd = dlg.m_histogram.GetHighClip();
			l_pAnalyzer->m_scale = dlg.m_histogram.GetScale();
			l_pAnalyzer->m_dynamicHeight = dlg.m_histogram.GetHeightOption() == histogram::dynamic;
			l_pAnalyzer->m_histogram.SetHeightOption(dlg.m_histogram.GetHeightOption());
			l_pAnalyzer->m_histogram.SetScale(l_pAnalyzer->m_scale ? histogram::logarithmic : histogram::linear);
			l_pAnalyzer->m_default = dlg.m_histogram.GetPeakDefault();
			l_pAnalyzer->m_histogram.SetPeakDefault(l_pAnalyzer->m_default);
			l_pAnalyzer->m_threshold = dlg.m_histogram.GetPeakThreshold();
			l_pAnalyzer->m_histogram.SetPeakThreshold(l_pAnalyzer->m_threshold);
			l_pAnalyzer->m_minHeight = dlg.m_histogram.GetMinHeight();
			l_pAnalyzer->m_histogram.SetMinHeight(l_pAnalyzer->m_minHeight);
			l_pAnalyzer->m_valleyDefault = dlg.m_histogram.GetValleyDefault();
			l_pAnalyzer->m_histogram.SetValleyDefault(l_pAnalyzer->m_valleyDefault);
			l_pAnalyzer->m_lowValley = dlg.m_histogram.GetValleyLow();
			l_pAnalyzer->m_histogram.SetValleyLow(l_pAnalyzer->m_lowValley);
			l_pAnalyzer->m_highValley = dlg.m_histogram.GetValleyHigh();
			l_pAnalyzer->m_histogram.SetValleyHigh(l_pAnalyzer->m_highValley);
			l_pAnalyzer->m_fixedHeight = dlg.m_histogram.GetFixedHeight();
			l_pAnalyzer->m_histogram.SetFixedHeight(l_pAnalyzer->m_fixedHeight);
			l_pAnalyzer->m_accumulate = dlg.m_histogram.GetDisplay() == histogram::accumulated;
			l_pAnalyzer->m_histogram.SetDisplay(l_pAnalyzer->m_accumulate ? histogram::accumulated : histogram::normal);
			l_pAnalyzer->m_histogram.Clip(l_pAnalyzer->m_rangeStart, l_pAnalyzer->m_rangeEnd);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::SVLinearAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVLinearAnalyzerClass*>(SVObjectManagerClass::Instance().GetObject(rObjectId));

	SVInspectionProcess* pInspection(nullptr);

	if (nullptr != pAnalyzer && nullptr != (pInspection = dynamic_cast<SVInspectionProcess*>(pAnalyzer->GetInspection())))
	{
		std::string Title = SvUl::LoadStdString(IDS_ADJUSTMENT_STRING);
		// Get Complete Name up to the tool level...
		Title = pAnalyzer->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType) + _T(" ") + Title;

		SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc(pInspection->GetUniqueObjectID());

		SvOp::SVLinearEdgeProcessingClass *pEdgeA = pAnalyzer->GetEdgeA();
		SvOp::SVLinearEdgeProcessingClass *pEdgeB = pAnalyzer->GetEdgeB();

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog(Title.c_str());

		measureDialog.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		SVProfileEdgeMarkerAdjustmentPageClass *pPageA = nullptr;
		SVProfileEdgeMarkerAdjustmentPageClass *pPageB = nullptr;

		if (nullptr != pEdgeA)
		{
			pPageA = new SVProfileEdgeMarkerAdjustmentPageClass(pInspection->GetUniqueObjectID(), pEdgeA->GetUniqueObjectID(), pEdgeA->getEdgeEmbeddedGuids(), IDS_EDGE_A);
		}

		if (nullptr != pEdgeB)
		{
			pPageB = new SVProfileEdgeMarkerAdjustmentPageClass(pInspection->GetUniqueObjectID(), pEdgeB->GetUniqueObjectID(), pEdgeB->getEdgeEmbeddedGuids(), IDS_EDGE_B);
		}

		if (nullptr != pIPDoc && (nullptr != pPageA || nullptr != pPageB))
		{
			if (nullptr != pPageA && nullptr != pEdgeA)
			{
				pPageA->SetNormalizer(pAnalyzer->m_svNormalizer);
				pPageA->m_bEdgeA = true;

				//set enable flags
				pPageA->m_bEnableDirection = pAnalyzer->m_bEnableDirection;
				pPageA->m_bEnableEdgeSelect = pAnalyzer->m_bEnableEdgeSelect;
				pPageA->m_bEnablePolarisation = pAnalyzer->m_bEnablePolarisation;
				pPageA->m_bEnablePosition = pAnalyzer->m_bEnablePosition;
				pPageA->m_bEnableThreshold = pAnalyzer->m_bEnableThreshold;

				measureDialog.AddPage(pPageA);
			}

			if (nullptr != pPageB && nullptr != pEdgeB)
			{
				pPageB->SetNormalizer(pAnalyzer->m_svNormalizer);
				pPageB->m_bEdgeA = false;

				//set enable flags
				pPageB->m_bEnableDirection = pAnalyzer->m_bEnableDirection;
				pPageB->m_bEnableEdgeSelect = pAnalyzer->m_bEnableEdgeSelect;
				pPageB->m_bEnablePolarisation = pAnalyzer->m_bEnablePolarisation;
				pPageB->m_bEnablePosition = pAnalyzer->m_bEnablePosition;
				pPageB->m_bEnableThreshold = pAnalyzer->m_bEnableThreshold;

				measureDialog.AddPage(pPageB);
			}

			if (IDOK == measureDialog.DoModal())
			{
				// Make sure the persistence objects are updated.
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		if (pPageA)
		{
			delete pPageA;
		}

		if (pPageB)
		{
			delete pPageB;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVSetupDialogManager::SVLuminanceAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* PParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::SVAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass*> (SVObjectManagerClass::Instance().GetObject(rObjectId));

	if (nullptr != pAnalyzer && SvPb::SVLuminanceAnalyzerObjectType == pAnalyzer->GetObjectSubType())
	{
		SvOg::SVLuminanceAnalyzerDlg dlg {pAnalyzer->GetInspection()->GetUniqueObjectID(), rObjectId};
		dlg.DoModal();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVOCVAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* PParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::SVOCVAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVOCVAnalyzerClass*>(SVObjectManagerClass::Instance().GetObject(rObjectId));

	if (nullptr != pAnalyzer)
	{
		SvOp::SVOCVAnalyzeResultClass* pOCVResult = dynamic_cast<SvOp::SVOCVAnalyzeResultClass*> (pAnalyzer->GetResultObject());
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pAnalyzer->GetInspection());

		if (nullptr != pOCVResult && nullptr != pInspection)
		{
			SvOg::SVOCVSheet dlg(pInspection->GetUniqueObjectID(), pOCVResult->GetUniqueObjectID());

			if (IDOK == dlg.DoModal())
			{
				SVIPDoc*pIPDoc = TheSVObserverApp.GetIPDoc(pInspection->GetUniqueObjectID());

				if (nullptr != pIPDoc)
				{
					pIPDoc->SetModifiedFlag(true);
				}

				if (SVOLicenseManager::Instance().HasMatroxIdentificationLicense())
				{
					pOCVResult->LoadMatchString();

					//
					// Make sure the OCV parameters string for scripting 
					// to be saved as part of the configuration is updated.
					//
					if (pOCVResult->GenerateFontModel())
					{
						//
						// Run the toolset once in case another window is using the
						// same OCV Font Training file.
						// 16 Dec 1999 - frb (100)
						//
						// Reset all objects...
						bool bOk = pInspection->GetToolSet()->resetAllObjects();
						if (bOk)
						{
							l_Status =  SvCmd::RunOnceSynchronous(pInspection->GetUniqueObjectID());
						}
						else
						{
							l_Status = E_FAIL;
						}
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
			else
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVPixelAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_FALSE;

	SvAo::SVPixelAnalyzerClass* pAnalyzer = dynamic_cast<SvAo::SVPixelAnalyzerClass*> (SVObjectManagerClass::Instance().GetObject(rObjectId));

	if (nullptr != pAnalyzer)
	{
		if (nullptr != pAnalyzer->GetInspection())
		{
			const SVGUID& rInspectionID = pAnalyzer->GetInspection()->GetUniqueObjectID();
			SVIPDoc* pIPDoc = TheSVObserverApp.GetIPDoc(rInspectionID);

			if (nullptr != pIPDoc)
			{
				pIPDoc->SetModifiedFlag();

				SVPixelAnalyzerDlg Dlg(rInspectionID, rObjectId);
				Dlg.DoModal();
				l_Status = S_OK;
			}
		}
	}
	return l_Status;
}

HRESULT SVSetupDialogManager::SVPatternAnalyzerClassSetupDialog(const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;
	SvAo::SVPatternAnalyzerClass* l_pAnalyzer = dynamic_cast<SvAo::SVPatternAnalyzerClass*>(SVObjectManagerClass::Instance().GetObject(rObjectId));

	SVInspectionProcess* pInspection(nullptr);

	if (nullptr != l_pAnalyzer && nullptr != (pInspection = dynamic_cast<SVInspectionProcess*>(l_pAnalyzer->GetInspection())))
	{
		double dParam;
		long lParam;
		long lSpeedFactor[5] = {SVValueVeryLow, SVValueLow, SVValueMedium, SVValueHigh, SVValueVeryHigh};
		int index;

		SVPatAnalyzeSetupDlgSheet SetupDlgSheet(_T("Pattern Analyzer Setup"), pParentWnd);

		SetupDlgSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
		// Pages on this sheet
		SvOg::SVPatternAnalyzerModelPage ModelPage(l_pAnalyzer->GetInspection()->GetUniqueObjectID(), l_pAnalyzer->GetUniqueObjectID());
		SVPatGeneralPageClass GeneralPage;
		SVPatAdvancedPageClass AdvancedPage;

		// General page
		l_pAnalyzer->msv_lpatMaxOccurances.GetValue(lParam);
		GeneralPage.m_lOccurances = (int)lParam;
		l_pAnalyzer->msv_dpatAcceptanceThreshold.GetValue(dParam);
		GeneralPage.m_lAcceptance = (long)dParam;
		l_pAnalyzer->msv_dpatCertaintyThreshold.GetValue(dParam);
		GeneralPage.m_lCertainty = (long)dParam;
		l_pAnalyzer->msv_bpatSearchAngleMode.GetValue(GeneralPage.m_bAngleSearch);
		l_pAnalyzer->msv_lpatAccuracy.GetValue(lParam);
		for (index = 0; index < 4; index++)
		{
			if (lSpeedFactor[index] == lParam)
			{
				GeneralPage.m_nPosAccuracyIndex = index - 1;
				break;
			}
		}
		l_pAnalyzer->msv_lpatSpeed.GetValue(lParam);
		for (index = 0; index < 4; index++)
		{
			if (lSpeedFactor[index] == lParam)
			{
				GeneralPage.m_nSpeedIndex = index;
				break;
			}
		}

		l_pAnalyzer->msv_dpatSearchAngle.GetValue(GeneralPage.m_dSearchAngle);
		l_pAnalyzer->msv_dpatAngleDeltaNeg.GetValue(GeneralPage.m_dAngleDeltaNegative);
		l_pAnalyzer->msv_dpatAngleDeltaPos.GetValue(GeneralPage.m_dAngleDeltaPositive);
		l_pAnalyzer->msv_dpatAngleTolerance.GetValue(GeneralPage.m_dAngleTolerance);

		GeneralPage.m_bAccuracy = l_pAnalyzer->m_bAngleAccuracy;

		l_pAnalyzer->msv_dpatAngleAccuracy.GetValue(GeneralPage.m_dAngularAccuracy);
		if (GeneralPage.m_dAngularAccuracy == SVValueDefault || GeneralPage.m_dAngularAccuracy == SVValueDisable)
		{
			GeneralPage.m_dAngularAccuracy = 0.1;
		}

		double l_dInterpMode = 0;
		l_pAnalyzer->msv_dpatAngleInterpolation.GetValue(l_dInterpMode);
		GeneralPage.m_dInterpolationMode = static_cast<SVImageOperationTypeEnum> (static_cast<long>(l_dInterpMode));

		GeneralPage.m_pPatAnalyzer = l_pAnalyzer;

		// get Values for Advanced page
		l_pAnalyzer->msv_dpatPreliminaryAcceptanceThreshold.GetValue(AdvancedPage.m_dPreliminaryAcceptanceThreshold);
		l_pAnalyzer->msv_lpatFastFind.GetValue(AdvancedPage.m_lFastFind);
		l_pAnalyzer->msv_lpatModelStep.GetValue(AdvancedPage.m_lModelStep);
		l_pAnalyzer->msv_lpatBeginningResolutionLevel.GetValue(AdvancedPage.m_lBeginningResolutionLevel);
		l_pAnalyzer->msv_lpatFinalResolutionLevel.GetValue(AdvancedPage.m_lFinalResolutionLevel);
		l_pAnalyzer->msv_lpatAdditionalCandidates.GetValue(AdvancedPage.m_lAdditionalCandidates);
		l_pAnalyzer->msv_dpatCandidateSpacingXMin.GetValue(AdvancedPage.m_dCandidateSpacingXMin);
		l_pAnalyzer->msv_dpatCandidateSpacingYMin.GetValue(AdvancedPage.m_dCandidateSpacingYMin);

		// Add pages to the property Sheet
		SetupDlgSheet.AddPage(&ModelPage);
		SetupDlgSheet.AddPage(&GeneralPage);
		SetupDlgSheet.AddPage(&AdvancedPage);

		if (IDOK == SetupDlgSheet.DoModal())
		{
			l_pAnalyzer->m_bAngleAccuracy = GeneralPage.m_bAccuracy ? true : false;

			//@TODO[gra][8.00][15.01.2018]: The data controller should be moved into the dialog
			typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
			Controller Values {SvOg::BoundValues{ pInspection->GetUniqueObjectID(), rObjectId }};
			Values.Init();

			// Save General Page
			Values.Set<long>(SVpatMaxOccurancesObjectGuid, GeneralPage.m_lOccurances);
			Values.Set<double>(SVpatAcceptThresholdObjectGuid, static_cast<double> (GeneralPage.m_lAcceptance));
			Values.Set<double>(SVpatCertainThresholdObjectGuid, static_cast<double> (GeneralPage.m_lCertainty));
			Values.Set<long>(SVpatAccuracyObjectGuid, lSpeedFactor[GeneralPage.m_nPosAccuracyIndex + 1]);
			Values.Set<long>(SVpatSpeedObjectGuid, lSpeedFactor[GeneralPage.m_nSpeedIndex]);
			Values.Set<bool>(SVpatSearchAngleModeObjectGuid, GeneralPage.m_bAngleSearch ? true : false);

			if (GeneralPage.m_bAngleSearch)
			{
				Values.Set<double>(SVpatSearchAngleObjectGuid, GeneralPage.m_dSearchAngle);
				Values.Set<double>(SVpatAngleDeltaNegObjectGuid, GeneralPage.m_dAngleDeltaNegative);
				Values.Set<double>(SVpatAngleDeltaPosObjectGuid, GeneralPage.m_dAngleDeltaPositive);
				Values.Set<double>(SVpatAngleToleranceObjectGuid, GeneralPage.m_dAngleTolerance);
				Values.Set<double>(SVpatAngleInterpolationObjectGuid, GeneralPage.m_dInterpolationMode);

				double dAngleAccuracy = l_pAnalyzer->m_bAngleAccuracy ? GeneralPage.m_dAngularAccuracy : static_cast<double> (SVValueDisable);
				Values.Set<double>(SVpatAngleAccuracyObjectGuid, dAngleAccuracy);
			}

			// Save Advanced Page
			Values.Set<long>(SVpatFastFindObjectGuid, AdvancedPage.m_lFastFind);
			Values.Set<long>(SVpatAdditionalCandidatesObjectGuid, AdvancedPage.m_lAdditionalCandidates);
			Values.Set<double>(SVpatCandidateSpacingXMinObjectGuid, AdvancedPage.m_dCandidateSpacingXMin);
			Values.Set<double>(SVpatCandidateSpacingYMinObjectGuid, AdvancedPage.m_dCandidateSpacingYMin);
			Values.Set<double>(SVpatPreliminaryAcceptanceThresholdObjectGuid, AdvancedPage.m_dPreliminaryAcceptanceThreshold);
			Values.Set<long>(SVpatModelStepObjectGuid, AdvancedPage.m_lModelStep);
			Values.Set<long>(SVpatBeginningResolutionLevelObjectGuid, AdvancedPage.m_lBeginningResolutionLevel);
			Values.Set<long>(SVpatFinalResolutionLevelObjectGuid, AdvancedPage.m_lFinalResolutionLevel);

			Values.Commit();
		}
		else
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVResultClassSetupDialog(const SVGUID& rObjectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SvOp::SVResultClass* l_pResult = dynamic_cast<SvOp::SVResultClass*> (SVObjectManagerClass::Instance().GetObject(rObjectId));

	if (nullptr != l_pResult)
	{
		SvOp::SVRangeClass* pRange = l_pResult->GetResultRange();
		if (pRange)
		{
			RangeXDialogClass dlg(pRange->GetInspection()->GetUniqueObjectID(), pRange->GetParent()->GetUniqueObjectID(), pRange->GetUniqueObjectID(), pParentWnd);
			if (IDOK != dlg.DoModal())
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

