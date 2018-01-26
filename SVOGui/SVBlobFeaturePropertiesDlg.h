//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobFeaturePropertiesDlg
//* .File Name       : $Workfile:   SVBlobFeaturePropertiesDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#pragma endregion Includes

namespace SvOg
{
	class SVBlobFeaturePropertiesDlg : public CDialog
	{
	// Construction
	public:
		SVBlobFeaturePropertiesDlg(const std::string& rFeatureName, double DefaultValue, CWnd* pParent = nullptr);   // standard constructor
		virtual ~SVBlobFeaturePropertiesDlg();

		enum { IDD = IDD_BLOB_FEATURE_PROPERTIES };

		double getDefaultValue() { return m_DefaultValue; }

	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		virtual void OnOK() override;
		virtual BOOL OnInitDialog() override;
		DECLARE_MESSAGE_MAP()

	private:
		const std::string& m_rFeatureName;
		double	m_DefaultValue;
	};
} //namespace SvOg