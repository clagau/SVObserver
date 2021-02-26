//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalTool
//* .File Name       : $Workfile:   SVExternalTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:46  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVExternalTool : public SVToolClass
{
	SV_DECLARE_CLASS( SVExternalTool );

public:
	SVExternalTool( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SV_EXTERNAL_TOOL );
	virtual ~SVExternalTool();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;

	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

protected:
	virtual HRESULT UpdateImageWithExtent() override;

	SvVol::SVBoolValueObjectClass m_svPerformTranslation;

private:
	void Initialize();

#pragma region Member Variables
private:
	// String value object for Source Image Names
	SvVol::SVStringValueObjectClass m_SourceImageNames;
#pragma endregion Member Variables
};

} //namespace SvTo
