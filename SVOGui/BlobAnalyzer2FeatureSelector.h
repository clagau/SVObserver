//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2FeatureSelector.h
/// This is the class for the feature tab of Blob Analyzer 2
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes

namespace SvOg
{
	struct FeatureData4Selector
	{
		FeatureData4Selector(const std::string& name, unsigned long type, bool isActive, bool isNecessary = false, bool isCustomFeature = false) 
			: m_name(name)
			, m_type(type)
			, m_isActive(isActive)
			, m_isNecessary(isNecessary)
			, m_isCustomFeature(isCustomFeature) {};
		std::string m_name;
		unsigned long m_type;
		bool m_isActive = false;
		bool m_isNecessary = false;
		bool m_isCustomFeature = false;
	};

	class BlobAnalyzer2FeatureSelector : public CDialog
	{
	protected:
		//{{AFX_DATA(BlobAnalyzer2FeatureSelector)
		enum { IDD = IDD_BLOB2_FEATURE_SELECTOR_DIALOG };
		//}}AFX_DATA

	public:
		// Standard constructor
		explicit BlobAnalyzer2FeatureSelector(std::vector<FeatureData4Selector>, int currentNumberOfActive);

		// Standard destructor
		virtual ~BlobAnalyzer2FeatureSelector();


	public:
		const std::vector<FeatureData4Selector>& getFeatureData() const { return m_featureData;	};

		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(BlobAnalyzer2FeatureSelector)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(BlobAnalyzer2FeatureSelector)
		virtual BOOL OnInitDialog() override;
		void OnGridClick(NMHDR*, LRESULT*);
		//}}AFX_MSG


	private:
		/// Initialized the grid control. Define the number and size of the column and add the header names.
		void initGridControl();

		// Fill the grid Control. Define the number of rows and fill it with the data.
		void FillGridControl();

		void updateControls();


#pragma region Member Variables
	private:
		std::vector<FeatureData4Selector> m_featureData;

		int m_numberOfActive = 0;

		SvGcl::GridCtrl m_Grid;						//The grid displaying the name and the formulas
		CStatic m_numberLabel;
#pragma endregion Member Variables
	};
} //namespace SvOg
