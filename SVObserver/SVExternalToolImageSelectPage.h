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
#include "SVExternalToolTask.h"
#include "SVImageListClass.h"
#include "SVView.h"
#include "PictureDisplay.h"
#include "PropertyTree\PropTree.h"
#pragma endregion Includes

#pragma region Declarations
class SVExternalToolDetailsSheet;
class SVExternalTool;
class SVExternalToolTask;
#pragma endregion Declarations

class SVExternalToolImageSelectPage : public CPropertyPage
{
#pragma region Constructor
public:
	SVExternalToolImageSelectPage( SVExternalToolDetailsSheet* pParent = NULL, int id = IDD );
	~SVExternalToolImageSelectPage();
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

	//************************************
	// Method:    createAvailableImageList
	// Description: create a list of the available images and store them to the parameter m_AvailableImages.
	// Parameter: const SVGUID toolSetGUID The GUID of the toolset.
	// Parameter: const SVIPDoc * l_pIPDoc Pointer to the used IP document.
	// Returns:   void
	//************************************
	void createAvailableImageList( const SVGUID toolSetGUID, const SVIPDoc* l_pIPDoc );
#pragma endregion Private Methods

#pragma region Member variables
private:
	// Dialog Data
	//{{AFX_DATA(SVExternalToolImageSelectPage)
	enum { IDD = IDD_EXTERNAL_TOOL_IMAGES };
	PictureDisplay m_ImageDisplay;
	//}}AFX_DATA

	SVExternalToolDetailsSheet*	m_pParentDialog;
	SVInObjectInfoStruct*		m_pInputImageInfo[SVExternalToolTaskData::NUM_INPUT_IMAGES];
	SVImageListClass			m_AvailableImages;
	SVExternalTool*				m_pTool;
	SVExternalToolTask*			m_pTask;
	SVRPropTree					m_Tree;
#pragma endregion Member variables
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

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