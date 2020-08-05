//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Draw.h
/// This is the class for the draw tab of Blob Analyzer 2
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "DataController.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvOg
{
	class BlobAnalyzer2Draw : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(BlobAnalyzer2Draw)
		enum { IDD = IDD_BLOB2_DRAW_DIALOG };

		SvMc::SVEnumerateComboClass m_cbBlobType;
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		BlobAnalyzer2Draw(uint32_t inspectionId, uint32_t taskObjectId);

		// Standard destructor
		virtual ~BlobAnalyzer2Draw();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(BlobAnalyzer2Draw)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(BlobAnalyzer2Draw)
		virtual BOOL OnInitDialog() override;
		void OnCheckFillBlob();
		//}}AFX_MSG

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		bool setInspectionData();

		void enableControls();

#pragma endregion Private Methods

#pragma region Member Variables
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;

		ValueController m_Values;

		int m_fillColor = 0;
		BOOL	m_useFillBlob = false;
#pragma endregion Member Variables
	};
} //namespace SvOg
#pragma once
