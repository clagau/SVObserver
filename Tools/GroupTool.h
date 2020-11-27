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

	virtual long  setToolPosition(long ToolPosition) override;
	virtual int InsertBefore(uint32_t objectBeforeID, SVTaskObjectClass* pTaskObject) override;
	virtual bool propagateSizeAndPosition() override;
	virtual bool usePropagateSizeAndPosition() const override;
	virtual void addOverlays(const SvIe::SVImageClass* p_Image, SvPb::OverlayDesc& rOverlay) const override;
	virtual bool areAuxExtentsAvailable() const override { return false; };

protected:
	virtual bool useOverlayColorTool() const override { return false; };

private:
	void Initialize();

private:
};

} //namespace SvTo
