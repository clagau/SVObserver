//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// \file TADialogTableDefinesPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet to defines the table in the Table-tool.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "ISVPropertyPageDialog.h"
#pragma endregion Includes

namespace SvOg
{
	class TADialogTableDefinesPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TADialogTableDefinesPage)
		enum { IDD = IDD_TA_TABLE_DEFINE_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		TADialogTableDefinesPage( uint32_t inspectionId, uint32_t taskObjectId );

		// Standard destructor
		virtual ~TADialogTableDefinesPage();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TADialogTableDefinesPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		//}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(TADialogTableDefinesPage)
		virtual BOOL OnInitDialog() override;

		virtual BOOL OnKillActive( ) override;
		void OnBnClickedButtonRemove();
		void OnBnClickedButtonAdd();

		void OnBnClickedMoveUp();
		void OnBnClickedMoveDown();

		void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
		void OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
		void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
		void OnSelectionChanged(NMHDR *pNotifyStruct, LRESULT* pResult);		
		//}}AFX_MSG

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		/// Validate the data
		/// \returns HRESULT
		HRESULT ValidateData();

		/// Initialized the grid control. Define the number and size of the column and add the header names.
		void initGridControl();

		// Fill the grid Control. Define the number of rows and fill it with the data.
		void FillGridControl();

		/// Check if the table name is unique (not used yet)
		/// \param name [in] Name to check.
		/// \returns bool True if name not used yet.
		bool isTableNameUnique(const std::string& name);

		/// Show the context menu for right mouse click
		/// \param point [in] Mouse position
		void showContextMenu( CPoint point );

		/// Move Column in front of other column.
		/// \param moveId [in] ID of the object to move.
		/// \param preId [in] ID of the object in front of the other object should be moved.
		void MoveColumn(uint32_t moveId, uint32_t preId);
		
		/// Update the state of the buttons, which are enabled and disabled.
		void UpdateEnableButtons();

		HRESULT AddColumn(const std::string& rName, uint32_t addPreId = SvDef::InvalidObjectId);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;

		SvGcl::GridCtrl m_Grid;						//The grid displaying the name and the formulas
		SvUl::NameObjectIdList m_gridList;
#pragma endregion Member Variables
	};
} //namespace SvOg
