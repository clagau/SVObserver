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

#include "stdafx.h"
#include <boost/assign.hpp>
#include "SVSetupDialogManager.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "GuiCommands/InspectionRunOnce.h"

#include "SVAnalyzerLuminance.h"
#include "SVAnalyzerLuminanceSetup.h"
#include "SVBarCodeAnalyzerClass.h"
#include "SVBarCodeProperties.h"
#include "SVBarCodeResult.h"
#include "SVBlobAnalyzer.h"
#include "SVBlobAnalyzerDialog.h"
#include "SVColorThreshold.h"
#include "SVColorTool.h"
#include "SVEdgeMarkerAdjustmentPageClass.h"
#include "SVHistogramAnalyzer.h"
#include "SVHistogramAnalyzerSetup.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVLine.h"
#include "SVLinearAnalyzerClass.h"
#include "SVLinearEdgeProcessingClass.h"
#include "SVLineEdgeCountingAnalyzer.h"
#include "SVLineEdgePositionAnalyzer.h"
#include "SVLineLinearMeasurementAnalyzer.h"
#include "SVLineMaximumBackgroundObjectAnalyzer.h"
#include "SVLineMaximumForegroundObjectAnalyzer.h"
#include "SVLineMaximumObjectAnalyzer.h"
#include "SVLinePixelCountingAnalyzer.h"
#include "SVLineROI.h"
#include "SVMeasureAnalyzerAdjustmentSheet.h"
#include "SVOCRAnalyzer.h"
#include "SVOCRAnalyzerResult.h"
#include "SVOCRDialog.h"
#include "SVOCRGrayAnalyzer.h"
#include "SVOCRGrayAnalyzerResult.h"
#include "SVOCRGrayDialog.h"
#include "SVOCVAnalyzer.h"
#include "SVOCVAnalyzerResult.h"
#include "SVOCVDialog.h"
#include "SVPatAdvancedPageClass.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVPatGeneralPageClass.h"
#include "SVPatSelectModelPageClass.h"
#include "SVPatternAnalyzerClass.h"
#include "SVPixelAnalyzer.h"
#include "SVPixelAnalyzerSetup.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVThresholdAdjustmentPageClass.h"
#include "SVResult.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "RangeXDialogClass.h"
#include "SVRange.h"

SVSetupDialogManager& SVSetupDialogManager::Instance()
{
	static SVSetupDialogManager l_Object;

	return l_Object;
}

SVSetupDialogManager::~SVSetupDialogManager()
{
}

HRESULT SVSetupDialogManager::SetupDialog( const SVGUID& p_rClassId, const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVSetupDialogMap::iterator l_Iter = m_SetupDialogs.find( p_rClassId );

	if( l_Iter != m_SetupDialogs.end() )
	{
		l_Status = ( l_Iter->second )( p_rObjectId, PParentWnd );
	}
	else
	{
		l_Status = E_NOTIMPL;
	}

	return l_Status;
}

SVSetupDialogManager::SVSetupDialogManager()
: m_SetupDialogs()
{
	m_SetupDialogs = boost::assign::map_list_of<SVGUID, SVSetupDialogFunction>
		( SVAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImageAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLuminanceAnalyzerClassGuid, &SVSetupDialogManager::SVLuminanceAnalyzerClassSetupDialog )
		( SVAngularProfileToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVArchiveToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVAutoThresholdEquationClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVBarCodeAnalyzerClassGuid, &SVSetupDialogManager::SVBarCodeAnalyzerClassSetupDialog )
		( SVBarCodeResultClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVBlobAnalyzerClassGuid, &SVSetupDialogManager::SVBlobAnalyzerClassSetupDialog )
		( SVBoolValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVByteValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVCharValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVColorThresholdClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVColorToolClassGuid, &SVSetupDialogManager::SVColorToolClassSetupDialog )
		( SVConditionalClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVCustomFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVCylindricalWarpToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDataBufferClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDoubleValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDPointValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDPointXResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVDPointYResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVDualLineAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDWordValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEnumerateValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEquationClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateTranslationXClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateTranslationYClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateRotationXClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateRotationYClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateRotationAngleClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateCenterXClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateCenterYClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateStartRadiusClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateEndRadiusClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateStartAngleClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateEndAngleClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateBetaClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateXClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEvaluateYClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVExternalToolGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVExternalToolTaskGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVFileNameValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVGageToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVHistogramAnalyzerClassGuid, &SVSetupDialogManager::SVHistogramAnalyzerClassSetupDialog )
		( SVImageArithmeticClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImageClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImagePolarTransformClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImageROIClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImageToLineProjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImageTransformClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVInspectionProcessGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVInt64ValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVIOControllerGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLineClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLineAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLinearAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearEdgeAProcessingClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLinearEdgeBProcessingClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLinearEdgeProcessingClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLinearImageOperatorListClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLinearEdgeCountingLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearEdgePositionLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearMaximumForegroundObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearMaximumObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearPixelCountingLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearMeasurementAnalyzerClassGuid, &SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog )
		( SVLinearToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEdgeCountingLineAnalyzerClassGuid, &SVSetupDialogManager::SVEdgeCountingLineAnalyzerClassSetupDialog )
		( SVEdgePositionLineAnalyzerClassGuid, &SVSetupDialogManager::SVEdgePositionLineAnalyzerClassSetupDialog )
		( SVLinearMeasurementLineAnalyzerClassGuid, &SVSetupDialogManager::SVLinearMeasurementLineAnalyzerClassSetupDialog )
		( SVMaximumBackgroundObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVMaximumBackgroundObjectLineAnalyzerClassSetupDialog )
		( SVMaximumForegroundObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVMaximumForegroundObjectLineAnalyzerClassSetupDialog )
		( SVMaximumObjectLineAnalyzerClassGuid, &SVSetupDialogManager::SVMaximumObjectLineAnalyzerClassSetupDialog )
		( SVPixelCountingLineAnalyzerClassGuid, &SVSetupDialogManager::SVPixelCountingLineAnalyzerClassSetupDialog )
		( SVLineROIClassGuid, &SVSetupDialogManager::SVLineROIClassSetupDialog )
		( SVLongValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLowerThresholdEquationClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLUTEquationClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLUTOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVMathContainerClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVMathEquationClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVMathToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVObjectAppClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVOCRAnalyzerClassGuid, &SVSetupDialogManager::SVOCRAnalyzerClassSetupDialog )
		( SVOCRAnalyzerResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVOCRGrayAnalyzerClassGuid, &SVSetupDialogManager::SVOCRGrayAnalyzerClassSetupDialog )
		( SVOCRGrayAnalyzerResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVOCVAnalyzerClassGuid, &SVSetupDialogManager::SVOCVAnalyzerClassSetupDialog )
		( SVOCVAnalyzerResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVOCVCharacterResultClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVUnaryOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVBinaryOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVUnaryImageOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVThresholdClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVSmoothFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVSharpenFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVSharpen2FilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVHorizEdgeFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVVertEdgeFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEdgeDetectFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVEdgeDetect2FilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLaplacianEdgeFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLaplacianEdge2FilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVMaxMinusMinFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVErosionFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDilationFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVOpeningFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVClosingFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVShapeMaskHelperClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVUserMaskOperatorClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVPatternAnalyzerClassGuid, &SVSetupDialogManager::SVPatternAnalyzerClassSetupDialog )
		( SVPerspectiveToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVPixelAnalyzerClassGuid, &SVSetupDialogManager::SVPixelAnalyzerClassSetupDialog )
		( SVPointValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVPolarTransformClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVPolarTransformationToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVRangeClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVRankingFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVRemoteInputToolGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVRemoteOutputDataControllerGUID, &SVSetupDialogManager::SVNotImplemented )
		( SVRemoteOutputGroupGUID, &SVSetupDialogManager::SVNotImplemented )
		( SVRemoteOutputObjectGUID, &SVSetupDialogManager::SVNotImplemented )
		( SVResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVAnalyzeFeatureClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVDoubleResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVLongResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVPointXResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVPointYResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVStringResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVROIClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVRotateReferenceClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVSingleLineAnalyzerClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVSkeletonFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVStatisticsToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVStringValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVTaskObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVTaskObjectListClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVThickeningFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVThinningFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVTimerValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVAcquisitionToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVBuildReferenceToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVImageToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVLoadImageToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVProfileToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVToolSetClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVTransformClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVTransformationToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVStdImageOperatorListClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVInPlaceImageOperatorListClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVUpperThresholdEquationClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVVariantResultClassGuid, &SVSetupDialogManager::SVResultClassSetupDialog )
		( SVVariantValueObjectClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVWatershedFilterClassGuid, &SVSetupDialogManager::SVNotImplemented )
		( SVWindowToolClassGuid, &SVSetupDialogManager::SVNotImplemented )
	;
}

HRESULT SVSetupDialogManager::SVNotImplemented( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	return E_NOTIMPL;
}

HRESULT SVSetupDialogManager::SVBarCodeAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVBarCodeAnalyzerClass* l_pAnalyzer = dynamic_cast< SVBarCodeAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	SVInspectionProcess* pInspection( nullptr );
	
	if( nullptr != l_pAnalyzer && nullptr != (pInspection = l_pAnalyzer->GetInspection()) )
	{
		SVIPDoc* pIPDoc( nullptr );

		pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );

		if( nullptr != pIPDoc )
		{
			pIPDoc->SetModifiedFlag();
		}

		SVBarCodeProperties dlgProp;
		int iResult;
		CString   csTemp;
		BOOL bUseSingle;
		CString csRegExp;
		BOOL bSaveInFile;
		CString csSingleFile;
		BOOL bUseMultiple;
		CString csMultiFile;
		
		SVBarCodeResultClass *pResult = dynamic_cast< SVBarCodeResultClass* >( l_pAnalyzer->GetResultObject() );
		
		dlgProp.m_dlgBarCodeGeneral.SetBarCodeSearchSpeed(l_pAnalyzer->msv_dSpeed);
		dlgProp.m_dlgBarCodeGeneral.SetBarCodeType(l_pAnalyzer->msv_lBarCodeType);
		dlgProp.m_dlgBarCodeGeneral.SetOrientation(l_pAnalyzer->msv_dOrientation);
		dlgProp.m_dlgBarCodeGeneral.SetSkewNegative(l_pAnalyzer->msv_dSkewNegative);
		dlgProp.m_dlgBarCodeGeneral.SetSkewPositive(l_pAnalyzer->msv_dSkewPositive);
		dlgProp.m_dlgBarCodeGeneral.SetThreshold(l_pAnalyzer->msv_dThreshold);
		dlgProp.m_dlgBarCodeGeneral.SetForegroundColor(l_pAnalyzer->msv_dForegroundColor);
		dlgProp.m_dlgBarCodeGeneral.SetBarCodeStringSize(l_pAnalyzer->msv_dStringSize);
		dlgProp.m_dlgBarCodeGeneral.SetTimeout(l_pAnalyzer->msv_lBarcodeTimeout);

		// To support special DMCs May 2008.
		dlgProp.m_dlgBarCodeGeneral.SetBarcodeStringFormat( l_pAnalyzer->msv_eStringFormat );
		dlgProp.m_dlgBarCodeGeneral.SetBarcodeThresholdType( l_pAnalyzer->msv_lThresholdType );

		//for support for the Uneven grid step for DMCs
		dlgProp.m_dlgBarCodeGeneral.SetUnEvenGrid( l_pAnalyzer->msv_bUnEvenGrid );

		BOOL bTmp = FALSE;
		l_pAnalyzer->m_bWarnOnFailedRead.GetValue(bTmp);
		dlgProp.m_dlgBarCodeGeneral.SetWarnOnFail(bTmp);
		
		dlgProp.m_dlgBarCodeAttributes.SetEncoding (l_pAnalyzer->msv_dEncoding);
		dlgProp.m_dlgBarCodeAttributes.SetErrCorrection (l_pAnalyzer->msv_dErrorCorrection);
		
		l_pAnalyzer->LoadRegExpression();
		
		pResult->msv_bUseSingleMatchString.GetValue( bUseSingle );
		l_pAnalyzer->msv_szRegExpressionValue.GetValue( csRegExp );
		l_pAnalyzer->msv_bSaveStringInFile.GetValue( bSaveInFile );
		l_pAnalyzer->msv_szStringFileName.GetValue ( csSingleFile );
		pResult->msv_bUseMatchStringFile.GetValue( bUseMultiple );
		pResult->msv_szMatchStringFileName.GetValue( csMultiFile );
		
		dlgProp.m_dlgBarCodeStringMatch.SetUseSingle( bUseSingle );
		dlgProp.m_dlgBarCodeStringMatch.SetRegExpression( csRegExp );
		dlgProp.m_dlgBarCodeStringMatch.SetSaveInFile( bSaveInFile );
		dlgProp.m_dlgBarCodeStringMatch.SetSingleFileName( csSingleFile );
		dlgProp.m_dlgBarCodeStringMatch.SetUseMultiple( bUseMultiple );
		dlgProp.m_dlgBarCodeStringMatch.SetMultiFileName( csMultiFile );
		
		dlgProp.m_dlgBarCodeDataMatrix.SetCellX (l_pAnalyzer->msv_dCellNumberX);
		dlgProp.m_dlgBarCodeDataMatrix.SetCellY (l_pAnalyzer->msv_dCellNumberY);
		dlgProp.m_dlgBarCodeDataMatrix.SetMinCellSize (l_pAnalyzer->msv_dCellMinSize);
		dlgProp.m_dlgBarCodeDataMatrix.SetMaxCellSize (l_pAnalyzer->msv_dCellMaxSize);

		//remove the apply button
		dlgProp.m_psh.dwFlags |= PSH_NOAPPLYNOW;
		
		iResult = static_cast<int>(dlgProp.DoModal());
		
		if (iResult == IDOK)
		{
			dlgProp.m_dlgBarCodeStringMatch.GetUseSingle( bUseSingle );
			dlgProp.m_dlgBarCodeStringMatch.GetRegExpression( csRegExp );
			dlgProp.m_dlgBarCodeStringMatch.GetSaveInFile( bSaveInFile );
			dlgProp.m_dlgBarCodeStringMatch.GetSingleFileName( csSingleFile );
			dlgProp.m_dlgBarCodeStringMatch.GetUseMultiple( bUseMultiple );
			dlgProp.m_dlgBarCodeStringMatch.GetMultiFileName( csMultiFile );
				
			try
			{
				SVToolClass *l_psvTool = l_pAnalyzer->GetTool();

				pInspection->AddInputRequest( &l_pAnalyzer->msv_lBarCodeType, dlgProp.m_dlgBarCodeGeneral.GetBarCodeType() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dSpeed, dlgProp.m_dlgBarCodeGeneral.GetBarCodeSearchSpeed() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dOrientation, dlgProp.m_dlgBarCodeGeneral.GetOrientation() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dSkewNegative, dlgProp.m_dlgBarCodeGeneral.GetSkewNegative() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dSkewPositive, dlgProp.m_dlgBarCodeGeneral.GetSkewPositive() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dThreshold, dlgProp.m_dlgBarCodeGeneral.GetThreshold() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dForegroundColor, dlgProp.m_dlgBarCodeGeneral.GetForegroundColor() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dStringSize, dlgProp.m_dlgBarCodeGeneral.GetBarCodeStringSize() );
				pInspection->AddInputRequest( &l_pAnalyzer->m_bWarnOnFailedRead, dlgProp.m_dlgBarCodeGeneral.GetWarnedOnFail() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dEncoding, dlgProp.m_dlgBarCodeAttributes.GetEncoding() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dErrorCorrection, dlgProp.m_dlgBarCodeAttributes.GetErrorCorrection() );
				pInspection->AddInputRequest( &( pResult->msv_bUseSingleMatchString ), bUseSingle );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_szRegExpressionValue, static_cast< LPCTSTR >( csRegExp ) );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_bSaveStringInFile, bSaveInFile );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_szStringFileName, static_cast< LPCTSTR >( csSingleFile ) );
				pInspection->AddInputRequest( &( pResult->msv_bUseMatchStringFile ), bUseMultiple );
				pInspection->AddInputRequest( &( pResult->msv_szMatchStringFileName ), static_cast< LPCTSTR >( csMultiFile ) );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dCellNumberX, dlgProp.m_dlgBarCodeDataMatrix.GetCellX() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dCellNumberY, dlgProp.m_dlgBarCodeDataMatrix.GetCellY() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dCellMinSize, dlgProp.m_dlgBarCodeDataMatrix.GetMinCellSize() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_dCellMaxSize, dlgProp.m_dlgBarCodeDataMatrix.GetMaxCellSize() );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_lBarcodeTimeout, dlgProp.m_dlgBarCodeGeneral.GetTimeout() );

				// To support special DMCs May 2008.
				pInspection->AddInputRequest( &l_pAnalyzer->msv_eStringFormat, dlgProp.m_dlgBarCodeGeneral.GetBarcodeStringFormat( ) );
				pInspection->AddInputRequest( &l_pAnalyzer->msv_lThresholdType, dlgProp.m_dlgBarCodeGeneral.GetBarcodeThresholdType( ) );

				//for support for the Uneven grid step for DMCs
				pInspection->AddInputRequest( &l_pAnalyzer->msv_bUnEvenGrid, dlgProp.m_dlgBarCodeGeneral.GetUnEvenGrid( ) );

				pInspection->AddInputRequestMarker();

				SVGUID l_ToolId;

				if( nullptr != l_psvTool )
				{
					l_ToolId = l_psvTool->GetUniqueObjectID();
				}

				GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
				SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

				l_Status = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
			}
			catch( ... )
			{
				ASSERT( FALSE );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVBlobAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVBlobAnalyzerClass* l_pAnalyzer = dynamic_cast< SVBlobAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVIPDoc* l_pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}

		SVBlobAnalyzeFeatureDialogClass dlg( l_pAnalyzer->GetTool(), l_pAnalyzer, l_pIPDoc, PParentWnd );

		if ( dlg.DoModal() == IDOK )
		{
			BOOL l_bIsFillBlob;
			l_pAnalyzer->m_bvoFillBlobs.GetValue( l_bIsFillBlob );

			if ( l_bIsFillBlob )
			{
				SVMatroxBlobInterface::Set( l_pAnalyzer->msvResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueEnable) );
			}
			else
			{
				SVMatroxBlobInterface::Set( l_pAnalyzer->msvResultBufferID, SVEBlobSaveRuns, static_cast<long>(SVValueDisable) );
			}

			l_pAnalyzer->CreateArray();
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

HRESULT SVSetupDialogManager::SVColorToolClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVColorToolClass* l_pTool = dynamic_cast< SVColorToolClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pTool != NULL )
	{
		CString strTitle;
		
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );

		// Get Complete Name up to the tool level...
		strTitle = l_pTool->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + _T( " " ) + strTitle;

		SVInspectionProcess* pInspection = l_pTool->GetInspection();

		// Create the Train Color Property Sheet
		SVTADlgColorThresholdSheet dlg( l_pTool, strTitle );
			
		if( nullptr != pInspection )
		{
			SVColorThresholdClass* pThreshold = ( SVColorThresholdClass * )l_pTool->GetAt(0);
			pThreshold->bShowHistogram = TRUE;
			
			SVIPDoc* pIPDoc( nullptr );
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );
			ASSERT( nullptr != pIPDoc );

			if( nullptr != pIPDoc )
			{
				if( dlg.DoModal() == IDOK )
				{
					pIPDoc->SetModifiedFlag();
				}
				else
				{
					l_Status = S_FALSE;
				}
			
				pThreshold->bShowHistogram = FALSE;

				pIPDoc->UpdateAllViews( NULL );
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

HRESULT SVSetupDialogManager::SVEdgeCountingLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVEdgeCountingLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVEdgeCountingLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + _T( " " ) + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		SVEdgeMarkerAdjustmentPageClass* pPage = new SVEdgeMarkerAdjustmentPageClass( IDS_OBJECTNAME_EDGE );

		if( pIPDoc && pLine && pPage )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = TRUE;

			pPage->PCurrentAnalyzer = l_pAnalyzer;
			pPage->SetNormalizer( pLine->m_svNormalizer );
			pPage->m_bEdgeA			= TRUE;

			pPage->m_pvoUseLowerThresholdSelectable			 = &l_pAnalyzer->m_voUseLowerThresholdSelectable;
			pPage->m_pvoUseLowerThresholdMaxMinusPercentDiff = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiff;
			pPage->m_pvoUseLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffset;
			pPage->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffset;
			pPage->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiff;
			pPage->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffset;
			pPage->m_pvoLowerThresholdMinPlusOffset			 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffset;

			pPage->m_pvoUseUpperThresholdSelectable			 = &l_pAnalyzer->m_voUseUpperThresholdSelectable;
			pPage->m_pvoUseUpperThresholdMaxMinusPercentDiff = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiff;
			pPage->m_pvoUseUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffset;
			pPage->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffset;
			pPage->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiff;
			pPage->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffset;
			pPage->m_pvoUpperThresholdMinPlusOffset			 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffset;

			pPage->m_pvoEdgeDirection			= &l_pAnalyzer->m_voEdgeADirection;
			pPage->m_pvoEdgeSelect				= &l_pAnalyzer->m_voEdgeAEdgeSelect;
			pPage->m_pvoEdgePolarisation		= &l_pAnalyzer->m_voEdgeAPolarisation;
			pPage->m_pvoEdgeSelectThisValue		= &l_pAnalyzer->m_voEdgeAEdgeSelectThisValue;
			pPage->m_pvoEdgeIsFixedEdgeMarker	= &l_pAnalyzer->m_voEdgeAIsFixedEdgeMarker;
			pPage->m_pvoEdgePosition			= &l_pAnalyzer->m_voEdgeAPosition;
			pPage->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeAPositionOffsetValue;
			pPage->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPage->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			//set enable flags
			pPage->m_bEnableDirection = l_pAnalyzer->m_bEnableDirection;
			pPage->m_bEnableEdgeSelect = l_pAnalyzer->m_bEnableEdgeSelect;
			pPage->m_bEnablePolarisation = l_pAnalyzer->m_bEnablePolarisation;
			pPage->m_bEnablePosition = l_pAnalyzer->m_bEnablePosition;
			pPage->m_bEnableThreshold = l_pAnalyzer->m_bEnableThreshold;

			measureDialog.AddPage( pPage );
			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance object is updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}
				
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = FALSE;
			
			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPage )
			delete pPage;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVEdgePositionLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVEdgePositionLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVEdgePositionLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + SV_TSTR_SPACE + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		SVEdgeMarkerAdjustmentPageClass* pPage = new SVEdgeMarkerAdjustmentPageClass( IDS_OBJECTNAME_EDGE );

		if( pIPDoc && pLine && pPage )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = TRUE;

			pPage->PCurrentAnalyzer = l_pAnalyzer;
			pPage->SetNormalizer( pLine->m_svNormalizer );
			pPage->m_bEdgeA			= TRUE;

			pPage->m_pvoUseLowerThresholdSelectable			 = &l_pAnalyzer->m_voUseLowerThresholdSelectable;
			pPage->m_pvoUseLowerThresholdMaxMinusPercentDiff = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiff;
			pPage->m_pvoUseLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffset;
			pPage->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffset;
			pPage->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiff;
			pPage->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffset;
			pPage->m_pvoLowerThresholdMinPlusOffset			 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffset;

			pPage->m_pvoUseUpperThresholdSelectable			 = &l_pAnalyzer->m_voUseUpperThresholdSelectable;
			pPage->m_pvoUseUpperThresholdMaxMinusPercentDiff = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiff;
			pPage->m_pvoUseUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffset;
			pPage->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffset;
			pPage->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiff;
			pPage->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffset;
			pPage->m_pvoUpperThresholdMinPlusOffset			 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffset;

			pPage->m_pvoEdgeDirection			= &l_pAnalyzer->m_voEdgeADirection;
			pPage->m_pvoEdgeSelect				= &l_pAnalyzer->m_voEdgeAEdgeSelect;
			pPage->m_pvoEdgePolarisation		= &l_pAnalyzer->m_voEdgeAPolarisation;
			pPage->m_pvoEdgeSelectThisValue		= &l_pAnalyzer->m_voEdgeAEdgeSelectThisValue;
			pPage->m_pvoEdgeIsFixedEdgeMarker	= &l_pAnalyzer->m_voEdgeAIsFixedEdgeMarker;
			pPage->m_pvoEdgePosition			= &l_pAnalyzer->m_voEdgeAPosition;
			pPage->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeAPositionOffsetValue;
			pPage->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPage->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			//set enable flags
			pPage->m_bEnableDirection = l_pAnalyzer->m_bEnableDirection;
			pPage->m_bEnableEdgeSelect = l_pAnalyzer->m_bEnableEdgeSelect;
			pPage->m_bEnablePolarisation = l_pAnalyzer->m_bEnablePolarisation;
			pPage->m_bEnablePosition = l_pAnalyzer->m_bEnablePosition;
			pPage->m_bEnableThreshold = l_pAnalyzer->m_bEnableThreshold;

			measureDialog.AddPage( pPage );
			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance object is updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}
				
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = FALSE;
			
			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPage )
			delete pPage;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVHistogramAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVHistogramAnalyzerClass* l_pAnalyzer = dynamic_cast< SVHistogramAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVHistogramAnalyzerSetupClass dlg;
		dlg.m_pAnalyzer = l_pAnalyzer;
		dlg.m_histogram.SetScale(static_cast<histogram::scale>(l_pAnalyzer->m_scale));
		dlg.m_histogram.SetPeakThreshold(l_pAnalyzer->m_threshold);
		dlg.m_histogram.SetPeakDefault(l_pAnalyzer->m_default);
		dlg.m_histogram.SetDisplay(l_pAnalyzer->m_accumulate?histogram::accumulated:histogram::normal);
		dlg.m_histogram.SetMinHeight(l_pAnalyzer->m_minHeight);
		dlg.m_histogram.SetValleyDefault(l_pAnalyzer->m_valleyDefault);
		dlg.m_histogram.SetValleyLow(l_pAnalyzer->m_lowValley);
		dlg.m_histogram.SetValleyHigh(l_pAnalyzer->m_highValley);
		dlg.m_histogram.SetFixedHeight(l_pAnalyzer->m_fixedHeight);
		dlg.m_histogram.SetHeightOption(l_pAnalyzer->m_dynamicHeight?histogram::dynamic:histogram::fixed);
		dlg.m_histogram.Clip(l_pAnalyzer->m_rangeStart, l_pAnalyzer->m_rangeEnd);

		if (IDOK == dlg.DoModal())
		{
			l_pAnalyzer->m_rangeStart = dlg.m_histogram.GetLowClip();
			l_pAnalyzer->m_rangeEnd = dlg.m_histogram.GetHighClip();
			l_pAnalyzer->m_scale = dlg.m_histogram.GetScale();
			l_pAnalyzer->m_dynamicHeight = dlg.m_histogram.GetHeightOption() == histogram::dynamic;
			l_pAnalyzer->m_histogram.SetHeightOption(dlg.m_histogram.GetHeightOption());
			l_pAnalyzer->m_histogram.SetScale(l_pAnalyzer->m_scale?histogram::logarithmic:histogram::linear);
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
			l_pAnalyzer->m_histogram.SetDisplay(l_pAnalyzer->m_accumulate?histogram::accumulated:histogram::normal);
			l_pAnalyzer->m_histogram.Clip(l_pAnalyzer->m_rangeStart, l_pAnalyzer->m_rangeEnd);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVLinearAnalyzerClass* l_pAnalyzer = dynamic_cast< SVLinearAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	SVInspectionProcess* pInspection( nullptr );

	if( nullptr != l_pAnalyzer && nullptr != (pInspection = l_pAnalyzer->GetInspection()) )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + _T( " " ) + strTitle;

		SVIPDoc* pIPDoc( nullptr);

		pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pInspection->GetUniqueObjectID() );

		SVLinearEdgeProcessingClass *l_psvEdgeA = l_pAnalyzer->GetEdgeA();
		SVLinearEdgeProcessingClass *l_psvEdgeB = l_pAnalyzer->GetEdgeB();

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );

		measureDialog.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		SVEdgeMarkerAdjustmentPageClass *pPageA = NULL;
		SVEdgeMarkerAdjustmentPageClass *pPageB = NULL;

		if( l_psvEdgeA != NULL )
		{
			pPageA = new SVProfileEdgeMarkerAdjustmentPageClass( IDS_EDGE_A );
		}

		if( l_psvEdgeB != NULL )
		{
			pPageB = new SVProfileEdgeMarkerAdjustmentPageClass( IDS_EDGE_B );
		}

		if( nullptr != pIPDoc && ( pPageA != NULL || pPageB != NULL ) )
		{
			if( pPageA != NULL && l_psvEdgeA != NULL )
			{
				pPageA->PCurrentAnalyzer = l_pAnalyzer;
				pPageA->SetNormalizer( l_pAnalyzer->m_svNormalizer );
				pPageA->m_bEdgeA = TRUE;

				pPageA->m_pvoUseLowerThresholdSelectable = &l_psvEdgeA->m_svUseLowerThresholdSelectable;
				pPageA->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_psvEdgeA->m_svUseLowerThresholdMaxMinusPercentDiff;		
				pPageA->m_pvoUseLowerThresholdMaxMinusOffset = &l_psvEdgeA->m_svUseLowerThresholdMaxMinusOffset;		
				pPageA->m_pvoUseLowerThresholdMinPlusOffset = &l_psvEdgeA->m_svUseLowerThresholdMinPlusOffset;			
				pPageA->m_pvoLowerThresholdMaxMinusPercentDiff = &l_psvEdgeA->m_svLowerMaxMinusPercentDiffValue;		
				pPageA->m_pvoLowerThresholdMaxMinusOffset = &l_psvEdgeA->m_svLowerMaxMinusOffsetValue;		
				pPageA->m_pvoLowerThresholdMinPlusOffset = &l_psvEdgeA->m_svLowerMinPlusOffsetValue;		

				pPageA->m_pvoUseUpperThresholdSelectable = &l_psvEdgeA->m_svUseUpperThresholdSelectable;		
				pPageA->m_pvoUseUpperThresholdMaxMinusPercentDiff = &l_psvEdgeA->m_svUseUpperThresholdMaxMinusPercentDiff;		
				pPageA->m_pvoUseUpperThresholdMaxMinusOffset = &l_psvEdgeA->m_svUseUpperThresholdMaxMinusOffset;		
				pPageA->m_pvoUseUpperThresholdMinPlusOffset = &l_psvEdgeA->m_svUseUpperThresholdMinPlusOffset;		
				pPageA->m_pvoUpperThresholdMaxMinusPercentDiff = &l_psvEdgeA->m_svUpperMaxMinusPercentDiffValue;		
				pPageA->m_pvoUpperThresholdMaxMinusOffset = &l_psvEdgeA->m_svUpperMaxMinusOffsetValue;		
				pPageA->m_pvoUpperThresholdMinPlusOffset = &l_psvEdgeA->m_svUpperMinPlusOffsetValue;
				pPageA->m_pvoEdgeDirection = &l_psvEdgeA->m_svDirection;
				pPageA->m_pvoEdgeSelect	= &l_psvEdgeA->m_svEdgeSelect;
				pPageA->m_pvoEdgePolarisation	= &l_psvEdgeA->m_svPolarisation;
				pPageA->m_pvoEdgeSelectThisValue = &l_psvEdgeA->m_svEdgeSelectThisValue;
				pPageA->m_pvoEdgeIsFixedEdgeMarker = &l_psvEdgeA->m_svIsFixedEdgeMarker;
				pPageA->m_pvoEdgePosition = &l_psvEdgeA->m_svPosition;
				pPageA->m_pvoEdgePositionOffsetValue = &l_psvEdgeA->m_svPositionOffsetValue;
				pPageA->m_pvoEdgeLowerThresholdValue = &l_psvEdgeA->m_svLowerThresholdValue;
				pPageA->m_pvoEdgeUpperThresholdValue = &l_psvEdgeA->m_svUpperThresholdValue;

				//set enable flags
				pPageA->m_bEnableDirection = l_pAnalyzer->m_bEnableDirection;
				pPageA->m_bEnableEdgeSelect = l_pAnalyzer->m_bEnableEdgeSelect;
				pPageA->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
				pPageA->m_bEnablePosition	= l_pAnalyzer->m_bEnablePosition;
				pPageA->m_bEnableThreshold = l_pAnalyzer->m_bEnableThreshold;

				measureDialog.AddPage( pPageA );
			}

			if( pPageB != NULL && l_psvEdgeB != NULL )
			{
				pPageB->PCurrentAnalyzer = l_pAnalyzer;
				pPageB->SetNormalizer( l_pAnalyzer->m_svNormalizer );
				pPageB->m_bEdgeA = FALSE;

				pPageB->m_pvoUseLowerThresholdSelectable = &l_psvEdgeB->m_svUseLowerThresholdSelectable;
				pPageB->m_pvoUseLowerThresholdMaxMinusPercentDiff = &l_psvEdgeB->m_svUseLowerThresholdMaxMinusPercentDiff;		
				pPageB->m_pvoUseLowerThresholdMaxMinusOffset = &l_psvEdgeB->m_svUseLowerThresholdMaxMinusOffset;		
				pPageB->m_pvoUseLowerThresholdMinPlusOffset = &l_psvEdgeB->m_svUseLowerThresholdMinPlusOffset;			
				pPageB->m_pvoLowerThresholdMaxMinusPercentDiff = &l_psvEdgeB->m_svLowerMaxMinusPercentDiffValue;		
				pPageB->m_pvoLowerThresholdMaxMinusOffset = &l_psvEdgeB->m_svLowerMaxMinusOffsetValue;		
				pPageB->m_pvoLowerThresholdMinPlusOffset = &l_psvEdgeB->m_svLowerMinPlusOffsetValue;		

				pPageB->m_pvoUseUpperThresholdSelectable = &l_psvEdgeB->m_svUseUpperThresholdSelectable;		
				pPageB->m_pvoUseUpperThresholdMaxMinusPercentDiff = &l_psvEdgeB->m_svUseUpperThresholdMaxMinusPercentDiff;		
				pPageB->m_pvoUseUpperThresholdMaxMinusOffset = &l_psvEdgeB->m_svUseUpperThresholdMaxMinusOffset;		
				pPageB->m_pvoUseUpperThresholdMinPlusOffset = &l_psvEdgeB->m_svUseUpperThresholdMinPlusOffset;		
				pPageB->m_pvoUpperThresholdMaxMinusPercentDiff = &l_psvEdgeB->m_svUpperMaxMinusPercentDiffValue;		
				pPageB->m_pvoUpperThresholdMaxMinusOffset = &l_psvEdgeB->m_svUpperMaxMinusOffsetValue;		
				pPageB->m_pvoUpperThresholdMinPlusOffset = &l_psvEdgeB->m_svUpperMinPlusOffsetValue;		

				pPageB->m_pvoEdgeDirection = &l_psvEdgeB->m_svDirection;
				pPageB->m_pvoEdgeSelect = &l_psvEdgeB->m_svEdgeSelect;
				pPageB->m_pvoEdgePolarisation = &l_psvEdgeB->m_svPolarisation;
				pPageB->m_pvoEdgeSelectThisValue = &l_psvEdgeB->m_svEdgeSelectThisValue;
				pPageB->m_pvoEdgeIsFixedEdgeMarker = &l_psvEdgeB->m_svIsFixedEdgeMarker;
				pPageB->m_pvoEdgePosition = &l_psvEdgeB->m_svPosition;
				pPageB->m_pvoEdgePositionOffsetValue = &l_psvEdgeB->m_svPositionOffsetValue;
				pPageB->m_pvoEdgeLowerThresholdValue = &l_psvEdgeB->m_svLowerThresholdValue;
				pPageB->m_pvoEdgeUpperThresholdValue = &l_psvEdgeB->m_svUpperThresholdValue;

				//set enable flags
				pPageB->m_bEnableDirection = l_pAnalyzer->m_bEnableDirection;
				pPageB->m_bEnableEdgeSelect = l_pAnalyzer->m_bEnableEdgeSelect;
				pPageB->m_bEnablePolarisation = l_pAnalyzer->m_bEnablePolarisation;
				pPageB->m_bEnablePosition = l_pAnalyzer->m_bEnablePosition;
				pPageB->m_bEnableThreshold = l_pAnalyzer->m_bEnableThreshold;

				measureDialog.AddPage( pPageB );
			}

			l_pAnalyzer->MakeDirty();

			BOOL l_bShowA = FALSE;
			BOOL l_bShowB = FALSE;

			l_pAnalyzer->m_svShowAllEdgeAOverlays.GetValue( l_bShowA );
			l_pAnalyzer->m_svShowAllEdgeBOverlays.GetValue( l_bShowB );


			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance objects are updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}

			try
			{
				if( l_pAnalyzer->m_svShowAllEdgeAOverlays.GetOwner() != NULL )
				{
					l_pAnalyzer->GetInspection()->AddInputRequest( &l_pAnalyzer->m_svShowAllEdgeAOverlays, l_bShowA );
				}

				if( l_pAnalyzer->m_svShowAllEdgeBOverlays.GetOwner() != NULL )
				{
					l_pAnalyzer->GetInspection()->AddInputRequest( &l_pAnalyzer->m_svShowAllEdgeBOverlays, l_bShowB );
				}

				l_pAnalyzer->GetInspection()->AddInputRequestMarker();

				SVToolClass* pTool = l_pAnalyzer->GetTool();

				SVGUID l_ToolId;

				if( pTool != NULL )
				{
					l_ToolId = pTool->GetUniqueObjectID();
				}

				GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
				SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

				l_Status = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
			}
			catch( ... )
			{
				l_Status = E_FAIL;
				ASSERT( FALSE );
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPageA )
			delete pPageA;

		if( pPageB )
			delete pPageB;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVLinearMeasurementLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVLinearMeasurementLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVLinearMeasurementLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + _T( " " ) + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		SVEdgeMarkerAdjustmentPageClass* pPageA = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_A );
		SVEdgeMarkerAdjustmentPageClass* pPageB = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_B );

		if( pIPDoc && pLine && pPageA && pPageB )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = TRUE;

			pPageA->PCurrentAnalyzer = l_pAnalyzer;
			pPageB->PCurrentAnalyzer = l_pAnalyzer;
			pPageA->SetNormalizer( pLine->m_svNormalizer );
			pPageB->SetNormalizer( pLine->m_svNormalizer );
			pPageA->m_bEdgeA		 = TRUE;
			pPageB->m_bEdgeA		 = FALSE;

			pPageA->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableA;
			pPageA->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetA;			
			pPageA->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetA;		

			pPageA->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetA;		
			pPageA->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetA;		

			pPageA->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeADirection;
			pPageA->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeAEdgeSelect;
			pPageA->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeAPolarisation;
			pPageA->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeAEdgeSelectThisValue;
			pPageA->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeAIsFixedEdgeMarker;
			pPageA->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeAPosition;
			pPageA->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeAPositionOffsetValue;
			pPageA->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPageA->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			//set enable flags
			pPageA->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageA->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageA->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageA->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageA->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			pPageB->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableB;
			pPageB->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetB;			
			pPageB->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetB;		

			pPageB->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetB;		
			pPageB->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetB;		

			pPageB->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeBDirection;
			pPageB->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeBEdgeSelect;
			pPageB->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeBPolarisation;
			pPageB->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeBEdgeSelectThisValue;
			pPageB->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeBIsFixedEdgeMarker;
			pPageB->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeBPosition;
			pPageB->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeBPositionOffsetValue;
			pPageB->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeBLowerThresholdValue;
			pPageB->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeBUpperThresholdValue;

			//set enable flags
			pPageB->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageB->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageB->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageB->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageB->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			measureDialog.AddPage( pPageA );
			measureDialog.AddPage( pPageB );

			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance objects are updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}

			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = FALSE;
			
			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPageA )
			delete pPageA;

		if( pPageB )
			delete pPageB;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVLineROIClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVLineROIClass* l_pLine = dynamic_cast< SVLineROIClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pLine != NULL )
	{
		// Route call to all line output users...
		SVLineClass* pOutputLine  = l_pLine->getOutputLine();
		if( pOutputLine )
		{
			SVOutObjectInfoStruct& rOutInfo = pOutputLine->GetObjectOutputInfo();

			long l_lCount = static_cast<long>(rOutInfo.GetInputSize());

			for( int i = 0; i < l_lCount; ++ i )
			{
				SVInObjectInfoStruct& rUserInInfo = rOutInfo.GetInputAt( i );

				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rUserInInfo.UniqueObjectID );

				if( pObject != NULL )
				{
					SVSetupDialogManager::Instance().SetupDialog( pObject->GetClassID(), pObject->GetUniqueObjectID(), PParentWnd );
				}
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

HRESULT SVSetupDialogManager::SVLuminanceAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVLuminanceAnalyzerClass* l_pAnalyzer = dynamic_cast< SVLuminanceAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVLuminanceAnalyzerSetupClass dlg;
		dlg.m_pAnalyzer = l_pAnalyzer;
		dlg.DoModal();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVMaximumBackgroundObjectLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVMaximumBackgroundObjectLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVMaximumBackgroundObjectLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + SV_TSTR_SPACE + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		SVEdgeMarkerAdjustmentPageClass* pPageA = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_A );
		SVEdgeMarkerAdjustmentPageClass* pPageB = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_B );

		if( pIPDoc && pLine && pPageA && pPageB )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = TRUE;

			pPageA->PCurrentAnalyzer = l_pAnalyzer;
			pPageB->PCurrentAnalyzer = l_pAnalyzer;
			pPageA->SetNormalizer( pLine->m_svNormalizer );
			pPageB->SetNormalizer( pLine->m_svNormalizer );
			pPageA->m_bEdgeA		 = TRUE;
			pPageB->m_bEdgeA		 = FALSE;

			pPageA->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableA;
			pPageA->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetA;			
			pPageA->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetA;		

			pPageA->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetA;		
			pPageA->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetA;		

			pPageA->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeADirection;
			pPageA->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeAEdgeSelect;
			pPageA->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeAPolarisation;
			pPageA->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeAEdgeSelectThisValue;
			pPageA->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeAIsFixedEdgeMarker;
			pPageA->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeAPosition;
			pPageA->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeAPositionOffsetValue;
			pPageA->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPageA->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			//set enable flags
			pPageA->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageA->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageA->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageA->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageA->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			pPageB->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableB;
			pPageB->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetB;			
			pPageB->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetB;		

			pPageB->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetB;		
			pPageB->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetB;		

			pPageB->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeBDirection;
			pPageB->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeBEdgeSelect;
			pPageB->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeBPolarisation;
			pPageB->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeBEdgeSelectThisValue;
			pPageB->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeBIsFixedEdgeMarker;
			pPageB->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeBPosition;
			pPageB->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeBPositionOffsetValue;
			pPageB->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeBLowerThresholdValue;
			pPageB->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeBUpperThresholdValue;

			//set enable flags
			pPageB->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageB->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageB->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageB->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageB->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			measureDialog.AddPage( pPageA );
			measureDialog.AddPage( pPageB );
			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance objects are updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}

			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = FALSE;
			
			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPageA )
			delete pPageA;

		if( pPageB )
			delete pPageB;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVMaximumForegroundObjectLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVMaximumForegroundObjectLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVMaximumForegroundObjectLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + SV_TSTR_SPACE + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		SVEdgeMarkerAdjustmentPageClass* pPageA = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_A );
		SVEdgeMarkerAdjustmentPageClass* pPageB = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_B );

		if( pIPDoc && pLine && pPageA && pPageB )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = TRUE;

			pPageA->PCurrentAnalyzer = l_pAnalyzer;
			pPageB->PCurrentAnalyzer = l_pAnalyzer;
			pPageA->SetNormalizer( pLine->m_svNormalizer );
			pPageB->SetNormalizer( pLine->m_svNormalizer );
			pPageA->m_bEdgeA		 = TRUE;
			pPageB->m_bEdgeA		 = FALSE;

			pPageA->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableA;
			pPageA->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetA;			
			pPageA->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetA;		

			pPageA->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetA;		
			pPageA->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetA;		

			pPageA->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeADirection;
			pPageA->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeAEdgeSelect;
			pPageA->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeAPolarisation;
			pPageA->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeAEdgeSelectThisValue;
			pPageA->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeAIsFixedEdgeMarker;
			pPageA->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeAPosition;
			pPageA->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeAPositionOffsetValue;
			pPageA->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPageA->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			//set enable flags
			pPageA->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageA->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageA->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageA->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageA->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			pPageB->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableB;
			pPageB->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetB;			
			pPageB->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetB;		

			pPageB->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetB;		
			pPageB->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetB;		

			pPageB->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeBDirection;
			pPageB->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeBEdgeSelect;
			pPageB->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeBPolarisation;
			pPageB->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeBEdgeSelectThisValue;
			pPageB->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeBIsFixedEdgeMarker;
			pPageB->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeBPosition;
			pPageB->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeBPositionOffsetValue;
			pPageB->m_pvoEdgeLowerThresholdValue  = &l_pAnalyzer->m_voEdgeBLowerThresholdValue;
			pPageB->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeBUpperThresholdValue;

			//set enable flags
			pPageB->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageB->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageB->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageB->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageB->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			measureDialog.AddPage( pPageA );
			measureDialog.AddPage( pPageB );
			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance objects are updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}

			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = FALSE;
			
			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPageA )
			delete pPageA;

		if( pPageB )
			delete pPageB;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVMaximumObjectLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVMaximumObjectLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVMaximumObjectLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + SV_TSTR_SPACE + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		SVEdgeMarkerAdjustmentPageClass* pPageA = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_A );
		SVEdgeMarkerAdjustmentPageClass* pPageB = new SVEdgeMarkerAdjustmentPageClass( IDS_EDGE_B );

		BOOL BRetVal = FALSE;
		if( pIPDoc && pLine && pPageA && pPageB )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = TRUE;

			pPageA->PCurrentAnalyzer = l_pAnalyzer;
			pPageB->PCurrentAnalyzer = l_pAnalyzer;
			pPageA->SetNormalizer( pLine->m_svNormalizer );
			pPageB->SetNormalizer( pLine->m_svNormalizer );
			pPageA->m_bEdgeA		 = TRUE;
			pPageB->m_bEdgeA		 = FALSE;

			pPageA->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableA;
			pPageA->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetA;			
			pPageA->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetA;		
			pPageA->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetA;		

			pPageA->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetA;		
			pPageA->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffA;		
			pPageA->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetA;		
			pPageA->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetA;		

			pPageA->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeADirection;
			pPageA->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeAEdgeSelect;
			pPageA->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeAPolarisation;
			pPageA->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeAEdgeSelectThisValue;
			pPageA->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeAIsFixedEdgeMarker;
			pPageA->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeAPosition;
			pPageA->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeAPositionOffsetValue;
			pPageA->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPageA->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			//set enable flags
			pPageA->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageA->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageA->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageA->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageA->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			pPageB->m_pvoUseLowerThresholdSelectable		 = &l_pAnalyzer->m_voUseLowerThresholdSelectableB;
			pPageB->m_pvoUseLowerThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseLowerThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffsetB;			
			pPageB->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffsetB;		
			pPageB->m_pvoLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffsetB;		

			pPageB->m_pvoUseUpperThresholdSelectable		 = &l_pAnalyzer->m_voUseUpperThresholdSelectableB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusPercentDiff= &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUseUpperThresholdMaxMinusOffset	 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffsetB;		
			pPageB->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiffB;		
			pPageB->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffsetB;		
			pPageB->m_pvoUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffsetB;		

			pPageB->m_pvoEdgeDirection			 = &l_pAnalyzer->m_voEdgeBDirection;
			pPageB->m_pvoEdgeSelect				 = &l_pAnalyzer->m_voEdgeBEdgeSelect;
			pPageB->m_pvoEdgePolarisation		 = &l_pAnalyzer->m_voEdgeBPolarisation;
			pPageB->m_pvoEdgeSelectThisValue	 = &l_pAnalyzer->m_voEdgeBEdgeSelectThisValue;
			pPageB->m_pvoEdgeIsFixedEdgeMarker	 = &l_pAnalyzer->m_voEdgeBIsFixedEdgeMarker;
			pPageB->m_pvoEdgePosition			 = &l_pAnalyzer->m_voEdgeBPosition;
			pPageB->m_pvoEdgePositionOffsetValue = &l_pAnalyzer->m_voEdgeBPositionOffsetValue;
			pPageB->m_pvoEdgeLowerThresholdValue = &l_pAnalyzer->m_voEdgeBLowerThresholdValue;
			pPageB->m_pvoEdgeUpperThresholdValue = &l_pAnalyzer->m_voEdgeBUpperThresholdValue;

			//set enable flags
			pPageB->m_bEnableDirection	 = l_pAnalyzer->m_bEnableDirection;
			pPageB->m_bEnableEdgeSelect	 = l_pAnalyzer->m_bEnableEdgeSelect;
			pPageB->m_bEnablePolarisation= l_pAnalyzer->m_bEnablePolarisation;
			pPageB->m_bEnablePosition	 = l_pAnalyzer->m_bEnablePosition;
			pPageB->m_bEnableThreshold	 = l_pAnalyzer->m_bEnableThreshold;

			measureDialog.AddPage( pPageA );
			measureDialog.AddPage( pPageB );
			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance objects are updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}

			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayAnalyzeLine = FALSE;
			
			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPageA )
			delete pPageA;

		if( pPageB )
			delete pPageB;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVOCRAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
#ifdef _WIN64
	AfxMessageBox("OCR OCV Analyzer is not supported on this platform.");
	return E_FAIL;
#else
	HRESULT l_Status = S_OK;

	SVOCRAnalyzerClass* l_pAnalyzer = dynamic_cast< SVOCRAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVOCRAnalyzeResultClass * pOCRResult = ( SVOCRAnalyzeResultClass* ) l_pAnalyzer->GetResultObject();
		SVToolClass * pTool = l_pAnalyzer->GetTool();
		SVInspectionProcess* pInspection = l_pAnalyzer->GetInspection();

		if( nullptr != pOCRResult && nullptr != pTool && nullptr != pInspection )
		{
			SVOCRDialogClass dlg;

			SVIPDoc* pIPDoc = NULL;

			if( l_pAnalyzer->GetInspection() != NULL )
			{
				pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
			}

			dlg.pTool = pTool;
			dlg.pOCRAnalyzerResult = pOCRResult;
			dlg.pDocument = pIPDoc;

			if ( dlg.DoModal() == IDOK )
			{
				pOCRResult->LoadMatchString();

				// Make sure the OCR parameters string for scripting 
				// to be saved as part of the configuration is updated.
				//
				pOCRResult->UpdateOCRScriptString();
				
				if ( pOCRResult->GenerateFontModel() )
				{
					//
					//
					// Run the toolset once in case another window is using the
					// same OCR Font Training file.
					// 16 Dec 1999 - frb (100)
					//
					// Reset all objects...
					bool bOk = SVSendMessage( (SVObjectClass *)pInspection->GetToolSet(), 
															 SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;
					if ( bOk )
					{
						GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
						SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

						l_Status = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
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
#endif
}

HRESULT SVSetupDialogManager::SVOCRGrayAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
#ifdef _WIN64
	AfxMessageBox("OCR OCV Gray Scale Analyzer is not supported on this platform.");
	return E_FAIL;
#else
	HRESULT l_Status = S_OK;

	SVOCRGrayAnalyzerClass* l_pAnalyzer = dynamic_cast< SVOCRGrayAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVOCRGrayAnalyzeResultClass* pOCRResult = ( SVOCRGrayAnalyzeResultClass* ) l_pAnalyzer->GetResultObject();
		SVToolClass * pTool = l_pAnalyzer->GetTool();
		SVInspectionProcess* pInspection = l_pAnalyzer->GetInspection();

		if( nullptr != pOCRResult && nullptr != pTool && nullptr != pInspection )
		{
			SVOCRGrayDialogClass dlg;

			SVIPDoc* pIPDoc = NULL;

			if( l_pAnalyzer->GetInspection() != NULL )
			{
				pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
			}

			dlg.pTool = pTool;
			dlg.pOCRAnalyzerResult = pOCRResult;
			dlg.pDocument = pIPDoc;

			if ( dlg.DoModal() == IDOK )
			{
				pOCRResult->LoadMatchString();

				//
				// Make sure the OCR parameters string for scripting 
				// to be saved as part of the configuration is updated.
				//
				pOCRResult->UpdateOCRScriptString();

				if ( pOCRResult->GenerateFontModel() )
				{
					//
					// Run the toolset once in case another window is using the
					// same OCR Font Training file.
					// 16 Dec 1999 - frb (100)
					//
					// Reset all objects...
					bool bOk = SVSendMessage( (SVObjectClass *)pInspection->GetToolSet(), 
						 SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;

					if ( bOk )
					{
						GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
						SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

						l_Status = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
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
#endif
}

HRESULT SVSetupDialogManager::SVOCVAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVOCVAnalyzerClass* l_pAnalyzer = dynamic_cast< SVOCVAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVOCVAnalyzeResultClass* pOCVResult = ( SVOCVAnalyzeResultClass* ) l_pAnalyzer->GetResultObject();
		SVToolClass * pTool = l_pAnalyzer->GetTool();
		SVInspectionProcess* pInspection = l_pAnalyzer->GetInspection();
		
		if( nullptr != pOCVResult && nullptr != pTool && nullptr != pInspection )
		{
			SVOCVDialogClass dlg;

			SVIPDoc* pIPDoc = NULL;

			if( l_pAnalyzer->GetInspection() != NULL )
			{
				pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
			}
			
			dlg.pTool = pTool;
			dlg.pOCVAnalyzerResult = pOCVResult;
			dlg.pDocument = pIPDoc;
			
			if( dlg.DoModal() == IDOK )
			{
				if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
				{
					pOCVResult->LoadMatchString();
					
					//
					// Make sure the OCV parameters string for scripting 
					// to be saved as part of the configuration is updated.
					//
					if( pOCVResult->GenerateFontModel() )
					{			
						//
						// Run the toolset once in case another window is using the
						// same OCV Font Training file.
						// 16 Dec 1999 - frb (100)
						//
						// Reset all objects...
						bool bOk = SVSendMessage( (SVObjectClass *) pInspection->GetToolSet(), 
											SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;
						if( bOk )
						{
							GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID() );
							SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

							l_Status = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
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

HRESULT SVSetupDialogManager::SVPixelAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* pParentWnd )
{
	HRESULT l_Status = S_OK;

	SVPixelAnalyzerClass* l_pAnalyzer = dynamic_cast< SVPixelAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		SVIPDoc* l_pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );

			if( l_pIPDoc != NULL )
			{
				l_pIPDoc->SetModifiedFlag();
			}
		}

		//SVPixelAnalyzerSetupClass   Dlg (this, pParentWnd);
		SVPixelAnalyzerSetupClass   Dlg (l_pAnalyzer, pParentWnd);

		Dlg.DoModal();
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVPatternAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* pParentWnd )
{
	HRESULT l_Status = S_OK;

	SVPatternAnalyzerClass* l_pAnalyzer = dynamic_cast< SVPatternAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	SVInspectionProcess* pInspection( nullptr );

	if( nullptr != l_pAnalyzer && nullptr != (pInspection = l_pAnalyzer->GetInspection()) )
	{
		double dParam;
		long lParam;
		CString strValue;
		BOOL bPreProcess = FALSE;
		long lSpeedFactor[5] = { SVValueVeryLow, SVValueLow, SVValueMedium, SVValueHigh, SVValueVeryHigh };
		int index;

		SVPatAnalyzeSetupDlgSheet SetupDlgSheet("Pattern Analyzer Setup", pParentWnd);

		SetupDlgSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	// Pages on this sheet
		SVPatModelPageClass ModelPage;
		SVPatGeneralPageClass GeneralPage;
		SVPatAdvancedPageClass AdvancedPage;
		
		// Model Selection values
		ModelPage.m_nXPos = l_pAnalyzer->m_lpatModelOriginX;
		ModelPage.m_nYPos = l_pAnalyzer->m_lpatModelOriginY;
		ModelPage.m_lModelWidth = l_pAnalyzer->m_lpatModelWidth;
		ModelPage.m_lModelHeight = l_pAnalyzer->m_lpatModelHeight;
		l_pAnalyzer->GetModelImageFileName(ModelPage.m_strModelName); 

		// General page
		l_pAnalyzer->msv_lpatMaxOccurances.GetValue(lParam); 
		GeneralPage.m_nOccurances = (int)lParam;
		l_pAnalyzer->msv_dpatAcceptanceThreshold.GetValue(dParam);
		GeneralPage.m_lAcceptance = (long)dParam;
		l_pAnalyzer->msv_dpatCertaintyThreshold.GetValue(dParam); 
		GeneralPage.m_lCertainty = (long)dParam;
		l_pAnalyzer->msv_bpatSearchAngleMode.GetValue(GeneralPage.m_bAngleSearch); 
		l_pAnalyzer->msv_lpatAccuracy.GetValue(lParam);
		for(index = 0; index < 4; index++)
		{
			if(lSpeedFactor[index] == lParam)
			{
				GeneralPage.m_nPosAccuracyIndex = index - 1;
				break;
			}
		}
		l_pAnalyzer->msv_lpatSpeed.GetValue(lParam);
		for(index = 0; index < 4; index++)
		{
			if(lSpeedFactor[index] == lParam)
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
		if(GeneralPage.m_dAngularAccuracy == SVValueDefault || GeneralPage.m_dAngularAccuracy == SVValueDisable )
		{
			GeneralPage.m_dAngularAccuracy = 0.1;
		}

		double l_dInterpMode = 0;
		l_pAnalyzer->msv_dpatAngleInterpolation.GetValue(l_dInterpMode);
		GeneralPage.m_dInterpolationMode = static_cast<SVImageOperationTypeEnum>( static_cast<long>(l_dInterpMode) );
		
		ModelPage.m_pPatAnalyzer = l_pAnalyzer;
		GeneralPage.m_pPatAnalyzer = l_pAnalyzer;

		l_pAnalyzer->msv_bpatCircularOverscan.GetValue(ModelPage.m_bCircularOverscan);
		l_pAnalyzer->msv_szModelImageFile.GetValue( ModelPage.m_strModelName );

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

		if(SetupDlgSheet.DoModal() == IDOK)
		{
			l_pAnalyzer->m_bAngleAccuracy = GeneralPage.m_bAccuracy;

			try
			{
				// Do Model Page
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_szModelImageFile ), static_cast< LPCTSTR >( ModelPage.m_strModelName ) );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_bpatCircularOverscan ), ModelPage.m_bCircularOverscan );

				// Do General Page
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatMaxOccurances ), GeneralPage.m_nOccurances );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAcceptanceThreshold ), GeneralPage.m_lAcceptance );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatCertaintyThreshold ), GeneralPage.m_lCertainty );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatAccuracy ), lSpeedFactor[GeneralPage.m_nPosAccuracyIndex + 1] );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatSpeed ), lSpeedFactor[GeneralPage.m_nSpeedIndex] );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_bpatSearchAngleMode ), GeneralPage.m_bAngleSearch );

				// Do Advanced Page
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatFastFind ), AdvancedPage.m_lFastFind );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatAdditionalCandidates), AdvancedPage.m_lAdditionalCandidates );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatCandidateSpacingXMin), AdvancedPage.m_dCandidateSpacingXMin );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatCandidateSpacingYMin), AdvancedPage.m_dCandidateSpacingYMin );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatPreliminaryAcceptanceThreshold), AdvancedPage.m_dPreliminaryAcceptanceThreshold );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatModelStep), AdvancedPage.m_lModelStep );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatBeginningResolutionLevel), AdvancedPage.m_lBeginningResolutionLevel );
				pInspection->AddInputRequest( &( l_pAnalyzer->msv_lpatFinalResolutionLevel), AdvancedPage.m_lFinalResolutionLevel );
				
				if (GeneralPage.m_bAngleSearch)
				{
					pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatSearchAngle ), GeneralPage.m_dSearchAngle);
					pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAngleDeltaNeg ), GeneralPage.m_dAngleDeltaNegative);
					pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAngleDeltaPos ), GeneralPage.m_dAngleDeltaPositive);
					pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAngleTolerance ), GeneralPage.m_dAngleTolerance );
					if(l_pAnalyzer->m_bAngleAccuracy)
					{
						pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAngleAccuracy ), GeneralPage.m_dAngularAccuracy);
					}
					else
					{
						pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAngleAccuracy ), SVValueDisable );
					}
					pInspection->AddInputRequest( &( l_pAnalyzer->msv_dpatAngleInterpolation ), GeneralPage.m_dInterpolationMode );
				}

				pInspection->AddInputRequestMarker();

				SVToolClass* pTool = l_pAnalyzer->GetTool();

				SVGUID l_ToolId;

				if( pTool != NULL )
				{
					l_ToolId = pTool->GetUniqueObjectID();
				}

				GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( pInspection->GetUniqueObjectID(), l_ToolId );
				SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( pInspection->GetUniqueObjectID(), l_CommandPtr );

				l_Status = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
			}
			catch( ... )
			{
				l_Status = E_FAIL;

				ASSERT( FALSE );
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

	return l_Status;
}

HRESULT SVSetupDialogManager::SVPixelCountingLineAnalyzerClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVPixelCountingLineAnalyzerClass* l_pAnalyzer = dynamic_cast< SVPixelCountingLineAnalyzerClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pAnalyzer != NULL )
	{
		CString strTitle;
		strTitle.LoadString( IDS_ADJUSTMENT_STRING );
		// Get Complete Name up to the tool level...
		strTitle = l_pAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ) + SV_TSTR_SPACE + strTitle;

		SVLineClass* pLine = l_pAnalyzer->getInputLine();

		SVIPDoc* pIPDoc = NULL;

		if( l_pAnalyzer->GetInspection() != NULL )
		{
			pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( l_pAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		SVMeasureAnalyzerAdjustmentSheetClass measureDialog( strTitle );
		//SVThresholdAdjustmentPageClass* pPage = new SVThresholdAdjustmentPageClass;

		SVThresholdAdjustmentPageClass* pPage = new SVThresholdAdjustmentPageClass;

		if( pLine && pIPDoc && pPage )
		{
			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayBWAnalyzeLine = TRUE;

			pPage->PCurrentAnalyzer = l_pAnalyzer;
			pPage->Normalizer		= pLine->m_svNormalizer;
			pPage->PCurrentDocument = pIPDoc;

			pPage->m_pvoUseLowerThresholdSelectable			 = &l_pAnalyzer->m_voUseLowerThresholdSelectable;
			pPage->m_pvoUseLowerThresholdMaxMinusPercentDiff = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusPercentDiff;
			pPage->m_pvoUseLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMaxMinusOffset;
			pPage->m_pvoUseLowerThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseLowerThresholdMinPlusOffset;
			pPage->m_pvoLowerThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voLowerThresholdMaxMinusPercentDiff;
			pPage->m_pvoLowerThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voLowerThresholdMaxMinusOffset;
			pPage->m_pvoLowerThresholdMinPlusOffset			 = &l_pAnalyzer->m_voLowerThresholdMinPlusOffset;

			pPage->m_pvoUseUpperThresholdSelectable			 = &l_pAnalyzer->m_voUseUpperThresholdSelectable;
			pPage->m_pvoUseUpperThresholdMaxMinusPercentDiff = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusPercentDiff;
			pPage->m_pvoUseUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMaxMinusOffset;
			pPage->m_pvoUseUpperThresholdMinPlusOffset		 = &l_pAnalyzer->m_voUseUpperThresholdMinPlusOffset;
			pPage->m_pvoUpperThresholdMaxMinusPercentDiff	 = &l_pAnalyzer->m_voUpperThresholdMaxMinusPercentDiff;
			pPage->m_pvoUpperThresholdMaxMinusOffset		 = &l_pAnalyzer->m_voUpperThresholdMaxMinusOffset;
			pPage->m_pvoUpperThresholdMinPlusOffset			 = &l_pAnalyzer->m_voUpperThresholdMinPlusOffset;

			pPage->m_pvoLowerThreshold = &l_pAnalyzer->m_voEdgeALowerThresholdValue;
			pPage->m_pvoUpperThreshold = &l_pAnalyzer->m_voEdgeAUpperThresholdValue;

			measureDialog.AddPage( pPage );
			l_pAnalyzer->MakeDirty();

			if( measureDialog.DoModal() == IDOK )
			{
				//
				// Make sure the persistance object is updated.
				//
				pIPDoc->SetModifiedFlag();
			}
			else
			{
				l_Status = S_FALSE;
			}

			pLine->m_bDisplayGraph = l_pAnalyzer->m_bDisplayBWAnalyzeLine = FALSE;

			pIPDoc->UpdateAllViews( NULL );
		}
		else
		{
			l_Status = E_FAIL;
		}

		if( pPage )
			delete pPage;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVResultClassSetupDialog( const SVGUID& p_rObjectId, CWnd* PParentWnd )
{
	HRESULT l_Status = S_OK;

	SVResultClass* l_pResult = dynamic_cast< SVResultClass* >( SVObjectManagerClass::Instance().GetObject( p_rObjectId ) );
	
	if( l_pResult != NULL )
	{
		SVRangeClass* pRange = l_pResult->GetResultRange();
		if( pRange )
		{
			RangeXDialogClass dlg( pRange, PParentWnd );
			if( dlg.DoModal() != IDOK )
			{
				l_Status = S_FALSE;
			}
			else
			{
				////mec_Range Call validate for tool which owns the range
				//SVObjectClass*  pObjectclass = pRange;
				//SVObjectClass*  pObjectclassOwner = nullptr;
				//SVToolClass*   pToolclass = nullptr; 
				//while(nullptr != pObjectclass) 
				//{
				//	pObjectclassOwner = pObjectclass->GetOwner();
				//	pToolclass = dynamic_cast<SVToolClass* >(pObjectclassOwner);
				//	if(pToolclass)
				//	{
				//		pToolclass->Validate();
				//		break;
				//	}
				//	else
				//	{
				//		pObjectclass = pObjectclassOwner;
				//	}

				//} 

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

