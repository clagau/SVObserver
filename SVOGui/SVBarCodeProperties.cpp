//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeProperties
//* .File Name       : $Workfile:   SVBarCodeProperties.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:42:32  $
//******************************************************************************

#include "stdafx.h"

#include "SVBarCodeProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVBarCodeProperties
namespace SvOg
{
	IMPLEMENT_DYNAMIC(SVBarCodeProperties, CPropertySheet)

		SVBarCodeProperties::SVBarCodeProperties(uint32_t inspectionId, uint32_t analyzerId, uint32_t resultId, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
		:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
		, m_inspectionId(inspectionId)
		, m_analyzerId(analyzerId)
		, m_resultId(resultId)
		, m_analyzerValues{ SvOg::BoundValues{ inspectionId, analyzerId } }
		, m_resultValues{ SvOg::BoundValues{ inspectionId, resultId } }
	{
		AddPage(&m_dlgBarCodeGeneral);
		AddPage(&m_dlgBarCodeStringMatch);
		AddPage(&m_dlgBarCodeAttributes);
		AddPage(&m_dlgBarCodeDataMatrix);

		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		requestCmd.mutable_getbarcodetypeinfosrequest();

		HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getbarcodetypeinfosresponse())
		{
			m_dlgBarCodeAttributes.setBarCodeTypeInfos(responseCmd.getbarcodetypeinfosresponse());
			m_dlgBarCodeGeneral.setBarCodeTypeInfos(responseCmd.getbarcodetypeinfosresponse());
		}
		else
		{
			assert(false);
		}
		
		m_analyzerValues.Init();
		m_resultValues.Init();

		m_dlgBarCodeGeneral.SetBarCodeSearchSpeed(m_analyzerValues.Get<long>(SvPb::BCSpeedEId));
		m_dlgBarCodeGeneral.SetBarCodeType(m_analyzerValues.Get<long>(SvPb::BCTypeEId));
		m_dlgBarCodeGeneral.SetOrientation(m_analyzerValues.Get<long>(SvPb::BCOrientationEId));
		m_dlgBarCodeGeneral.SetSkewNegative(m_analyzerValues.Get<long>(SvPb::BCSkewNegativeEId));
		m_dlgBarCodeGeneral.SetSkewPositive(m_analyzerValues.Get<long>(SvPb::BCSkewPositiveEId));
		m_dlgBarCodeGeneral.SetThreshold(m_analyzerValues.Get<long>(SvPb::BCThresholdEId));
		m_dlgBarCodeGeneral.SetForegroundColor(m_analyzerValues.Get<long>(SvPb::BCForegroundColorEId));
		m_dlgBarCodeGeneral.SetBarCodeStringSize(m_analyzerValues.Get<long>(SvPb::BCStringSizeEId));
		m_dlgBarCodeGeneral.SetTimeout(m_analyzerValues.Get<long>(SvPb::BCTimeoutEId));
		m_dlgBarCodeGeneral.SetBarcodeStringFormat(m_analyzerValues.GetEnumTypes(SvPb::BCStringFormatEId), m_analyzerValues.Get<long>(SvPb::BCStringFormatEId));
		m_dlgBarCodeGeneral.SetBarcodeThresholdType(m_analyzerValues.Get<long>(SvPb::BCThresholdTypeEId));

		//for support for the Uneven grid step for DMCs
		m_dlgBarCodeGeneral.SetUnEvenGrid(m_analyzerValues.Get<bool>(SvPb::BCUnevenGridEId));
		m_dlgBarCodeGeneral.SetWarnOnFail(m_analyzerValues.Get<bool>(SvPb::BCWarnOnFailedReadEId));

		m_dlgBarCodeAttributes.SetEncoding(m_analyzerValues.Get<double>(SvPb::BCEncodingEId));
		m_dlgBarCodeAttributes.SetErrCorrection(m_analyzerValues.Get<double>(SvPb::BCErrorCorrectionEId));

		m_dlgBarCodeStringMatch.SetUseSingle(m_resultValues.Get<bool>(SvPb::BCUseSingleMatchStringEId));
		m_dlgBarCodeStringMatch.SetRegExpression(m_analyzerValues.Get<CString>(SvPb::RegExpressionEId));
		m_dlgBarCodeStringMatch.SetSaveInFile(m_analyzerValues.Get<bool>(SvPb::BCSaveStringInFileEId));
		m_dlgBarCodeStringMatch.SetSingleFileName(m_analyzerValues.Get<CString>(SvPb::BCStringFileNameEId));
		m_dlgBarCodeStringMatch.SetUseMultiple(m_resultValues.Get<bool>(SvPb::BCUseMatchStringFileEId));
		m_dlgBarCodeStringMatch.SetMultiFileName(m_resultValues.Get<CString>(SvPb::BCMatchStringFileNameEId));

		m_dlgBarCodeDataMatrix.SetCellX(m_analyzerValues.Get<long>(SvPb::BCCellNumberXEId));
		m_dlgBarCodeDataMatrix.SetCellY(m_analyzerValues.Get<long>(SvPb::BCCellNumberYEId));
		m_dlgBarCodeDataMatrix.SetMinCellSize(m_analyzerValues.Get<long>(SvPb::BCCellMinSizeEId));
		m_dlgBarCodeDataMatrix.SetMaxCellSize(m_analyzerValues.Get<long>(SvPb::BCCellMaxSizeEId));

		//remove the apply button
		m_psh.dwFlags |= PSH_NOAPPLYNOW;
	}


	SVBarCodeProperties::~SVBarCodeProperties()
	{
	}


	BEGIN_MESSAGE_MAP(SVBarCodeProperties, CPropertySheet)
		//{{AFX_MSG_MAP(SVBarCodeProperties)
			// NOTE - the ClassWizard will add and remove mapping macros here.
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void SVBarCodeProperties::SetInspectionData()
	{
		m_analyzerValues.Set<long>(SvPb::BCTypeEId, m_dlgBarCodeGeneral.GetBarCodeType());
		m_analyzerValues.Set<double>(SvPb::BCSpeedEId, m_dlgBarCodeGeneral.GetBarCodeSearchSpeed());
		m_analyzerValues.Set<double>(SvPb::BCOrientationEId, m_dlgBarCodeGeneral.GetOrientation());
		m_analyzerValues.Set<double>(SvPb::BCSkewNegativeEId, m_dlgBarCodeGeneral.GetSkewNegative());
		m_analyzerValues.Set<double>(SvPb::BCSkewPositiveEId, m_dlgBarCodeGeneral.GetSkewPositive());
		m_analyzerValues.Set<double>(SvPb::BCThresholdEId, m_dlgBarCodeGeneral.GetThreshold());
		m_analyzerValues.Set<double>(SvPb::BCForegroundColorEId, m_dlgBarCodeGeneral.GetForegroundColor());
		m_analyzerValues.Set<double>(SvPb::BCStringSizeEId, m_dlgBarCodeGeneral.GetBarCodeStringSize());
		m_analyzerValues.Set<bool>(SvPb::BCWarnOnFailedReadEId, m_dlgBarCodeGeneral.GetWarnedOnFail() ? true : false);
		m_analyzerValues.Set<long>(SvPb::BCTimeoutEId, m_dlgBarCodeGeneral.GetTimeout());
		m_analyzerValues.Set<CString>(SvPb::BCStringFormatEId, m_dlgBarCodeGeneral.GetBarcodeStringFormat());
		m_analyzerValues.Set<long>(SvPb::BCThresholdTypeEId, m_dlgBarCodeGeneral.GetBarcodeThresholdType());
		m_analyzerValues.Set<bool>(SvPb::BCUnevenGridEId, m_dlgBarCodeGeneral.GetUnEvenGrid() ? true : false);

		m_analyzerValues.Set<double>(SvPb::BCEncodingEId, m_dlgBarCodeAttributes.GetEncoding());
		m_analyzerValues.Set<double>(SvPb::BCErrorCorrectionEId, m_dlgBarCodeAttributes.GetErrorCorrection());
		m_analyzerValues.Set<CString>(SvPb::RegExpressionEId, m_dlgBarCodeStringMatch.GetRegExpression());
		m_analyzerValues.Set<bool>(SvPb::BCSaveStringInFileEId, m_dlgBarCodeStringMatch.GetSaveInFile() ? true : false);
		m_analyzerValues.Set<CString>(SvPb::BCStringFileNameEId, m_dlgBarCodeStringMatch.GetSingleFileName());

		m_analyzerValues.Set<double>(SvPb::BCCellNumberXEId, static_cast<double> (m_dlgBarCodeDataMatrix.GetCellX()));
		m_analyzerValues.Set<double>(SvPb::BCCellNumberYEId, static_cast<double> (m_dlgBarCodeDataMatrix.GetCellY()));
		m_analyzerValues.Set<double>(SvPb::BCCellMinSizeEId, static_cast<double> (m_dlgBarCodeDataMatrix.GetMinCellSize()));
		m_analyzerValues.Set<double>(SvPb::BCCellMaxSizeEId, static_cast<double> (m_dlgBarCodeDataMatrix.GetMaxCellSize()));

		m_analyzerValues.Commit();

		m_resultValues.Init();

		m_resultValues.Set<bool>(SvPb::BCUseSingleMatchStringEId, m_dlgBarCodeStringMatch.GetUseSingle() ? true : false);
		m_resultValues.Set<bool>(SvPb::BCUseMatchStringFileEId, m_dlgBarCodeStringMatch.GetUseMultiple() ? true : false);
		m_resultValues.Set<CString>(SvPb::BCMatchStringFileNameEId, m_dlgBarCodeStringMatch.GetMultiFileName());
		m_resultValues.Commit();
	}

}