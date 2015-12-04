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
#include <map>
#include "SVOGui\ImageController.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#include "PropertyTree\PropTree.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class SVExternalToolImageSelectPage : public CPropertyPage, protected SvOg::ImageController
		{
			typedef std::map<UINT, SVString> ImageInputList; // maps CtrlID to InputName

		#pragma region Constructor
		public:
			SVExternalToolImageSelectPage( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, long numImages, int id = IDD );
			virtual ~SVExternalToolImageSelectPage();
		#pragma endregion Constructor

		#pragma region Protected Methods
		// Overrides
			// ClassWizard generate virtual function overrides
			//{{AFX_VIRTUAL(SVExternalToolImageSelectPage)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			virtual BOOL OnInitDialog();
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
			void Refresh();

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
			void setImages();

		#pragma endregion Private Methods

		#pragma region Member variables
		private:
			// Dialog Data
			//{{AFX_DATA(SVExternalToolImageSelectPage)
			enum { IDD = IDD_EXTERNAL_TOOL_IMAGES };
			SvOg::PictureDisplay m_ImageDisplay;
			//}}AFX_DATA

			SVRPropTree	m_Tree;
			SVGUID m_InspectionID;
			SVGUID m_TaskObjectID;
			int m_numImages;
			ImageInputList m_imageInputList;
		#pragma endregion Member variables
		};
	}
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

namespace SvOg = Seidenader::SVOGui;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVExternalToolImageSelectPage.h_v  $
 * 
 *    Rev 1.1   26 Jun 2014 17:30:38   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   rearrange the dialog
 * use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:32:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:58:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Jul 2004 12:32:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/