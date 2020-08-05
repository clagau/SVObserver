//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Sheet.h
/// This is the class for the Sheet of Blob Analyzer 2
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes


namespace SvOg
{

	class BlobAnalyzer2Sheet : public CPropertySheet
	{
		DECLARE_DYNAMIC(BlobAnalyzer2Sheet)

		//{{AFX_VIRTUAL(BlobAnalyzer2Sheet)
	public:
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL

		//{{AFX_MSG(BlobAnalyzer2Sheet)
	protected:
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnOK();
		afx_msg void OnCancel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	public:
		BlobAnalyzer2Sheet(uint32_t inspectionID, uint32_t toolId, uint32_t taskObjectID, UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 1);
		BlobAnalyzer2Sheet(uint32_t inspectionID, uint32_t toolId, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 1);

		virtual ~BlobAnalyzer2Sheet();

	protected:
		void init();
		void addPages();

	private:
		uint32_t m_InspectionID;
		uint32_t m_toolId;
		uint32_t m_TaskObjectID;
	};
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

