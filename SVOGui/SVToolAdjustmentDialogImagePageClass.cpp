//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogImagePageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogImagePageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <afxctl.h>
#include "SVToolAdjustmentDialogImagePageClass.h"
#include "SVObjectLibrary\SVClsIDs.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvOg.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader { namespace SVOGui
{
	static LPCTSTR ImageTag = _T("Image");
	static LPCTSTR NoImageTag = _T("(No Image Available)");

	BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogImagePageClass, CPropertyPage)
		//{{AFX_MSG_MAP(SVToolAdjustmentDialogImagePageClass)
		ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVToolAdjustmentDialogImagePageClass::SVToolAdjustmentDialogImagePageClass(const SVGUID & rInspectionID, const SVGUID& rTaskObjectID, int id ) 
	: CPropertyPage( id )
	, ImageController(rInspectionID, rTaskObjectID)
	{
	}

	SVToolAdjustmentDialogImagePageClass::~SVToolAdjustmentDialogImagePageClass()
	{
	}

	HRESULT SVToolAdjustmentDialogImagePageClass::SetInspectionData()
	{
		UpdateData(true); // get data from dialog

		HRESULT hr = ToolRunOnce();

		UpdateData(false);

		return hr;
	}

	void SVToolAdjustmentDialogImagePageClass::refresh()
	{
		SetInspectionData();
		m_dialogImage.Refresh();
	}

	void SVToolAdjustmentDialogImagePageClass::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVToolAdjustmentDialogImagePageClass)
		DDX_Control(pDX, IDC_COMBO1, m_availableSourceImageListBox);
		DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
		//}}AFX_DATA_MAP
	}

	BOOL SVToolAdjustmentDialogImagePageClass::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();
		Init();
		const SvUl::NameGuidList& rAvailableImageList = GetAvailableImageList();
	
		// This requires that the input name sorts in descending natural order
		// and that the images we are concerned with are first in the list
		SVString selectedImageName;
		const SvUl::InputNameGuidPairList& rImageList = GetConnectedImageList();
		if (rImageList.size())
		{
			m_inputName = rImageList.begin()->first;
			selectedImageName = rImageList.begin()->second.first;
		}
	
		m_availableSourceImageListBox.Init(rAvailableImageList, selectedImageName, NoImageTag);
		m_dialogImage.AddTab(ImageTag);
		m_dialogImage.setImage(GetImage(selectedImageName));
		m_dialogImage.Refresh();

		UpdateData(false); // set data to dialog

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	void SVToolAdjustmentDialogImagePageClass::OnSelchangeCombo1() 
	{
		UpdateData(TRUE); // get data from dialog

		int index = m_availableSourceImageListBox.GetCurSel();
		if (LB_ERR != index)
		{
			bool bIsValid = IsToolValid();
			CString imageName;
			m_availableSourceImageListBox.GetLBText(index, imageName);
			if (!imageName.IsEmpty() && imageName != NoImageTag)
			{
				SVString svImageName(imageName);
				ConnectToImage(m_inputName, svImageName);
				IPictureDisp* pImage = GetImage(svImageName);
				m_dialogImage.setImage(pImage);
				refresh();
				SvStl::MessageContainerVector errorMessages;
				HRESULT result = ResetTask(errorMessages);

				if (bIsValid && S_OK != result)
				{
					SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
					Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Error_ToolPositionError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10232 );
				}
			}
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */

