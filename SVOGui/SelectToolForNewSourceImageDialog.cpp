//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SelectToolForNewSourceImageDialog.cpp
/// All Rights Reserved
//*****************************************************************************
/// A dialog to select a tool for a new SourceImage.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SelectToolForNewSourceImageDialog.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	SelectToolForNewSourceImageDialog::SelectToolForNewSourceImageDialog(uint32_t inspectionId, SvPb::GetToolsWithReplaceableSourceImageResponse list, CWnd* pParent) : CDialog(SelectToolForNewSourceImageDialog::IDD, pParent)
		, m_InspectionID(inspectionId)
		, m_list(list)
	{

	}// end ctor

	SelectToolForNewSourceImageDialog::~SelectToolForNewSourceImageDialog()
	{
	}

	void SelectToolForNewSourceImageDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SelectToolForNewSourceImageDialog)
		DDX_Control(pDX, IDC_TOOL_COMBOBOX, m_toolList);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(SelectToolForNewSourceImageDialog, CDialog)
		//{{AFX_MSG_MAP(SelectToolForNewSourceImageDialog)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	BOOL SelectToolForNewSourceImageDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();

		if (0 >= m_list.list_size())
		{
			return false;
		}

		for (auto item : m_list.list())
		{
			m_toolList.AddString(item.objectname().c_str());
		}
		m_toolList.SetCurSel(0);

		UpdateData(false);

		return true;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}// end OnInitDialog

	void SelectToolForNewSourceImageDialog::OnOK()
	{
		int pos = m_toolList.GetCurSel();
		if (m_list.list_size() > pos && 0 <= pos)
		{
			m_selectedToolNameAndId = { m_list.list(pos).objectname(), m_list.list(pos).objectid() };
		}
		else
		{
			m_selectedToolNameAndId = {};
		}
		CDialog::OnOK();
	}

	SvPb::GetToolsWithReplaceableSourceImageResponse getToolsWithReplaceableSourceImage(uint32_t inspectionId)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_gettoolswithreplaceablesourceimagerequest();
		pRequest->set_objectid(inspectionId);

		HRESULT hr = SvCmd::InspectionCommands(inspectionId, requestCmd, &responseCmd);
		if (S_OK != hr || false == responseCmd.has_gettoolswithreplaceablesourceimageresponse())
		{
			Log_Assert(false);
			return {};
		}

		return responseCmd.gettoolswithreplaceablesourceimageresponse();
	}
}  //namespace SvOg
