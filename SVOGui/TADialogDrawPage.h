//*****************************************************************************
/// \copyright (c) 2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of the tool adjustment dialog for Draw-tool
//*****************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "DataController.h"
#include "DrawTaskTree.h"
#include "DrawToolController.h"
#include "DrawToolHelperStructs.h"
#include "ImageController.h"
#pragma endregion Includes

namespace SvOg
{
	class TADialogDrawPage : public CPropertyPage
	{
	public:
		TADialogDrawPage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~TADialogDrawPage();
		//HRESULT SetInspectionData();

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
		ImageController& getImageController() { return m_ImageController; }

		//{{AFX_MSG(TADialogDrawPage)
		virtual BOOL OnInitDialog() override;
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		void OnSelchangedTree(NMHDR*, LRESULT* pResult);
		void OnButtonCheck(UINT nID);
		void OnButtonButton(UINT nID);
		void OnButtonLinkedValue(UINT nID);
		void OnKillFocusEdit(UINT nID);
		void OnSelchangeCombo2();
		void OnSelchangeBackGroundCombo();
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
		std::string m_availableSourceInputName;
		DrawTaskTree m_treeCtrl;
		PictureDisplay m_dialogImage;
		//}}AFX_DATA


	private:
		void refreshNodeText(HTREEITEM hItem);
		void refreshAllChildItems(HTREEITEM hItem);
		void refreshTree();
		void updateImages();
		void resetOverlay();
		bool setBOSAData();
		void setBOSACtrl();
		void setAddTaskCtrl(const TreeNodeData& rData);
		void setBaseImageGeneralCtrl();
		void setBaseImageSizeCtrl();
		void setSizeCtrl(TreeNodeData& rData);
		void setColorCtrl(TreeNodeData& rData);
		void setPositionCtrl(TreeNodeData& rData);
		void addNodeInto(HTREEITEM item, SvPb::ClassIdEnum classId, DrawTaskTree::addPosEnum atBegin);
		void loadNode(HTREEITEM item, uint32_t objectId);
		void hideAllBOSACtrl();
		void setValueCtrlData(SvPb::EmbeddedIdEnum embeddedId, ValueController& rValueController, TADialogDrawPage::BOSAEnum ctrlEnum, int min, int max, const std::string& fieldName, bool readOnly = false);
		std::pair<std::string, std::string> getBGImageNamePair();
		void ObjectChangedExDialogImage(long, long, VARIANT* ParameterList, VARIANT* ParameterValue);
	
	private:
		uint32_t m_InspectionID;
		uint32_t m_TaskObjectID;
		
		ImageController m_ImageController;
		long m_OutputTabHandle {0};
		std::shared_ptr<ValueController> m_pValues;

		std::vector<std::pair<std::string, uint32_t>> m_availableList;

		HTREEITEM m_currentItem = nullptr;
		bool m_isDragging = false;

		//overlay
		long m_handleToOverlayObjects = -1;

		std::array<EditCtrlData, 5> m_editCtrlDataList = {EditCtrlData{IDC_EDIT1}, EditCtrlData{IDC_EDIT2}, EditCtrlData{IDC_EDIT3}, EditCtrlData{IDC_EDIT4}, EditCtrlData{IDC_EDIT5}};
		DrawToolController m_drawToolController;

		std::array<CWnd, BOSAEnum::NUM_OF_CONTROL> m_BSOAControls;
	};
} //namespace SvOg
