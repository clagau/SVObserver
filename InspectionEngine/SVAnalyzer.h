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
#include "SVTaskObjectList.h"
#pragma endregion Includes

class SVResultClass;

class SVAnalyzerClass : public SVTaskObjectListClass
{
	SV_DECLARE_CLASS( SVAnalyzerClass );
public:
	SVAnalyzerClass( LPCSTR ObjectName /* = "Empty Analyzer" */ );
	SVAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVANALYZER );
	virtual ~SVAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual void DisconnectImages() {};

	virtual SvOi::IObjectClass* GetResultObject();
	
// used in pattern and Blob Analyzers.
	virtual bool IsPtOverResult( const POINT& rPoint ){ return false;}
	virtual void DisplayAnalyzerResult(){};
// End. Sri

protected:
	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

	virtual void connectChildObject( SVTaskObjectClass& rChildObject ) override;

protected:
	SVResultClass* m_pAnalyzerResult;

private:
	void init();
};
