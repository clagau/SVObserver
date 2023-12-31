//*****************************************************************************
/// \copyright (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgTableParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the Table-tool.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "SVOGuiUtility/DataController.h"
#include "ObjectInterfaces/IFormulaController.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgTableParameterPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TADlgTableParameterPage)
		enum { IDD = IDD_TA_TABLE_PARAMETER_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		TADlgTableParameterPage(uint32_t inspectionID, uint32_t taskID);

		// Standard destructor
		virtual ~TADlgTableParameterPage();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TADlgTableParameterPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		//}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(TADlgTableParameterPage)
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

		SvOgu::ValueController m_values;
#pragma endregion Member Variables
	};
} //namespace SvOg
