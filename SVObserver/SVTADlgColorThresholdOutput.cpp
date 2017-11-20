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
#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVTADlgColorThresholdOutput.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVInspectionProcess.h"
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
	SetTaskObject(m_pTool);

	// Get the color threshold object
	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SvDef::SVOperatorObjectType;
	objectInfo.SubType = SvDef::SVColorThresholdObjectType;

	m_pThreshold = dynamic_cast<SVColorThresholdClass*> (m_pTool->getFirstObject(objectInfo));

	if(nullptr !=  m_pThreshold)
	{
		SVImageClass& rImage = m_pThreshold->GetOutputImage();

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
	UpdateData();

	AddInputRequest(m_pEnabled, m_Enabled);
	AddInputRequestMarker();
	if (nullptr != m_pTool)
	{
		m_pTool->resetAllObjects();
		RunOnce(m_pTool->GetUniqueObjectID());
	}
	m_svDlgImage.refresh();
}
