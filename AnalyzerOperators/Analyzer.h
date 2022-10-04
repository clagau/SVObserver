//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Analyzer
//* .File Name       : $Workfile:   Analyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 10:09:12  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "InspectionEngine/SVTaskObjectList.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/ITool.h"
#include "Tools/SVToolExtentclass.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvAo
{

class Analyzer : public SvIe::SVTaskObjectListClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS
public:
	explicit Analyzer( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	Analyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVANALYZER );
	virtual ~Analyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void DisconnectImages() {};

	virtual SvOi::IObjectClass* GetResultObject();
	
	virtual bool IsPtOverResult( const POINT&  ){ return false;}
	virtual SvDef::StringVector getAnalyzerResult() {return SvDef::StringVector{};}

	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& , std::back_insert_iterator<SvOi::ParametersForML> ) const {};
	const SVImageExtentClass& GetImageExtent() const { return m_imageExtent; }
	virtual const SVImageExtentClass* GetImageExtentPtr() const
	{
		return &(m_imageExtent);
	};

	const SvTo::SVToolExtentClass* getToolExtentPtr() const
	{
		auto *pTool = GetToolInterface();
		if (pTool)
		{
			return &(pTool->GetToolExtent());

		}
		else
		{
			return nullptr;
		}

	}
protected:
	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject( SVTaskObjectClass& rChildObject ) override;

private:
	void init();
public:	
	SVImageExtentClass m_imageExtent;	
};

} //namespace SvAo
