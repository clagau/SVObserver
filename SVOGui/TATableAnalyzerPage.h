//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TaTableAnalyzerPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Tool adjustment page to control the analyzer for the table.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVMFCControls\ObjectsListBox.h"
#include "SVUtilityLibrary\NameGuidList.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "ObjectSelectorController.h"
#include "ISVPropertyPageDialog.h"
#include "ObjectInterfaces\IFormulaController.h"
#pragma endregion Includes

namespace SvOg
{
	class TaTableAnalyzerPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TaTableAnalyzerPage)
		enum { IDD = IDD_TA_TABLEANALYZER_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		TaTableAnalyzerPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
		virtual ~TaTableAnalyzerPage();
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
		void OnAddColumnFormula();

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

		/// Update the displayed property controls of the selected sort analyzer.
		void setSortProperties();

		/// Update the column combo box depend of the selected analyzer..
		void setColumnSelectionCB();

		/// Update the displayed property controls of the selected exclude analyzer.
		void setExcludeProperties();

		/// Update the displayed property controls of the selected limit analyzer.
		void setLimitProperties();

		void setAddColumnProperties();

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
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvMc::ObjectsListBox m_analyzerListBox;
		SvMc::AvailableObjectListComboBox m_availableAnaylzerCB;
		SvMc::AvailableObjectListComboBox m_columnSelectionCB;
		CEdit m_EditExcludeHigh;
		CEdit m_EditExcludeLow;
		CEdit m_EditLimitValue;
		CButton m_ButtonExcludeHigh;
		CButton m_ButtonExcludeLow;
		CButton m_ButtonLimitValue;
		CEdit m_EditAddColumnName;
		CBitmap m_downArrowBitmap;
		ObjectSelectorController m_objectSelector;
		CString m_AddFormulaString;

		int m_SortDirection;
		const SVGUID m_InspectionID;
		const SVGUID m_TaskObjectID;
		SVGUID m_selectedAnalyzerID;
		long m_selectedSubType;
		SvUl::NameGuidList m_availableColumn;
		std::string m_inputName; 

		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
		std::shared_ptr<Controller> m_pValues;

		SvOi::IFormulaControllerPtr m_pSelectedAddEquationFormula;
#pragma endregion Member Variables
	};
} //namespace SvOg
