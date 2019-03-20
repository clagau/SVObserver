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
#include "SVToolExtentClass.h"
#include "ObjectInterfaces/ITool.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IToolSet.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{
class SVConditionalClass;
}

namespace SvTo
{
class SVToolClass : public SvIe::SVTaskObjectListClass, public SvOi::ITool
{
	SV_DECLARE_CLASS(SVToolClass);

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
	virtual SvPb::EAutoSize GetAutoSizeEnabled() const;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages = nullptr);

	void SetImageExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject);

	virtual HRESULT SetImageExtent(const SVImageExtentClass& rImageExtent) override;

	const SvIe::SVImageClass* GetToolImage() const;

	void SetAlwaysUpdate(bool p_bAlwaysUpdate);

	HRESULT GetRootOffsetData(SVExtentOffsetStruct& p_rsvOffsetData);
	HRESULT UpdateOffsetData(SvIe::SVImageClass* p_svToolImage);
	HRESULT TranslatePointToSource(SVPoint<double> inPoint, SVPoint<double>& rOutPoint);
	void setAuxiliaryExtents();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual HRESULT GetPropertyInfo(SvPb::SVExtentPropertyEnum p_eProperty, SvIe::SVExtentPropertyInfoStruct& p_rInfo) const override;
	HRESULT SetExtentPropertyInfo(SvPb::SVExtentPropertyEnum p_eProperty, const SvIe::SVExtentPropertyInfoStruct& p_rInfo);

	bool getConditionalResult(bool bRunMode = false) const;

	virtual HRESULT CollectOverlays(SvIe::SVImageClass *pImage, SVExtentMultiLineStructVector& rMultiLineArray) override;

	// Auxiliary Source Image functions
	HRESULT GetSourceImages(SvIe::SVImageClassPtrVector* p_psvImageList) const;
	HRESULT SetAuxSourceImage(SvIe::SVImageClass* p_psvImage);

	virtual HRESULT IsAuxInputImage(const SvOl::SVInObjectInfoStruct* p_psvInfo) const override;

	const SvOl::SVInObjectInfoStruct* GetAuxInputImageInfo() const { return &m_AuxSourceImageObjectInfo;}

	// Set String value object for Source Image Names
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames();
	virtual bool SetFirstInputImageName(LPCTSTR FirstName) override;

	virtual HRESULT updateImageExtent() override;
	virtual HRESULT UpdateImageWithExtent() override;
	virtual HRESULT GetParentExtent(SVImageExtentClass& p_rParent) const;

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
	virtual bool canResizeToParent() const override { return m_canResizeToParent; };

	//
	// Flag to indicate this tool is selected for SVIM operator move.
	// 26 Jan 2000 - frb.
	//
	const SVImageInfoClass* getFirstImageInfo() const;

	/// Search an object with the embeddedId and add it to the ParameterList for the needed in monitor list.
	/// \param retList [in,out] The ParameterList
	/// \param rEmbeddedId [in] The EmbeddedId of the object
	bool addEntryToMonitorList(SvOi::ParametersForML &retList, const SVGUID& rEmbeddedId) const;

	SVToolExtentClass& getToolExtent() {return m_toolExtent;}
	HRESULT UpdateOffsetDataToImage(SVExtentOffsetStruct& p_rsvOffsetData, SvIe::SVImageClass* p_svToolImage);

	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) { return nullptr; };

	virtual HRESULT propagateSizeAndPosition();
	virtual bool usePropagateSizeAndPosition() const;

#pragma region ITool methods
	virtual bool areAuxExtentsAvailable() const override;
	virtual SvUl::NameGuidList getAvailableAuxSourceImages() const override;
	virtual SvUl::NameGuidPair getAuxSourceImage() const override;
	virtual HRESULT setAuxSourceImage(const SVGUID& rObjectID) override;
	virtual void SetToolImage(const SVGUID& rObjectID) override;
	virtual long getToolPosition() const override;
	virtual void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const override;
	virtual HRESULT getParentExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const override;
	virtual SvOi::ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const override;
	virtual void finishAddTool() override;
	virtual SvPb::EAutoSize getAutoSizeEnabled() const override;
	virtual HRESULT updateExtentFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eAction, long dx, long dy) override;
	virtual HRESULT setExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double value) override;
	virtual HRESULT setExtentList(const ::google::protobuf::RepeatedPtrField<::SvPb::ExtentParameter> param) override;
	virtual HRESULT setExtentToParent() override;
	//!  Return false  if the input location 
	//! is not allowed for the tool 
	//! \param Location [in]
	//! \param Direction 
	//! \returns bool
	virtual bool isAllowedLocation(const SvPb::SVExtentLocationPropertyEnum Location, SvPb::SVExtentDirectionsEnum Direction = SvPb::SVExtentDirectionBoth) const override;
#pragma endregion ITool methods

protected:
	void UpdateAuxiliaryExtents();
	virtual bool Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	//Set the counter and State is called in Run()
	inline  virtual void  UpdateStateAndCounter(SVRunStatusClass& rRunStatus);

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
	SvVol::SVBoolValueObjectClass ToolSelectedForOperatorMove;
	SvOp::SVConditionalClass* m_pToolConditional;

	SvVol::SVBoolValueObjectClass enabled;


	SvOi::IToolSet* m_pCurrentToolSet;

	SvOl::SVInObjectInfoStruct m_inputConditionBoolObjectInfo;
	SvOl::SVInObjectInfoStruct m_AuxSourceImageObjectInfo;

	// Conditional tool set drawing flag.
	SvVol::SVEnumerateValueObjectClass	m_drawToolFlag;
	SvVol::SVTimerValueObjectClass m_ToolTime;

	// Passed, if TRUE ( Reset Value: FALSE )
	SvVol::SVBoolValueObjectClass m_Passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SvVol::SVBoolValueObjectClass m_Warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SvVol::SVBoolValueObjectClass m_Failed;
	// Failed, if TRUE ( Reset Value: FALSE )
	SvVol::SVBoolValueObjectClass m_ExplicitFailed;

	SvVol::SVLongValueObjectClass m_PassedCount;
	SvVol::SVLongValueObjectClass m_FailedCount;
	SvVol::SVLongValueObjectClass m_WarnedCount;

	SvVol::SVLongValueObjectClass m_EnabledCount;
	SvVol::SVLongValueObjectClass m_ProcessedCount;

	SvVol::SVLongValueObjectClass m_ToolPosition;

	// Embedded Objects for Extents
	SvVol::SVDoubleValueObjectClass m_ExtentLeft;
	SvVol::SVDoubleValueObjectClass m_ExtentTop;
	SvVol::SVDoubleValueObjectClass m_ExtentRight;
	SvVol::SVDoubleValueObjectClass m_ExtentBottom;
	SvVol::SVDoubleValueObjectClass m_ExtentWidth;
	SvVol::SVDoubleValueObjectClass m_ExtentHeight;
	SvVol::SVDoubleValueObjectClass m_ExtentWidthScaleFactor;
	SvVol::SVDoubleValueObjectClass m_ExtentHeightScaleFactor;

	//***** New source image extent value objects
	SvVol::SVBoolValueObjectClass m_svUpdateAuxiliaryExtents;

	SvVol::SVDoubleValueObjectClass m_svAuxiliarySourceX;
	SvVol::SVDoubleValueObjectClass m_svAuxiliarySourceY;
	SvVol::SVDoubleValueObjectClass m_svAuxiliarySourceAngle;
	SvVol::SVStringValueObjectClass m_svAuxiliaryDrawType;
	SvVol::SVStringValueObjectClass m_svAuxiliarySourceImageName;
	//***** New source image extent value objects

	// Tool Comments
	SvVol::SVStringValueObjectClass m_ToolComment;

	SVToolExtentClass m_toolExtent;

	bool m_canResizeToParent;
	bool m_hasToolExtents{true};
};

} //namespace SvTo
