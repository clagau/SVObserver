//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFileImageSourcePage
//* .File Name       : $Workfile:   SVTADlgFileImageSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "PictureDisplay.h"
#include "ImageController.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTADlgFileImageSourcePage : public CPropertyPage
	{
	#pragma region Constructor
	public:
		SVTADlgFileImageSourcePage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~SVTADlgFileImageSourcePage();
	#pragma endregion Constructor

	#pragma region Protected Methods
	protected:
		//{{AFX_MSG(SVTADlgFileImageSourcePage)
		afx_msg void OnBrowseButton();
		afx_msg void OnReloadCheck();
		afx_msg void OnColorImageCheck();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVTADlgFileImageSourcePage)
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
		SVFileNameClass m_svfncImageSourceFile;
		SvOg::ImageController m_ImageController;
		ValueController m_values;
		//{{AFX_DATA(SVTADlgFileImageSourcePage)
		enum { IDD = IDD_TA_FILE_IMAGE_DIALOG };
		SvOg::PictureDisplay m_imageCtrl;
		CString	m_PathName;
		BOOL m_BContinuousReload{ false };
		BOOL m_BIsColorImage{ false };
		//}}AFX_DATA
	#pragma endregion Member variables
	};
} //namespace SvOg
