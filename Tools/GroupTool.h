//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file GroupTool.h
/// All Rights Reserved 
//*****************************************************************************
/// Implements the GroupTool
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#pragma endregion Includes

namespace SvOp
{
	class ParameterTask;
}

namespace SvTo
{

/// Implements the GroupTool
class GroupTool :public SVToolClass
{
	SV_DECLARE_CLASS(GroupTool);
public:
	GroupTool(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_GROUPTOOL);
	virtual ~GroupTool();
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual long setToolPosition(long ToolPosition) override;
	virtual int InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject) override;
	virtual bool propagateSizeAndPosition() override;
	virtual bool usePropagateSizeAndPosition() const override;
	virtual void addOverlays(const SvIe::SVImageClass* p_Image, SvPb::OverlayDesc& rOverlay) const override;
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual SvPb::InspectionCmdResponse getInvalidDependencies() const override;
	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

	virtual uint32_t getFirstClosedParent(uint32_t stopSearchAtObjectId) const override;

protected:
	virtual bool useOverlayColorTool() const override { return false; };

private:
	void Initialize();
	SvDef::StringPairVector getInvalidDependenciesList() const;

private:
	SvVol::SVBoolValueObjectClass m_isClosed;
	SvOp::ParameterTask* m_pInputTask = nullptr;
	SvOp::ParameterTask* m_pResultTask = nullptr;
};

} //namespace SvTo
