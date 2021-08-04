//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Range.h
/// This is the class for the range tab of Blob Analyzer 2
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "SVUtilityLibrary/NameObjectIdList.h"
#include "ISVPropertyPageDialog.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "RangeController.h"
#include "Definitions/RangeEnum.h"
#include "LinkedValueWidgetHelper.h"
#pragma endregion Includes

namespace SvOg
{
	class BlobAnalyzer2Range : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(BlobAnalyzer2Range)
		enum { IDD = IDD_BLOB2_RANGE_DIALOG	};
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		BlobAnalyzer2Range(uint32_t inspectionId, uint32_t toolId, uint32_t taskObjectId);

		// Standard destructor
		virtual ~BlobAnalyzer2Range();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(BlobAnalyzer2Range)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(BlobAnalyzer2Range)
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;
		void OnButtonRange(UINT nID);
		void OnKillFocusRange(UINT nID);
		void OnGridClick(NMHDR*, LRESULT*);
		void OnGridBeginEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
		void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
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

		HRESULT loadFeatureData();
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_toolID;
		const uint32_t m_TaskObjectID;

		RangeController m_NumberRangeController;
		std::array<std::unique_ptr<LinkedValueWidgetHelper>, RangeEnum::ER_COUNT> m_RangeWidgets;
		std::array<CButton, RangeEnum::ER_COUNT> m_RangeButtons;
		std::array<CEdit, RangeEnum::ER_COUNT> m_RangeEdits;
		CBitmap m_downArrowBitmap;
		CImageList m_ImageList;

		SvGcl::GridCtrl m_Grid;						//The grid displaying the name and the formulas
		SvUl::NameObjectIdList m_gridList;
		google::protobuf::RepeatedPtrField< SvPb::FeatureData > m_featureData;
#pragma endregion Member Variables
	};
} //namespace SvOg
#pragma once
