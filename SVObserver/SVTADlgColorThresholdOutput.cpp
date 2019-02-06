//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdOutput
//* .File Name       : $Workfile:   SVTADlgColorThresholdOutput.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Tools/SVColorTool.h"
#include "Operators/SVColorThreshold.h"
#include "SVTADlgColorThresholdOutput.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVInspectionProcess.h"
#include "SVOGui/ValuesAccessor.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdOutput, SVTADlgColorThresholdBasePage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdOutput, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdOutput)
	ON_BN_CLICKED(IDC_ENABLED_COLOR_THRESHOLD, OnEnabledThreshold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdOutput::SVTADlgColorThresholdOutput() : SVTADlgColorThresholdBasePage(SVTADlgColorThresholdOutput::IDD)
, m_pEnabled(nullptr)
, m_Enabled(false)
{
}

SVTADlgColorThresholdOutput::~SVTADlgColorThresholdOutput()
{
}

void SVTADlgColorThresholdOutput::DoDataExchange(CDataExchange* pDX)
{
	SVTADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdOutput)
	DDX_Check(pDX, IDC_ENABLED_COLOR_THRESHOLD, m_Enabled);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdOutput message handlers

BOOL SVTADlgColorThresholdOutput::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();

	ASSERT( m_pSheet );

	m_pTool = m_pSheet->GetTool();

	// Get the color threshold object
	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SvPb::SVOperatorObjectType;
	objectInfo.SubType = SvPb::SVColorThresholdObjectType;

	m_pThreshold = dynamic_cast<SvOp::SVColorThresholdClass*> (m_pTool->getFirstObject(objectInfo));

	if(nullptr !=  m_pThreshold)
	{
		SvIe::SVImageClass& rImage = m_pThreshold->GetOutputImage();

		m_svDlgImage.UpdateImageInfo( rImage.GetInspection()->GetUniqueObjectID(), rImage.GetUniqueObjectID() );
		m_svDlgImage.refresh();
		m_pEnabled = m_pThreshold->GetOutputThresholdEnabled();
		m_pEnabled->GetValue(m_Enabled);
	}
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVTADlgColorThresholdOutput::OnEnabledThreshold()
{
	UpdateData(true);

	if (nullptr != m_pThreshold)
	{
		//@TODO[gra][8.00][15.01.2018]: The data controller should be used like the rest of SVOGui
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller Values{ SvOg::BoundValues{ m_pThreshold->GetInspection()->GetUniqueObjectID(), m_pThreshold->GetUniqueObjectID() } };
		Values.Init();
		Values.Set<bool>(m_pEnabled->GetEmbeddedID(), m_Enabled ? true : false);
		Values.Commit(SvOg::PostAction::doReset | SvOg::PostAction::doRunOnce);
	};
	m_svDlgImage.refresh();
}
