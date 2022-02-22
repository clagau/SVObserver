//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalToolImageSelectPage
//* .File Name       : $Workfile:   SVTADlgExternalToolImageSelectPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 17:30:38  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVOResource/resource.h"
#include "ImageController.h"
#include "PictureDisplay.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "ExternalToolTaskController.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTADlgExternalToolImageSelectPage : public CPropertyPage
	{
		typedef std::map<UINT, std::string> ImageInputList; // maps CtrlID to InputName

	#pragma region Constructor
	public:
		SVTADlgExternalToolImageSelectPage(uint32_t inspectionId, uint32_t taskObjectId, int id = IDD);
		virtual ~SVTADlgExternalToolImageSelectPage() {}
	#pragma endregion Constructor

	#pragma region Protected Methods
	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(SVTADlgExternalToolImageSelectPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(SVTADlgExternalToolImageSelectPage)
		void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
		void OnPropClick(NMHDR* pNotifyStruct, LRESULT* plResult);
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
		void setImages(ImageController &imgCtrl);

	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		// Dialog Data
		//{{AFX_DATA(SVTADlgExternalToolImageSelectPage)
		enum { IDD = IDD_TA_EXTERNAL_IMAGES };
		PictureDisplay m_ImageDisplay;
		//}}AFX_DATA

		SVRPropTree	m_Tree;
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		ExternalToolTaskController m_externalToolTaskController;
		size_t m_numImages;
		ImageInputList m_imageInputList;
		
	#pragma endregion Member variables
	};
} //namespace SvOg
