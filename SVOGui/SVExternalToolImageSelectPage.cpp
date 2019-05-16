//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolImageSelectPage
//* .File Name       : $Workfile:   SVExternalToolImageSelectPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   26 Jun 2014 17:30:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExternalToolImageSelectPage.h"
#include "SVRPropertyTree/SVRPropTreeItemCombo.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{
	BEGIN_MESSAGE_MAP(SVExternalToolImageSelectPage, CPropertyPage)
		//{{AFX_MSG_MAP(SVExternalToolImageSelectPage)
		ON_NOTIFY(PTN_ITEMCHANGED, IDC_IMAGE_LIST, OnItemChanged)
		ON_NOTIFY(PTN_PROPCLICK, IDC_IMAGE_LIST, OnPropClick)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	#pragma endregion Declarations

	#pragma region Constructor
	SVExternalToolImageSelectPage::SVExternalToolImageSelectPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, long numImages, int id )
	: CPropertyPage(id)
	, SvOg::ImageController(rInspectionID, rTaskObjectID)
	, m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_numImages(numImages)
	{
	}

	SVExternalToolImageSelectPage::~SVExternalToolImageSelectPage()
	{
	}
	#pragma endregion Constructor

	#pragma region Protected Methods
	void SVExternalToolImageSelectPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(SVExternalToolImageSelectPage)
		DDX_Control(pDX, IDC_IMAGE_DISPLAY, m_ImageDisplay);
		//}}AFX_DATA_MAP
	}

	BOOL SVExternalToolImageSelectPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();
		Init();
	
		// Only show as many as the external tool has exposed...
		const SvUl::InputNameGuidPairList& images = GetConnectedImageList(GUID_NULL, m_numImages);
		if (m_numImages > 0)
		{
			GetDlgItem(IDC_NO_IMAGES_TXT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_IMAGE_LIST)->ShowWindow(SW_SHOW);

			buildPropertyTree();
			setImages();
			UpdateData(false); // set data to dialog
		}

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void SVExternalToolImageSelectPage::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
	{
		UpdateData(true); // get data from dialog
		LPNMPROPTREE pNMPropTree = reinterpret_cast<LPNMPROPTREE>(pNotifyStruct);
		*plResult = S_FALSE;

		if (nullptr != pNMPropTree->pItem)
		{
			SVRPropertyItemCombo* pItem = dynamic_cast<SVRPropertyItemCombo *>(pNMPropTree->pItem);
			if (nullptr != pItem)
			{
				UINT ctrlID = pItem->GetCtrlID();
				if (ctrlID >= 0 && static_cast<int>(ctrlID) < m_numImages)
				{
					const SvUl::NameGuidList& availImages = GetAvailableImageList();
					long dist = -1;
					pItem->GetItemValue(dist);
					if (dist >= 0 && dist < static_cast<int>(availImages.size()))
					{
						SvUl::NameGuidList::const_iterator it = availImages.begin();
						std::advance(it, dist);
						if (it != availImages.end())
						{
							const std::string& imageName = it->first;
							std::string inputName = m_imageInputList[ctrlID];
							if (!imageName.empty() && !inputName.empty())
							{
								HRESULT hr = ConnectToImage(inputName, imageName);
								if (S_OK == hr)
								{
									setImages();
									Refresh();

									*plResult = S_OK;
								}
							}
						}
					}
				}
			}
		}
	}

	void SVExternalToolImageSelectPage::OnPropClick(NMHDR* pNotifyStruct, LRESULT* plResult)
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
	void SVExternalToolImageSelectPage::Refresh()
	{
		ToolRunOnce();
		m_ImageDisplay.Refresh();
		UpdateData(false); // set data to dialog
	}

	void SVExternalToolImageSelectPage::buildPropertyTree()
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
		ASSERT(pRoot);
		pRoot->SetCanShrink(false);
		pRoot->SetLabelText(_T("Input Source Images"));
		pRoot->SetInfoText(_T(""));

		const SvUl::NameGuidList& availImages = GetAvailableImageList();
		const SvUl::InputNameGuidPairList& images = GetConnectedImageList(GUID_NULL, m_numImages);
	
		int i = 0;
		for (SvUl::InputNameGuidPairList::const_iterator it = images.begin();it != images.end();++it)
		{
			std::string Temp = SvUl::Format( _T("Image %02d"), i + 1);

			SVRPropertyItemCombo* pCombo = static_cast<SVRPropertyItemCombo *>(m_Tree.InsertItem(new SVRPropertyItemCombo(), pRoot));

			if (nullptr != pCombo)
			{
				UINT ctrlID = static_cast<UINT>(i++);
				pCombo->SetCtrlID(ctrlID);
				pCombo->SetLabelText( Temp.c_str() );
				pCombo->CreateComboBox();
				int curSel = 0;
				for (SvUl::NameGuidList::const_iterator availIt = availImages.begin();availIt != availImages.end();++availIt)
				{
					int index = pCombo->AddString(availIt->first.c_str());
					size_t imageIndex = std::distance(availImages.begin(), availIt);
					pCombo->SetItemData(index, imageIndex);
					if (availIt->first == it->second.first)
					{
						curSel = static_cast<int>(imageIndex);
					}
				}
				pCombo->SetItemValue(curSel);

				m_imageInputList.insert(std::make_pair(ctrlID, it->first));
			}
			m_ImageDisplay.AddTab( Temp.c_str() );
		}
		pRoot->Expand();
	}

	void SVExternalToolImageSelectPage::setImages()
	{
		const SvUl::InputNameGuidPairList& images = GetConnectedImageList(GUID_NULL, m_numImages);
		int imageIndex = 0;
		for (SvUl::InputNameGuidPairList::const_iterator it = images.begin(); it != images.end(); ++it)
		{
			m_ImageDisplay.setImage(GetImage(it->second.second.ToGUID()), imageIndex);
			m_ImageDisplay.SetZoom(imageIndex++, -1.0);
		}
		m_ImageDisplay.Refresh();
	}

	#pragma endregion Private Methods
} //namespace SvOg
