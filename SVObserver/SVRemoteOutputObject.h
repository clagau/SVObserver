//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputObject
//* .File Name       : $Workfile:   SVRemoteOutputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:52:42  $
//******************************************************************************

#ifndef SVREMOTEOUTPUTOBJECT_H
#define SVREMOTEOUTPUTOBJECT_H

#include <vector>
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVValueObject.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVObjectLibrary/SVObjectXMLWriter.h"

class SVRemoteOutputObject : public SVObjectClass
{
public:
	SV_DECLARE_CLASS( SVRemoteOutputObject );

	SVRemoteOutputObject(void);
	virtual ~SVRemoteOutputObject(void);
	
	// Get Functions
	SVString GetInputValueObjectName();
	HRESULT GetInputValueObjectGUID( GUID& p_rGUID ) const;
	SVGUID GetInputValueObjectGUID() const;

	SVValueObjectClass* GetValueObject();	

	SVString GetGroupID( ) const ;

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	bool GetParameters( SVObjectXMLWriter& rWriter ) const;

	// Set Functions
	HRESULT SetInputObject( SVValueObjectClass* p_pObject );
	HRESULT SetInputObjectId( GUID p_ObjectId );
	HRESULT SetGroupID( const SVString& p_strGroupID );

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

private:
	// Member Variables
	// The Value Objects name.
	SVString m_strObjectName;

	// The value object
	SVValueObjectClass* m_pValueObject;	

	// Guid of the value object used to output
	SVGUID m_InputObjectId;	

	SVString m_strGroupID;	// Which Output Group this output belongs to.

};

typedef std::vector<SVRemoteOutputObject*> SVRemoteOutputObjectList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputObject.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:52:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Oct 2012 11:30:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   25 Sep 2012 15:26:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jan 2012 11:50:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.  Added new output stream functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2007 14:46:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 14:43:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2003 11:40:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified the base OutputObject class and its derived for easier RemoteOutput support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:52:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
