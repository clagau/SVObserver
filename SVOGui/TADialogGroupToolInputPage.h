//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file TADialogGroupToolInputPage.h
/// This is the class for the input tab of Group Tool
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "DataController.h"
#include "ISVPropertyPageDialog.h"
#pragma endregion Includes


namespace SvOg
{
	const std::string c_inputName = "Input";
	struct GroupInputData
	{
		std::string m_name{ c_inputName };
		SvPb::EmbeddedIdEnum m_oldEmbeddedId = SvPb::NoEmbeddedId;
		SvPb::InputTypeEnum m_type = SvPb::InputTypeEnum::TypeDecimal;
		std::string m_defaultValue = "0";
		std::string m_ValueStr = "0";
	};

	class TADialogGroupToolInputPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(TADialogGroupToolInputPage)
		enum { IDD = IDD_GROUPTOOL_INPUT_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		TADialogGroupToolInputPage(uint32_t inspectionId, uint32_t toolId, uint32_t taskId);

		// Standard destructor
		virtual ~TADialogGroupToolInputPage() = default;
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TADialogGroupToolInputPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterst�tzung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(TADialogGroupToolInputPage)
		virtual BOOL OnInitDialog() override;

		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;

		void OnGridClick(NMHDR*, LRESULT*);
		void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
		void OnSelectionChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

		void OnBnClickedButtonRemove();
		void OnBnClickedButtonAdd();

		void OnBnClickedMoveUp();
		void OnBnClickedMoveDown();
		//}}AFX_MSG

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		bool setInspectionData();

		/// Initialized the grid control. Define the number and size of the column and add the header names.
		void initGridControl();

		// Fill the grid Control. Define the number of rows and fill it with the data.
		void FillGridControl();

		/// Show the context menu for right mouse click
		/// \param point [in] Mouse position
		void showContextMenu(CPoint point);

		SvOi::NameValuePair getType(const std::string& rTypeName);

		void loadDataList();

		void UpdateEnableButtons();
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_toolId;
		const uint32_t m_TaskObjectID;

		CBitmap m_downArrowBitmap;
		CImageList m_ImageList;
		ValueController m_Values;

		SvGcl::GridCtrl m_Grid;						//The grid displaying the name and the formulas
		std::vector<GroupInputData> m_inputData;

		//int m_numbersObjects = 0;
#pragma endregion Member Variables
	};
} //namespace SvOg
#pragma once