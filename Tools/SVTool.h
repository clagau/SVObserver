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
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "SVValueObjectLibrary/SVTimerValueObjectClass.h"
#pragma endregion Includes

namespace SvIe
{
struct SVExtentPropertyInfoStruct;
class RunStatus;
}

namespace SvOp
{
class SVConditional;
}

namespace SvTo
{

enum class ExtType
{
	Left,Top,Right,Bottom,Width,Height
};

class  EmbeddedExtents
{
public:
	EmbeddedExtents() = delete;
	explicit EmbeddedExtents(SVToolClass* pOwner);
	void SetTypeNameForExtents();
	void RegisterEmbeddedExtents();
	void	SetDefaults();
	void SetAttributes();
	
	SVToolClass* const m_pOwner; 
	// Embedded Objects for Extents
	SvVol::SVDoubleValueObjectClass m_ExtentLeft;
	SvVol::SVDoubleValueObjectClass m_ExtentTop;
	SvVol::SVDoubleValueObjectClass m_ExtentRight;
	SvVol::SVDoubleValueObjectClass m_ExtentBottom;
	SvVol::SVDoubleValueObjectClass m_ExtentWidth;
	SvVol::SVDoubleValueObjectClass m_ExtentHeight;
	SvVol::LinkedValue m_ExtentWidthFactorContent;
	SvVol::LinkedValue m_ExtentHeightFactorContent;
	SvVol::LinkedValue m_ExtentWidthFactorFormat;
	SvVol::LinkedValue m_ExtentHeightFactorFormat;

	

	SvVol::SVDoubleValueObjectClass m_svAuxiliarySourceX;
	SvVol::SVDoubleValueObjectClass m_svAuxiliarySourceY;
	SvVol::SVDoubleValueObjectClass m_svAuxiliarySourceAngle;
	SvVol::SVStringValueObjectClass m_svAuxiliaryDrawType;
	SvVol::SVStringValueObjectClass m_svAuxiliarySourceImageName;
};


class SVToolClass : public SvIe::SVTaskObjectListClass, public SvOi::ITool
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	SVToolClass() = delete;
	explicit SVToolClass(bool hasEmbedededExtents , SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTOOL);
	virtual ~SVToolClass();

	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool CloseObject() override;

	virtual void GetDrawInfo(SVExtentMultiLineStruct& p_rMultiLine) override;

	bool IsEnabled() const;

	bool WasEnabled() const;

	virtual bool allowExtensionCopy() const override
	{
		return (nullptr != m_pEmbeddedExtents.get());
	}

	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual SvPb::EAutoSize GetAutoSizeEnabled() const;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages = nullptr);

	void SetImageExtentProperty(SvPb::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject);

	virtual HRESULT SetImageExtent(const SVImageExtentClass& rImageExtent) ;
	const SVImageExtentClass& GetImageExtent() const;


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
	virtual void UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine) const override;
	virtual void addOverlays(const SvIe::SVImageClass* p_Image, SvPb::OverlayDesc& rOverlay) const;

	// Auxiliary Source Image functions
	HRESULT GetSourceImages(SvIe::SVImageClassPtrVector* p_psvImageList) const;
	HRESULT SetAuxSourceImage(SvIe::SVImageClass* p_psvImage);

	virtual SVObjectClass* overwriteInputObject(uint32_t uniqueId, SvPb::EmbeddedIdEnum embeddedId) override;

	// Set String value object for Source Image Names
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames();
	virtual bool SetFirstInputImageName(LPCTSTR FirstName) override;

	virtual HRESULT updateImageExtent(bool init) override;
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
	/// \param embeddedId [in] The EmbeddedId of the object
	bool addEntryToMonitorList(SvOi::ParametersForML &retList, SvPb::EmbeddedIdEnum embeddedID) const;

	SVToolExtentClass& getToolExtent() {return m_toolExtent;}
	HRESULT UpdateOffsetDataToImage(SVExtentOffsetStruct& p_rsvOffsetData, SvIe::SVImageClass* p_svToolImage);

	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& ) { return nullptr; };

	virtual bool propagateSizeAndPosition();
	virtual bool usePropagateSizeAndPosition() const;
	
	 void removeTaskMessages(DWORD MessageCode, SvStl::MessageTextEnum AdditionalTextId);
	 const SvVol::SVDWordValueObjectClass& getColorObject() const { return m_statusColor; };
	 virtual void overwriteInputSource(SvOi::SVImageBufferHandlePtr imageHandlePtr) {};
	


	 virtual void resetCounters();
	 virtual const SVImageExtentClass* GetImageExtentPtr() const;
#pragma region ITool methods
	virtual bool areAuxExtentsAvailable() const override;
	virtual SvUl::NameObjectIdList getAvailableAuxSourceImages() const override;
	virtual SvUl::NameObjectIdPair getAuxSourceImage() const override;
	virtual HRESULT setAuxSourceImage(uint32_t objectID) override;
	virtual void SetToolImage(uint32_t objectID) override;
	virtual uint32_t GetToolImage() const override;
	virtual long getToolPosition() const override;
	virtual void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const override;
	virtual HRESULT getParentExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties, SvPb::SVExtentTranslationEnum& rTranslationType) const override;
	virtual SvOi::ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const override;
	virtual void finishAddTool() override;
	virtual SvPb::EAutoSize getAutoSizeEnabled() const override;
	//!control the ROI of Inputwindow
	virtual HRESULT updateExtentFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eAction, long dx, long dy) override;
	virtual HRESULT setExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double value) override;
	virtual HRESULT setExtentList(const ::google::protobuf::RepeatedPtrField<::SvPb::ExtentParameter> param) override;
	virtual HRESULT setExtentToParent() override;
	//!  Return false  if the input location 
	//! is not allowed for the tool 
	virtual bool isAllowedLocation(const SvPb::SVExtentLocationPropertyEnum Location, SvPb::SVExtentDirectionsEnum Direction = SvPb::SVExtentDirectionBoth) const override;
	virtual SvPb::InspectionCmdResponse getInvalidDependencies() const override { return {}; };
	virtual std::vector<std::string> getToolAdjustNameList() const override { return {}; }
	virtual bool isToolActive() const;
#pragma endregion ITool methods
	

protected:
	void UpdateAuxiliaryExtents();
	virtual bool Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	//Set the counter and State is called in Run()
	inline  virtual void  UpdateStateAndCounter(SvIe::RunStatus& rRunStatus);

	

	virtual bool onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual void UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine) override;
	virtual bool createAllObjectsFromChild(SVObjectClass& rChildObject) override;
	virtual void connectChildObject(SVTaskObjectClass& rChildObject) override;
	virtual bool useOverlayColorTool() const { return true; };

	void setStateValueToOverlay(SvPb::Overlay& rOverlay) const;

private:
	void init();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages = nullptr) const;

protected:

	std::unique_ptr<EmbeddedExtents> m_pEmbeddedExtents = nullptr;
	
	SvVol::SVBoolValueObjectClass ToolSelectedForOperatorMove;
	SvOp::SVConditional* m_pToolConditional;

	SvVol::SVBoolValueObjectClass enabled;


	SvOi::IToolSet* m_pCurrentToolSet;

	SvOl::InputObject m_conditionBoolInput;
	SvOl::InputObject m_AuxSourceImageInput;
	bool m_OverlayColorToolInputIsSet = false;
	SvOl::InputObject m_overlayColorToolInput;

	SvVol::SVBoolValueObjectClass  m_isObjectValid;	//	Embedded

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
	SvVol::SVBoolValueObjectClass m_editFreezeFlag;

	//***** New source image extent value objects
	SvVol::SVBoolValueObjectClass m_svUpdateAuxiliaryExtents;

	// Tool Comments
	SvVol::SVStringValueObjectClass m_ToolComment;

	//SVImageExtentClass  is a member of SVToolExtentClass
	SVToolExtentClass m_toolExtent;

	bool m_canResizeToParent {false};
	
	
};

bool isValidScaleFactor(double value);
bool isValidScaleFactorLV(SvVol::LinkedValue& scaleFactorValue);

} //namespace SvTo
