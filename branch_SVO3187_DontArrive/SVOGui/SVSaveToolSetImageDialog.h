//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSaveToolSetImageDialog
//* .File Name       : $Workfile:   SVSaveToolSetImageDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:14:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "PictureDisplay.h"
#include "ImageController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVSaveToolSetImageDialogClass : public CDialog
	{
	public:
		SVSaveToolSetImageDialogClass(uint32_t inspectionId, uint32_t taskObjectId, CWnd* pParent = nullptr);   // standard constructor

		//{{AFX_VIRTUAL(SVSaveToolSetImageDialogClass)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:
		// Generated message map functions
		//{{AFX_MSG(SVSaveToolSetImageDialogClass)
		afx_msg void OnSaveButton();
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeAvailableImagesCombo();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void setImages();

	private:
		//{{AFX_DATA(SVSaveToolSetImageDialogClass)
		enum { IDD = IDD_SELECT_TOOLSET_IMAGE_TO_SAVE_DIALOG };
		PictureDisplay m_currentSelectedImageCtrl;
		CComboBox m_availableImagesComboCtrl;
		//}}AFX_DATA

		ImageController m_ImageController;
	};
} //namespace SvOg
