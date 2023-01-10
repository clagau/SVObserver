//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxBlobInterface.h"
#include "AnalyzerOperators/Analyzer.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGui/SVLuminanceAnalyzerDlg.h"
#include "SVOGui/SVBarCodeProperties.h"
#include "AnalyzerOperators/BarCodeAnalyzer.h"
#include "Operators/SVBarCodeResult.h"
#include "AnalyzerOperators/BlobAnalyzer.h"
#include "SVOGui/SVBlobAnalyzerDialog.h"
#include "Tools/SVColorTool.h"
#include "AnalyzerOperators/HistogramAnalyzer.h"
#include "SVHistogramAnalyzerSetup.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "AnalyzerOperators/LinearAnalyzer.h"
#include "Operators/SVLinearEdgeProcessingClass.h"
#include "SVMeasureAnalyzerAdjustmentSheet.h"
#include "AnalyzerOperators/OCVAnalyzer.h"
#include "Operators/SVOCVAnalyzerResult.h"
#include "SVOGui/SVOCVSheet.h"
#include "SVOGui/SVPatAdvancedPageClass.h"
#include "SVOGui/SVPatAnalyzeSetupDlgSheet.h"
#include "SVOGui/SVPatGeneralPageClass.h"
#include "SVOGui/SVPatternAnalyzerModelPage.h"
#include "AnalyzerOperators/PatternAnalyzer.h"
#include "AnalyzerOperators/PixelAnalyzer.h"
#include "SVOGui/SVPixelAnalyzerDlg.h"
#include "SVOGui/SVProfileEdgeMarkerAdjustmentPage.h"
#include "Operators/SVResult.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVOGui/RangeXDialogClass.h"
#include "Operators/SVRange.h"
#include "SVToolSet.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "InspectionCommands/CommandFunction.h"
#include "SVOGui/BlobAnalyzer2Sheet.h"
#include "SVOGui/SVEdgeMarkerAdjustmentPageClass.h"
#pragma endregion Inlcudes

	
SVSetupDialogManager& SVSetupDialogManager::Instance()
{
	static SVSetupDialogManager l_Object;

	return l_Object;
}

HRESULT SVSetupDialogManager::SetupDialog(SvPb::ClassIdEnum classId, uint32_t inspectionId, uint32_t objectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	auto iter = std::find_if(m_SetupDialogs.begin(), m_SetupDialogs.end(), [&classId](const std::pair<SvPb::ClassIdEnum, SVSetupDialogFunction>& rEntry) {return rEntry.first == classId; });

	if (m_SetupDialogs.end() != iter)
	{
		l_Status = (iter->second)(inspectionId, objectId, pParentWnd);
	}
	else
	{
		l_Status = E_NOTIMPL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVBarCodeAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* )
{
	HRESULT l_Status = S_OK;

	SvAo::BarCodeAnalyzer* pAnalyzer = dynamic_cast<SvAo::BarCodeAnalyzer*>(SVObjectManagerClass::Instance().GetObject(objectId));

	SvOp::SVBarCodeResult *pResult{ nullptr };

	if (nullptr != pAnalyzer &&
		nullptr != (pResult = dynamic_cast<SvOp::SVBarCodeResult*>(pAnalyzer->GetResultObject())))
	{
		SVIPDoc* pIPDoc = GetIPDocByInspectionID(inspectionId);

		if (nullptr != pIPDoc)
		{
			pIPDoc->SetModifiedFlag();
		}
		
		SvOg::SVBarCodeProperties dlgProp(inspectionId, objectId, pResult->getObjectId());
		
		int iResult = static_cast<int>(dlgProp.DoModal());

		if (iResult == IDOK)
		{
			dlgProp.SetInspectionData();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVBlobAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SvAo::BlobAnalyzer* pAnalyzer = dynamic_cast<SvAo::BlobAnalyzer*> (SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pAnalyzer)
	{
		SVIPDoc* pIPDoc = GetIPDocByInspectionID(inspectionId);

		if (nullptr != pIPDoc)
		{
			pIPDoc->SetModifiedFlag();

			SvOg::SVBlobAnalyzerDialog dlg(inspectionId, objectId, pParentWnd);

			if (IDOK == dlg.DoModal())
			{
				BOOL l_bIsFillBlob;
				pAnalyzer->m_bvoFillBlobs.GetValue(l_bIsFillBlob);
				SVMatroxBlobInterface::Set(pAnalyzer->m_BlobContextID, SVEBlobSaveRuns, static_cast<long>(l_bIsFillBlob ? SVValueEnable : SVValueDisable));
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
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::BlobAnalyzer2SetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* pParentWnd)
{
	HRESULT status = E_FAIL;
	auto* pAnalyzer = dynamic_cast<SvOl::SVObjectAppClass*> (SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pAnalyzer && nullptr != pAnalyzer->GetTool())
	{
		SvOg::BlobAnalyzer2Sheet dlg(inspectionId, pAnalyzer->GetTool()->getObjectId(), objectId, "Blob Analyzer 2 Adjustment", pParentWnd);
		dlg.DoModal();
		status = S_OK;
	}
	return status;
}

HRESULT SVSetupDialogManager::SVColorToolClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* )
{
	HRESULT l_Status = E_FAIL;

	SvTo::SVColorToolClass* pTool = dynamic_cast<SvTo::SVColorToolClass*>(SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pTool)
	{
		std::string Title = SvUl::LoadStdString(IDS_ADJUSTMENT_STRING);

		// Get Complete Name up to the tool level...
		Title = pTool->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType) + _T(" ") + Title;

		SVIPDoc* pIPDoc = GetIPDocByInspectionID(inspectionId);
		Log_Assert(nullptr != pIPDoc);

		if (nullptr != pIPDoc)
		{
			// Create the Train Color Property Sheet
			SVTADlgColorThresholdSheet dlg(pTool, Title.c_str());
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
	return l_Status;
}

HRESULT SVSetupDialogManager::HistogramAnalyzerSetupDialog(uint32_t, uint32_t objectId, CWnd* )
{
	HRESULT l_Status = S_OK;

	SvAo::HistogramAnalyzer* l_pAnalyzer = dynamic_cast<SvAo::HistogramAnalyzer*>(SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != l_pAnalyzer)
	{
		SVHistogramAnalyzerSetup dlg;
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

HRESULT SVSetupDialogManager::SVLinearAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* )
{
	HRESULT l_Status = S_OK;

	SvAo::LinearAnalyzer* pAnalyzer = dynamic_cast<SvAo::LinearAnalyzer*>(SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pAnalyzer)
	{
		std::string Title = SvUl::LoadStdString(IDS_ADJUSTMENT_STRING);
		// Get Complete Name up to the tool level...
		Title = pAnalyzer->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType) + _T(" ") + Title;

		SVIPDoc* pIPDoc = GetIPDocByInspectionID(inspectionId);

		SvOp::SVLinearEdgeProcessingClass *pEdgeA = pAnalyzer->GetEdgeA();
		SvOp::SVLinearEdgeProcessingClass *pEdgeB = pAnalyzer->GetEdgeB();

		SVMeasureAnalyzerAdjustmentSheet measureDialog(Title.c_str());

		measureDialog.m_psh.dwFlags |= PSH_NOAPPLYNOW;

		SvOg::SVProfileEdgeMarkerAdjustmentPage *pPageA = nullptr;
		SvOg::SVProfileEdgeMarkerAdjustmentPage *pPageB = nullptr;

		if (nullptr != pEdgeA)
		{
			if (SvPb::SVLinearPixelCountingAnalyzerObjectType == pAnalyzer->GetObjectSubType() || SvPb::SVLinearEdgeCountingAnalyzerObjectType == pAnalyzer->GetObjectSubType())
			{
				auto embeddedIds = SvOg::g_EdgeAEmbeddedIds;
				embeddedIds[SvOg::EdgeDistance] = SvPb::NoEmbeddedId;
				pPageA = new SvOg::SVProfileEdgeMarkerAdjustmentPage(inspectionId, pEdgeA->getObjectId(), embeddedIds, objectId, IDS_EDGE_A);
			}
			else
			{
				pPageA = new SvOg::SVProfileEdgeMarkerAdjustmentPage(inspectionId, pEdgeA->getObjectId(), SvOg::g_EdgeAEmbeddedIds, objectId, IDS_EDGE_A);
			}
		}

		if (nullptr != pEdgeB)
		{
			pPageB = new SvOg::SVProfileEdgeMarkerAdjustmentPage(inspectionId, pEdgeB->getObjectId(), SvOg::g_EdgeBEmbeddedIds, objectId, IDS_EDGE_B);
		}

		if (nullptr != pIPDoc && (nullptr != pPageA || nullptr != pPageB))
		{
			if (nullptr != pPageA && nullptr != pEdgeA)
			{
				pPageA->m_bEdgeA = true;
				pPageA->m_existEdgeB = (nullptr != pPageB);

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

HRESULT SVSetupDialogManager::SVLuminanceAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* )
{
	HRESULT l_Status = S_OK;

	SvAo::Analyzer* pAnalyzer = dynamic_cast<SvAo::Analyzer*> (SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pAnalyzer && SvPb::SVLuminanceAnalyzerObjectType == pAnalyzer->GetObjectSubType())
	{
		SvOg::SVLuminanceAnalyzerDlg dlg {inspectionId, objectId};
		dlg.DoModal();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVSetupDialogManager::SVOCVAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* )
{
	HRESULT l_Status = S_OK;

	SvAo::OCVAnalyzer* pAnalyzer = dynamic_cast<SvAo::OCVAnalyzer*>(SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pAnalyzer)
	{
		SvOp::SVOCVAnalyzerResult* pOCVResult = dynamic_cast<SvOp::SVOCVAnalyzerResult*> (pAnalyzer->GetResultObject());
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(inspectionId));

		if (nullptr != pOCVResult && nullptr != pInspection)
		{
			SvOg::SVOCVSheet dlg(inspectionId, pOCVResult->getObjectId());

			if (IDOK == dlg.DoModal())
			{
				SVIPDoc*pIPDoc = GetIPDocByInspectionID(inspectionId);

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
						//The GetToolSet return SVToolSet and need for this the include SVToolSet.h
						bool bOk = pInspection->GetToolSet()->resetAllObjects();
						if (bOk)
						{
							l_Status =  SvCmd::RunOnceSynchronous(inspectionId);
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

HRESULT SVSetupDialogManager::PixelAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd*)
{
	HRESULT l_Status = S_FALSE;

	SvAo::PixelAnalyzer* pAnalyzer = dynamic_cast<SvAo::PixelAnalyzer*> (SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != pAnalyzer)
	{
		SVIPDoc* pIPDoc = GetIPDocByInspectionID(inspectionId);

		if (nullptr != pIPDoc)
		{
			pIPDoc->SetModifiedFlag();

			SvOg::SVPixelAnalyzerDlg Dlg(inspectionId, objectId);
			Dlg.DoModal();
			l_Status = S_OK;
		}
	}
	return l_Status;
}

HRESULT SVSetupDialogManager::SVPatternAnalyzerClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;
	SvAo::PatternAnalyzer* l_pAnalyzer = dynamic_cast<SvAo::PatternAnalyzer*>(SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != l_pAnalyzer)
	{
		double dParam;
		long lParam;
		long lSpeedFactor[5] = {SVValueVeryLow, SVValueLow, SVValueMedium, SVValueHigh, SVValueVeryHigh};
		int index;

		SvOg::SVPatAnalyzeSetupDlgSheet SetupDlgSheet(_T("Pattern Analyzer Setup"), pParentWnd);

		SetupDlgSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
		// Pages on this sheet
		SvOg::SVPatternAnalyzerModelPage ModelPage(inspectionId, l_pAnalyzer->getObjectId());
		SvOg::SVPatGeneralPageClass GeneralPage;
		SvOg::SVPatAdvancedPageClass AdvancedPage;

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
		if (static_cast<SVMatroxBufferValues>(GeneralPage.m_dAngularAccuracy) == SVValueDefault || static_cast<SVMatroxBufferValues>(GeneralPage.m_dAngularAccuracy) == SVValueDisable)
		{
			GeneralPage.m_dAngularAccuracy = 0.1;
		}

		double l_dInterpMode = 0;
		l_pAnalyzer->msv_dpatAngleInterpolation.GetValue(l_dInterpMode);
		GeneralPage.m_dInterpolationMode = static_cast<SVImageOperationTypeEnum> (static_cast<long>(l_dInterpMode));
		SvOi::IObjectClass* pAnalyzerResult = l_pAnalyzer->GetResultObject();

		if (pAnalyzerResult)
		{
			GeneralPage.m_resultClassId = pAnalyzerResult->GetClassID();
			GeneralPage.m_inspectionId = inspectionId;
			GeneralPage.m_resultObjectId = pAnalyzerResult->getObjectId();
		}

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
			SvOgu::ValueController Values {SvOgu::BoundValues{ inspectionId, objectId }};
			Values.Init();

			// Save General Page
			Values.Set<long>(SvPb::PatMaxOccurancesEId, GeneralPage.m_lOccurances);
			Values.Set<double>(SvPb::PatAcceptThresholdEId, static_cast<double> (GeneralPage.m_lAcceptance));
			Values.Set<double>(SvPb::PatCertainThresholdEId, static_cast<double> (GeneralPage.m_lCertainty));
			Values.Set<long>(SvPb::PatAccuracyEId, lSpeedFactor[GeneralPage.m_nPosAccuracyIndex + 1]);
			Values.Set<long>(SvPb::PatSpeedEId, lSpeedFactor[GeneralPage.m_nSpeedIndex]);
			Values.Set<bool>(SvPb::PatSearchAngleModeEId, GeneralPage.m_bAngleSearch ? true : false);

			if (GeneralPage.m_bAngleSearch)
			{
				Values.Set<double>(SvPb::PatSearchAngleEId, GeneralPage.m_dSearchAngle);
				Values.Set<double>(SvPb::PatAngleDeltaNegEId, GeneralPage.m_dAngleDeltaNegative);
				Values.Set<double>(SvPb::PatAngleDeltaPosEId, GeneralPage.m_dAngleDeltaPositive);
				Values.Set<double>(SvPb::PatAngleToleranceEId, GeneralPage.m_dAngleTolerance);
				Values.Set<double>(SvPb::PatAngleInterpolationEId, GeneralPage.m_dInterpolationMode);

				double dAngleAccuracy = l_pAnalyzer->m_bAngleAccuracy ? GeneralPage.m_dAngularAccuracy : static_cast<double> (SVValueDisable);
				Values.Set<double>(SvPb::PatAngleAccuracyEId, dAngleAccuracy);
			}

			// Save Advanced Page
			Values.Set<long>(SvPb::PatFastFindEId, AdvancedPage.m_lFastFind);
			Values.Set<long>(SvPb::PatAdditionalCandidatesEId, AdvancedPage.m_lAdditionalCandidates);
			Values.Set<double>(SvPb::PatCandidateSpacingXMinEId, AdvancedPage.m_dCandidateSpacingXMin);
			Values.Set<double>(SvPb::PatCandidateSpacingYMinEId, AdvancedPage.m_dCandidateSpacingYMin);
			Values.Set<double>(SvPb::PatPreliminaryAcceptanceThresholdEId, AdvancedPage.m_dPreliminaryAcceptanceThreshold);
			Values.Set<long>(SvPb::PatModelStepEId, AdvancedPage.m_lModelStep);
			Values.Set<long>(SvPb::PatBeginningResolutionLevelEId, AdvancedPage.m_lBeginningResolutionLevel);
			Values.Set<long>(SvPb::PatFinalResolutionLevelEId, AdvancedPage.m_lFinalResolutionLevel);

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

HRESULT SVSetupDialogManager::SVResultClassSetupDialog(uint32_t inspectionId, uint32_t objectId, CWnd* pParentWnd)
{
	HRESULT l_Status = S_OK;

	SvOp::SVResult* l_pResult = dynamic_cast<SvOp::SVResult*> (SVObjectManagerClass::Instance().GetObject(objectId));

	if (nullptr != l_pResult)
	{
		SvOp::SVRange* pRange = l_pResult->GetResultRange();
		if (pRange)
		{
			SvOg::RangeXDialogClass dlg(inspectionId, pRange->GetTool()->getObjectId(), pRange->GetParent()->getObjectId(), pRange->getObjectId(), pParentWnd);
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

