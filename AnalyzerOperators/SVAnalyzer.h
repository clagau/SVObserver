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
	SV_DECLARE_CLASS( SVAnalyzerClass );
public:
	SVAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVANALYZER );
	virtual ~SVAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void DisconnectImages() {};

	virtual SvOi::IObjectClass* GetResultObject();
	
	virtual bool IsPtOverResult( const POINT& rPoint ){ return false;}
	virtual SvDef::StringVector getAnalyzerResult() {return SvDef::StringVector{};}

	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const {};

protected:
	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject( SVTaskObjectClass& rChildObject ) override;

private:
	void init();
};

} //namespace SvAo
