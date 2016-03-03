//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogTwoImagePage
//* .File Name       : $Workfile:   SVToolAdjustmentDialogTwoImagePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:24  $
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVMFCControls\AvailableObjectListComboBox.h"

//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui\PictureDisplay.h"
#include "SVOGui\ImageController.h"
#include "SVOGui\BoundValue.h"
#include "SVOGui\ValuesAccessor.h"
#include "SVOGui\GuiController.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVToolAdjustmentDialogTwoImagePageClass : public CPropertyPage, protected SvOg::ImageController
		{
			DECLARE_MESSAGE_MAP()

			typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
			typedef SvOg::GuiController<Values, Values::value_type> ValueController;

		#pragma region Constructor
		public:
			SVToolAdjustmentDialogTwoImagePageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
			virtual ~SVToolAdjustmentDialogTwoImagePageClass();
		#pragma endregion Constructor

		#pragma region Protected Methods
		//******************************************************************************
		// Class Wizard Generated Virtual Function(s):
		//******************************************************************************
			//{{AFX_VIRTUAL(SVToolAdjustmentDialogTwoImagePageClass)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
			//}}AFX_VIRTUAL

		//******************************************************************************
		// Class Wizard Generated Message Map Entries:
		//******************************************************************************
			// Generated message map functions
		protected:
			//{{AFX_MSG(SVToolAdjustmentDialogTwoImagePageClass)
			virtual BOOL OnInitDialog();
			afx_msg void OnSelchangeCombo1();
			afx_msg void OnSelChangeCombo2();
			afx_msg void OnSelchangeOperatorCombo();
			//}}AFX_MSG
		#pragma endregion Protected Methods

		#pragma region Private Methods
		private:
			HRESULT SetInspectionData();
			void refresh();
			void setImages();
			void RetreiveCurrentlySelectedImageNames();
			void RetreiveResultImageNames();
		#pragma endregion Private Methods

		#pragma region Member variables
		protected:
			//{{AFX_DATA(SVToolAdjustmentDialogTwoImagePageClass)
			enum { IDD = IDD_TA_TWO_IMAGE_DIALOG };
			CComboBox m_operatorCtrl;
			SvOg::PictureDisplay m_secondImageCtrl;
			SvOg::PictureDisplay m_firstImageCtrl;
			SvMc::AvailableObjectListComboBox m_firstAvailableSourceImageListBoxCtl;
			SvMc::AvailableObjectListComboBox m_secondAvailableSourceImageListBoxCtl;
			//}}AFX_DATA

		private:
			SVString m_firstInputName;
			SVString m_firstImageName;
			SVString m_secondInputName;
			SVString m_secondImageName;
			SVString m_resultImageName;
			SVGUID m_resultImageID;

			SVGUID m_InspectionID;
			SVGUID m_TaskObjectID;

			ValueController m_Values;
		#pragma endregion Member variables
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogTwoImagePage.h_v  $
 * 
 *    Rev 1.1   26 Jun 2014 18:29:24   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:41:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Aug 2005 08:45:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 16:46:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Jan 2003 09:02:14   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnInitDialog method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 14:00:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   theSVObjectManager
 * ValueObject Data Indexes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 May 2000 16:00:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  145
 * SCR Title:  Half field images and rotation
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Moved SVToolAdjustDialogTwoImagePageClass from 
 * SVToolAdjustmentDialog.cpp and SVToolAdjustmentDialog.h
 * to it's own header and implementation file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/