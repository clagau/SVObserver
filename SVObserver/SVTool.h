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

#pragma once

#pragma region Includes
#include "ObjectInterfaces\ITool.h"
#include "SVTaskObjectList.h"
#include "SVToolExtentClass.h"
#include "SVImageClass.h"
#include "SVValueObjectImpl.h"
#include "SVStatusLibrary\MessageManager.h"			// SvStl::DataOnly
#include "SVStatusLibrary\MessageManagerResource.h" // SvStl::MessageMgrDisplayAndNotify(),
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

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

enum EAutoSize 
{
	EnableNone = 0x0, 
	EnableSize = 0x1, 
	EnablePosition = 0x2, 
	EnableSizeAndPosition = 0x3,
	EnableNegativePosition = 0x4,
	EnableAll  =  	0x3 | 0x4
};

class SVToolClass : public SVTaskObjectListClass, public SvOi::ITool
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
	SVToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOL );
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
	virtual HRESULT EnableAuxiliaryExtents( bool p_bEnable );

	virtual HRESULT ResetObject();

	virtual SVToolPropertyEntryStruct* GetSpecialPropertyList( int& RCount );
	virtual HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;
	HRESULT SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo );

	virtual BOOL Validate();
	virtual BOOL OnValidate();
	virtual BOOL PrepareForRunning();

	virtual bool getConditionalResult() const;
	virtual bool getConditionalResult(long p_lIndex) const;

	virtual HRESULT CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	void UpdateTaskObjectOutputListAttributes( SVObjectReference refTarget, UINT uAttributes );

	// Auxiliary Source Image functions
	HRESULT GetSourceImages( SVImageListClass* p_psvImageList ) const;
	SVImageClass* GetAuxSourceImage() const;
	HRESULT SetAuxSourceImage( SVImageClass* p_psvImage );

	virtual HRESULT IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo ); 

	// Set String value object for Source Image Names
	virtual SVStaticStringValueObjectClass* GetInputImageNames( );

	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index );
	virtual HRESULT GetParentExtent( SVImageExtentClass& p_rParent ) const;

	//************************************
	//!  Return false  if the input location 
	//! is not allowed for the tool 
	//! \param Location [in]
	//! \param Direction 
	//! \returns bool
	//************************************
	virtual bool IsAllowedLocation(const SVExtentLocationPropertyEnum Location, SVExtentDirectionsEnum Direction  = SVExtentDirectionBoth ) const;

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
	SVConditionalClass* m_pToolConditional;

	SVBoolValueObjectClass enabled;

	SVInObjectInfoStruct m_AuxSourceImageObjectInfo;

	SVValueObjectClass* GetToolComment();

#pragma region ITool methods
	virtual bool areAuxExtentsAvailable() const override;
	virtual SvUl::NameGuidList getAvailableAuxSourceImages() const override;
	virtual SvUl::NameGuidPair getAuxSourceImage() const override;
	virtual HRESULT setAuxSourceImage(const SVGUID& rObjectID) override;
#pragma endregion ITool methods

protected:
	void UpdateAuxiliaryExtents(long resultDataIndex);
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

	SVToolSetClass* pCurrentToolSet;

	SVClassInfoStructListClass availableAnalyzerList;
	SVClassInfoStructListClass availableFilterList;

	SVToolPropertyEntryStruct* pPropertyArray;
	int propertyCount;

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
	SVInObjectInfoStruct inputConditionBoolObjectInfo;

	// Conditional tool set drawing flag.
	SVEnumerateValueObjectClass	drawToolFlag;
	SVTimerValueObjectClass ToolTime;

	// Embedded Objects for Extents
	SVDoubleValueObjectClass extentLeft;
	SVDoubleValueObjectClass extentTop;
	SVDoubleValueObjectClass extentRight;
	SVDoubleValueObjectClass extentBottom;
	SVDoubleValueObjectClass extentWidth;
	SVDoubleValueObjectClass extentHeight;
	SVDoubleValueObjectClass extentWidthScaleFactor;
	SVDoubleValueObjectClass extentHeightScaleFactor;

	//***** New source image extent value objects
	SVBoolValueObjectClass m_svUpdateAuxiliaryExtents;

	SVDoubleValueObjectClass m_svAuxiliarySourceX;
	SVDoubleValueObjectClass m_svAuxiliarySourceY;
	SVDoubleValueObjectClass m_svAuxiliarySourceAngle;
	SVStringValueObjectClass m_svAuxiliaryDrawType;
	SVStringValueObjectClass m_svAuxiliarySourceImageName;
	//***** New source image extent value objects

	// Tool Comments
	SVStaticStringValueObjectClass m_svToolComment;

protected:
	SVToolExtentClass m_svToolExtent;

private:
	void init();

#pragma region Run Error helper functionality
	// @HACK
	// This Run Error helper functionality IS NOT THREAD SAFE!!!  Probably
	// not a problem because it is not expected that anybody will be running 
	// the same object (Tool) from different threads at the same time. 
	//
	// It is desired for Error Data to be able to be passed out during the Run. 
	// It is understood that this implementation competes with a similarly 
	// intended implementation for validation that is used in the Archive 
	// Tool.  It is my [Jim] belief that both implementations need replaced 
	// with a more robust common component.
	// 
	// The developer who intends to use this error information must ensure 
	// that it was cleared (ClearRunError()) prior to the period they are 
	// concerned with. The error code clearing is done automatically within 
	// the SVToolClass::Run() function, which will cover the required clearing for
	// many of the use cases.  
	//
	// In general, the Run Error data will track through the OnRun() and 
	// ResetObject() logic.
public:
	HRESULT	ClearRunError();

	HRESULT	SetRunErrorData(const SVString& errorString);
	HRESULT	ClearRunErrorData();

	SVString GetRunErrorData() const;

	HRESULT	SetRunErrorCode(const HRESULT errorCode);
	HRESULT	GetRunErrorCode() const;

	bool GetRunDisplayed() const;
	HRESULT	SetRunDisplayed(const bool displayed);

protected:
	SvStl::MessageData m_RunError;

#pragma endregion Run Error helper functionality

#pragma region Validation Error helper functionality
	// @HACK
	// This Validate Error helper functionality IS NOT THREAD SAFE!!!  
	// Probably not a problem because it is not expected that anybody will be 
	// running validation on the same object (Tool) from different threads at
	// the same time. 
	//
	// It is desired for Error Data to be able to be passed out during the 
	// Validation process. It is understood that this implementation competes 
	// with a similarly intended implementation for validation that is used 
	// in the Archive Tool.  It is my [Jim] belief that both implementations 
	// need replaced with a more robust common component.
	// 
	// The developer who intends to use this error information must ensure 
	// that it was cleared (ClearValidationError () prior to the period they 
	// are concerned with. 
	
	
	// The error code clearing is done automatically within the 
	// SVToolClass::onRun() function and the SVToolClass::Validate() 
	// function, which will cover the required clearing for many of the use 
	// cases.
public:
	HRESULT ClearValidationError();
	HRESULT SetValidationErrorData(const SVString& errorString);
	HRESULT ClearValidationErrorData();

	SVString GetValidationErrorData() const;

	HRESULT SetValidationErrorCode(const HRESULT errorCode);
	HRESULT GetValidationErrorCode() const;

protected:
	SvStl::MessageData m_ValidationError;

#pragma endregion Validation Error helper functionality
};

