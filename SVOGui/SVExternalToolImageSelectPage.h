//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolImageSelectPage
//* .File Name       : $Workfile:   SVExternalToolImageSelectPage.h  $
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
#include "Operators/SVDLLToolDefinitionStructs.h"
#pragma endregion Includes

namespace SvOg
{
	class SVExternalToolImageSelectPage : public CPropertyPage
	{
		typedef std::map<UINT, std::string> ImageInputList; // maps CtrlID to InputName

	#pragma region Constructor
	public:
		SVExternalToolImageSelectPage( uint32_t inspectionId, uint32_t taskObjectId, const std::vector<SvOp::InputImageInformationStruct>& rInfostructVector, int id = IDD );
		virtual ~SVExternalToolImageSelectPage() {}
	#pragma endregion Constructor

	#pragma region Protected Methods
	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(SVExternalToolImageSelectPage)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		virtual BOOL OnInitDialog() override;
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(SVExternalToolImageSelectPage)
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
		//{{AFX_DATA(SVExternalToolImageSelectPage)
		enum { IDD = IDD_EXTERNAL_TOOL_IMAGES };
		PictureDisplay m_ImageDisplay;
		//}}AFX_DATA

		std::vector<SvOp::InputImageInformationStruct> m_Infostructs;

		SVRPropTree	m_Tree;
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		size_t m_numImages;
		ImageInputList m_imageInputList;
	#pragma endregion Member variables
	};
} //namespace SvOg
