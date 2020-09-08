//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2General.h
/// This is the class for the general tab of Blob Analyzer 2
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "DataController.h"
#include "ImageController.h"
#pragma endregion Includes


namespace SvOg
{
	class BlobAnalyzer2General : public CPropertyPage , public ISVPropertyPageDialog
	{
		//******************************************************************************
		// Class Wizard Generated Message Map Entries:
		//******************************************************************************
		// Generated message map functions
	protected:
		//{{AFX_MSG(BlobAnalyzer2General)
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSelchangeCombo1();
		afx_msg void OnCheckGrayImage();
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	public:
		BlobAnalyzer2General(uint32_t inspectionID, uint32_t toolId, uint32_t taskObjectID);
		virtual ~BlobAnalyzer2General();

		//************************************
		// Method:    setImages
		// Description: Set image to the activeX-control. 
		//************************************
		void setImages();
		virtual bool QueryAllowExit() override { return true; };

	protected:
		HRESULT setInspectionData();
		void refresh();

		//******************************************************************************
		// Class Wizard Generated Virtual Function(s):
		//******************************************************************************
		//{{AFX_VIRTUAL(BlobAnalyzer2General)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
																	 //}}AFX_VIRTUAL
		BOOL OnKillActive() override;
		virtual BOOL OnSetActive() override;
		virtual void OnOK() override;

		bool checkIfGrayFeatureUsedAndRemoveThem();

		//******************************************************************************
		// Class Wizard Generated Dialog Data:
		//******************************************************************************
	protected:
		//{{AFX_DATA(BlobAnalyzer2General)
		enum { IDD = IDD_BLOB2_GENERAL_DIALOG };
		PictureDisplay	m_resultImage;
		PictureDisplay	m_grayImage;
		SvMc::AvailableObjectListComboBox<uint32_t> m_availableGrayImageListBox;
		//}}AFX_DATA

		//******************************************************************************
		// Data Element(s):
		//******************************************************************************
	private:
		std::string m_inputGrayName;
		std::string m_selectedGrayName;
		const uint32_t m_InspectionID;
		const uint32_t m_toolId;
		const uint32_t m_TaskObjectID;
		ImageController m_AnalyzerImageController;
		ImageController m_ToolImageController;
		ValueController m_Values;

		int m_blobColor = 0;
		int m_connectivityType = 0;
		long m_maxBlobArraySize{ 100 };
		BOOL	m_useGrayImage = false;
	};
} //namespace SvOg
