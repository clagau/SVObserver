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
#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "SVDisplayObject.h"
#include "SVPublishList.h"
#include "SVRegressionTestStruct.h"
#include "SVIPProductStruct.h"
#include "InspectionEngine/SVIPResultData.h"
#include "InspectionEngine/SVVirtualCamera.h"
#include "SVToolGrouping.h"
#include "ObjectInterfaces/IFormulaController.h"
#include "NavigatorElement.h"
#pragma endregion Includes


#pragma region Declarations
class SVConditionalClass;
class SVResultListClass;
class SVToolSetClass;
class SVInspectionProcess;
class CSVRegressionRunDlg;
class SVRegressionFileSelectSheet;
class SVImageViewClass;
class ToolSetView;
class ResultTabbedView;
namespace SvOi
{
	class IObjectWriter;
}
#pragma endregion Declarations

class SVIPDoc : public CDocument
{
	DECLARE_DYNCREATE( SVIPDoc )

	friend class SVDisplayObject;
	friend class SVImageViewClass;
	friend class SVIPSplitterFrame;

public:
	enum SVIPViewUpdateHints
	{
		RefreshView = 0x00000001,
		ItemRenamed = 0x00000002,
		RefreshDelete = 0x4
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

	bool InitAfterSystemIsDocked();

	void CloseDocument();

	bool IsColorInspectionDocument() const;

	void RecreateImageSurfaces();

	void SaveViews(SvOi::IObjectWriter& rWriter);
	void SaveViewPlacements(SvOi::IObjectWriter& rWriter);
	void SaveToolGroupings(SvOi::IObjectWriter& rWriter);
	void SaveViewedVariables(SvOi::IObjectWriter& rWriter);
	
	bool GetParameters(SvOi::IObjectWriter& rWriter);

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	bool GoOnline();
	bool GoOffline();

	HRESULT RemoveImage(SvIe::SVImageClass* pImage);
	HRESULT RemoveImage(const SVGUID& p_rImageId);

	void RefreshPublishedList();

	SvIe::SVVirtualCameraPtrVector GetCameras() const;

	HRESULT UpdateWithLastProduct();
	bool RunOnce();

	
	//Regression Test Methods
	void SetRegressionTestRunMode( RegressionRunModeEnum newMode );
	void SetRegressionTestPlayMode( RegressionPlayModeEnum newPlayMode );
	bool IsRegressionTestRunning();

	RegressionRunModeEnum GetRegressionRunMode();
	void SetRegressionTimeoutPeriod(int p_TimeoutMS);

	SVGUID GetSelectedToolID() const;
	void SetSelectedToolID( const SVGUID& p_rToolID );

	bool IsToolPreviousToSelected( const SVGUID& p_rToolID ) const;

	std::string GetCompleteToolSetName() const;
	std::string CheckName( const std::string& rToolName ) const;

	const SVToolGrouping& GetToolGroupings() const;
	SVToolGrouping& GetToolGroupings();
	
	//Returns an index of the ToolsetList. The new Tool should be insert after this tool
	int GetToolToInsertBefore(const std::string& rName, int listIndex) const;

	CList< RegressionTestStruct*, RegressionTestStruct* > m_listRegCameras; // @WARNING:  bad practice making members public

	HANDLE m_hDisplayChangedEvent; // Set if the display settings have been changed since the Doc was opened. // @WARNING:  bad practice making members public

	bool IsNew; // @WARNING:  bad practice making members public
	bool mbInitImagesByName; // @WARNING:  bad practice making members public

	#ifdef _DEBUG
		virtual void AssertValid() const override;
		virtual void Dump(CDumpContext& dc) const override;
	#endif

	//{{AFX_MSG(SVIPDoc)
	afx_msg void RegressionTestComplete();
	afx_msg void RegressionTestModeChanged();
	afx_msg void OnUpdateStatusInfo( CCmdUI *pCmdUI );
	afx_msg void OnAddShiftTool();
	afx_msg void OnAddWindowTool();
	afx_msg void OnAddCylindricalWarpTool();
	afx_msg void OnEditTool();
	afx_msg void OnEditToolTab1();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy( CCmdUI* PCmdUI );
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste( CCmdUI* PCmdUI );
	afx_msg void OnShowFirstError();
	afx_msg void OnUpdateShowFirstError( CCmdUI* PCmdUI );
	afx_msg void OnAddParameterToMonitorList();
	afx_msg void OnUpdateAddParameterToMonitorList(CCmdUI* PCmdUI);
	afx_msg void OnRemoveParameterToMonitorList();
	afx_msg void OnUpdateRemoveParameterToMonitorList(CCmdUI* PCmdUI);
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
	afx_msg void OnResultsTablePicker();
	afx_msg void OnSaveResultsToFile();
	afx_msg void OnSaveTableResultsToFile();
	afx_msg void OnPublishedResultsPicker();
	afx_msg void OnAddLoadImageTool();
	afx_msg void RunRegressionTest();
	afx_msg void OnAddAcquisitionTool();
	afx_msg void OnAddTransformationTool();
	afx_msg void OnUpdateViewToolSetDraw(CCmdUI* pCmdUI);
	afx_msg void OnAddPolarUnwrapTool();
	afx_msg void OnUpdateFileExit(CCmdUI* pCmdUI);
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
	afx_msg void OnAddLoopTool();
	afx_msg void OnViewResetAllCounts();
	afx_msg void OnViewResetCountsCurrentIP();
	afx_msg void OnUpdateViewResetCountsAllIPs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResetCountsCurrentIP(CCmdUI* pCmdUI);
	afx_msg void OnEditDataDefinitionLists();
	afx_msg void OnUpdateEditDataDefinitionLists(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRunRegressionTest( CCmdUI *pCmdUI );
	afx_msg void OnChangeToolSetDrawFlag( UINT nId );
	afx_msg void OnUpdateViewToolSetDrawSubMenus( CCmdUI* PCmdUI );
	afx_msg void OnPublishedResultImagesPicker();
	afx_msg void OnAddRingBufferTool();
	afx_msg void OnAddTableTool();
	afx_msg void OnAddTableAnalyzerTool();
	afx_msg void OnAllowAdjustLightReference(CCmdUI* pCmdUI);
	afx_msg void OnAllowAdjustLut(CCmdUI* pCmdUI);
	afx_msg void OnAddResizetool();
	afx_msg void OnShowToolRelations();
	afx_msg void OnUpdateShowToolRelations(CCmdUI* pCmdUI);
	afx_msg void OnToolDependencies();
	afx_msg void OnUpdateToolDependencies(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddGeneralTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddCylindricalWarpTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddTransformationTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddColorTool(CCmdUI* PCmdUI);
	afx_msg void OnUpdateAddLoopTool(CCmdUI* PCmdUI);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP() // Contains "protected:"

	//{{AFX_VIRTUAL(SVIPDoc)
public:
	//************************************
	//! Open the toolAdjustmentDialog for selected Tool
	//! \param tab [in] selected tab
	//! \returns void
	//************************************
	void OpenToolAdjustmentDialog(int tab); 
	virtual BOOL OnNewDocument() override;
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame) override;
	virtual void SetTitle(LPCTSTR lpszTitle) override;
	virtual CFile* GetFile( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError ) override;
	virtual void SetModifiedFlag(BOOL bModified = TRUE) override;

	typedef std::deque<SvIe::SVIPResultItemDefinition> SVResultDefinitionDeque;
	double getResultDefinitionUpdatTimeStamp() const;
	HRESULT GetResultDefinitions(SVResultDefinitionDeque& p_rDefinitions) const;
	void GetResultData(SvIe::SVIPResultData& p_rResultData) const;
	std::vector <SvIe::IPResultTableData> getResultTableData() const;

	HRESULT IsToolSetListUpdated() const;
	ToolSetView* GetToolSetView() const;
	SVToolSetClass* GetToolSet() const;
	void SetRegressionTestUsePlayCondition(bool usePlayCondition) { m_bRegressionTestUsePlayCondition = usePlayCondition; };
	void SetRegressionTestPlayEquationController(SvOi::IFormulaControllerPtr pRegressionTestPlayEquationController) { m_pRegressionTestPlayEquationController = pRegressionTestPlayEquationController; }
	/// Return true if Regression Test should go to pause because of the Play condition.
	/// \returns bool
	bool shouldPauseRegressionTestByCondition();

	bool isImageAvailable(SvPb::SVObjectSubTypeEnum ImageType) const;
	SvTrc::ITriggerRecordRPtr getLastTriggerRecord() const { return m_triggerRecord; };

protected:
	virtual BOOL SaveModified() override;
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

	typedef SVRingBuffer< SVIPProductStruct, SVElementClear > SVProductDataQueue;

	struct SVImageDataStruct
	{
		std::string m_ImageData;
		SVExtentMultiLineStructVector m_OverlayData;
		SVImageViewPtrImageViewStatusMap m_ImageViews;

		SVImageDataStruct() : m_ImageData(), m_OverlayData(), m_ImageViews() {}
	};

	typedef std::map< SVGUID, SVImageDataStruct > SVImageIdImageDataStructMap;

	CMDIChildWnd* GetMDIChild();
	void SetMDIChild( CMDIChildWnd* p_pMDIChildWnd );

	bool AddTool(const SVGUID& rClassId);
	bool deleteTool(NavigatorElement* pNaviElement);

	bool AddToolGrouping(bool bStartGroup = true);

	CView* getView() const;
	SVImageViewClass* GetImageView( int p_Index = 0 );
	ResultTabbedView* GetResultView();
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
	HRESULT GetImageData( const SVGUID& p_rImageId, std::string& p_rImageData, SVExtentMultiLineStructVector& p_rMultiLineArray ) const;
	HRESULT SetImageData( const SVGUID& p_rImageId, const std::string& p_rImageData, const SVExtentMultiLineStructVector& p_rMultiLineArray );
	HRESULT MarkImageDataSent( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );
	HRESULT MarkImageDataUpdated( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );
	HRESULT MarkImageDataDisplayed( const SVGUID& p_rImageId, SVImageViewClass* p_pImageView );

	HRESULT UpdateExtents(SvIe::SVTaskObjectClass* pTask, const SVImageExtentClass& rExtents);
	HRESULT UpdateExtentsToFit(SvIe::SVTaskObjectClass* pTask, const SVImageExtentClass& rExtents);
	SvIe::SVImageClass* GetImageByName( LPCTSTR ImageName ) const;

	bool checkOkToDelete(SvIe::SVTaskObjectClass* pTaskObject );

	mutable double m_ToolSetListTimestamp;
	mutable double m_PPQListTimestamp;

	bool m_bAllowRefresh;
	SVProductDataQueue m_NewProductData;
	SVMasterImageRegisterMap m_RegisteredImages;
	SVImageIdImageDataStructMap m_Images;
	SvIe::SVIPResultData m_Results;
	SvTrc::ITriggerRecordRPtr m_triggerRecord = nullptr;
	long m_AllViewsUpdated;

	//
	// View windows height and width saved for serialization. 15 Jun 1999 - frb.
	//
	int m_nHeightResultView;
	int m_nWidthToolSetView;

private:
#pragma region Private Methods
	void init();
	void InitMenu();

	void ClearRegressionTestStructures();

	bool LoadViewedVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);
	/// Load the Regression Test variables from configuration.
	/// \param rTree [in]
	/// \param htiParent [in]
	void LoadRegressionTestVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent);

	/// Save Regression Test variables.
	/// \param rWriter [in,out]
	void SaveRegressionTestVariables(SvOi::IObjectWriter& rWriter);

	//! Translates the selected objects to a set of object names for the result picker
	//! \param rSelectedObjects [in] const reference to the selected objects
	//! \param rInspectionName [in] the inspection name
	//! \returns the selected string set of object names
	SvDef::StringSet TranslateSelectedObjects(const SVObjectReferenceVector& rSelectedObjects, const std::string& rInspectionName) const;

	//!if not all tools from the toolset are in toolGrouping, tool grouping is cleared. 
	//! Initialize the tool Groupings (for older saved configurations)
	void correctToolGrouping();
#pragma endregion Private Methods

	RegressionRunModeEnum m_regtestRunMode;
	RegressionPlayModeEnum m_regtestRunPlayMode;
	HANDLE m_hRegressionHandle;
	int m_iRegessionTimeoutMS;
	bool m_bRegressionTestRunning;
	bool m_bRegressionTestStopping;
	bool m_bRegressionTestUsePlayCondition;
	SvOi::IFormulaControllerPtr m_pRegressionTestPlayEquationController;
	std::string m_RegressionTestLoadEquationText;
	bool m_bRegressionTestInitEquationText;

	SVGUID m_InspectionID;
	SVDisplayObject m_oDisplay;

	CMDIChildWnd* m_pMDIChildWnd;
	SVToolGrouping m_toolGroupings;
};

