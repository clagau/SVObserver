//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADialogGroupToolResultPage.h
/// This is the class for the result tab of Group Tool
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/GroupToolHelper.h"
#include "ISVPropertyPageDialog.h"
#pragma endregion Includes


namespace SvOg
{
	class TADialogGroupToolResultPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TADialogGroupToolResultPage)
		enum { IDD = IDD_GROUPTOOL_INPUTRESULT_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		/// \param inspectionId [int]
		/// \param toolId [in]
		/// \param taskId [in]
		/// \param isInputsChangeAble [in] If false, for the inputs only the values is changeable.
		TADialogGroupToolResultPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId, bool isInputsChangeAble = true);

		// Standard destructor
		virtual ~TADialogGroupToolResultPage() = default;
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TADialogGroupToolResultPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(TADialogGroupToolResultPage)
		virtual BOOL OnInitDialog() override;

		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;

		void OnGridClick(NMHDR*, LRESULT*);
		void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
		void OnGridValueSelectionChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
		void OnSelectionChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

		void OnBnClickedButtonRemove();
		void OnBnClickedButtonAdd();
		void OnKillFocusComment();

		void OnBnClickedMoveUp();
		void OnBnClickedMoveDown();
		//}}AFX_MSG

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		bool setInspectionData(bool displayMessageBox = true);

		/// Initialized the grid control. Define the number and size of the column and add the header names.
		void initGridControl();

		// Fill the grid Control. Define the number of rows and fill it with the data.
		void FillGridControl();

		void setValueColumn(int pos);

		/// Show the context menu for right mouse click
		/// \param point [in] Mouse position
		void showContextMenu(CPoint point);

		SvDef::NameValuePair getType(const std::string& rTypeName);

		void loadDataList();

		void UpdateEnableButtons();

		bool OnValueChanged(int row, int column);

		void setGridControlReadOnlyFlag(int row, int column, bool isChangeable);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_toolId;
		const uint32_t m_TaskObjectID;

		const bool m_isChangeAble;

		CBitmap m_downArrowBitmap;
		CImageList m_ImageList;
		SvOgu::ValueController m_Values;

		SvGcl::GridCtrl m_Grid;						//The grid displaying the name and the formulas
		std::vector<SvOgu::GroupInputResultData> m_resultData;
		CString m_strComment;

		SvStl::MessageContainerVector m_errorMessages;
#pragma endregion Member Variables
	};
} //namespace SvOg
#pragma once
