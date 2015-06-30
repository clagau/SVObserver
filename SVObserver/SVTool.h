//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTool
//* .File Name       : $Workfile:   SVTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
//******************************************************************************

#ifndef SVTOOL_H
#define SVTOOL_H

#include "SVTaskObjectList.h"
#include "SVToolExtentClass.h"
#include "SVImageClass.h"
#include "SVValueObjectImpl.h"

class SVAnalyzerClass;
class SVExtentClass;
class SVToolSetClass;
class SVThresholdClass;
class SVIPDoc;
class SVConditionalClass;

struct SVToolPropertyEntryStruct  
{
	SVToolPropertyEntryStruct()
	{
		DWValue = 0L;
		ID		= 0;

		RadioStart = 0;
		RadioEnd   = 0;
	};

	SVToolPropertyEntryStruct( SVToolPropertyEntryStruct& S2 )
	{
		DWValue = S2.DWValue;
		ID		= S2.ID;
		StrName = S2.StrName;

		RadioStart = S2.RadioStart;
		RadioEnd   = S2.RadioEnd;
	};

	SVToolPropertyEntryStruct operator=( SVToolPropertyEntryStruct& S2 )
	{
		DWValue = S2.DWValue;
		ID		= S2.ID;
		StrName = S2.StrName;

		RadioStart = S2.RadioStart;
		RadioEnd   = S2.RadioEnd;

		return( *this );
	};
	

	DWORD	DWValue;
	CString StrName;
	int		ID;

	int		RadioStart;
	int		RadioEnd;
};

class SVToolClass;

struct AllowResizeToParent{};

struct SVToolMoveStruct
{
	SVToolMoveStruct()
	{
		Init();
	};// end ctor

	SVToolMoveStruct( SVToolMoveStruct& p_rCopyStruct )
	{
		pTool		 = p_rCopyStruct.pTool;
		pAnalyzer	 = p_rCopyStruct.pAnalyzer;
		strToolName	 = p_rCopyStruct.strToolName;
		dLeft		 = p_rCopyStruct.dLeft;
		dTop		 = p_rCopyStruct.dTop;
		dWidth		 = p_rCopyStruct.dWidth;
		dHeight		 = p_rCopyStruct.dHeight;
		dX1			 = p_rCopyStruct.dX1;
		dY1			 = p_rCopyStruct.dY1;
		dX2			 = p_rCopyStruct.dX2;
		dY2			 = p_rCopyStruct.dY2;
		dCenterX	 = p_rCopyStruct.dCenterX;
		dCenterY	 = p_rCopyStruct.dCenterY;
		dStartRadius = p_rCopyStruct.dStartRadius;
		dEndRadius	 = p_rCopyStruct.dEndRadius;
		dStartAngle  = p_rCopyStruct.dStartAngle;
		dEndAngle	 = p_rCopyStruct.dEndAngle;
	};// end copy ctor

	SVToolMoveStruct operator=( SVToolMoveStruct& p_rAssignStruct )
	{
		pTool		 = p_rAssignStruct.pTool;
		pAnalyzer	 = p_rAssignStruct.pAnalyzer;
		strToolName	 = p_rAssignStruct.strToolName;
		dLeft		 = p_rAssignStruct.dLeft;
		dTop		 = p_rAssignStruct.dTop;
		dWidth		 = p_rAssignStruct.dWidth;
		dHeight		 = p_rAssignStruct.dHeight;
		dX1			 = p_rAssignStruct.dX1;
		dY1			 = p_rAssignStruct.dY1;
		dX2			 = p_rAssignStruct.dX2;
		dY2			 = p_rAssignStruct.dY2;
		dCenterX	 = p_rAssignStruct.dCenterX;
		dCenterY	 = p_rAssignStruct.dCenterY;
		dStartRadius = p_rAssignStruct.dStartRadius;
		dEndRadius	 = p_rAssignStruct.dEndRadius;
		dStartAngle  = p_rAssignStruct.dStartAngle;
		dEndAngle	 = p_rAssignStruct.dEndAngle;

		return( *this );
	};// end =

	void Init( void )
	{
		pTool		 = NULL;
		pAnalyzer	 = NULL;
		strToolName	 = _T( "" );
		dLeft		 = -999999.999F;
		dTop		 = -999999.999F;
		dWidth		 = -999999.999F;
		dHeight		 = -999999.999F;
		dX1			 = -999999.999F;
		dY1			 = -999999.999F;
		dX2			 = -999999.999F;
		dY2			 = -999999.999F;
		dCenterX	 = -999999.999F;
		dCenterY	 = -999999.999F;
		dStartRadius = -999999.999F;
		dEndRadius	 = -999999.999F;
		dStartAngle  = -999999.999F;
		dEndAngle	 = -999999.999F;
	};// end Init

	SVToolClass *pTool;
	SVAnalyzerClass *pAnalyzer;
	CString strToolName;
	double	dLeft;
	double	dTop;
	double	dWidth;
	double	dHeight;
	double	dX1;
	double	dY1;
	double	dX2;
	double	dY2;
	double	dCenterX;
	double	dCenterY;
	double	dStartRadius;
	double	dEndRadius;
	double	dStartAngle;
	double	dEndAngle;

};// end SVToolMoveStruct


enum EAutoSize 
{
	EnableNone = 0x0, 
	EnableSize = 0x1, 
	EnablePosition = 0x2, 
	EnableSizeAndPosition = 0x3
};

class SVToolClass : public SVTaskObjectListClass
{
	SV_DECLARE_CLASS( SVToolClass );

	friend class SVImageClass;
	friend class SVInspectionProcess;
	friend class SVPixelAnalyzeResultClass;
	friend class SVBlobAnalyzeResultClass;
	friend class SVXReferenceAnalyzerResultClass;
	friend class SVYReferenceAnalyzerResultClass;
	friend class SVXYReferenceAnalyzerResultClass;
	friend class SVBlobAnalyzeFeatureDialogClass;
	friend class SVToolAdjustmentDialogAnalyzerPageClass;
	friend class SVToolExtentClass;

public:
	SVToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVTOOL );
	virtual ~SVToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL DisconnectInput(SVInObjectInfoStruct* pInObjectInfo);

	virtual HRESULT GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine );

	bool IsEnabled() const;
	bool IsEnabled(long p_lIndex) const;

	bool WasEnabled() const;

	virtual BOOL isFreeMoveable();


	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual EAutoSize GetAutoSizeEnabled();
	
	virtual BOOL IsOkToEdit();
	virtual BOOL SetDefaultFormulas();
	
	virtual SVImageTypeEnum GetImageType();

	virtual HRESULT GetImageExtentProperty( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue );
	virtual HRESULT SetImageExtentProperty( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue );

	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent );
	virtual HRESULT GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent );
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList );

	const SVImageClass* GetToolImage() const;
	void SetToolImage( SVImageClass* p_pExtentImage );

	void SetAlwaysUpdate( bool p_bAlwaysUpdate );

	virtual HRESULT GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData );
	virtual HRESULT UpdateOffsetData( SVImageClass* p_svToolImage );
	virtual HRESULT TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut );
	virtual HRESULT EnableAuxilliaryExtents( bool p_bEnable );

	virtual HRESULT ResetObject();

	virtual SVToolPropertyEntryStruct* GetSpecialPropertyList( int& RCount );
	virtual HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;
	HRESULT SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo );

	virtual BOOL Validate();
	virtual BOOL OnValidate();
	virtual BOOL PrepareForRunning();

	virtual bool getConditionalResult() const;
	virtual bool getConditionalResult(long p_lIndex) const;

	BOOL ValidateDrive(LPCTSTR szFilePath, CString& szDrv);

	virtual HRESULT CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	void UpdateTaskObjectOutputListAttributes( SVObjectReference refTarget, UINT uAttributes );

	// Auxiliary Source Image functions
	HRESULT GetSourceImages( SVImageListClass* p_psvImageList );
	SVImageClass* GetAuxSourceImage();
	HRESULT SetAuxSourceImage( SVImageClass* p_psvImage );

	virtual HRESULT IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo ); 

	// Set String value object for Source Image Names
	virtual HRESULT CollectInputImageNames( SVRunStatusClass& RRunStatus );
	virtual HRESULT GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames );

	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index );
	virtual HRESULT GetParentExtent( SVImageExtentClass& p_rParent ) const;

	
	//************************************
	//!  Return SVExtentLocationPropertyUnknown if the input location 
	//! is not allowed for the tool 
	//! \param Location [in]
	//! \returns Location or SVExtentLocationPropertyUnknown
	//************************************
	virtual SVExtentLocationPropertyEnum FilterAllowedLocation(const SVExtentLocationPropertyEnum Location) const;
	


	//************************************
	//! Calculates bottom and Right 
	//! \returns void
	//************************************
	void UpdateBottomAndRight();

	//
	// Flag to indicate this tool is selected for SVIM operator move.
	// 26 Jan 2000 - frb.
	//
	SVBoolValueObjectClass ToolSelectedForOperatorMove;
	SVConditionalClass		*m_pToolConditional;

	SVBoolValueObjectClass enabled;

	SVInObjectInfoStruct		m_AuxSourceImageObjectInfo;

	SVValueObjectClass*  GetToolComment();

protected:
	virtual BOOL Run( SVRunStatusClass& RRunStatus );
	virtual BOOL RunWithNewDisable( SVRunStatusClass& RRunStatus );

	HRESULT UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage ); 

	// Remove Embedded Extents
	virtual void removeEmbeddedExtents( bool p_DisconnectExtents = true );

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	virtual HRESULT UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine );

	// Sends SVM_CREATE_ALL_OBJECTS to the child object
	// and returns the result of this message.
	virtual DWORD_PTR createAllObjectsFromChild( SVObjectClass* pChildObject ) override;

	SVToolSetClass*					pCurrentToolSet;

	SVClassInfoStructListClass		availableAnalyzerList;
	SVClassInfoStructListClass		availableFilterList;

	SVToolPropertyEntryStruct*		pPropertyArray;
	int								propertyCount;

	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	explicitFailed;

	SVLongValueObjectClass passedCount;
	SVLongValueObjectClass failedCount;
	SVLongValueObjectClass warnedCount;

	SVLongValueObjectClass enabledCount;
	SVLongValueObjectClass processedCount;

	// Conditional input
	SVInObjectInfoStruct		inputConditionBoolObjectInfo;

	// Conditional tool set drawing flag.
	SVEnumerateValueObjectClass	drawToolFlag;
	SVTimerValueObjectClass		ToolTime;

	// Embedded Objects for Extents
	SVDoubleValueObjectClass extentLeft;
	SVDoubleValueObjectClass extentTop;
	SVDoubleValueObjectClass extentRight;
	SVDoubleValueObjectClass extentBottom;
	SVDoubleValueObjectClass extentWidth;
	SVDoubleValueObjectClass extentHeight;

	//***** New source image extent value objects
	SVBoolValueObjectClass m_svUpdateAuxilliaryExtents;

	SVDoubleValueObjectClass m_svAuxilliarySourceX;
	SVDoubleValueObjectClass m_svAuxilliarySourceY;
	SVDoubleValueObjectClass m_svAuxilliarySourceAngle;
	SVStringValueObjectClass m_svAuxilliaryDrawType;
	SVStringValueObjectClass m_svAuxilliarySourceImageName;
	//***** New source image extent value objects

	// Tool Comments
	SVStaticStringValueObjectClass m_svToolComment;

protected:
	SVToolExtentClass m_svToolExtent;

private:
	void init();
	

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTool.h_v  $
 * 
 *    Rev 1.5   09 Dec 2014 09:47:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed OperatorMovable from class definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 13:10:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 12:18:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Nov 2013 07:23:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed tool comment from SVStringvalueobjectclass to SVStaticstringvalueobjectclass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:21:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   19 Apr 2013 09:09:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetParentExtent.
 * Added AllowResizeToParent trait.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:32:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   05 Dec 2012 12:12:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed extentHeight from a public member to a protected member
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   03 Aug 2012 11:12:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OperatorMovable base to check for operator move context menu option
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   01 Aug 2012 13:06:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   01 Aug 2012 12:41:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   25 Jul 2012 14:54:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   18 Jul 2012 14:27:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   16 Jul 2012 13:03:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   05 Jul 2012 14:27:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Tool Comment support to SVTool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   02 Jul 2012 17:48:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   16 Mar 2011 13:59:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   11 Mar 2011 14:33:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   30 Jul 2009 13:09:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   20 Oct 2008 11:02:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  635
 * SCR Title:  Blob Analyzer Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsEnabled method  that uses a dataIndex.
 * Added WasEnabled method  that uses a dataIndex.
 * Added getConditionalResult method that uses a dataIndex.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   10 Jun 2008 12:18:34   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upated source code to clear parent image information from the embedded image objects and the tool extent object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   07 Mar 2008 07:45:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Extent Type Names to ValueObjects.
 * 
 * Added virtual functions to support source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35.1.0   14 Jul 2009 15:19:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   21 Jun 2006 13:51:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  566
 * SCR Title:  Fix problem with Image Mask
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move find image input from tool class to task object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   19 Jan 2006 11:22:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to not process Aux Extent when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   16 Jan 2006 17:31:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   01 Sep 2005 12:04:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New functions to support Auxilliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   10 Aug 2005 13:28:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added UpdateTaskObjectOutputListAttributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   09 Aug 2005 08:46:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   22 Feb 2005 10:50:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed load image tool problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   18 Feb 2005 11:38:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed method of tracking values objects from full object manager rebuild to a value object registration methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   18 Feb 2005 07:05:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new extents.  Also changes made to support new creat / reset object methodolory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   12 Feb 2004 16:52:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed unused function that were originally designed for handling SetToolParameterList functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   19 Jan 2004 15:00:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  405
 * SCR Title:  Modify Image Overlay drawing for the display and SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made short term fix to force overlays to update when an image is requested through the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   08 Jul 2003 10:42:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new code the processing of InputRequests (from SetToolParameterList) to verify that extents have changed or the change is ignored.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   02 Jun 2003 15:06:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   22 Apr 2003 16:37:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   10 Apr 2003 10:13:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  271
 * SCR Title:  Adding timer features for Tool Execution times.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added ToolTime member varaible as a SVTimerValueObject, which will keep track to tool times
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   21 Mar 2003 12:33:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added member variable m_bFileSetup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   05 Mar 2003 12:13:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   10 Jan 2003 16:13:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Added method to return the overlay figure from this object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   03 Jan 2003 15:55:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   03 Jan 2003 15:35:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added methods for  - Set (Left, Top, Width, Height)Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   20 Nov 2002 13:55:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed class SVSystemClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   14 Aug 2001 10:41:46   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  220
 * SCR Title:  Fix invalid path message when adding a Load Image tool
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added ValidateDrive function to class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   19 Jul 2001 21:18:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  217
 * SCR Title:  Fix Dr. Watson errors in Patch Match Tool
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved getConditionalResult from protected to public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   19 Apr 2001 19:58:38   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   19 Apr 2001 19:49:48   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   26 Apr 2000 14:59:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetTransform method override.
 * Revised onDraw Method to call GetTransform.
 * Added message handler for 
 * SVMSGID_SET_SHADOWED_ABSEXTENTS.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   23 Mar 2000 17:36:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced SetDefaultFormulas(). Default implementation
 * return TRUE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   23 Mar 2000 16:46:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Commented out predeclaration of SVUnaryImageOperatorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   03 Mar 2000 13:13:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVImageInfoClass methods to get image extents.
 * 
 * Added SVDoubleValueObjects for exposed extents.
 * Added handler for SVMSGID_SET_SHADOWED_EXTENT.
 * Added method setShadowedExtents.
 * Removed SetExtent, SetAbsoluteExtent, GetExtent, and
 * GetAbsoluteExtent methods.
 * Added removeEmbeddedExtents method to provide a way for tools wishing to not expose extents to do so.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   Mar 01 2000 13:02:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *    Revised to make checkDrawAllowed(...) public.
 *  Revised onDraw(...) to don't route, if not necessary.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   Feb 24 2000 12:15:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new conditional tool drawing, using enumerated draw flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Feb 2000 15:02:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed MoveObjectsBase method.
 * Revised onDraw to use SVDrawContext* and relative extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 17:05:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Feb 01 2000 09:41:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  90
 * SCR Title:  Save  Operator Tool Move Priviledge in Configuration
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added code to accomodate the operator tool move feature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:46:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   09 Nov 1999 17:47:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added IsOkToEdit method (primarily for Build Ref Tool)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   Oct 13 1999 15:14:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Roberts bug fixes for Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   Sep 24 1999 09:09:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  31
 * SCR Title:  Integrate file manager/tracker.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Completed File Tracker.
 * Added Passed-Failed-Warned-Enabled-Processed-Counter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   Sep 23 1999 09:06:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added missing explicitFailed ( used for Reset Timer ).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   13 Sep 1999 18:03:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Conditional execution
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   11 Sep 1999 19:39:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Enable Method for conditional dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   Sep 10 1999 19:53:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add in failed, warned, passed value objects again.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Sep 10 1999 18:29:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Restore Run function and other code removed in error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Sep 10 1999 18:03:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed Obsolete Functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Sep 1999 08:55:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Tool Color (color now lives in SVObjectClass).
 * Revised Run logic.
 * Revised onDraw for color.
 * Removed override of SetObjectDepth.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Aug 1999 20:07:30   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced Input Interface Handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Aug 1999 19:05:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run method to use SVRunStatusClass.
 * Revised to use SVConditionalClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Aug 1999 19:01:46   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Working on PixelAnalyzerResultClass.
 * 
 * Changing White Pixel Result references to generic Pixel Result references.
 * 
 * 
 * Changed
 *   friend class SVWhitePixelAnalyzeResultClass;
 * to
 *   friend class SVPixelAnalyzeResultClass;
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Aug 25 1999 22:50:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised Validate method to be OnValidate method.
 *  ( OnValidate - local scope validation).
 * Added Validate method (Routes to all owned objects)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Aug 1999 20:36:40   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Deleted old Gage Tool, Profile Tool, Reference Tool, Rotate Tool.
 * Made changings to support new onUpdate() and onDraw() right.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
