//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAppClass
//* .File Name       : $Workfile:   SVObjectAppClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   16 Dec 2014 18:00:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "ObjectInterfaces/IObjectAppClass.h"
#pragma endregion Includes

class SVInspectionProcess;
class SVToolClass;
class SVAnalyzerClass;

class SVObjectAppClass : public SVObjectClass, public SvOi::IObjectAppClass
{
	SV_DECLARE_CLASS( SVObjectAppClass )

public:
	virtual ~SVObjectAppClass();
	SVObjectAppClass( LPCSTR LPSZObjectName );
	SVObjectAppClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOBJECTAPPCLASS );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct ) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStruct ) override;

	SVInspectionProcess* GetInspection() const;
	SVToolClass* GetTool() const;
	SVAnalyzerClass* GetAnalyzer() const;

#pragma region virtual methods (IObjectAppClass)
	virtual bool CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context) override;
#pragma endregion virtual methods (IObjectAppClass)

	// Call this method at the object owner to create an object.
	// If the owner object is not created yet, it returns false.
	// Otherwise called the method createAllOBjectsFromChild
	// and returns the result of this message.
	virtual bool CreateChildObject( SVObjectClass* pChildObject, DWORD context = 0 ) override;

protected:
	void UpdateConnections( const SVObjectLevelCreateStruct* PCreateStruct );

	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

private:
	SVInspectionProcess* m_psvInspection;
	SVToolClass* m_psvTool;
	SVAnalyzerClass* m_psvAnalyzer;
};

