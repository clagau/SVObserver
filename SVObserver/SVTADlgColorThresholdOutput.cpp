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

#include "stdafx.h"
#include "SVColorTool.h"
#include "SVColorThreshold.h"
#include "SVTADlgColorThresholdOutput.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVInspectionProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdOutput, SVTADlgColorThresholdBasePage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdOutput, SVTADlgColorThresholdBasePage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdOutput::SVTADlgColorThresholdOutput() : SVTADlgColorThresholdBasePage(SVTADlgColorThresholdOutput::IDD)
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdOutput)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVTADlgColorThresholdOutput::~SVTADlgColorThresholdOutput()
{
}

void SVTADlgColorThresholdOutput::DoDataExchange(CDataExchange* pDX)
{
	SVTADlgColorThresholdBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdOutput)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdOutput message handlers

BOOL SVTADlgColorThresholdOutput::OnInitDialog() 
{
	SVTADlgColorThresholdBasePage::OnInitDialog();

	ASSERT( m_pSheet );

	mpTool = m_pSheet->GetTool();

	// Get the color threshold object
	SVObjectTypeInfoStruct objectInfo;
	objectInfo.ObjectType = SVOperatorObjectType;
	objectInfo.SubType = SVColorThresholdObjectType;

	mpThreshold = reinterpret_cast <SVColorThresholdClass*> ( ::SVSendMessage( mpTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ) );

	if( mpThreshold )
	{
		SVImageClass* pImage = mpThreshold->GetOutputImage();

		if( pImage )
		{
			m_svDlgImage.UpdateImageInfo( pImage->GetInspection()->GetUniqueObjectID(), pImage->GetUniqueObjectID() );
			m_svDlgImage.refresh();
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

