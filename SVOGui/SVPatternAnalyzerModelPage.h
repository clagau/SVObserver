//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatternAnalyzerModelPage
//* .File Name       : $Workfile:   SVPatSelectModelPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   21 Aug 2014 10:40:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "PictureDisplay.h"
#include "ImageController.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvOg
{
	class SVPatternAnalyzerModelPage : public CPropertyPage, protected ImageController
	{
	#pragma region Constructor
	public:
		SVPatternAnalyzerModelPage(const SVGUID& rInspectionID, const SVGUID& rAnalyzerID);
		virtual ~SVPatternAnalyzerModelPage();
	#pragma endregion Constructor

	// Dialog Data
		//{{AFX_DATA(SVPatternAnalyzerModelPage)
		enum { IDD = IDD_PAT_SETUP_MODEL };
		//}}AFX_DATA

	#pragma region Protected Methods
	protected:
	// Overrides
	#pragma region AFX Virtual Methods
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(SVPatternAnalyzerModelPage)
		virtual void OnCancel() override;
		virtual void OnOK() override;
		virtual BOOL OnApply() override;
		virtual BOOL OnSetActive() override;
		virtual BOOL OnKillActive() override;
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
		//}}AFX_VIRTUAL
	#pragma endregion AFX Virtual Methods

	#pragma region AFX MSG
		// Generated message map functions
		//{{AFX_MSG(SVPatternAnalyzerModelPage)
		afx_msg void OnFileButton();
		afx_msg void OnFileDontCareButton();
		afx_msg void OnCreateModel();
		afx_msg void OnKillModelFileName();
		afx_msg void OnKillDontCareFileName();
		afx_msg void OnUseDontCareClicked();
		afx_msg void OnCircularOverscanClicked();
		afx_msg void OnKillModelCenter();
		afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
		DECLARE_EVENTSINK_MAP()
	#pragma endregion AFX MSG

	// Implementation
	protected:
		//************************************
		// Method:    ObjectChangedExDialogimage
		// Description: Event-methods, called if overlay-object is changed. Set the new values to the mask properties.
		//************************************
		void ObjectChangedExDialogImage(long Tab, long Handle, VARIANT* ParameterList, VARIANT* ParameterValue);
	
		/// Event-methods, called if tab-selection is changed. Set the properties for this tab.
		/// \param Tab [in] Tab handle.
		void TabChangeDialogImage(long Tab);
	#pragma endregion Protected Methods

	#pragma region Private Methods
	private:
		// Validation routines
		void ValidateModelParameters(bool shouldReset = false);
		void ValidateModelWidth();
		void ValidateModelHeight();
		void ValidateModelFilename();

		void InitializeData(); 
		BOOL ProcessOnKillFocus(UINT nId);
		bool RestoreImagesFromFile(SvStl::MessageContainerVector *pErrorMessages=nullptr);
		bool GetModelFile(bool bMode, CString& rFileName);

		HRESULT BuildPropertyList();
		HRESULT RefreshProperties();
		void setImages();
		void setOverlay();
		void setCircularToolOverlay();
		void setCircularModelOverlay();
		void setModelCenterOverlay();
		//************************************
		// Method:    setCircularOverscanCheckboxState
		// Description: Enable or Disable the checkbox if the height and width large enough or not.
		//				Uncheck the checkbox, if it is not big enough
		// Returns:   void
		//************************************
		void setCircularOverscanCheckboxState();

		/// Set the values from the GUI to the analyzer (business logic)
		HRESULT SetValuesToAnalyzer(SvStl::MessageContainerVector *pErrorMessages=nullptr, bool shouldResetTask = false);

		/// Set the enable state for the don't care controls.
		/// \returns void
		void SetEnableStateDontCareControls();
	#pragma endregion Private Methods

	#pragma region Member Variables
	private:
		enum 
		{ 
			ModelWidth_Property,
			ModelHeight_Property,
			ModelOriginX_Property,
			ModelOriginY_Property
		} Model_Property_Enums;

		bool m_bAllowExit;

		CButton m_CircularOverscanCheckbox;
		SvOg::PictureDisplay m_dialogImage;
		CString	m_strModelName;
		BOOL m_bCircularOverscan;
		CString	m_strDontCareName;
		BOOL m_bDontCare;
		SVRPropTree          m_Tree;

		int	m_nXPos;
		int	m_nYPos;
		long	m_lModelWidth;
		long	m_lModelHeight;
		long m_CenterX;
		long m_CenterY;
		SVGUID m_AnalyzerImageGUID;

		long m_sourceImageWidth;
		long m_sourceImageHeight;

		CString m_strOldModelName;
		long m_handleToModelOverlayObject;
		long m_handleToCircleOverlayObject;
		long m_handleToSquareOverlayObject;
		long m_handleToCircleOverlayObject2;
		long m_handleToSquareOverlayObject2;
		long m_handleToModelCenterOverlay;

		const SVGUID& m_rInspectionID;
		const SVGUID& m_rAnalyzerID;
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::DataController<ValueCommand, ValueCommand::value_type> Controller;
		Controller m_values;
	#pragma endregion Member Variables
	};
} //namespace SvOg