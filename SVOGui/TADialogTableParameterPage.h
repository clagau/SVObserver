//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// \file TADialogTableParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the Table-tool.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "ObjectInterfaces/IFormulaController.h"
#pragma endregion Includes

namespace SvOg
{
	class TADialogTableParameterPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TADialogTableParameterPage)
		enum { IDD = IDD_TA_TABLE_PARAMETER_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		TADialogTableParameterPage(uint32_t inspectionID, uint32_t taskID);

		// Standard destructor
		virtual ~TADialogTableParameterPage();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TADialogTableParameterPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		//}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(TADialogTableParameterPage)
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnKillActive() override;
		void OnBnClickedButtonFormula();
		//}}AFX_MSG

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		/// Set the date of the page in the business logic
		/// \returns HRESULT
		HRESULT SetPageData();

		/// Set the equation text to control
		void setEquationText();

		//Reset the inspection.
		void resetInspection();
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		uint32_t m_InspectionID;
		uint32_t m_TaskObjectID;
		uint32_t m_ClearEquationID = SvDef::InvalidObjectId;
		long m_maxRows;

		SvOi::IFormulaControllerPtr m_pFormulaController;
		CString m_clearString;

		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
		Controller m_Values;
#pragma endregion Member Variables
	};
} //namespace SvOg
