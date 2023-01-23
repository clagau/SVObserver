//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathTool
//* .File Name       : $Workfile:   SVMathTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#pragma endregion Includes

namespace SvOp
{
class SVDoubleResult;
class SVMathEquation;
}

namespace SvTo
{

class SVMathToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

public:
	SVMathToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMATHTOOL );

	virtual ~SVMathToolClass();

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual SvOi::ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const override;
	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual std::vector<std::string> getToolAdjustNameList() const override;
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

protected:
	virtual bool useOverlayColorTool() const override { return false; };

private:
	void init(void);

	SvOp::SVMathEquation* m_pMathEquation;
	SvOp::SVDoubleResult* m_pResult {nullptr};
};

} //namespace SvTo
