//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgTableAnalyzerPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to control the analyzer for the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls/AvailableObjectListComboBox.h"
#include "SVMFCControls/ObjectsListBox.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#include "ISVPropertyPageDialog.h"
#include "ObjectInterfaces/IFormulaController.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgTableAnalyzerPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TADlgTableAnalyzerPage)
		enum { IDD = IDD_TA_TABLEANALYZER_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		TADlgTableAnalyzerPage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~TADlgTableAnalyzerPage();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
		virtual BOOL OnInitDialog() override;
		void OnButtonClearAll();

		void OnButtonDeleteCurrentAnalyzer();
		void OnButtonInsertNewAnalyzer();
		void OnSelchangeAnalyzerList();
		void OnChangeColumnSelection();
		void OnButtonClickExcludeHigh();
		void OnButtonClickExcludeLow();
		void OnButtonClickLimitValue();
		void OnKillFocusExcludeHigh();
		void OnKillFocusExcludeLow();
		void OnKillFocusLimitValue();
		void OnAddColumnFormula();

		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;

		HRESULT SetInspectionData();
#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		/// Refresh the analyzer lists and update the displayed controls depend of the selected analyzer.
		void refresh();

		/// Update the displayed property controls depend of the selected analyzer.
		void SetPropertyControls( );

		/// Show or hide controls depend of the selected analyzer.
		void ShowControls(long SubType);

		/// Retrieve the available analyzer in the list
		HRESULT RetrieveAvailableColumnList();

		void updateValueController();

		void resetValueController();
		/// Update the displayed property controls of the selected sort analyzer.
		void setSortProperties();

		/// Update the column combo box depend of the selected analyzer..
		void setColumnSelectionCB(bool useSourceAvailable = false);

		/// Update the displayed property controls of the selected exclude analyzer.
		void setExcludeProperties();

		/// Update the displayed property controls of the selected limit analyzer.
		void setLimitProperties();

		void setAddColumnProperties();
		void setDeleteColumnProperties();

		/// Set the data of the old (current) selection and if this failed, it display an error and reset the selection to this analyzer.
		/// \returns HRESULT S_OK, if setting works.
		HRESULT prepareSwitchOfAnalyzerSelection();

		/// Reset one analyzer by one analyzer and check if the correct. If the find an error display an message and set the focus to this analyzer.
		/// \returns HRESULT S_OK, if all analyzer correct.
		HRESULT checkAllAnaylzer();

		/// Set Data for the Add-Analyzer
		/// \param rErrorMessageList [in,out] List of ErrorMessages.
		/// \returns HRESULT S_OK, if setting correct.
		HRESULT SetAddAnalyzerData(SvStl::MessageContainerVector &rErrorMessageList);

		SvUl::NameObjectIdList getTableAnalyzer();

		void setSourceTableObjectId();
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvMc::ObjectsListBox m_analyzerListBox;
		SvMc::AvailableObjectListComboBox<SvPb::ClassIdEnum> m_availableAnaylzerCB;
		SvMc::AvailableObjectListComboBox<uint32_t> m_columnSelectionCB;
		CEdit m_EditExcludeHigh;
		CEdit m_EditExcludeLow;
		CEdit m_EditLimitValue;
		CButton m_ButtonExcludeHigh;
		CButton m_ButtonExcludeLow;
		CButton m_ButtonLimitValue;
		std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_ExcludeHighWidget;
		std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_ExcludeLowWidget;
		std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_LimitWidget;
		CEdit m_EditAddColumnName;
		CString m_AddFormulaString;

		int m_SortDirection;
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		uint32_t m_selectedAnalyzerID = SvDef::InvalidObjectId;
		long m_selectedSubType;
		SvUl::NameObjectIdList m_availableColumn;
		SvUl::NameObjectIdList m_availableSourceColumn;
		std::string m_inputName; 
		uint32_t m_sourceTableObjectId = SvDef::InvalidObjectId;

		std::shared_ptr<SvOgu::ValueController> m_pValues;

		SvOi::IFormulaControllerPtr m_pSelectedAddEquationFormula;
#pragma endregion Member Variables
	};
} //namespace SvOg
