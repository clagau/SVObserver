//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerClass
//* .File Name       : $Workfile:   SVAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 10:09:12  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "InspectionEngine/SVTaskObjectList.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/ITool.h"
#pragma endregion Includes

namespace SvAo
{

class SVAnalyzerClass : public SvIe::SVTaskObjectListClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS
public:
	explicit SVAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVANALYZER );
	virtual ~SVAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void DisconnectImages() {};

	virtual SvOi::IObjectClass* GetResultObject();
	
	virtual bool IsPtOverResult( const POINT&  ){ return false;}
	virtual SvDef::StringVector getAnalyzerResult() {return SvDef::StringVector{};}

	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& , std::back_insert_iterator<SvOi::ParametersForML> ) const {};

protected:
	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject( SVTaskObjectClass& rChildObject ) override;

private:
	void init();
};

} //namespace SvAo
