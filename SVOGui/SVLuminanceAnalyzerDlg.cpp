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
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
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
		typedef SvCmd::GetInstanceIDByTypeInfo Command;
		typedef std::shared_ptr<Command> CommandPtr;

		SvDef::SVObjectTypeInfoStruct info(SvDef::SVResultObjectType, SvDef::SVResultLongObjectType);

		CommandPtr commandPtr = CommandPtr(new Command(m_rTaskObjectID, info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK != hr)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16090);
		}
		else  if ( S_OK != SvOi::SetupDialogManager(SVLongResultClassGuid, commandPtr->GetInstanceID(), GetSafeHwnd()))
		{
		
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16091);
		}

	}
} //namespace SvOg
