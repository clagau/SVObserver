//*****************************************************************************
// \copyright COPYRIGHT (c) 2021/09/14,2021/09/14 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file ToolSizeHelper.h
/// DESCRIPTION
//******************************************************************************

#pragma once
#include "Definitions/SVExtentEnums.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "Definitions/ObjectDefines.h"
#include "DataController.h"



namespace SvOg
{

using SizeModes = std::map<SvDef::ToolSizeAdjustEnum, SvDef::ToolSizeModes>;
using SizeValues = std::map<SvDef::ToolSizeAdjustEnum, std::string>;
using Extents = ::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >;

class ToolSizeController
{
public:
	struct EQAdjustStruct
	{
		uint32_t m_Id = SvDef::InvalidObjectId;
		SvPb::SVObjectSubTypeEnum m_subType = SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType;
		SvPb::EmbeddedIdEnum m_inputModeEmbeddedId = SvPb::NoEmbeddedId;
		SvPb::SVExtentPropertyEnum m_extentProp;
		std::string m_name;
		long m_IDSname = 0;
	};

	ToolSizeController(uint32_t inspectionId, uint32_t toolId);
	virtual ~ToolSizeController();
	bool   GetToolSizeMode(bool init, SizeModes& rModes, SizeValues& rDisplayValues);
	bool   GetToolSizeMode(bool init, SizeModes& rModes);
	//call reset to call SetExtendPropertyAutoReset(); 
	bool SetToolSizeMode(SizeModes modes, bool reset);
	bool SetAllToolSizeMode(SvDef::ToolSizeModes mode, bool reset);
	//Insert actual size to formula if mode is TSFormula
	bool SetFormulas(bool init, bool overwrite);
	//Insert string from rvalues to  formula if mode is TSFormula
	bool SetFormulas(bool init, bool overwrite, const SizeValues& rValues);
	SvOi::NameValueVector GetEnumTypes(SvDef::ToolSizeAdjustEnum adjustenum) const;
	void  InitValues();
	bool IsFullSizeAllowed(bool init);
	bool IsFullSize(bool init);

	bool hasAdjustToolSizeParameter() const {return m_hasAdjustToolSizeParameter; };
	bool IsAdjustSizeAllowed() const { return m_isAdjustSizeAllowed; };
	bool IsAdjustPositionAllowed() const { return m_isAdjustPositionAllowed; };
	bool CanResizeToParent() const { return m_canResizetoparent; };
	SvPb::EAutoSize GetAutoSizeEnabled() const { return  m_autoSizeEnabled; };
	const EQAdjustStruct GetEquationStruct(SvDef::ToolSizeAdjustEnum adjustenum) const;
	bool StoreExtents(bool init);
	bool HasRotation(bool init);
	const Extents& GetExtents(bool init);
	bool   GetValuesFromExtents(const Extents& rExtents, SizeValues& rValues);

	double  getValueForProperties(bool init, SvPb::SVExtentPropertyEnum  eProperty);
	bool   setValueForProperties(double value, SvPb::SVExtentPropertyEnum  eProperty);
	bool HasChanged(bool init);
	bool RestoreExtents();
	bool SetExtentToParent();
	bool Propagate();

	//call updatefrom output space
	bool  AdjustTool(SvPb::SVExtentLocationPropertyEnum eAction, int dx, int dy);
	bool AdjustToolAngle(bool init, double dDAngle);
	bool Isallowedlocation(SvPb::SVExtentLocationPropertyEnum eAction, SvPb::SVExtentDirectionsEnum Direction);
	bool ResetTool();
	bool ResetToolSizeAdjustTask();
	const SvUl::NameObjectIdList& GetAvailableToolList() const;
	bool InitToolList();
	const unsigned IDS_Objectnames[4] = {IDS_OBJECTNAME_EXTENT_LEFT,IDS_OBJECTNAME_EXTENT_TOP,IDS_OBJECTNAME_EXTENT_WIDTH, IDS_OBJECTNAME_EXTENT_HEIGHT};
	bool isToolValid() const { return m_isToolValid; };
	HRESULT  SetExtentsMap(const std::map< SvPb::SVExtentPropertyEnum, double>& extents);
	std::map< SvPb::SVExtentPropertyEnum, double> GetExtentValues();
	HRESULT CheckExtents() { return SetExtents(m_Extents); };
	bool GetAutofitAllowedFlag() const { return m_isAutofitAllowedFlag; };
private:
	void  InitHelper();
	HRESULT  SetExtents(const Extents& extents);
	
private:

	SvUl::NameObjectIdList m_availableToolList;

	std::unique_ptr<ValueController> m_pTaskValueController;
	uint32_t m_ipId {SvDef::InvalidObjectId};
	uint32_t m_toolId {SvDef::InvalidObjectId};;
	uint32_t m_SizeAdjustTaskId {SvDef::InvalidObjectId};

	bool m_hasAdjustToolSizeParameter = false;
	bool m_isAdjustSizeAllowed = false;
	bool m_isAdjustPositionAllowed = false;
	SvPb::EAutoSize m_autoSizeEnabled = SvPb::EAutoSize::EnableNone;
	bool m_canResizetoparent {false};
	bool m_isToolValid {false};
	bool m_isAutofitAllowedFlag {false};


	Extents m_OriginalExtents;
	Extents m_Extents;
	Extents m_ParentExtents;
	SvPb::SVExtentTranslationEnum m_translationType = SvPb::SVExtentTranslationNone;


	

	EQAdjustStruct m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSValuesCount] =
	{
		{0,SvPb::EQSizePositionXType,SvPb::ToolSizeAdjustSizePositionXModeEId, SvPb::SVExtentPropertyPositionPointX,"",IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_X},
		{0,SvPb::EQSizePositionYType,SvPb::ToolSizeAdjustSizePositionYModeEId, SvPb::SVExtentPropertyPositionPointY,"",IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_Y},
		{0,SvPb::EQSizeWidthType,    SvPb::ToolSizeAdjustSizeWidthModeEId,     SvPb::SVExtentPropertyWidth,          "",IDS_CLASSNAME_EQ_ADJUSTSIZE_WIDTH},
		{0,SvPb::EQSizeHeightType,SvPb::ToolSizeAdjustSizeHeightModeEId, SvPb::SVExtentPropertyHeight,"",IDS_CLASSNAME_EQ_ADJUSTSIZE_HEIGHT}
	};
};

}

