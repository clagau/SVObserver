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
#include "SVOCore/SVTaskObjectList.h"
#include "SVToolExtentClass.h"
#include "SVOCore/SVImageClass.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVAnalyzerClass;
class SVExtentClass;
class SVToolSetClass;
class SVThresholdClass;
class SVConditionalClass;
class SVToolClass;

struct AllowResizeToParent{};

enum EAutoSize 
{
	EnableNone = 0x0, 
	EnableSize = 0x1, 
	EnablePosition = 0x2, 
	EnableSizeAndPosition = 0x3,
	EnableNegativePosition = 0x4,
	EnableAll = EnableSizeAndPosition | EnableNegativePosition
};

class SVToolClass : public SVTaskObjectListClass, public SvOi::ITool
{
	SV_DECLARE_CLASS( SVToolClass );

	friend class SVInspectionProcess; // For access to Run()
	friend class SVToolExtentClass; // For access to UpdateOffsetDataToImage()

public:
	SVToolClass( BOOL BCreateDefaultTaskList = false, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOL );
	virtual ~SVToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;
	virtual bool DisconnectObjectInput(SVInObjectInfoStruct* pInObjectInfo) override;

	virtual HRESULT GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine ) override;

	bool IsEnabled() const;

	bool WasEnabled() const;

	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual EAutoSize GetAutoSizeEnabled();
	
	BOOL IsOkToEdit();
	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	
	SVImageTypeEnum GetImageType();

	HRESULT SetImageExtentProperty( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue );

	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent ) override;
	virtual HRESULT GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent );
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;
	virtual HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList ) override;

	const SVImageClass* GetToolImage() const;

	void SetAlwaysUpdate( bool p_bAlwaysUpdate );

	HRESULT GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData );
	HRESULT UpdateOffsetData( SVImageClass* p_svToolImage );
	HRESULT TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut );
	HRESULT EnableAuxiliaryExtents( bool p_bEnable );

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const override;
	HRESULT SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo );

	bool getConditionalResult() const;
	bool getConditionalResult(long p_lIndex) const;

	virtual HRESULT CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray ) override;

	void UpdateTaskObjectOutputListAttributes( SVObjectReference refTarget, UINT uAttributes );

	// Auxiliary Source Image functions
	HRESULT GetSourceImages( SVImageListClass* p_psvImageList ) const;
	SVImageClass* GetAuxSourceImage() const;
	HRESULT SetAuxSourceImage( SVImageClass* p_psvImage );

	virtual HRESULT IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo ) override; 

	// Set String value object for Source Image Names
	virtual SVStaticStringValueObjectClass* GetInputImageNames( );
	virtual bool SetFirstInputImageName( LPCTSTR FirstName) override;


	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index ) override;
	virtual HRESULT GetParentExtent( SVImageExtentClass& p_rParent ) const;

	//************************************
	//!  Return false  if the input location 
	//! is not allowed for the tool 
	//! \param Location [in]
	//! \param Direction 
	//! \returns bool
	//************************************
	bool IsAllowedLocation(const SVExtentLocationPropertyEnum Location, SVExtentDirectionsEnum Direction  = SVExtentDirectionBoth ) const;

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

	const SVImageInfoClass* getFirstImageInfo() const;

#pragma region ITool methods
	virtual bool areAuxExtentsAvailable() const override;
	virtual SvUl::NameGuidList getAvailableAuxSourceImages() const override;
	virtual SvUl::NameGuidPair getAuxSourceImage() const override;
	virtual HRESULT setAuxSourceImage(const SVGUID& rObjectID) override;
	virtual void SetToolImage( const SVGUID& rObjectID ) override;
#pragma endregion ITool methods

protected:
	void UpdateAuxiliaryExtents(long resultDataIndex);
	virtual bool Run( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	bool RunWithNewDisable( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	HRESULT UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage ); 

	// Remove Embedded Extents
	void removeEmbeddedExtents( bool p_DisconnectExtents = true );

	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual HRESULT UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine ) override;

	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject( SVTaskObjectClass& rChildObject ) override;

private:
	void init();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

protected:
	SVToolSetClass* m_pCurrentToolSet;

	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass explicitFailed;

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
};

