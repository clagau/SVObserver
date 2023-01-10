//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeAttributesDialog
//* .File Name       : $Workfile:   SVBarCodeAttributesDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:44  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvOg
{
	class SVBarCodeAttributesDialog : public CPropertyPage
	{
		// Construction
	public:
		SVBarCodeAttributesDialog(CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVBarCodeAttributesDialog();

		DWORD SetErrCorrection(DWORD dwErrCorrection);
		DWORD SetEncoding(DWORD dwEncoding);
		void InitSelections();
		void InitSelections(long lBarCodeType);
		void SetErrCorrection(double errCorrection);
		void SetEncoding(double encoding);
		double GetErrorCorrection();
		double GetEncoding();
		BOOL OnSetActive();
		void setBarCodeTypeInfos(const SvPb::GetBarCodeTypeInfosResponse& barCodeTypeInfos) { m_barCodeTypeInfos = barCodeTypeInfos; };

		// Dialog Data
			//{{AFX_DATA(SVBarCodeAttributesDialog)
		enum { IDD = IDD_BARCODE_ATTRIBUTES };
		int		m_iEncoding = -1;
		int		m_iCorrection = -1;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SVBarCodeAttributesDialog)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(SVBarCodeAttributesDialog)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeBarcodeErrorCorrection();
		afx_msg void OnSelchangeBarcodeEncoding();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		const SvPb::BarCodeTypeParameter& getBarCodeInfoByType(long type) const;

		template <typename T>
		std::string GetInfoNameByMil(const T& rMilVector, long MilID)
		{
			auto iter = std::find_if(rMilVector.begin(), rMilVector.end(), [MilID](const auto& rElement) { return MilID == rElement.value(); });
			if (rMilVector.end() != iter)
			{
				return iter->name();
			}

			Log_Assert(false);
			return {};
		};

		double m_dErrorCorrection;
		double m_dEncoding;
		SvPb::GetBarCodeTypeInfosResponse m_barCodeTypeInfos;
	};
}