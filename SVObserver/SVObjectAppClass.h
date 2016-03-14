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

#include "SVObjectLibrary/SVObjectClass.h"
#include "ObjectInterfaces/IObjectAppClass.h"
#include "ObjectInterfaces/IObjectClass.h"

class SVInspectionProcess;
class SVToolClass;
class SVAnalyzerClass;

class SVObjectAppClass : virtual public SvOi::IObjectAppClass
								, public SVObjectClass  
{
	SV_DECLARE_CLASS( SVObjectAppClass )

public:
	virtual ~SVObjectAppClass();
	SVObjectAppClass( LPCSTR LPSZObjectName );
	SVObjectAppClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOBJECTAPPCLASS );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual HRESULT ConnectObject( SVObjectLevelCreateStruct* PCreateStruct );

	SVInspectionProcess* GetInspection() const;
	SVToolClass* GetTool() const;
	SVAnalyzerClass* GetAnalyzer() const;

#pragma region virtual methods (IObjectAppClass)
	virtual SvOi::IObjectClass* GetToolInterface() const override;
	virtual DWORD_PTR CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context) override;
#pragma endregion virtual methods (IObjectAppClass)

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void UpdateConnections( SVObjectLevelCreateStruct* PCreateStruct );

	// Sends SVM_CREATE_ALL_OBJECTS to the child object
	// and returns the result of this message.
	virtual DWORD_PTR SVObjectAppClass::createAllObjectsFromChild( SVObjectClass* pChildObject );
	// Call this method at the object owner to create an object.
	// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
	// Otherwise called the method createAllOBjectsFromChild
	// and returns the result of this message.
	// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
	DWORD_PTR CreateChildObject( SVObjectClass* pChildObject, DWORD context );

private:
	SVInspectionProcess* m_psvInspection;
	SVToolClass* m_psvTool;
	SVAnalyzerClass* m_psvAnalyzer;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

