//*****************************************************************************
/// \copyright (c) 2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the tool adjustment dialog for Draw-tool
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TADialogDrawPage.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/Color.h"
#include "DisplayHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
bool isChangable(const SvOg::ValueController& rValueController, SvPb::EmbeddedIdEnum embeddedId)
{
	auto data = rValueController.Get<SvOg::LinkedValueData>(embeddedId);
	return SvPb::LinkedSelectedType::DirectValue == data.m_type;
}
}

namespace SvOg
{
static const std::vector<std::pair<SvPb::ClassIdEnum, SvPb::SVObjectSubTypeEnum>> c_typeConvVec = {
	{SvPb::DrawRectangleClassId, SvPb::DrawRectangleTaskType},
	{SvPb::DrawOvalClassId, SvPb::DrawOvalTaskType}};

static LPCTSTR ImageTag = _T("Image");
static LPCTSTR NoImageTag = _T("(No Image Available)");

BEGIN_MESSAGE_MAP(TADialogDrawPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADialogDrawPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE, OnBeginLabelEdit)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE, OnEndLabelEdit)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE, OnDragTree)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK1, IDC_CHECK2, OnButtonCheck)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON1, IDC_BUTTON6, OnButtonButton)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LINKED_VALUE_BUTTON1, IDC_LINKED_VALUE_BUTTON6, OnButtonLinkedValue)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT6, OnKillFocusEdit)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(TADialogDrawPage, CDialog)
	ON_EVENT(TADialogDrawPage, IDC_DIALOGIMAGE, 8, TADialogDrawPage::ObjectChangedExDialogImage, VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()

TADialogDrawPage::TADialogDrawPage(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(TADialogDrawPage::IDD)
	, m_InspectionID(inspectionId)
	, m_TaskObjectID(taskObjectId)
	, m_ImageController {inspectionId, taskObjectId, SvPb::SVNotSetSubObjectType}
    , m_pValues {std::make_shared<ValueController>(SvOg::BoundValues {inspectionId, taskObjectId})}
	, m_treeCtrl(inspectionId)
	, m_drawToolController{*m_pValues, m_editCtrlDataList}
{
	
}

TADialogDrawPage::~TADialogDrawPage()
{}

void TADialogDrawPage::refresh()
{
	refreshTree();
	updateImages();
}

void TADialogDrawPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TADialogDrawPage)
	DDX_Control(pDX, IDC_COMBO2, m_comboBox2);
	DDX_Control(pDX, IDC_TREE, m_treeCtrl);
	DDX_Control(pDX, IDC_DIALOGIMAGE, m_dialogImage);
	DDX_Control(pDX, IDC_CHECK1, m_BSOAControls[BOSAEnum::Check1]);
	DDX_Control(pDX, IDC_CHECK2, m_BSOAControls[BOSAEnum::Check2]);
	DDX_Control(pDX, IDC_CHECK3, m_BSOAControls[BOSAEnum::Check3]);
	DDX_Control(pDX, IDC_CHECK4, m_BSOAControls[BOSAEnum::Check4]);
	DDX_Control(pDX, IDC_CHECK5, m_BSOAControls[BOSAEnum::Check5]);
	DDX_Control(pDX, IDC_STATIC1, m_BSOAControls[BOSAEnum::Static1]);
	DDX_Control(pDX, IDC_EDIT1, m_BSOAControls[BOSAEnum::Edit1]);
	DDX_Control(pDX, IDC_LINKED_VALUE_BUTTON1, m_BSOAControls[BOSAEnum::LinkedValueButton1]);
	DDX_Control(pDX, IDC_STATIC2, m_BSOAControls[BOSAEnum::Static2]);
	DDX_Control(pDX, IDC_EDIT2, m_BSOAControls[BOSAEnum::Edit2]);
	DDX_Control(pDX, IDC_LINKED_VALUE_BUTTON2, m_BSOAControls[BOSAEnum::LinkedValueButton2]);
	DDX_Control(pDX, IDC_STATIC3, m_BSOAControls[BOSAEnum::Static3]);
	DDX_Control(pDX, IDC_EDIT3, m_BSOAControls[BOSAEnum::Edit3]);
	DDX_Control(pDX, IDC_LINKED_VALUE_BUTTON3, m_BSOAControls[BOSAEnum::LinkedValueButton3]);
	DDX_Control(pDX, IDC_STATIC4, m_BSOAControls[BOSAEnum::Static4]);
	DDX_Control(pDX, IDC_EDIT4, m_BSOAControls[BOSAEnum::Edit4]);
	DDX_Control(pDX, IDC_LINKED_VALUE_BUTTON4, m_BSOAControls[BOSAEnum::LinkedValueButton4]);
	DDX_Control(pDX, IDC_STATIC5, m_BSOAControls[BOSAEnum::Static5]);
	DDX_Control(pDX, IDC_EDIT5, m_BSOAControls[BOSAEnum::Edit5]);
	DDX_Control(pDX, IDC_LINKED_VALUE_BUTTON5, m_BSOAControls[BOSAEnum::LinkedValueButton5]);
	DDX_Control(pDX, IDC_BUTTON1, m_BSOAControls[BOSAEnum::Button1]);
	DDX_Control(pDX, IDC_BUTTON2, m_BSOAControls[BOSAEnum::Button2]);
	DDX_Control(pDX, IDC_BUTTON3, m_BSOAControls[BOSAEnum::Button3]);
	DDX_Control(pDX, IDC_BUTTON4, m_BSOAControls[BOSAEnum::Button4]);
	DDX_Control(pDX, IDC_BUTTON5, m_BSOAControls[BOSAEnum::Button5]);
	DDX_Control(pDX, IDC_BUTTON6, m_BSOAControls[BOSAEnum::Button6]);

	for (auto& rValueData : m_editCtrlDataList)
	{
		if (nullptr == rValueData.m_Widget)
		{
			DDX_Text(pDX, rValueData.m_nIDC, rValueData.m_value);
		}
	}
	//}}AFX_DATA_MAP
}

BOOL TADialogDrawPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_ImageController.Init();
	m_pValues->Init();
	m_drawToolController.init();

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getcreatableobjectsrequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::DrawTaskType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getcreatableobjectsresponse())
	{
		const auto& rList = responseCmd.getcreatableobjectsresponse().list();
		std::transform(rList.begin(), rList.end(), std::back_inserter(m_availableList),
			[](const auto& rEntry) -> SvUl::NameClassIdPair { return {rEntry.objectname(), rEntry.classid()}; });
	}

	auto hItem = m_treeCtrl.InsertNode({m_TaskObjectID, DrawNodeType::BaseImage, DrawNodeSubType::MainNode}, TVI_ROOT, "Base Image");
	m_treeCtrl.InsertNode({m_TaskObjectID, DrawNodeType::BaseImage, DrawNodeSubType::GeneralData, m_pValues}, hItem, "");
	m_treeCtrl.InsertNode({m_TaskObjectID, DrawNodeType::BaseImage, DrawNodeSubType::SizeData, m_pValues}, hItem, "");
	auto hLastInterNode = m_treeCtrl.InsertNode({m_TaskObjectID, DrawNodeType::BaseImage, DrawNodeSubType::Color, m_pValues}, hItem, "");
	auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(hItem));
	if (nullptr != pData)
	{
		pData->m_lastInternItem = hLastInterNode;
	}
	loadNode(hItem, m_TaskObjectID);
	m_treeCtrl.Expand(hItem, TVE_EXPAND);

	m_dialogImage.AddTab(ImageTag, &m_OutputTabHandle);
	refresh();

	UpdateData(false); // set data to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL TADialogDrawPage::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
			case VK_RETURN:
			{
				if (m_treeCtrl.isLableEditing())
				{
					m_treeCtrl.EndEditLabelNow(false);
					return true;
				}
				auto* pFocusWnd = GetFocus();
				auto iter = std::ranges::find_if(m_BSOAControls, [pFocusWnd](const auto& rEntry){ return pFocusWnd == &rEntry; });
				if (m_BSOAControls.end() != iter)
				{
					OnKillFocusEdit(iter->GetDlgCtrlID());
					return true;
				}
				break;
			}
			case VK_F2:
			{
				if (&m_treeCtrl == GetFocus())
				{
					m_treeCtrl.EditLabel(m_treeCtrl.GetSelectedItem());
					return true;
				}
				break;
			}
			default:
				break;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

void TADialogDrawPage::OnSelchangedTree(NMHDR*, LRESULT* pResult)
{
	HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
	if (hItem && m_currentItem != hItem)
	{
		bool isOk = setBOSAData();
		if (isOk)
		{
			hideAllBOSACtrl();
			m_currentItem = hItem;
			setBOSACtrl();
		}
		else
		{
			m_treeCtrl.SelectItem(m_currentItem);
			*pResult = S_FALSE;
		}
	}
	refresh();
}

void TADialogDrawPage::OnButtonCheck(UINT nID)
{
	int index(nID - IDC_CHECK1);
	if (0 <= index && 2 > index && 0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData)
		{
			switch (pData->m_type)
			{
				case DrawNodeType::BaseImage:
					switch (pData->m_subType)
					{
						case DrawNodeSubType::GeneralData:
							m_drawToolController.setIsColor( reinterpret_cast<CButton*>(&m_BSOAControls[BOSAEnum::Check1])->GetCheck() );
							m_drawToolController.setUseBackgroundImage( reinterpret_cast<CButton*>(&m_BSOAControls[BOSAEnum::Check2])->GetCheck() );
							hideAllBOSACtrl();
							setBaseImageGeneralCtrl();
							m_drawToolController.setBaseImageGeneralData();
							break;
						case DrawNodeSubType::SizeData:
							m_drawToolController.setAutoFit(reinterpret_cast<CButton*>(&m_BSOAControls[BOSAEnum::Check1])->GetCheck());
							m_drawToolController.setBaseImageSizeData();
							setBaseImageSizeCtrl();
							break;
						default:
							break;
					}
					break;
			}
			refresh();
		}
	}
}

void TADialogDrawPage::OnButtonButton(UINT nID)
{
	int index(nID - IDC_BUTTON1);
	if (0 <= index && 6 > index && 0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData)
		{
			switch (pData->m_subType)
			{
				case DrawNodeSubType::MainNode:
					SvPb::ClassIdEnum classID = static_cast<SvPb::ClassIdEnum>(m_comboBox2.getSelectedValue());
					switch (index)
					{
						case 0: //clear this item
							m_treeCtrl.clearItem(m_currentItem);
							break;
						case 1: //remove this item
						{
							auto item = m_currentItem;
							m_currentItem = nullptr;
							m_treeCtrl.deleteItem(item);
							break;
						}
						case 3: //add begin
							switch (pData->m_type)
							{
								case DrawNodeType::BaseImage:
									addNodeInto(m_currentItem, classID, DrawTaskTree::addPosEnum::IntoBegin);
									break;
								case DrawNodeType::Rectangle:
								case DrawNodeType::Oval:
									addNodeInto(m_currentItem, classID, DrawTaskTree::addPosEnum::Before);
									break;
							}
							break;
						case 4: //add end
							switch (pData->m_type)
							{
								case DrawNodeType::BaseImage:
									addNodeInto(m_currentItem, classID, DrawTaskTree::addPosEnum::IntoEnd);
									break;
								case DrawNodeType::Rectangle:
								case DrawNodeType::Oval:
									addNodeInto(m_currentItem, classID, DrawTaskTree::addPosEnum::After);
									break;
							}
							break;
					}
					m_pValues->RunOnce(m_InspectionID);
					refresh();
					break;
			}
		}
	}
}

void TADialogDrawPage::OnButtonLinkedValue(UINT nID)
{
	int index(nID - IDC_LINKED_VALUE_BUTTON1);
	assert(0 <= index && m_editCtrlDataList.size() > index && m_editCtrlDataList[index].m_Widget);
	if (0 <= index && m_editCtrlDataList.size() > index && m_editCtrlDataList[index].m_Widget)
	{
		m_editCtrlDataList[index].m_Widget->OnButton();
	}
}

void TADialogDrawPage::OnKillFocusEdit(UINT nID)
{
	UpdateData();

	auto ctrlDataIter = std::ranges::find_if(m_editCtrlDataList, [nID](const auto& rEntry){ return rEntry.m_nIDC == nID; });
	assert(m_editCtrlDataList.end() != ctrlDataIter);
	if (m_editCtrlDataList.end() != ctrlDataIter)
	{
		if (ctrlDataIter->m_Widget)
		{
			ctrlDataIter->m_Widget->EditboxToValue();
		}
		else if (false == ctrlDataIter->checkLimitsAndDisplayError())
		{
			GetDlgItem(nID)->SetFocus();
			return;
		}
		setBOSAData();
	}

	refresh();
}

bool TADialogDrawPage::setBOSAData()
{
	UpdateData();
	if (0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData)
		{
			return m_drawToolController.setBOSAData(*pData);
		}
	}
	return true;
}

void TADialogDrawPage::setBOSACtrl()
{
	if (0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData)
		{
			if (DrawNodeSubType::MainNode == pData->m_subType)
			{
				setAddTaskCtrl(*pData);
			}
			else
			{
				switch (pData->m_type)
				{
					case DrawNodeType::BaseImage:
						switch (pData->m_subType)
						{
							case DrawNodeSubType::GeneralData:
								setBaseImageGeneralCtrl();
								break;
							case DrawNodeSubType::SizeData:
								setBaseImageSizeCtrl();
								break;
							case DrawNodeSubType::Color:
								setColorCtrl(*pData);
								break;
							default:
								break;
						}
						break;
					default:
						switch (pData->m_subType)
						{
							case DrawNodeSubType::Position:
								setPositionCtrl(*pData);
								break;
							case DrawNodeSubType::SizeData:
								setSizeCtrl(*pData);
								break;
							case DrawNodeSubType::Color:
								setColorCtrl(*pData);
								break;
							default:
								break;
						}
						break;
				}
			}
		}
	}
}

void TADialogDrawPage::setAddTaskCtrl(const TreeNodeData& rData)
{
	switch (rData.m_type)
	{
		case DrawNodeType::BaseImage:
			m_BSOAControls[BOSAEnum::Button1].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Button1].SetWindowText("Clear all");
			m_BSOAControls[BOSAEnum::Button4].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Button4].SetWindowText("Add Begin");
			m_BSOAControls[BOSAEnum::Button5].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Button5].SetWindowText("Add End");
			break;
		case DrawNodeType::Rectangle:
		case DrawNodeType::Oval:
			m_BSOAControls[BOSAEnum::Button2].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Button2].SetWindowText("Delete Item");
			m_BSOAControls[BOSAEnum::Button4].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Button4].SetWindowText("Add Before");
			m_BSOAControls[BOSAEnum::Button5].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Button5].SetWindowText("Add After");
			break;
	}

	assert(m_availableList.size());
	if (0 < m_availableList.size())
	{
		m_comboBox2.Init(m_availableList, m_availableList[0].first, _T(""));
		m_comboBox2.ShowWindow(SW_SHOW);
	}
}

std::pair<std::string, std::string> TADialogDrawPage::getBGImageNamePair()
{
	const SvUl::InputNameObjectIdPairList& rImageList = m_ImageController.GetInputImageList(SvDef::InvalidObjectId, 1);
	if (rImageList.size())
	{
		return {rImageList.begin()->first, rImageList.begin()->second.first};
	}
	return {};
}

void TADialogDrawPage::setBaseImageGeneralCtrl()
{
	m_BSOAControls[BOSAEnum::Check1].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Check1].SetWindowText("Color Image");
	reinterpret_cast<CButton*>(&m_BSOAControls[BOSAEnum::Check1])->SetCheck(m_drawToolController.isColor());
	m_BSOAControls[BOSAEnum::Check2].ShowWindow(SW_SHOW);
	reinterpret_cast<CButton*>(&m_BSOAControls[BOSAEnum::Check2])->SetCheck(m_drawToolController.useBackgroundImage());

	if (m_drawToolController.useBackgroundImage())
	{
		m_BSOAControls[BOSAEnum::Static2].ShowWindow(SW_SHOW);
		m_BSOAControls[BOSAEnum::Static2].SetWindowText("Source Image:");
		const SvUl::NameObjectIdList& rAvailableImageList = m_ImageController.GetAvailableImageList();
		std::string selectedImageName;
		std::tie(m_availableSourceInputName, selectedImageName) = getBGImageNamePair();
		m_comboBox2.Init(rAvailableImageList, selectedImageName, NoImageTag);
		m_comboBox2.ShowWindow(SW_SHOW);

		if (false == m_drawToolController.isAutoFit())
		{
			m_BSOAControls[BOSAEnum::Static3].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Static3].SetWindowText("Offset X:");
			auto nId = static_cast<UINT>(m_BSOAControls[BOSAEnum::Edit3].GetDlgCtrlID());
			auto ctrlDataIter = std::ranges::find_if(m_editCtrlDataList, [nId](const auto& rEntry){ return rEntry.m_nIDC == nId; });
			assert(m_editCtrlDataList.end() != ctrlDataIter);
			if (m_editCtrlDataList.end() != ctrlDataIter)
			{
				auto* pEdit = static_cast<CEdit*>(&m_BSOAControls[BOSAEnum::Edit3]);
				auto* pButton = static_cast<CButton*>(&m_BSOAControls[BOSAEnum::LinkedValueButton3]);
				assert(pEdit && pButton);
				ctrlDataIter->m_Widget = std::make_unique<LinkedValueWidgetHelper>(*pEdit, *pButton, m_InspectionID, m_TaskObjectID, SvPb::PositionXEId, m_pValues.get(), ObjectSelectorData {m_TaskObjectID});
			}

			m_BSOAControls[BOSAEnum::Static4].ShowWindow(SW_SHOW);
			m_BSOAControls[BOSAEnum::Static4].SetWindowText("Offset Y:");
			nId = static_cast<UINT>(m_BSOAControls[BOSAEnum::Edit4].GetDlgCtrlID());
			ctrlDataIter = std::ranges::find_if(m_editCtrlDataList, [nId](const auto& rEntry){ return rEntry.m_nIDC == nId; });
			assert(m_editCtrlDataList.end() != ctrlDataIter);
			if (m_editCtrlDataList.end() != ctrlDataIter)
			{
				auto* pEdit = static_cast<CEdit*>(&m_BSOAControls[BOSAEnum::Edit4]);
				auto* pButton = static_cast<CButton*>(&m_BSOAControls[BOSAEnum::LinkedValueButton4]);
				assert(pEdit && pButton);
				ctrlDataIter->m_Widget = std::make_unique<LinkedValueWidgetHelper>(*pEdit, *pButton, m_InspectionID, m_TaskObjectID, SvPb::PositionYEId, m_pValues.get(), ObjectSelectorData {m_TaskObjectID});
			}
		}
	}
	else
	{
		//RedrawWindow, because else some lines of the m_comboBox2 can be left on the screen.
		RedrawWindow();
	}
	UpdateData(false);
}

void TADialogDrawPage::setBaseImageSizeCtrl()
{
	bool isAutoFit = m_drawToolController.isAutoFit();
	if (m_drawToolController.useBackgroundImage())
	{
		m_BSOAControls[BOSAEnum::Check1].ShowWindow(SW_SHOW);
		m_BSOAControls[BOSAEnum::Check1].SetWindowText("Auto Fit");
		reinterpret_cast<CButton*>(&m_BSOAControls[BOSAEnum::Check1])->SetCheck(m_drawToolController.isAutoFit());
	}
	else
	{
		isAutoFit = false;
	}

	m_BSOAControls[BOSAEnum::Static2].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Static2].SetWindowText("Width:");
	setValueCtrlData(SvPb::ExtentWidthEId, *m_pValues, BOSAEnum::Edit2, 1, 4000, "Width", isAutoFit);
	
	m_BSOAControls[BOSAEnum::Static3].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Static3].SetWindowText("Height:");
	setValueCtrlData(SvPb::ExtentHeightEId, *m_pValues, BOSAEnum::Edit3, 1, 4000, "Height", isAutoFit);
	UpdateData(false);
}

void TADialogDrawPage::setSizeCtrl(TreeNodeData& rData)
{
	assert(rData.m_pValues);
	m_BSOAControls[BOSAEnum::Static2].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Static2].SetWindowText("Width:");
	auto nId = static_cast<UINT>(m_BSOAControls[BOSAEnum::Edit2].GetDlgCtrlID());
	auto ctrlDataIter = std::ranges::find_if(m_editCtrlDataList, [nId](const auto& rEntry){ return rEntry.m_nIDC == nId; });
	assert(m_editCtrlDataList.end() != ctrlDataIter);
	if (m_editCtrlDataList.end() != ctrlDataIter)
	{
		auto* pEdit = static_cast<CEdit*>(&m_BSOAControls[BOSAEnum::Edit2]);
		auto* pButton = static_cast<CButton*>(&m_BSOAControls[BOSAEnum::LinkedValueButton2]);
		assert(pEdit && pButton);
		ctrlDataIter->m_Widget = std::make_unique<LinkedValueWidgetHelper>(*pEdit, *pButton, m_InspectionID, rData.m_objectId, SvPb::WidthEId, rData.m_pValues.get(), ObjectSelectorData {rData.m_objectId});
	}

	m_BSOAControls[BOSAEnum::Static3].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Static3].SetWindowText("Height:");
	ctrlDataIter++;
	assert(m_editCtrlDataList.end() != ctrlDataIter);
	if (m_editCtrlDataList.end() != ctrlDataIter)
	{
		auto* pEdit = static_cast<CEdit*>(&m_BSOAControls[BOSAEnum::Edit3]);
		auto* pButton = static_cast<CButton*>(&m_BSOAControls[BOSAEnum::LinkedValueButton3]);
		assert(pEdit && pButton);
		ctrlDataIter->m_Widget = std::make_unique<LinkedValueWidgetHelper>(*pEdit, *pButton, m_InspectionID, rData.m_objectId, SvPb::HeightEId, rData.m_pValues.get(), ObjectSelectorData {rData.m_objectId});
	}
	UpdateData(false);
}

void TADialogDrawPage::setPositionCtrl(TreeNodeData& rData)
{
	LPCSTR text1 = "Left:";
	SvPb::EmbeddedIdEnum eId1 {SvPb::LeftEId};
	LPCSTR text2 = "Top:";
	SvPb::EmbeddedIdEnum eId2 {SvPb::TopEId};
	switch (rData.m_type)
	{
		case DrawNodeType::Rectangle:
			break;
		case DrawNodeType::Oval:
			text1 = "CenterX:";
			eId1 = SvPb::CenterXEId;
			text2 = "CenterY:";
			eId2 = SvPb::CenterYEId;
			break;
	}

	assert(rData.m_pValues);
	m_BSOAControls[BOSAEnum::Static2].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Static2].SetWindowText(text1);
	auto nId = static_cast<UINT>(m_BSOAControls[BOSAEnum::Edit2].GetDlgCtrlID());
	auto ctrlDataIter = std::ranges::find_if(m_editCtrlDataList, [nId](const auto& rEntry){ return rEntry.m_nIDC == nId; });
	assert(m_editCtrlDataList.end() != ctrlDataIter);
	if (m_editCtrlDataList.end() != ctrlDataIter)
	{
		auto* pEdit = static_cast<CEdit*>(&m_BSOAControls[BOSAEnum::Edit2]);
		auto* pButton = static_cast<CButton*>(&m_BSOAControls[BOSAEnum::LinkedValueButton2]);
		assert(pEdit && pButton);
		ctrlDataIter->m_Widget = std::make_unique<LinkedValueWidgetHelper>(*pEdit, *pButton, m_InspectionID, rData.m_objectId, eId1, rData.m_pValues.get(), ObjectSelectorData {rData.m_objectId});
	}

	m_BSOAControls[BOSAEnum::Static3].ShowWindow(SW_SHOW);
	m_BSOAControls[BOSAEnum::Static3].SetWindowText(text2);
	ctrlDataIter++;
	assert(m_editCtrlDataList.end() != ctrlDataIter);
	if (m_editCtrlDataList.end() != ctrlDataIter)
	{
		auto* pEdit = static_cast<CEdit*>(&m_BSOAControls[BOSAEnum::Edit3]);
		auto* pButton = static_cast<CButton*>(&m_BSOAControls[BOSAEnum::LinkedValueButton3]);
		assert(pEdit && pButton);
		ctrlDataIter->m_Widget = std::make_unique<LinkedValueWidgetHelper>(*pEdit, *pButton, m_InspectionID, rData.m_objectId, eId2, rData.m_pValues.get(), ObjectSelectorData {rData.m_objectId});
	}
	UpdateData(false);
}

void TADialogDrawPage::setColorCtrl(TreeNodeData& rData)
{
	assert(rData.m_pValues);
	if (m_drawToolController.isColor())
	{
		m_BSOAControls[BOSAEnum::Static2].ShowWindow(SW_SHOW);
		m_BSOAControls[BOSAEnum::Static2].SetWindowText("Red:");
		setValueCtrlData(SvPb::Color1EId, *rData.m_pValues, BOSAEnum::Edit2, 0, 255, "Red");
		m_BSOAControls[BOSAEnum::Static3].ShowWindow(SW_SHOW);
		m_BSOAControls[BOSAEnum::Static3].SetWindowText("Green:");
		setValueCtrlData(SvPb::Color2EId, *rData.m_pValues, BOSAEnum::Edit3, 0, 255, "Green");
		m_BSOAControls[BOSAEnum::Static4].ShowWindow(SW_SHOW);
		m_BSOAControls[BOSAEnum::Static4].SetWindowText("Blue:");
		setValueCtrlData(SvPb::Color3EId, *rData.m_pValues, BOSAEnum::Edit4, 0, 255, "Blue");
	}
	else
	{
		m_BSOAControls[BOSAEnum::Static2].ShowWindow(SW_SHOW);
		m_BSOAControls[BOSAEnum::Static2].SetWindowText("Gray:");
		setValueCtrlData(SvPb::Color1EId, *rData.m_pValues, BOSAEnum::Edit2, 0, 255, "Gray");
	}
}

void TADialogDrawPage::OnSelchangeCombo2()
{
	if (0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData)
		{
			switch (pData->m_type)
			{
				case DrawNodeType::BaseImage:
					switch (pData->m_subType)
					{
						case DrawNodeSubType::MainNode:
							//Nothing to do, because the value is only needed if a button is pressed
							break;
						case DrawNodeSubType::GeneralData:
							OnSelchangeBackGroundCombo();
							break;
					}
			}
		}
	}
}

void TADialogDrawPage::OnSelchangeBackGroundCombo()
{
	UpdateData(TRUE); // get data from dialog

	int index = m_comboBox2.GetCurSel();
	if (LB_ERR != index)
	{
		bool bIsValid = m_ImageController.IsToolValid();
		CString imageName;
		m_comboBox2.GetLBText(index, imageName);
		if (!imageName.IsEmpty() && imageName != NoImageTag)
		{
			std::string svImageName(imageName);
			//setImage must be before ConnectToImage because ConnectToImage does a reset and then it cannot get the image.
			IPictureDisp* pImage = m_ImageController.GetImage(svImageName);
			m_dialogImage.setImage(pImage);
			m_ImageController.ConnectToImage(m_availableSourceInputName, svImageName);
			SvStl::MessageContainerVector errorMessages;
			HRESULT result = m_ImageController.ResetTask(errorMessages);

			if (bIsValid && S_OK != result)
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_ToolPositionError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10232);
				return;
			}
			m_ImageController.ToolRunOnce();
			refresh();
		}
	}
}

void TADialogDrawPage::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_treeCtrl.OnBeginLabelEdit(pNMHDR, pResult);
}

void TADialogDrawPage::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_treeCtrl.OnEndLabelEdit(pNMHDR, pResult);
}

void TADialogDrawPage::OnDragTree(NMHDR* pNMHDR, LRESULT*)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if (nullptr != pNMTreeView)
	{
		m_isDragging = m_treeCtrl.OnDragTree(*pNMTreeView);
		if (m_isDragging)
		{
			ShowCursor(false);
			SetCapture();
			SetTimer(1, 25, NULL);
		}
	}
}

void TADialogDrawPage::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isDragging)
	{
		m_treeCtrl.OnMouseMove(nFlags, point);
	}

	__super::OnMouseMove(nFlags, point);
}

void TADialogDrawPage::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_isDragging)
	{
		KillTimer(1);

		ReleaseCapture();
		ShowCursor(true);
		m_isDragging = false;
		if (m_treeCtrl.OnLButtonUp())
		{
			m_pValues->RunOnce(m_InspectionID);
			refresh();
		}
	}

	__super::OnLButtonUp(nFlags, point);
}

void TADialogDrawPage::refreshNodeText(HTREEITEM hItem)
{
	auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(hItem));
	if (0 != hItem && nullptr != pData)
	{
		std::string textStr = m_drawToolController.getNodeText(*pData);

		if (DrawNodeSubType::GeneralData == pData->m_subType && m_drawToolController.useBackgroundImage())
		{
			auto [_, selectedImageName] = getBGImageNamePair();
			textStr += selectedImageName;
		}
		if (false == textStr.empty())
		{
			m_treeCtrl.SetItemText(hItem, textStr.c_str());
		}
	}
}

void TADialogDrawPage::refreshAllChildItems(HTREEITEM hItem)
{
	HTREEITEM hChild = m_treeCtrl.GetNextItem(hItem, TVGN_CHILD);
	while (nullptr != hChild)
	{
		refreshNodeText(hChild);
		refreshAllChildItems(hChild);
		hChild = m_treeCtrl.GetNextItem(hChild, TVGN_NEXT);
	}
}

void TADialogDrawPage::refreshTree()
{
	HTREEITEM hItem = m_treeCtrl.GetNextItem(nullptr, TVGN_ROOT);
	while (nullptr != hItem)
	{
		refreshNodeText(hItem);
		refreshAllChildItems(hItem);
		hItem = m_treeCtrl.GetNextItem(hItem, TVGN_NEXT);
	}
}

void TADialogDrawPage::updateImages()
{
	IPictureDisp* pOutputImage = m_ImageController.GetImage(SvPb::OutputImageEId);
	assert(pOutputImage);
	m_dialogImage.setImage(pOutputImage, m_OutputTabHandle);
	m_dialogImage.Refresh();
	resetOverlay();
}

void TADialogDrawPage::resetOverlay()
{
	m_dialogImage.RemoveOverlay(0, m_handleToOverlayObjects);
	m_handleToOverlayObjects = -1;
	if (0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData && pData->m_pValues)
		{
			//add new shape types
			std::map<long, long> ParMap;
			bool bMove = false;
			bool bAll = false;
			switch (pData->m_type)
			{
				case DrawNodeType::Rectangle:
				{
					long x = pData->m_pValues->Get<LinkedValueData>(SvPb::LeftEId).m_Value;
					long y = pData->m_pValues->Get<LinkedValueData>(SvPb::TopEId).m_Value;
					long width = pData->m_pValues->Get<LinkedValueData>(SvPb::WidthEId).m_Value;
					long height = pData->m_pValues->Get<LinkedValueData>(SvPb::HeightEId).m_Value;
					ParMap[CDSVPictureDisplay::P_X1] = x;
					ParMap[CDSVPictureDisplay::P_Y1] = y;
					ParMap[CDSVPictureDisplay::P_X2] = x+width;
					ParMap[CDSVPictureDisplay::P_Y2] = y+height;
					ParMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::RectangleROI;
					bMove = isChangable(*pData->m_pValues, SvPb::LeftEId) && isChangable(*pData->m_pValues, SvPb::TopEId);
					bAll = bMove && isChangable(*pData->m_pValues, SvPb::WidthEId) && isChangable(*pData->m_pValues, SvPb::HeightEId);
					break;
				}
				case DrawNodeType::Oval:
				{
					long x = pData->m_pValues->Get<LinkedValueData>(SvPb::CenterXEId).m_Value;
					long y = pData->m_pValues->Get<LinkedValueData>(SvPb::CenterYEId).m_Value;
					long width = pData->m_pValues->Get<LinkedValueData>(SvPb::WidthEId).m_Value;
					long height = pData->m_pValues->Get<LinkedValueData>(SvPb::HeightEId).m_Value;
					ParMap[CDSVPictureDisplay::P_X1] = x - width / 2;
					ParMap[CDSVPictureDisplay::P_Y1] = y - height / 2;
					ParMap[CDSVPictureDisplay::P_X2] = x + static_cast<long>(ceil((width+1) / 2.0));
					ParMap[CDSVPictureDisplay::P_Y2] = y + static_cast<long>(ceil((height+1) / 2.0));
					ParMap[CDSVPictureDisplay::P_Type] = CDSVPictureDisplay::EllipseROI;
					bMove = isChangable(*pData->m_pValues, SvPb::CenterXEId) && isChangable(*pData->m_pValues, SvPb::CenterYEId);
					bAll = bMove && isChangable(*pData->m_pValues, SvPb::WidthEId) && isChangable(*pData->m_pValues, SvPb::HeightEId);
					break;
				}
				default:
					return;
			}
			ParMap[CDSVPictureDisplay::P_Color] = SvDef::Green;
			ParMap[CDSVPictureDisplay::P_SelectedColor] = SvDef::Green;
			ParMap[CDSVPictureDisplay::P_AllowEdit] = bAll ? CDSVPictureDisplay::AllowResizeAndMove : bMove ? CDSVPictureDisplay::AllowMove : CDSVPictureDisplay::AllowNone;

			COleSafeArray saPar, saVal;
			SvOg::DisplayHelper::CreateSafeArrayFromMap(ParMap, saPar, saVal);
			m_dialogImage.AddOverlay(0, static_cast<LPVARIANT>(saPar), static_cast<LPVARIANT>(saVal), &m_handleToOverlayObjects);
			m_dialogImage.EditOverlay(0, m_handleToOverlayObjects, static_cast<LPVARIANT>(saPar), static_cast<LPVARIANT>(saVal));
			saVal.Destroy();
			saPar.Destroy();
		}
	}
}

void TADialogDrawPage::addNodeInto(HTREEITEM item, SvPb::ClassIdEnum classId, DrawTaskTree::addPosEnum posEnum)
{
	auto iter = std::ranges::find_if(c_typeConvVec, [classId](const auto& rEntry) { return rEntry.first == classId; });
	assert(c_typeConvVec.end() != iter);
	if(c_typeConvVec.end() != iter)
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto [hInsertInto, hInsertAfter] = m_treeCtrl.prepareAddInto(item, classId, posEnum, *requestCmd.mutable_createobjectrequest());
		if (nullptr != hInsertInto && nullptr != hInsertAfter)
		{
			HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_createobjectresponse())
			{
				m_treeCtrl.addNodes(iter->second, hInsertInto, responseCmd.createobjectresponse().objectid(), responseCmd.createobjectresponse().name(), hInsertAfter);
				m_pValues->RunOnce(m_InspectionID);
			}
			else
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_CreationFilterFailed, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
}

void TADialogDrawPage::loadNode(HTREEITEM item, uint32_t objectId)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableobjectsrequest();
	pRequest->set_objectid(objectId);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::DrawTaskType);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		for (const auto& rTaskPair : responseCmd.getavailableobjectsresponse().list())
		{
			SvPb::InspectionCmdRequest requestParameterCmd;
			SvPb::InspectionCmdResponse responseParameterCmd;
			auto* pParameterRequest = requestParameterCmd.mutable_getobjectparametersrequest();
			pParameterRequest->set_objectid(rTaskPair.objectid());
			hr = SvCmd::InspectionCommands(m_InspectionID, requestParameterCmd, &responseParameterCmd);
			if (S_OK == hr && responseParameterCmd.has_getobjectparametersresponse())
			{
				m_treeCtrl.addNodes(responseParameterCmd.getobjectparametersresponse().typeinfo().subtype(), item, rTaskPair.objectid(), rTaskPair.objectname());
			}
		}
	}
}

void TADialogDrawPage::hideAllBOSACtrl()
{
	for (auto& rCtrl : m_BSOAControls)
	{
		rCtrl.ShowWindow(SW_HIDE);
	}

	m_comboBox2.ShowWindow(SW_HIDE);

	for (auto& rCtrl : m_editCtrlDataList)
	{
		rCtrl.m_useMinMax = false;
		rCtrl.m_fieldName = "";
		rCtrl.m_Widget = nullptr;
	}
}

void TADialogDrawPage::setValueCtrlData(SvPb::EmbeddedIdEnum embeddedId, ValueController& rValueController, TADialogDrawPage::BOSAEnum ctrlEnum, int min, int max, const std::string& fieldName, bool readOnly)
{
	auto nId = static_cast<UINT>(m_BSOAControls[ctrlEnum].GetDlgCtrlID());
	auto ctrlDataIter = std::ranges::find_if(m_editCtrlDataList, [nId](const auto& rEntry){ return rEntry.m_nIDC == nId; });
	assert(m_editCtrlDataList.end() != ctrlDataIter);
	if (m_editCtrlDataList.end() != ctrlDataIter)
	{
		m_BSOAControls[ctrlEnum].ShowWindow(SW_SHOW);
		m_BSOAControls[ctrlEnum].EnableWindow(false == readOnly);
		ctrlDataIter->m_value = rValueController.Get<int>(embeddedId);
		m_BSOAControls[ctrlEnum].SetWindowText(std::to_string(ctrlDataIter->m_value).c_str());
		ctrlDataIter->m_useMinMax = true;
		ctrlDataIter->m_fieldName = fieldName;
		ctrlDataIter->m_min = min;
		ctrlDataIter->m_max = max;
	}
}

void TADialogDrawPage::ObjectChangedExDialogImage(long, long, VARIANT* ParameterList, VARIANT* ParameterValue)
{
	////////////////////////////////////////////////////////
	// SET SHAPE PROPERTIES
	VariantParamMap ParaMap;
	SvOg::DisplayHelper::FillParameterMap(ParaMap, ParameterList, ParameterValue);
	bool isResizeable = 0 != (static_cast<int>(ParaMap[CDSVPictureDisplay::P_AllowEdit]) & static_cast<int>(CDSVPictureDisplay::AllowResize));
	bool isMoveable = 0 != (static_cast<int>(ParaMap[CDSVPictureDisplay::P_AllowEdit]) & static_cast<int>(CDSVPictureDisplay::AllowMove));

	if (0 != m_currentItem)
	{
		auto* pData = reinterpret_cast<TreeNodeData*>(m_treeCtrl.GetItemData(m_currentItem));
		if (nullptr != pData && pData->m_pValues)
		{
			long width = ParaMap[CDSVPictureDisplay::P_X2].lVal - ParaMap[CDSVPictureDisplay::P_X1].lVal;
			long Height = ParaMap[CDSVPictureDisplay::P_Y2].lVal - ParaMap[CDSVPictureDisplay::P_Y1].lVal;

			switch (pData->m_type)
			{
				case DrawNodeType::Rectangle:
				{
					if (isResizeable)
					{
						pData->m_pValues->Set(SvPb::WidthEId, width);
						pData->m_pValues->Set(SvPb::HeightEId, Height);
					}
					if (isMoveable)
					{
						pData->m_pValues->Set(SvPb::LeftEId, ParaMap[CDSVPictureDisplay::P_X1].lVal);
						pData->m_pValues->Set(SvPb::TopEId, ParaMap[CDSVPictureDisplay::P_Y1].lVal);
					}
					break;
				}
				case DrawNodeType::Oval:
				{
					if (isResizeable)
					{
						pData->m_pValues->Set(SvPb::WidthEId, width);
						pData->m_pValues->Set(SvPb::HeightEId, Height);
					}
					if (isMoveable)
					{
						long CenterX = ParaMap[CDSVPictureDisplay::P_X1].lVal + width / 2;
						long CenterY = ParaMap[CDSVPictureDisplay::P_Y1].lVal + Height / 2;
						pData->m_pValues->Set(SvPb::CenterXEId, CenterX);
						pData->m_pValues->Set(SvPb::CenterYEId, CenterY);
					}
					break;
				}
			}

			pData->m_pValues->Commit();
			refresh();
			setBOSACtrl();
		}
	}
}
} //namespace SvOg