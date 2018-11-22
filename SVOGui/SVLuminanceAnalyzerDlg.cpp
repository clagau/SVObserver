//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : <SVAnalyzerLuminanceSetup>
//* .File Name       : $Workfile:   SVAnalyzerLuminanceSetup.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:20  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVLuminanceAnalyzerDlg.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVLuminanceAnalyzerDlg, CDialog)
		//{{AFX_MSG_MAP(SVLuminanceAnalyzerDlg)
		ON_BN_CLICKED(IDC_RANGE, OnRange)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVLuminanceAnalyzerDlg::SVLuminanceAnalyzerDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, CWnd* pParent /*=nullptr*/)
	: CDialog(SVLuminanceAnalyzerDlg::IDD, pParent)
	, m_rInspectionID{ rInspectionID }
	, m_rTaskObjectID{ rTaskObjectID }
	, m_Values{ SvOg::BoundValues{ rInspectionID, rTaskObjectID } }
	{
		//{{AFX_DATA_INIT(SVLuminanceAnalyzerDlg)
		m_calcStdDev = FALSE;
		//}}AFX_DATA_INIT
	}

	SVLuminanceAnalyzerDlg::~SVLuminanceAnalyzerDlg()
	{
	}

	void SVLuminanceAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVLuminanceAnalyzerDlg)
		DDX_Check(pDX, IDC_CALC_STDDEV, m_calcStdDev);
		//}}AFX_DATA_MAP
	}

	HRESULT SVLuminanceAnalyzerDlg::SetInspectionData()
	{
		HRESULT l_hrOk = S_OK;

		UpdateData(true);

		m_Values.Set<bool>(SVCalcStdDevObjectGuid, m_calcStdDev ? true : false);
		m_Values.Commit();

		UpdateData(false);

		return l_hrOk;
	}

	void SVLuminanceAnalyzerDlg::OnOK()
	{
		SetInspectionData();

		CDialog::OnOK();
	}

	BOOL SVLuminanceAnalyzerDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_Values.Init();

		m_calcStdDev = m_Values.Get<bool>(SVCalcStdDevObjectGuid);

		UpdateData(false);

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVLuminanceAnalyzerDlg::OnCancel() 
	{
		CDialog::OnCancel();
	}

	void SVLuminanceAnalyzerDlg::OnRange() 
	{
		SvPb::InspectionCmdMsgs requestMessage, responseMessage;
		auto* pRequest = requestMessage.mutable_getobjectidrequest()->mutable_info();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_rTaskObjectID);
		pRequest->mutable_infostruct()->set_objecttype(SvDef::SVResultObjectType);
		pRequest->mutable_infostruct()->set_objecttype(SvDef::SVResultLongObjectType);

		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_rInspectionID, &requestMessage, &responseMessage);
		if (S_OK != hr || !responseMessage.has_getobjectidresponse())
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16090);
		}
		else  if ( S_OK != SvOi::SetupDialogManager(SVLongResultClassGuid, SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid()), GetSafeHwnd()))
		{
			SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16091);
		}
	}
} //namespace SvOg
