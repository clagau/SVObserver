//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Feature.h
/// This is the class for the feature tab of Blob Analyzer 2
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "ISVPropertyPageDialog.h"
#include "ObjectSelectorController.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes


namespace SvOg
{
	class BlobAnalyzer2Feature : public CPropertyPage , public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(BlobAnalyzer2Feature)
		enum { IDD = IDD_BLOB2_FEATURE_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		BlobAnalyzer2Feature(uint32_t inspectionId, uint32_t taskObjectId);

		// Standard destructor
		virtual ~BlobAnalyzer2Feature();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(BlobAnalyzer2Feature)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(BlobAnalyzer2Feature)
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;

		void OnBnClickedSelector();

		void OnBnClickedMoveUp();
		void OnBnClickedMoveDown();

		void OnGridClick(NMHDR*, LRESULT*);
		void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
		void OnSelectionChanged(NMHDR *, LRESULT*);
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

		/// Update the state of the buttons, which are enabled and disabled.
		void UpdateEnableButtons();

		HRESULT loadFeatureData();

		SvPb::GetAvailableFeaturesResponse getAvailableFeatures() const;

		bool ShowObjectSelector(std::string& rName, const std::string& title);

		void sortFeatures(int pos);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;

		SvGcl::CGridCtrl m_Grid;						//The grid displaying the name and the formulas
		google::protobuf::RepeatedPtrField< SvPb::FeatureData > m_featureData;

		ObjectSelectorController m_objectSelector;
		CBitmap m_downArrowBitmap;
		CImageList m_ImageList;
#pragma endregion Member Variables
	};
} //namespace SvOg
