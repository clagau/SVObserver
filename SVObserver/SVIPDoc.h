//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPDoc
//* .File Name       : $Workfile:   SVIPDoc.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   07 Jan 2015 17:44:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVList.h"
#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVDisplayObject.h"
#include "SVFileNameClass.h"
#include "SVPublishList.h"
#include "SVRGBMainImage.h"
#include "SVVirtualCamera.h"
#include "SVRegressionTestStruct.h"
#include "SVIPImageDataElement.h"
#include "SVIPProductStruct.h"
#include "SVIPResultData.h"
#include "SVValueObject.h"
#include "SVToolGrouping.h"
#pragma endregion Includes

#pragma region Declarations
class SVConditionalClass;
class SVInspectionThreadClass;
class SVObserverSrvrItem;
class SVResultListClass;
class SVToolSetClass;
class SVInspectionProcess;
class SVImageArchiveClass;
struct SVProductInfoStruct;
class CSVRegressionRunDlg;
class CSVRegressionFileSelectSheet;
class SVImageViewClass;
class ToolSetView;
class SVResultViewClass;
class SVResultsWrapperClass;
class SVObjectWriter;
#pragma endregion Declarations

class SVIPDoc : public CDocument
{
	DECLARE_DYNCREATE( SVIPDoc )

	friend class SVDisplayObject;
	friend class SVImageViewClass;
	friend class SVIPSplitterFrame;
	friend class SVResultViewClass;
	friend class SVToolSetListBoxClass;
	friend class ToolSetView;

public:
	enum SVIPViewUpdateHints
	{
		RefreshView = 0x00000001,
		ItemRenamed = 0x00000002,
	};

	enum
	{
		MinRegressionTime = 40,
	};

#pragma region Constructor
	SVIPDoc();
	virtual ~SVIPDoc();
#pragma endregion Constructor

	const GUID& GetInspectionID() const;
	void SetInspectionID( const SVGUID& p_InspectionID );

	void RebuildResultsList();

	BOOL InitAfterSystemIsDocked();

	void CloseDocument();

	BOOL IsColorInspectionDocument() const;

	void RecreateImageSurfaces();

	void SaveViews(SVObjectWriter& rWriter);
	void SaveViewPlacements(SVObjectWriter& rWriter);
	void SaveConditionalHistory(SVObjectWriter& rWriter);
	void SaveToolGroupings(SVObjectWriter& rWriter);
	void SaveViewedVariables(SVObjectWriter& rWriter);

	BOOL GetParameters(SVObjectWriter& rWriter);

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	BOOL GoOnline();
	BOOL GoOffline();

	HRESULT RemoveImage(SVImageClass* pImage);
	HRESULT RemoveImage(const SVGUID& p_rImageId);

	void RefreshPublishedList();
	HRESULT GetResults( SVResultsWrapperClass* p_pResults );

	SVImageClass* GetHSIMainImage();

	HRESULT GetCameras( SVVirtualCameraPtrSet& p_rCameras ) const;

	HRESULT UpdateWithLastProduct();
	BOOL RunOnce( SVToolClass* p_pTool = NULL );

	//Regression Test Methods
	void SetRegressionTestRunMode( RegressionRunModeEnum newMode );
	void SetRegressionTestPlayMode( RegressionPlayModeEnum newPlayMode );
	bool IsRegressionTestRunning();

	RegressionRunModeEnum GetRegressionRunMode();
	void SetRegressionTimeoutPeriod(int p_TimeoutMS);

	const SVGUID& GetSelectedToolID() const;
	void SetSelectedToolID( const SVGUID& p_rToolID );

	bool IsToolPreviousToSelected( const SVGUID& p_rToolID ) const;

	CString GetCompleteToolSetName() const;
	SVString CheckName( const SVString& rToolName ) const;

	const SVToolGrouping& GetToolGroupings() const;
	SVToolGrouping& GetToolGroupings();
	int GetSelectedToolIndex(const CString& toolName) const;
	int GetToolToInsertBefore(const CString& name, int listIndex) const;

	CList< RegressionTestStruct*, RegressionTestStruct* > m_listRegCameras; // @WARNING:  bad practice making members public

	HANDLE m_hDisplayChangedEvent; // Set if the display settings have been changed since the Doc was opened. // @WARNING:  bad practice making members public

	BOOL IsNew; // @WARNING:  bad practice making members public
	BOOL mbInitImagesByName; // @WARNING:  bad practice making members public

	#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
	#endif

	//{{AFX_MSG(SVIPDoc)
	afx_msg void RegressionTestComplete();
	afx_msg void RegressionTestModeChanged();
	afx_msg void OnUpdateStatusInfo( CCmdUI *pCmdUI );
	afx_msg void OnAddShiftTool();
	afx_msg void OnAddWindowTool();
	afx_msg void OnAddCylindricalWarpTool();
	afx_msg void OnEditTool();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy( CCmdUI* PCmdUI );
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste( CCmdUI* PCmdUI );
	afx_msg void OnEditDelete();
	afx_msg void OnAdjustLightReference();
	afx_msg void OnAdjustLut();
	afx_msg void OnFileSaveImage();
	afx_msg void OnAddImageTool();
	afx_msg void OnAddArchiveTool();
	afx_msg void OnAddMathTool();
	afx_msg void OnAddStatisticsTool();
	afx_msg void OnEditToolSetCondition();
	afx_msg void OnResultsPicker();
	afx_msg void OnPublishedResultsPicker();
	afx_msg void OnAddLoadImageTool();
	afx_msg void RunRegressionTest();
	afx_msg void OnAddAcquisitionTool();
	afx_msg void OnAddTransformationTool();
	afx_msg void OnUpdateViewToolSetDraw(CCmdUI* pCmdUI);
	afx_msg void OnAddPolarUnwrapTool();
	afx_msg void OnUpdateFileExit(CCmdUI* pCmdUI);
	afx_msg void OnSelectPPQVariable();
	afx_msg void OnAddColorTool();
	afx_msg void OnAddExternalTool();
	afx_msg void OnAddLinearTool();
	afx_msg void OnUpdateAddStartToolGrouping(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddEndToolGrouping(CCmdUI* pCmdUI);
	afx_msg void OnAddStartToolGrouping();
	afx_msg void OnAddEndToolGrouping();
	afx_msg void OnEditAdjustToolPosition();
	afx_msg void OnUpdateEditAdjustToolPosition(CCmdUI* pCmdUI);
	afx_msg void OnAddPerspectiveTool();
	afx_msg void OnViewResetAllCounts();
	afx_msg void OnViewResetCountsCurrentIP();
	afx_msg void OnUpdateViewResetCountsAllIPs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResetCountsCurrentIP(CCmdUI* pCmdUI);
	afx_msg void OnConditionalHistory();
	afx_msg void OnUpdateConditionalHistory( CCmdUI *pCmdUI );
	afx_msg void OnEditDataDefinitionLists();
	afx_msg void OnUpdateEditDataDefinitionLists(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRunRegressionTest( CCmdUI *pCmdUI );
	afx_msg void OnChangeToolSetDrawFlag( UINT nId );
	afx_msg void OnUpdateViewToolSetDrawSubMenus( CCmdUI* PCmdUI );
	afx_msg void OnPublishedResultImagesPicker();
	afx_msg void OnAddRemoteInputTool();
	afx_msg void OnAllowAdjustLightReference(CCmdUI* pCmdUI);
	afx_msg void OnAllowAdjustLut(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP() // Contains "protected:"

	//{{AFX_VIRTUAL(SVIPDoc)
public:
	virtual BOOL OnNewDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void OnCloseDocument();
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual CFile* GetFile( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError );
	virtual void SetModifiedFlag(BOOL bModified = TRUE);

protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

	static DWORD WINAPI SVRegressionTestRunThread( LPVOID lpParam );

	struct SVImageViewStatusStruct
	{
		bool m_ViewNotified;
		bool m_ViewDataUpdated;
		bool m_DisplayComplete;

		SVImageViewStatusStruct() : m_ViewNotified( false ), m_ViewDataUpdated( false ), m_DisplayComplete( false ) {}
	};

	typedef std::set< SVImageViewClass* > SVImageViewPtrSet;
	typedef std::map< SVGUID, SVImageViewPtrSet > SVMasterImageRegisterMap;
	typedef std::map< SVImageViewClass*, SVImageViewStatusStruct > SVImageViewPtrImageViewStatusMap;
	typedef std::deque< SVIPResultItemDefinition > SVResultDefinitionDeque;

	typedef SVRingBuffer< SVIPProductStruct, SVElementClear > SVProductDataQueue;

	struct SVImageDataStruct
	{
		SVByteVector m_ImageData;
		SVExtentMultiLineStructCArray m_OverlayData;
		SVImageViewPtrImageViewStatusMap m_ImageViews;

		SVImageDataStruct() : m_ImageData(), m_OverlayData(), m_ImageViews() {}
	};

	typedef std::map< SVGUID, SVImageDataStruct > SVImageIdImageDataStructMap;

	CMDIChildWnd* GetMDIChild();
	void SetMDIChild( CMDIChildWnd* p_pMDIChildWnd );

	void EditToolSetCondition();

	BOOL AddTool( SVToolClass* PTool );
	HRESULT DeleteTool(SVTaskObjectClass* pTaskObject);

	bool AddToolGrouping(bool bStartGroup = true);

	CView* getView() const;
	ToolSetView* GetToolSetView();
	SVImageViewClass* GetImageView( int p_Index = 0 );
	SVResultViewClass* GetResultView();
	SVToolSetClass* GetToolSet() const;
	SVConditionalClass* GetToolSetCondition();
	SVInspectionProcess* GetInspectionProcess() const;
	SVResultListClass* GetResultList() const;

	void RefreshDocument();

	void UpdateAllData();

	HRESULT RebuildImages();
	HRESULT CheckImages();

	HRESULT RegisterImage( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );
	HRESULT UnregisterImage( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );
	HRESULT UnregisterImageView( SVImageViewClass* p_pImageView );

	HRESULT IsImageDataSent( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView ) const;
	HRESULT IsImageDataUpdated( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView ) const;
	HRESULT IsImageDataDisplayed( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView ) const;
	HRESULT GetBitmapInfo( const SVGUID& p_rImageId, SVBitmapInfo& p_rBitmapInfo ) const;
	HRESULT GetImageData( const SVGUID& p_rImageId, SVByteVector& p_rImageData, SVExtentMultiLineStructCArray& p_rMultiLineArray ) const;
	HRESULT SetImageData( const SVGUID& p_rImageId, const SVByteVector& p_rImageData, const SVExtentMultiLineStructCArray& p_rMultiLineArray );
	HRESULT MarkImageDataSent( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );
	HRESULT MarkImageDataUpdated( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );
	HRESULT MarkImageDataDisplayed( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );

	HRESULT UpdateExtents( SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents );
	HRESULT UpdateExtentsToFit( SVTaskObjectClass* p_pTask, const SVImageExtentClass& p_rExtents );
	SVImageClass* GetImageByName( CString& p_imageName ) const;

	HRESULT IsResultDefinitionsUpdated() const;
	HRESULT GetResultDefinitions( SVResultDefinitionDeque& p_rDefinitions ) const;
	HRESULT GetResultData( SVIPResultData& p_rResultData ) const;

	HRESULT IsToolSetListUpdated() const;

	BOOL checkOkToDelete( SVTaskObjectClass* pTaskObject );

	mutable SVClock::SVTimeStamp m_ToolSetListTimestamp;
	mutable SVClock::SVTimeStamp m_PPQListTimestamp;
	mutable SVClock::SVTimeStamp m_ResultDefinitionsTimestamp;

	bool m_bAllowRefresh;
	SVProductDataQueue m_NewProductData;
	SVMasterImageRegisterMap m_RegisteredImages;
	SVImageIdImageDataStructMap m_Images;
	SVIPResultData m_Results;
	long m_AllViewsUpdated;
	BOOL mbRegressionSingleStep;

	//
	// View windows height and width saved for serialization. 15 Jun 1999 - frb.
	//
	int m_nHeightResultView;
	int m_nWidthToolSetView;

private:
#pragma region Private Methods
	void init();
	void InitMenu();
	void InitializeDirectX();

	void ClearRegressionTestStructures();

	bool LoadViewedVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);
#pragma endregion Private Methods

	RegressionRunModeEnum m_regtestRunMode;
	RegressionPlayModeEnum m_regtestRunPlayMode;
	HANDLE m_hRegressionHandle;
	int m_iRegessionTimeoutMS;
	bool m_bRegressionTestRunning;
	bool m_bRegressionTestStopping;

	SVFileNameClass msvFileName;

	SVGUID m_InspectionID;
	SVDisplayObject m_oDisplay;

	CMDIChildWnd* m_pMDIChildWnd;

	SVGUID m_SelectedToolID;
	SVToolGrouping m_toolGroupings;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVIPDoc.h_v  $
 * 
 *    Rev 1.8   07 Jan 2015 17:44:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Added methods GetResultList, SaveViewedVariables, and LoadViewedVariables.
 * Removed methods addToolsetObject2ResultDefinitions, addPPQInputs2ResultDefinitions, addPPQ_XParameter2ResultDefinitions, addEnvironmentObject2ResultDefinitions, and addViewableObject2ResultDefinitions (never implemented).
 * Cleaned up spacing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Dec 2014 04:13:06   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Copy Paste command and UI handlers
 * Changed CheckName to be part of the SVIPDoc class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Dec 2014 11:41:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed SetSelectedToolForOperatorMove function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Sep 2014 15:46:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added new methods:
 * addToolsetObject2ResultDefinitions,
 * addPPQInputs2ResultDefinitions,
 * addPPQ_XParameter2ResultDefinitions,
 * addEnvironmentObject2ResultDefinitions,
 * and addViewableObject2ResultDefinitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Jun 2014 16:49:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVToolGrouping member.
 * Revised AddTool to support Tool Groupings
 * Revised OnEditDelete  to support Tool Groupings
 * Added AddStartToolGrouping method
 * Added AddEndToolGrouping method
 * Added AddToolGrouping method
 * Revised Rename logic to support Tool Groupings
 * Added memu handlers for Add Start and End Tool Groupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Apr 2014 14:05:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  868
 * SCR Title:  Fix issue with switching from Run Mode to Regression Mode (eRoom 88)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Limit the minimum time for Regression test. Was delaying 0 seconds. The new minimum is now 40ms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Mar 2014 18:18:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed empty methods.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:16:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.91   29 Apr 2013 14:15:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:16:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.90   27 Mar 2013 14:11:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the OnAddGageTool and OnAddProfileTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.89   10 Jan 2013 16:16:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added missing log entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.88   07 Dec 2012 10:36:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method for OnAddShiftTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.87   04 Oct 2012 11:25:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.86   28 Sep 2012 14:49:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.85   25 Sep 2012 15:19:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.84.1.0   05 Dec 2012 11:53:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method for OnAddShiftTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.84   08 Aug 2012 15:39:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with LUT and Light Reference Dialogs when camera has no LUT or Light Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.83   01 Aug 2012 11:11:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  778
 * SCR Title:  Fix the linkage between SVObserver and the Help System
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed the OnHelp method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.82   02 Jul 2012 16:58:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.81   13 Mar 2012 13:38:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed dead code, added child elements as friends, and
 * protected class only methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.80   16 Jan 2012 15:22:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated inspection process and ip doc to allow the tool to queue a command for the inspection thread to process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.79   07 Oct 2011 14:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated MDI Child Window and IP Document to fix threading problems by moving the data through a lockless queue and notifying the IP Document via the MDI Child Window with a windows message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.78   06 Oct 2011 09:05:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated IPDoc to add locking to protect a member variable that was accessed by the display thread and the application thread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.77   04 Oct 2011 18:53:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated handshaking between IPDoc and ImageView to fix freezing of the ImageView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.76   03 Oct 2011 10:04:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the handshaking between the display object thread and the image view draw functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.75   28 Sep 2011 12:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.74   16 Sep 2011 16:00:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.73   17 May 2011 09:35:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  713
 * SCR Title:  Fix Help button on Configuration Assistant Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed F1 on IPDoc's to go to correct topic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.72   29 Apr 2011 08:06:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.71   08 Dec 2010 12:51:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.70   09 Nov 2010 16:21:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.69   08 Nov 2010 14:40:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.68   05 Nov 2010 14:54:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the un-necessary IP View Control from the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.67   04 Nov 2010 13:39:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.66   01 Jun 2010 10:43:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.65   21 Sep 2009 13:25:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added RecreateSurfaces method.
 * Moved DisplayChangedEvent to SVMainFrm.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.64   03 Sep 2009 10:07:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.63   30 Jul 2009 11:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.62   11 Dec 2008 10:25:38   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added RemoveImage() method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.61   12 Sep 2008 11:57:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added event to track changes to display settings.
 * Created InitializeDirectX function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.60   11 Jun 2008 13:57:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added UpdateCmdUI to menu option.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.59   30 May 2008 13:27:06   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed all DirectX changes since 5.00 Beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   10 Apr 2008 08:06:18   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Added event to track changes to display settings.
 * * Created InitializeDirectX function.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.58   10 Apr 2008 08:04:46   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Added event to track changes to display settings.
 * * Created InitializeDirectX function.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.57   07 Mar 2008 11:57:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Property sheet for Transfer Definition Lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.56   21 Jun 2007 13:05:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55.2.0   14 Jul 2009 14:31:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects and image methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   24 Jan 2006 14:17:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Conditional History bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   19 Jan 2006 10:33:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection process to not process Aux Extents if a gage tool is present in the inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   17 Jan 2006 09:59:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Conditional History menu handler
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   16 Jan 2006 17:07:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   02 Nov 2005 09:07:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   06 Oct 2005 14:46:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method to set main image indexes by product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   31 Aug 2005 08:40:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code call to handle new refresh methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   30 Aug 2005 11:20:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method for clearing the regression structures.  instead of clearing when regression test is stopped, it will clear when the ipdoc goes away
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   26 Aug 2005 15:01:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added m_bLoadFiles flag to reset structure so ResetObject in Pattern analyzer will reload files when going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   22 Aug 2005 13:44:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method to fix result image locking problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   22 Aug 2005 13:31:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Update_Command_UI handlers for view reset counts options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   22 Aug 2005 10:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   15 Aug 2005 13:40:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed unused code.  added member variable that will be set when the regression test is shutting down.  this fixes the problem of switching modes while the regression test is running without delay
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   03 Aug 2005 13:10:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  486
 * SCR Title:  Add Trigger Count to Tool Set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in reset counts logic and cleaned up code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   26 Jul 2005 14:04:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Reset Flag struct to replace flag in IPDoc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   26 Jul 2005 13:37:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved m_bInitializeOnReset to SVResetStruct
 * added m_bArchiveToolCreateFiles to SVResetStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   21 Jul 2005 14:41:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with regression test.  moved the run dialog to SVMainFrame, instead of having it owned by IPDoc. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   15 Jul 2005 11:31:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new regression test methods added
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   27 Jun 2005 09:42:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all freeze set variables and methods from the inspection document.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   18 May 2005 14:59:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Reset Counts message handlers to view menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   09 Mar 2005 13:13:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Allow the PerspectiveWarp Tool to be added from the add menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   09 Mar 2005 12:23:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CylindricalWarp functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   18 Feb 2005 12:13:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed ConnectToolsetMainImage to take a set of valid cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   18 Feb 2005 11:34:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the methodology to rebuild the value object list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   18 Feb 2005 11:02:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated IPDoc to allow the new Linear Tool to be added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   17 Feb 2005 14:35:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   16 Feb 2005 14:38:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   12 Oct 2004 12:55:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved some functionality of OnResultsPicker into a new function, RebuildResultsView so it could be called by other classes
 * added m_ prefix to member variables
 * added function DeleteTool`
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   12 Jul 2004 10:28:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added OnAddExternalTool
 * added GetInspectionProcess
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   17 Jun 2003 15:18:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added prototype code for using a map in the inspection to lookup objects that are used in the input request queue. It will be finalized after further testing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   07 May 2003 13:16:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified display logic so that image view displaying a SourceImage will look into the ProductInfoStruct to find the source image index for the camera from which they are derived instead of assuming it is the first camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   22 Apr 2003 10:31:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   11 Mar 2003 16:16:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  337
 * SCR Title:  Memory leak when editing a configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added virtual functions for DisconnectToolSetMainImage and ConnectToolSetMainImage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   05 Feb 2003 17:29:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   30 Jan 2003 11:50:40   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated GetImage, RunRegressionTest, RunInspection, BuildValueObjectList, OnEditDelete, processMessage, SingleRunModeLoop, AddTool, InitAfterSystemIsDocked, destructor and init methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   10 Jan 2003 14:02:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   10 Jan 2003 09:45:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added OnAdjustLut and helper function GetUniqueAcqDeviceCameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   19 Nov 2002 14:40:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   major changes to the file were made
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   09 Aug 2001 07:54:20   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  210
 * SCR Title:  Fix of shared data, multiple IPD's, same digitizer
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added declaration of "ValidateAndInitialize" function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   12 Jun 2001 15:36:58   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   
 * Modified the following functions: OnUpdateRunRegressionTest. 
 * Modified SVObserver state code in functions, SetTitle, SetPathName, SingleRunModeLoop, CanCloseFrame, SaveModified, OnCloseDocument, CloseDocument, OnAdjustLightReference, OnEditDelete, RunRegressionTest, OnUpdateViewToolSetDrawSubMenues. Added the GetFile function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Apr 2001 14:20:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   25 Jan 2001 16:17:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13.1.0   11 May 2001 09:43:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   06 Sep 2000 16:30:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   14 Jun 2000 16:32:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  161
 * SCR Title:  Inspection Document size keeps increasing.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Override for OnSaveDocument to bypass OLE
 * compound file storgae.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   17 May 2000 17:54:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnSelectPPQVariable menu handler.
 * Added UpdateViewedPPQDataList method.
 * Added RefreshViewedPQDataList method.
 * Added DestroyViewedPPQDataList method.
 * Added GetViewedPPQList method.
 * Added SVPQDataLinkInfoPtrList variable viewedPPQDataList.
 * Added include for SVPQVariableSelectionDialog.h
 * Revised PreDestroy to call DestroyViewedPPQDataList.
 * Revised Validate to refresh the result view.
 * Revised RefreshPPQDataList to call RefreshViewedPPQDataList.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   05 May 2000 13:41:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  134
 * SCR Title:  Restricted Operator Security level
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnUpdateFileExit message handler so exit menu item
 * will be disabled for restricted operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   23 Mar 2000 16:34:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced new fast refreshAlPanels(...) which can be called
 * inside the Tool Adjust Dialog pages to update all image and
 * result views.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   Mar 01 2000 12:29:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to handle new Message Handler for Menu: ADD->Polar Unwrap Tool.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   Feb 24 2000 11:54:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new conditional tool set drawing.
 * Revised InitMenu() to init tool set draw flag menu.
 * Added MessageHandler to handle tool set draw flag menu messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   21 Feb 2000 15:14:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Aded OnAddTransformationTool method.
 * Added include file SVTransformationTool.h.
 * Revised OnEditTool() to run the toolset (all tools) instead of
 * just the current tool being edited.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Feb 21 2000 10:36:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added resume message handler.
 * Introduced freeze set.
 * Changed view serialization (new function).
 * Revised versioning to v3.10 Beta 2.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   16 Feb 2000 16:49:34   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added multi-image display to the IPD
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   08 Feb 2000 11:05:04   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  96
 * SCR Title:  Scroll/Zoom images displayed in the inspection document.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Changed Serialize function to also serialize the views and
 * also the number of views in the IPD.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   07 Feb 2000 20:05:16   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced new Main Image Manager and Acquisition Tool Classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 09:01:30   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  91
 * SCR Title:  Archive Errors During Regression Test
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added definition for Update UI on regression test menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 12:55:08   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Nov 1999 12:44:00   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  60
 * SCR Title:  Create 'Utilities' menu option
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Modified initialization function to load the 'Utility' menu.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Nov 1999 09:11:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added HandleDocumentRenamed method to update PPQDataLinkInfo variables when a document gets renamed.
 * Added  RebuildResultsList method to rebuild the result list
 * when a document gets renamed.
 * Added InitToolSetResultDataLinkInfo method in support of
 * MultiThreaded Object Script parsing.
 * Correct a problem with Regression Testing and the Statistics
 * Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   28 Oct 1999 18:37:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  53
 * SCR Title:  Implement Regression Test Capability in SVObserver
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Implemented Regression Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   14 Oct 1999 19:29:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added call to checkOkToDelete.
 * Added checkOkToDelete method to show dependents
 * dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   05 Oct 1999 11:18:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Statistics Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   Sep 27 1999 19:35:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   BugFix: Wrong Type of PqdatalineInfoList was used.  It is now using PQdataLinkInfoPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Sep 24 1999 09:26:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added message handler for new load image tool.
 * Introduced reset_all_message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Sep 13 1999 13:43:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added message handler for 'Add Image Tool'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Sep 10 1999 19:18:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Profile Tool 'OnAdd...'.
 * Added Build Reference Tool 'On Add...'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Sep 1999 17:43:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for refreshing of the published list.
 * Corrected problem when deleting a tool.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Sep 1999 09:22:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for new SVPublishListClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Sep 1999 08:03:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Published List and Results List picking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Aug 1999 19:36:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVRunStatusClass.
 * Revised for ToolSet Conditional (Friend Architechture).
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/