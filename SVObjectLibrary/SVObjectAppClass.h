//******************************************************************************
//* COPYRIGHT (c) 2004 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVOResource/resource.h"
#pragma endregion Includes

namespace SvOi
{
class IInspectionProcess;
class ITool;
}

namespace SvOl
{

class SVObjectAppClass : public SVObjectClass, public SvOi::IObjectAppClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	virtual ~SVObjectAppClass();
	explicit SVObjectAppClass(LPCSTR LPSZObjectName);
	SVObjectAppClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOBJECTAPPCLASS );

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual void ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual void refreshAllObjects(const SVObjectLevelCreateStruct& rCreateStructure) override;

	SVObjectClass* GetInspection() const { return m_pInspection;  };
	SvOi::IInspectionProcess* GetInspectionInterface() const { return m_pInspectionInterface; };

	SVObjectClass* GetTool() const { return m_pTool; };
	SvOi::ITool* GetToolInterface() const { return m_pToolInterface; };

	SVObjectClass* GetAnalyzer() const { return m_pAnalyzer; };

#pragma region virtual methods (IObjectAppClass)
	virtual bool CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context) override;
#pragma endregion virtual methods (IObjectAppClass)

	// Call this method at the object owner to create an object.
	// If the owner object is not created yet, it returns false.
	// Otherwise called the method createAllOBjectsFromChild
	// and returns the result of this message.
	virtual bool CreateChildObject( SVObjectClass* pChildObject, DWORD context = 0 ) override;

protected:
	void UpdateConnections(const SVObjectLevelCreateStruct& rCreateStructure);

	virtual bool createAllObjectsFromChild( SVObjectClass& rChildObject ) override;

private:
	SVObjectClass* m_pInspection;
	SVObjectClass* m_pTool;
	SVObjectClass* m_pAnalyzer;

	//! We need both interface and object pointers to optimize speed in run mode
	SvOi::IInspectionProcess* m_pInspectionInterface;
	SvOi::ITool* m_pToolInterface;
};

} //namespace SvOl
