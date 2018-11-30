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
#include "ObjectInterfaces/ITool.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IToolSet.h"
#include "SVTaskObjectList.h"
#include "SVToolExtentClass.h"
#include "SVImageClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

class SVAnalyzerClass;
class SVExtentClass;
class SVToolSetClass;
class SVThresholdClass;
class SVConditionalClass;

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
	SV_DECLARE_CLASS(SVToolClass);

	friend class SVInspectionProcess; // For access to Run()
	friend class SVToolExtentClass; // For access to UpdateOffsetDataToImage()

public:
	SVToolClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOL);
	virtual ~SVToolClass();

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool CloseObject() override;
	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pInObjectInfo) override;

	virtual HRESULT GetDrawInfo(SVExtentMultiLineStruct& p_rMultiLine) override;

	bool IsEnabled() const;

	bool WasEnabled() const;

	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual EAutoSize GetAutoSizeEnabled();

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages = nullptr);

	void SetImageExtentProperty(SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject);

	virtual HRESULT SetImageExtent(const SVImageExtentClass& rImageExtent) override;
	virtual HRESULT GetFilteredImageExtentPropertyList(SVExtentPropertyVector& p_rPropertyList) override;

	const SVImageClass* GetToolImage() const;

	void SetAlwaysUpdate(bool p_bAlwaysUpdate);

	HRESULT GetRootOffsetData(SVExtentOffsetStruct& p_rsvOffsetData);
	HRESULT UpdateOffsetData(SVImageClass* p_svToolImage);
	HRESULT TranslatePointToSource(SVPoint<double> inPoint, SVPoint<double>& rOutPoint);
	HRESULT EnableAuxiliaryExtents(bool p_bEnable);

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual HRESULT GetPropertyInfo(SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo) const override;
	HRESULT SetExtentPropertyInfo(SvDef::SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo);

	bool getConditionalResult(bool bRunMode = false) const;

	virtual HRESULT CollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructVector &p_MultiLineArray) override;

	// Auxiliary Source Image functions
	HRESULT GetSourceImages(SVImageClassPtrVector* p_psvImageList) const;
	HRESULT SetAuxSourceImage(SVImageClass* p_psvImage);

	virtual HRESULT IsAuxInputImage(const SvOl::SVInObjectInfoStruct* p_psvInfo) const override;

	const SvOl::SVInObjectInfoStruct* GetAuxInputImageInfo() const { return &m_AuxSourceImageObjectInfo;}

	// Set String value object for Source Image Names
	virtual SVStringValueObjectClass* GetInputImageNames();
	virtual bool SetFirstInputImageName(LPCTSTR FirstName) override;

	virtual HRESULT updateImageExtent() override;
	virtual HRESULT UpdateImageWithExtent() override;
	virtual HRESULT GetParentExtent(SVImageExtentClass& p_rParent) const;

	//!  Return false  if the input location 
	//! is not allowed for the tool 
	//! \param Location [in]
	//! \param Direction 
	//! \returns bool
	bool IsAllowedLocation(const SvDef::SVExtentLocationPropertyEnum Location, SvDef::SVExtentDirectionsEnum Direction = SvDef::SVExtentDirectionBoth) const;

	//! Calculates bottom and Right 
	//! \returns void
	void UpdateBottomAndRight();

	//! Sets the tool position index (One based)
	//! \param Position
	//! Return value is next Tool Position 
	virtual long  setToolPosition(long ToolPosition) 
	{ 
		m_ToolPosition.SetValue(ToolPosition); 
		return (ToolPosition +1);
	};

	//! Check if tool can be resized to parent extents
	//! \returns true if tool can resize to parent
	bool canResizeToParent() { return m_canResizeToParent; };

	//
	// Flag to indicate this tool is selected for SVIM operator move.
	// 26 Jan 2000 - frb.
	//
	const SVImageInfoClass* getFirstImageInfo() const;

	/// Search an object with the embeddedId and add it to the ParameterList for the needed in monitor list.
	/// \param retList [in,out] The ParameterList
	/// \param rEmbeddedId [in] The EmbeddedId of the object
	bool addEntryToMonitorList(SvOi::ParametersForML &retList, const SVGUID& rEmbeddedId) const;

#pragma region ITool methods
	virtual bool areAuxExtentsAvailable() const override;
	virtual SvUl::NameGuidList getAvailableAuxSourceImages() const override;
	virtual SvUl::NameGuidPair getAuxSourceImage() const override;
	virtual HRESULT setAuxSourceImage(const SVGUID& rObjectID) override;
	virtual void SetToolImage(const SVGUID& rObjectID) override;
	virtual long getToolPosition() const override;
	virtual HRESULT getExtentProperty(const SvDef::SVExtentPropertyEnum& rExtentProperty, double& rValue) override;
	virtual SvOi::ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const override;
	virtual void finishAddTool() override;
#pragma endregion ITool methods

protected:
	void UpdateAuxiliaryExtents();
	virtual bool Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	//Set the counter and State is called in Run()
	inline  virtual void  UpdateStateAndCounter(SVRunStatusClass& rRunStatus);
	HRESULT UpdateOffsetDataToImage(SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage);

	// Remove Embedded Extents
	void removeEmbeddedExtents(bool p_DisconnectExtents = true);

	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual HRESULT UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine) override;

	virtual bool createAllObjectsFromChild(SVObjectClass& rChildObject) override;

	virtual void connectChildObject(SVTaskObjectClass& rChildObject) override;

private:
	void init();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages = nullptr) const;

protected:
	SVBoolValueObjectClass ToolSelectedForOperatorMove;
	SVConditionalClass* m_pToolConditional;

	SVBoolValueObjectClass enabled;


	SvOi::IToolSet* m_pCurrentToolSet;

	SvOl::SVInObjectInfoStruct m_inputConditionBoolObjectInfo;
	SvOl::SVInObjectInfoStruct m_AuxSourceImageObjectInfo;

	// Conditional tool set drawing flag.
	SVEnumerateValueObjectClass	m_drawToolFlag;
	SVTimerValueObjectClass m_ToolTime;

	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass m_Passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass m_Warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass m_Failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass m_ExplicitFailed;

	SVLongValueObjectClass m_PassedCount;
	SVLongValueObjectClass m_FailedCount;
	SVLongValueObjectClass m_WarnedCount;

	SVLongValueObjectClass m_EnabledCount;
	SVLongValueObjectClass m_ProcessedCount;

	SVLongValueObjectClass m_ToolPosition;

	// Embedded Objects for Extents
	SVDoubleValueObjectClass m_ExtentLeft;
	SVDoubleValueObjectClass m_ExtentTop;
	SVDoubleValueObjectClass m_ExtentRight;
	SVDoubleValueObjectClass m_ExtentBottom;
	SVDoubleValueObjectClass m_ExtentWidth;
	SVDoubleValueObjectClass m_ExtentHeight;
	SVDoubleValueObjectClass m_ExtentWidthScaleFactor;
	SVDoubleValueObjectClass m_ExtentHeightScaleFactor;

	//***** New source image extent value objects
	SVBoolValueObjectClass m_svUpdateAuxiliaryExtents;

	SVDoubleValueObjectClass m_svAuxiliarySourceX;
	SVDoubleValueObjectClass m_svAuxiliarySourceY;
	SVDoubleValueObjectClass m_svAuxiliarySourceAngle;
	SVStringValueObjectClass m_svAuxiliaryDrawType;
	SVStringValueObjectClass m_svAuxiliarySourceImageName;
	//***** New source image extent value objects

	// Tool Comments
	SVStringValueObjectClass m_ToolComment;

	SVToolExtentClass m_toolExtent;

	bool m_canResizeToParent;
	bool m_hasToolExtents{true};
};

