//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgExternalToolImageSelectPage
//* .File Name       : $Workfile:   TADlgExternalToolImageSelectPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 17:30:38  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVOResource/resource.h"
#include "SVOGuiUtility/ExternalToolTaskController.h"
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVRPropertyTree/SVRPropTree.h"
#pragma endregion Includes

namespace SvOg
{
	class TADlgExternalToolImageSelectPage : public CPropertyPage
	{
	#pragma region Constructor
	public:
		TADlgExternalToolImageSelectPage(uint32_t inspectionId, uint32_t taskObjectId, int id = IDD);
		virtual ~TADlgExternalToolImageSelectPage() {}
	#pragma endregion Constructor

	#pragma region Protected Methods
	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(TADlgExternalToolImageSelectPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(TADlgExternalToolImageSelectPage)
		void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
		void OnPropClick(NMHDR* pNotifyStruct, LRESULT* plResult);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	#pragma endregion Protected Methods

	#pragma region Private Methods
	private:
		//************************************
		// Method:    buildPropertyTree
		// Description: build up the property tree for the input image selection
		// Returns:   void
		//************************************
		void buildPropertyTree();

		//************************************
		// Method:    setImages
		// Description: Sets the images to the image control.
		// Returns:   void
		//************************************
		void setImages(SvOgu::ImageController &imgCtrl);

	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		// Dialog Data
		//{{AFX_DATA(TADlgExternalToolImageSelectPage)
		enum { IDD = IDD_TA_EXTERNAL_IMAGES };
		SvOgu::PictureDisplay m_ImageDisplay;
		//}}AFX_DATA

		SVRPropTree	m_Tree;
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		SvOgu::ExternalToolTaskController m_externalToolTaskController;
		size_t m_numImages;
	#pragma endregion Member variables
	};
} //namespace SvOg
