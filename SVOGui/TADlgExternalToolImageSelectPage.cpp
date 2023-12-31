//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgExternalToolImageSelectPage
//* .File Name       : $Workfile:   TADlgExternalToolImageSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   26 Jun 2014 17:30:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADlgExternalToolImageSelectPage.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h"
#pragma endregion Includes

namespace SvOg
{

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

	
SvPb::SVObjectSubTypeEnum GetImageSubtype(bool mayBeColor, bool mayBeBlackAndWhite)
{
	SvPb::SVObjectSubTypeEnum ImageSubType = SvPb::SVImageMonoType; //for the time being: b/w Images are always acceptable

	if (mayBeColor)
	{
		if (mayBeBlackAndWhite)
		{
			ImageSubType = SvPb::SVNotSetSubObjectType;
		}
		else
		{
			ImageSubType = SvPb::SVImageColorType;
		}

	}

	return ImageSubType;
}

	BEGIN_MESSAGE_MAP(TADlgExternalToolImageSelectPage, CPropertyPage)
		//{{AFX_MSG_MAP(TADlgExternalToolImageSelectPage)
		ON_NOTIFY(PTN_ITEMCHANGED, IDC_IMAGE_LIST, OnItemChanged)
		ON_NOTIFY(PTN_PROPCLICK, IDC_IMAGE_LIST, OnPropClick)
		ON_WM_SIZE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	#pragma endregion Declarations


	#pragma region Constructor
	TADlgExternalToolImageSelectPage::TADlgExternalToolImageSelectPage(uint32_t inspectionId, uint32_t taskObjectId, int id)
	: CPropertyPage(id)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_externalToolTaskController(inspectionId, taskObjectId)
	, m_numImages(m_externalToolTaskController.getInputImageInfoList().imageinfolist().size())
	{
	}

	#pragma endregion Constructor

	#pragma region Protected Methods
	void TADlgExternalToolImageSelectPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(TADlgExternalToolImageSelectPage)
		DDX_Control(pDX, IDC_IMAGE_DISPLAY, m_ImageDisplay);
		//}}AFX_DATA_MAP
	}

	BOOL TADlgExternalToolImageSelectPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();
	
		// Only show as many as the external tool has exposed...
		if (m_numImages > 0)
		{
			GetDlgItem(IDC_NO_IMAGES_TXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_IMAGE_LIST)->ShowWindow(SW_SHOW);

			buildPropertyTree();
			SvOgu::ImageController ImgCtrlDisplay(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType);
			ImgCtrlDisplay.Init();
			setImages(ImgCtrlDisplay);

			UpdateData(false); // set data to dialog
		}

		if (IsDynamicLayoutEnabled())
		{
			CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
			if (!dynamicLayout->HasItem(m_Tree.m_hWnd))
			{
				dynamicLayout->AddItem(m_Tree.m_hWnd, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeHorizontal(100));
			}
		}
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void TADlgExternalToolImageSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
	{
		UpdateData(true); // get data from dialog
		LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
		*plResult = S_FALSE;

		if (nullptr != pNMPropTree->pItem)
		{
			auto imageInfoList = m_externalToolTaskController.getInputImageInfoList().imageinfolist();
			SVRPropertyItemCombo* pItem = dynamic_cast<SVRPropertyItemCombo *>(pNMPropTree->pItem);
			if (nullptr != pItem)
			{
				UINT ctrlID = pItem->GetCtrlID();
				if (static_cast<int>(ctrlID) < m_numImages)
				{
					SvOgu::ImageController ImgCtrl(m_InspectionID, m_TaskObjectID, GetImageSubtype(imageInfoList[ctrlID].maybecolor(), imageInfoList[ctrlID].maybeblackandwhite()));
					
					ImgCtrl.Init();
					const SvUl::NameObjectIdList& availImages = ImgCtrl.GetAvailableImageList();
					long dist = -1;
					pItem->GetItemValue(dist);
					if (dist >= 0 && dist < static_cast<int>(availImages.size()))
					{
						SvUl::NameObjectIdList::const_iterator it = availImages.begin();
						std::advance(it, dist);
						if (it != availImages.end())
						{
							const std::string& imageName = it->first;
							if (!imageName.empty())
							{
								HRESULT hr = ImgCtrl.ConnectToImage(SvPb::ImageInputEId+static_cast<int>(ctrlID), imageName);
								if (S_OK == hr)
								{
									setImages(ImgCtrl);

									*plResult = S_OK;
								}
							}
						}
					}
				}
			}
		}
	}

	void TADlgExternalToolImageSelectPage::OnPropClick(NMHDR* pNotifyStruct, LRESULT* plResult)
	{
		LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
		*plResult = S_FALSE;

		if (nullptr != pNMPropTree->pItem)
		{
			SVRPropertyItemCombo* pItem = dynamic_cast<SVRPropertyItemCombo*>(pNMPropTree->pItem);
			if (nullptr != pItem && pItem->GetCtrlID() >= 0 && static_cast<int>(pItem->GetCtrlID()) < m_numImages)
			{
				//set displayed tab to the current selected entry in the propTree
				m_ImageDisplay.SelectTab(pItem->GetCtrlID());
				*plResult = S_OK;
			}
		}
	}
	#pragma endregion Protected Methods

	#pragma region Private Methods
	void TADlgExternalToolImageSelectPage::OnSize(UINT nType, int cx, int cy)
	{
		CPropertyPage::OnSize(nType, cx, cy);
		if (m_Tree)
		{
			
		}
	}

	void TADlgExternalToolImageSelectPage::buildPropertyTree()
	{
		// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;
		CRect rc;
		// Init the control's size to cover the entire client area
		GetDlgItem(IDC_IMAGE_LIST)->GetWindowRect(rc);
		ScreenToClient(rc);
		// Create SVRPropTree control
		m_Tree.Create(dwStyle, rc, this, IDC_IMAGE_LIST);
		m_Tree.SetColumn(static_cast<long>(rc.Width() * 0.15));
		m_Tree.ShowInfoText(false);

		SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
		Log_Assert(pRoot);
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Input Source Images"));
		pRoot->SetInfoText(_T(""));

		auto imageInfoList = m_externalToolTaskController.getInputImageInfoList().imageinfolist();
		//we need to iterate over an InputImageList
		//these are the same for all ImageControllers so let's generate one of them immediately
		//to obtain such a list
		SvOgu::ImageController justForInputImageListGeneration(m_InspectionID, m_TaskObjectID, SvPb::SVNotSetSubObjectType);
		justForInputImageListGeneration.Init();

		auto numSize = std::min(static_cast<int>(m_numImages), imageInfoList.size());
		for(int ctrlID = 0; ctrlID < numSize; ++ctrlID)
		{
			SvOgu::ImageController ImgCtrl(m_InspectionID, m_TaskObjectID, GetImageSubtype(imageInfoList[ctrlID].maybecolor(), imageInfoList[ctrlID].maybeblackandwhite()));
			ImgCtrl.Init();
			const SvUl::NameObjectIdList& availImages = ImgCtrl.GetAvailableImageList();
			std::string Temp = imageInfoList[ctrlID].displayname();

			SVRPropertyItemCombo* pCombo = static_cast<SVRPropertyItemCombo *>(m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot));

			if (nullptr != pCombo)
			{
				pCombo->SetCtrlID(ctrlID);
				pCombo->SetLabelText( Temp.c_str() );
				pCombo->CreateComboBox();
				int curSel = 0;
				auto connectedName = justForInputImageListGeneration.GetInputData(SvPb::ImageInputEId+ctrlID).connected_objectdottedname();
				for (SvUl::NameObjectIdList::const_iterator availIt = availImages.begin();availIt != availImages.end();++availIt)
				{
					int index = pCombo->AddString(availIt->first.c_str());
					size_t imageIndex = std::distance(availImages.begin(), availIt);
					pCombo->SetItemData(index, imageIndex);
					if (availIt->first == connectedName)
					{
						curSel = static_cast<int>(imageIndex);
					}
				}
				pCombo->SetItemValue(curSel);
			}
			m_ImageDisplay.AddTab( Temp.c_str() );
		}
		pRoot->Expand();
	}


	void TADlgExternalToolImageSelectPage::setImages(SvOgu::ImageController &imgCtrl)
	{
		imgCtrl.ToolRunOnce();
		
		for(int imageIndex = 0; imageIndex < m_numImages; ++imageIndex)
		{
			m_ImageDisplay.setImage(imgCtrl.GetImage(imgCtrl.GetInputData(SvPb::ImageInputEId + imageIndex).connected_objectdottedname()), imageIndex);
			m_ImageDisplay.SetZoom(imageIndex, -1.0);
		}
		m_ImageDisplay.Refresh();
		UpdateData(false); // set data to dialog
	}

#pragma endregion Private Methods
} //namespace SvOg
