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
#include "GuiController.h"
#include "ObjectSelectorController.h"
#include "ISVPropertyPageDialog.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui {
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

		virtual BOOL OnInitDialog() override;
		void OnButtonClearAll();
		void OnButtonDeleteCurrentAnalyzer();
		void OnButtonInsertNewAnalyzer();
		void OnSelchangeAnalyzerList();
		void OnChangeColumnSelection();
		void OnButtonClickExcludeHigh();
		void OnButtonClickExcludeLow();
		void OnButtonClickLimitValue();

		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
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

		/// Set the data of the old (current) selection and if this failed, it display an error and reset the selection to this analyzer.
		/// \returns HRESULT S_OK, if setting works.
		HRESULT prepareSwitchOfAnalyzerSelection();
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
		CBitmap m_downArrowBitmap;
		ObjectSelectorController m_objectSelector;

		int m_SortDirection;
		SVGUID m_InspectionID;
		SVGUID m_TaskObjectID;
		SVGUID m_selectedAnalyzerID;
		long m_selectedSubType;
		SvUl::NameGuidList m_availableColumn;
		SVString m_inputName; 

		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
		SVSharedPtr<Controller> m_Values;
#pragma endregion Member Variables
	};
}}

namespace SvOg = Seidenader::SVOGui;