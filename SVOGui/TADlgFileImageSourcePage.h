//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgFileImageSourcePage
//* .File Name       : $Workfile:   TADlgFileImageSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "FilesystemUtilities/FileHelper.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgFileImageSourcePage : public CPropertyPage
	{
	#pragma region Constructor
	public:
		TADlgFileImageSourcePage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~TADlgFileImageSourcePage();
	#pragma endregion Constructor

	#pragma region Protected Methods
	protected:
		//{{AFX_MSG(TADlgFileImageSourcePage)
		afx_msg void OnBrowseButton();
		afx_msg void OnReloadCheck();
		afx_msg void OnColorImageCheck();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(TADlgFileImageSourcePage)
		protected:
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		//}}AFX_VIRTUAL

	protected:
		void GetData();
		void SetData();
		HRESULT SetInspectionData();
		void refresh();
		void setImages();
	#pragma endregion Protected Methods

	#pragma region Member variables
	private:
		SvFs::FileHelper m_svfncImageSourceFile;
		SvOgu::ImageController m_ImageController;
		SvOgu::ValueController m_values;
		//{{AFX_DATA(TADlgFileImageSourcePage)
		enum { IDD = IDD_TA_FILE_IMAGE_DIALOG };
		SvOgu::PictureDisplay m_imageCtrl;
		CString	m_PathName;
		BOOL m_BContinuousReload{ false };
		BOOL m_BIsColorImage{ false };
		//}}AFX_DATA
	#pragma endregion Member variables
	};
} //namespace SvOg
