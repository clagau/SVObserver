//*****************************************************************************
/// \copyright (c) 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the tool adjustment dialog for Draw-tool
//*****************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/DrawTaskTree.h"
#include "SVOGuiUtility/DrawToolController.h"
#include "SVOGuiUtility/DrawToolHelperStructs.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVMFCControls/AvailableObjectListComboBox.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class TADialogDrawPage : public CPropertyPage
	{
	public:
		TADialogDrawPage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~TADialogDrawPage();

		enum BOSAEnum
		{
			Check1,
			Check2,
			Check3,
			Check4,
			Check5,
			Static1,
			Edit1,
			LinkedValueButton1,
			Static2,
			Edit2,
			LinkedValueButton2,
			Static3,
			Edit3,
			LinkedValueButton3,
			Static4,
			Edit4,
			LinkedValueButton4,
			Static5,
			Edit5,
			LinkedValueButton5,
			Edit6,
			LinkedValueButton6,
			Edit7,
			LinkedValueButton7,
			Edit8,
			LinkedValueButton8,
			Edit9,
			LinkedValueButton9,
			Edit10,
			LinkedValueButton10,
			Button1,
			Button2,
			Button3,
			Button4,
			Button5,
			Button6,
			NUM_OF_CONTROL
		};

	protected:
		void refresh();
		SvOgu::ImageController& getImageController() { return m_ImageController; }

		//{{AFX_MSG(TADialogDrawPage)
		virtual BOOL OnInitDialog() override;
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		virtual BOOL OnKillActive() override;
		void OnSelchangedTree(NMHDR*, LRESULT* pResult);
		void OnButtonCheck(UINT nID);
		void OnButtonButton(UINT nID);
		void OnColorButton(UINT nID);
		void OnButtonLinkedValue(UINT nID);
		void OnKillFocusEdit(UINT nID);
		void OnSelchangeCombo2();
		void OnSelchangeBackGroundCombo();
		void OnSelchangeGeneralCombo(SvOgu::TreeNodeData& rData);
		void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
		void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
		void OnDragTree(NMHDR* pNMHDR, LRESULT* pResult);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
		DECLARE_EVENTSINK_MAP()

		//{{AFX_VIRTUAL(TADialogDrawPage)
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
		//}}AFX_VIRTUAL

		//{{AFX_DATA(TADialogDrawPage)
		enum { IDD = IDD_TA_DRAW_DIALOG };
		SvMc::AvailableObjectListComboBox<uint32_t> m_comboBox2;
		SvMc::SVEnumerateCombo m_comboBox2Enum;
		SvOgu::DrawTaskTree m_treeCtrl;
		SvOgu::PictureDisplay m_dialogImage;
		//}}AFX_DATA


	private:
		void refreshNodeText(HTREEITEM hItem);
		void refreshAllChildItems(HTREEITEM hItem);
		void refreshTree();
		void updateImages();
		void resetOverlay();
		bool setBOSAData();
		void setBOSACtrl();
		void setAddTaskCtrl(const SvOgu::TreeNodeData& rData);
		void setBaseImageGeneralCtrl();
		void setBaseImageSizeCtrl();
		void setSizeCtrl(SvOgu::TreeNodeData& rData);
		void setColorCtrl(SvOgu::TreeNodeData& rData);
		void setAngleCtrl(SvOgu::TreeNodeData& rData);
		void setGeneralCtrl(SvOgu::TreeNodeData& rData);
		void setPositionCtrl(SvOgu::TreeNodeData& rData);
		void setPointsCtrl(SvOgu::TreeNodeData& rData);
		void addNodeInto(HTREEITEM item, SvPb::ClassIdEnum classId, SvOgu::DrawTaskTree::addPosEnum atBegin);
		void loadNode(HTREEITEM item, uint32_t objectId);
		void hideAllBOSACtrl();
		void setValueCtrlData(SvPb::EmbeddedIdEnum embeddedId, SvOgu::ValueController& rValueController, TADialogDrawPage::BOSAEnum ctrlEnum, int min, int max, const std::string& fieldName, bool readOnly = false);
		void ObjectChangedExDialogImage(long, long, VARIANT* ParameterList, VARIANT* ParameterValue);
		void setControl(TADialogDrawPage::BOSAEnum editEnum, TADialogDrawPage::BOSAEnum buttonEnum, SvPb::EmbeddedIdEnum embeddedId, SvOgu::TreeNodeData& rData, SvOgu::ValidCheckCallback validCallback = nullptr);
		void setControl(TADialogDrawPage::BOSAEnum staticEnum, LPCSTR staticText, TADialogDrawPage::BOSAEnum editEnum, TADialogDrawPage::BOSAEnum buttonEnum, SvPb::EmbeddedIdEnum embeddedId, SvOgu::TreeNodeData& rData, SvOgu::ValidCheckCallback validCallback = nullptr);
		void setOrRemoveColorItemForBase();
	
	private:
		uint32_t m_InspectionID;
		uint32_t m_TaskObjectID;
		
		SvOgu::ImageController m_ImageController;
		long m_OutputTabHandle {0};
		std::shared_ptr<SvOgu::ValueController> m_pValues;

		std::vector<std::pair<std::string, uint32_t>> m_availableList;

		HTREEITEM m_currentItem = nullptr;
		bool m_isDragging = false;

		CMFCButton m_colorButton;
		CMFCButton m_colorButton2;

		//overlay
		long m_handleToOverlayObjects = -1;

		std::array<SvOgu::EditCtrlData, SvOgu::NumberOfCtrls> m_editCtrlDataList = {SvOgu::EditCtrlData{IDC_EDIT1}, SvOgu::EditCtrlData{IDC_EDIT2}, SvOgu::EditCtrlData{IDC_EDIT3}, SvOgu::EditCtrlData{IDC_EDIT4}, SvOgu::EditCtrlData{IDC_EDIT5},
			SvOgu::EditCtrlData{IDC_EDIT6}, SvOgu::EditCtrlData{IDC_EDIT7}, SvOgu::EditCtrlData{IDC_EDIT8}, SvOgu::EditCtrlData{IDC_EDIT9}, SvOgu::EditCtrlData{IDC_EDIT10}};
		SvOgu::DrawToolController m_drawToolController;

		std::array<CWnd, BOSAEnum::NUM_OF_CONTROL> m_BSOAControls;
	};
} //namespace SvOg
