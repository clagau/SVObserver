//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWindowTool
//* .File Name       : $Workfile:   SVWindowTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVWindowToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVWindowToolClass );

public:
	SVWindowToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVWINDOWTOOL );
	virtual ~SVWindowToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent ) override;
	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtentToFit( const SVImageExtentClass& rImageExtent ) override;

	virtual SvIe::SVTaskObjectClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;

	virtual SvOi::ParametersForML getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const override;

protected:

private:
	void init();
	void BuildEmbeddedObjectList ();
	void BuildAvailableAnalyzerList ();
	void AddUnaryImageOperatorListAsChild ();

public:
	SvVol::SVStringValueObjectClass* GetInputImageNames( ) override;

#pragma region Member Variables
private:
	SvVol::SVStringValueObjectClass m_SourceImageNames;
#pragma endregion Member Variables
};

} //namespace SvTo