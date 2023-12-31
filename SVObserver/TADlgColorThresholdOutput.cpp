//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgColorThresholdOutput
//* .File Name       : $Workfile:   TADlgColorThresholdOutput.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Tools/SVColorTool.h"
#include "Operators/SVColorThreshold.h"
#include "TADlgColorThresholdOutput.h"
#include "TADlgColorThresholdSheet.h"
#include "SVInspectionProcess.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

//This comment is to avoid that the SVColorToolClass include is marked as not required due to forward declaration from a base class

IMPLEMENT_DYNCREATE(TADlgColorThresholdOutput, TADlgColorThresholdBasePage)

BEGIN_MESSAGE_MAP(TADlgColorThresholdOutput, TADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(TADlgColorThresholdOutput)
	ON_BN_CLICKED(IDC_ENABLED_COLOR_THRESHOLD, OnEnabledThreshold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TADlgColorThresholdOutput::TADlgColorThresholdOutput() : TADlgColorThresholdBasePage(TADlgColorThresholdOutput::IDD)
, m_pEnabled(nullptr)
, m_Enabled(false)
{
}

TADlgColorThresholdOutput::~TADlgColorThresholdOutput()
{
}

void TADlgColorThresholdOutput::DoDataExchange(CDataExchange* pDX)
{
	TADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADlgColorThresholdOutput)
	DDX_Check(pDX, IDC_ENABLED_COLOR_THRESHOLD, m_Enabled);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// TADlgColorThresholdOutput message handlers

BOOL TADlgColorThresholdOutput::OnInitDialog() 
{
	TADlgColorThresholdBasePage::OnInitDialog();

	Log_Assert( m_pSheet );

	m_pTool = m_pSheet->GetTool();

	// Get the color threshold object
	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.m_ObjectType = SvPb::SVOperatorObjectType;
	objectInfo.m_SubType = SvPb::SVColorThresholdObjectType;

	m_pThreshold = dynamic_cast<SvOp::SVColorThreshold*> (m_pTool->getFirstObject(objectInfo));

	if(nullptr !=  m_pThreshold)
	{
		SvIe::SVImageClass& rImage = m_pThreshold->GetOutputImage();

		m_svDlgImage.UpdateImageInfo( rImage.GetInspection()->getObjectId(), rImage.getObjectId() );
		m_svDlgImage.refresh();
		m_pEnabled = m_pThreshold->GetOutputThresholdEnabled();
		m_pEnabled->GetValue(m_Enabled);
	}
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TADlgColorThresholdOutput::OnEnabledThreshold()
{
	UpdateData(true);

	if (nullptr != m_pThreshold)
	{
		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		SvOgu::ValueController Values{ SvOgu::BoundValues{ m_pThreshold->GetInspection()->getObjectId(), m_pThreshold->getObjectId() } };
		Values.Init();
		Values.Set<bool>(m_pEnabled->GetEmbeddedID(), m_Enabled ? true : false);
		Values.Commit(SvOgu::PostAction::doReset | SvOgu::PostAction::doRunOnce);
	};
	m_svDlgImage.refresh();
}
