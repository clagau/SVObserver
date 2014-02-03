//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectClass
//* .File Name       : $Workfile:   SVObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:09:14  $
//******************************************************************************

#ifndef SVOBJECTCLASS_H
#define SVOBJECTCLASS_H

#include <set>
#include <vector>

#include "SVStatusLibrary/SVErrorClass.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVObjectScriptUsage.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectLibrary.h"
#include "SVOutObjectInfoStruct.h"
#include "SVPublicAttributeEntryStruct.h"
#include "SVObjectAttributeShim.h"
#include "SVObjectWriter.h"

struct SVInObjectInfoStruct;
struct SVObjectLevelCreateStruct;
struct SVObjectNameInfo;

class SVDrawContext;
class SVObjectAttributeClass;

/*
This class is the base class to all configuration level objects.  These objects will get created and managed by the object manager object.
*/
class SVObjectClass
{
	SV_DECLARE_CLASS( SVObjectClass );

public:
	typedef std::deque< SVObjectClass* > SVObjectPtrDeque;

	friend class SVObjectManagerClass;
	friend class SVPPQObject;
	friend class SVConfigurationObject;

	friend LONG_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );
	friend LONG_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );

	SVObjectClass();
	SVObjectClass( LPCSTR ObjectName );
	SVObjectClass( SVObjectClass *POwner, int StringResourceID );

	virtual ~SVObjectClass();

	template< typename SVObjectVisitor >
	HRESULT Accept( SVObjectVisitor& p_rVisitor );

	virtual HRESULT ResetObject();

	virtual void ResetPrivateInputInterface();

	int GetObjectDepth();
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );

	virtual long GetImageDepth() const;
	virtual BOOL SetImageDepth( long lDepth );

	virtual BOOL InitObject( SVObjectClass* PObject );
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ConnectObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL IsValid();
	virtual BOOL Validate();
	virtual BOOL OnValidate();
	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual BOOL SetObjectOwner( const GUID& RNewOwnerGUID );

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );
	virtual void SetInvalid();
	virtual void SetDisabled();
	virtual DWORD	GetObjectColor();
	virtual DWORD	GetObjectState();
	virtual HICON	GetObjectIcon();

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectSubScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
	virtual void PutAttributesInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent );
	virtual void PutPQDataLinkInfoInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent );
	virtual void PutFriendGuidsInObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent );
	virtual void MakeUniqueFriendAlias( CString& RStrScript );
#endif
	// SEJ 102 - new persistence methods
	virtual void Persist( SVObjectWriter& rWriter );
	virtual void PersistAttributes( SVObjectWriter& rWriter );

	virtual bool IsArray() const;
	virtual int  GetArraySize() const;

	virtual BOOL ReinitObjectInfos();
	
	virtual BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );

	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;
	
	virtual HRESULT ResetObjectInputs();

	BOOL ConnectObjectInput( SVInObjectInfoStruct* PObjectInInfo );
	BOOL DisconnectObjectInput( SVInObjectInfoStruct* PObjectInInfo );
	BOOL IsObjectValid( GUID& RValidationReferenceID );
	BOOL IsCreated() const;
	BOOL IsDescendantOf( SVObjectClass* PAncestorObject );
	BOOL IsDescendantOfType( const SVObjectInfoStruct& rAncestorInfo );

	virtual void ResetName();
	virtual void SetName( LPCTSTR StrString );
	virtual void SetObjectName( int StringResourceID );

	void SetObjectEmbedded( const GUID& REmbeddedID, SVObjectClass* POwner, int NewStringResourceID );

	BOOL AddFriend( const GUID& RFriendGUID );
	BOOL RemoveFriend( const GUID& RFriendGUID );
	void DestroyFriends();
	SVPublicAttributeEntryStruct* GetPublicAttribute();
	SVObjectClass* GetAncestor( SVObjectTypeEnum AncestorObjectType );
	LPCTSTR GetName() const;
	int GetNameLength() const;
	LPCTSTR GetObjectName() const;
	int GetObjectNameLength() const;
	CString GetCompleteObjectName() const;
	void GetCompleteObjectName( CString& rString ) const;

	CString GetCompleteObjectNameToObjectType( LPCSTR LPSZCompleteName = NULL, SVObjectTypeEnum  objectTypeToInclude = SVToolSetObjectType ) const;
	LPTSTR GetCompleteObjectName2( LPCTSTR LPSZCompleteName = NULL );
	int GetCompleteObjectNameLength( int Length );
	int GetResourceID() const;
	SVObjectClass* GetOwner();
	long GetObjectSubType() const;
	SVOutObjectInfoStruct& GetObjectOutputInfo();

	const SVGUID& GetUniqueObjectID() const;
	const SVGUID& GetEmbeddedID() const;
	const SVGUID& GetOwnerID() const;
	const SVObjectTypeEnum& GetObjectType() const;
	const SVObjectInfoStruct&	GetOwnerInfo();
	const SVObjectInfoStruct&	GetObjectInfo();
	const SVObjectInfoArrayClass& GetFriendList ();

protected:
	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	virtual SVObjectClass *UpdateObject( const GUID &friendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner );
	
	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );

	void buildCompleteObjectName( LPTSTR LPSZCompleteName, int MaxLength );

	virtual HRESULT RemoveObjectConnection( const GUID& p_rObjectID );

public:

	//This attribute holds the object level error information.
	SVErrorClass msvError;

	const UINT ObjectAttributesAllowed() const;
	const UINT ObjectAttributesSet(int iIndex=0) const;
	UINT& ObjectAttributesAllowedRef();
	SVObjectAttributeShim ObjectAttributesSetRef(int iIndex=0);
	void SetObjectAttributesSet(UINT uAttributes, int iIndex=0, UINT WhichBits = SET_ALL_BITS);
	void SetDefaultObjectAttributesSet(UINT uAttributes);

protected:	// private:
	//This attribute holds the enumerated bits of allowed object attributes.
	UINT m_uObjectAttributesAllowed;
	//This attribute holds the enumerated bits of set object attributes.
	UINT m_uObjectAttributesSet;
	//This attribute holds the array enumerated bits of set object attributes based on object array depth.
	std::vector<UINT> m_auObjectAttributesSet;
	//This attribute holds the enumerated bits of default object attributes.
	UINT m_uDefaultObjectAttributesSet;

	//This attribute holds the data depth of the embedded data elements.
	int objectDepth;

	//This attribute holds the image depth of the embedded image elements.
	long mlImageDepth;

	//This attribute holds the validity state of the object.
	mutable BOOL isObjectValid;
	// Refer to IsCreated()
	BOOL isCreated;
	//This attribute holds the validation reference identification number.
	SVGUID validationReferenceID;
	//If object is embedded, set this ID
	SVGUID embeddedID;
	//Owner Info
	SVObjectInfoStruct ownerObjectInfo;
	//Contains the object info and could also be used as task out info.
	SVOutObjectInfoStruct outObjectInfo;
	//Contains a list of friend objects, which will be informed about certain actions or messages this object is doing/processing.
	SVObjectInfoArrayClass friendList;
	//Output table
	SVPublicAttributeEntryStruct publicAttribute;

	//user given name
	CString strName;
private:
	void init();

	//String resource ID, of NOT user changeable name.
	int resourceID;
	//NOT user changeable name
	CString strObjectName;

};

/*
This specialization of the standard set template creates a container object to hold pointers to SVObjectClass objects.
*/
typedef std::set<SVObjectClass*> SVStdSetSVObjectClassPtr;
/*
This specialization of the standard vector template creates a container object to hold pointers to SVObjectClass objects.
*/
typedef std::vector<SVObjectClass*> SVStdVectorSVObjectClassPtr;
/*
This specialization of the SVArrayClass template creates a container object to hold pointers to SVObjectClass objects.
*/
typedef SVVector< SVObjectClass* > SVObjectClassPtrArray;

#include "SVObjectClass.inl"

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectClass.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:09:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:46:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   10 Jan 2013 14:58:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805, 801
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   07 Dec 2012 10:33:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   New method for GetObjectValue
 * new method for SetObjectValue
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   04 Sep 2012 13:24:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new parsing and saving functionality to object framework.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27.1.1   08 Jan 2013 11:41:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to Object Class to fix adjusting Object Depth of Value Object and Last Set Index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27.1.0   05 Dec 2012 11:47:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add new virtual methods for GetObjectValue & SetObjectValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   27 Jul 2012 08:00:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the visitor fucntionality to make it more flexable without having to add new elements to the framework.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   18 Jul 2012 13:14:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   12 Jul 2012 14:03:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   02 Jul 2012 16:02:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   18 Jun 2012 17:13:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   07 Dec 2010 15:51:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Nov 2010 14:26:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   28 Oct 2010 13:35:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   29 Jun 2010 13:58:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   01 Jun 2010 08:29:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   15 Feb 2010 10:38:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   15 Dec 2009 12:55:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   03 Sep 2009 09:11:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Jul 2009 08:35:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   14 Jul 2009 10:18:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update const issues with object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   20 Jun 2007 13:27:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   21 Sep 2005 07:35:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Sep 2005 15:40:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to handle attributes for "entire array" and first element correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Sep 2005 15:39:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to handle attributes for "entire array" and first element correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   17 Aug 2005 10:18:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added SVGuid.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Aug 2005 09:21:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new typedef for a standard set of SVObjectClass pointers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Aug 2005 10:17:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed GetObjectDataTable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jul 2005 07:54:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   consolidated common constructor code in init()
 * set m_uDefaultObjectAttributesSet in init() which is called from all constructors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Jul 2005 15:26:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_uDefaultObjectAttributesSet and SetDefaultObjectAttributesSet
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jul 2005 11:45:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   inlined functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jun 2005 14:49:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVObjectClassVector typedef
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 15:11:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   BOOL --> SVHRESULT on SetObjectValue
 * added array functionality
 * const-corrected
 * added accessors to object attributes
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 11:12:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed methods that used old extents and Draw Context Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:54:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   12 Jul 2004 13:04:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented SV_HIDDEN
 * fixed SV_GET_KIND_OF
 * added copy constructor / operator = to SVInObjectInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   12 Jul 2004 13:01:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  428
 * SCR Title:  Improve Load Configuration time
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented GetCompleteObjectName optimization
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   04 Dec 2003 12:59:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed SVInObjectInfoStruct to have a const copy constructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   22 Apr 2003 11:34:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   30 Jan 2003 15:08:22   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SetImageDepth and GetImageDepth methods to class.  Updated constructor to initialize mlImageDepth attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   22 Jan 2003 13:42:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   10 Jan 2003 17:49:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   forgot the = sign for the const UINT message defines
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   10 Jan 2003 16:54:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   forgot to add ; to constants that are now const UINT instead of #define
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   10 Jan 2003 15:45:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   03 Jan 2003 15:28:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new attributes.  SV_SETABLE_ONLINE, SV_SETABLE_REMOTELY, SV_EXTENT_OBJECT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   17 Dec 2002 15:20:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );
 * 
 * Changed #define to const <typename> declarations. (So the values can be seen in the debugger)
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   09 Dec 2002 19:44:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added SV_PRINTABLE flag to attribute flages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   20 Nov 2002 09:11:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed TheClassRegister -> TheSVClassRegister
 * Added constants
 * SVInObjectInfoStruct: Added strName member var
 * SVIPDocLevelCreateStruct: removed references to SVObjectInfoStruct
 * Removed references to  SVPQDataLinkInfo
 * SVObjectClass: Added friend classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   04 Apr 2002 10:09:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added debug information variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   02 Oct 2001 15:05:50   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added message definitions for 
 * SVMSGID_GET_IMAGE_BAND0_OBJECT & 
 * SVM_GET_IMAGE_BAND0_OBJECT.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   19 Apr 2001 14:48:08   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   06 Sep 2000 16:31:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   14 Jun 2000 16:20:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  159
 * SCR Title:  Incorrect placement of Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new message definition for SVM_IMAGE_SOURCE_CHANGED.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   19 May 2000 16:18:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  148
 * SCR Title:  Formulas not updated when Tools are renamed.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the SVM_OBJECT_RENAMED message so that it
 * will also be routed to friend objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   11 May 2000 08:56:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  139
 * SCR Title:  Math Tool Invalid State
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVM_CREATE_ALL_CLOSED_OBJECTS to
 * also route this message to friend objects.
 * 
 * Added IsDescendantOfType method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   26 Apr 2000 16:13:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetImageInputContextGuid method.
 * Added new message 
 * SVM_SET_SHADOWED_ABSEXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   23 Mar 2000 14:03:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  117
 * SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced new SetDisabled() member function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   03 Mar 2000 16:00:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fixed SVM_RESIZING Message definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   03 Mar 2000 12:24:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVM_GET_ABSOLUTE_EXTENT, 
 * SVM_GET_EXTENT Message Comments .
 * Added new Message SVM_SET_SHADOWED_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   Mar 01 2000 12:41:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to make checkDrawAllowed(...) public.
 * Added new messages:
 * SVM_MOVE, SVM_LT_SIZE, SVM_RT_SIZE, SVM_LB_SIZE,
 * SVM_RB_SIZE, SVM_L_SIZE, SVM_R_SIZE, SVM_T_SIZE, SVM_B_SIZE
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Feb 2000 16:07:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added new method GetTransform.
 * Added new message SVM_GET_CONTEXT_EXTENT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   21 Feb 2000 15:37:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added comments for Move/Size messages to detail
 * parameters.
 * Added checkDrawAllowed method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Feb 21 2000 10:45:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Versioning to v3.10 Beta 2.
 * Add new Macro for Type Safe Casting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 17:00:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   07 Feb 2000 20:47:56   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Added new Object function GetObjectIcon() which can be overwritten and be used as generic 'get icon func' for any kind of list or tree icon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:18:40   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   Oct 29 1999 17:59:04   steve
 * 
 *    Rev 1.35   18 Oct 1999 17:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected description of SVMSGID_OBJECT_RENAMED
 * parameter usage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   Oct 18 1999 11:53:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   RO - Bug fix - Added DataLinkID scripting.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   14 Oct 1999 17:00:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added HasDependents method to SVOutputInfoListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   Oct 13 1999 15:14:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Roberts bug fixes for Beta 10.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   Oct 12 1999 08:16:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  44
 * SCR Title:  Version 3.00 Beta 9 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   Oct 07 1999 15:37:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Go Online and Go Offline messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   06 Oct 1999 15:42:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected problem with SV_ALL_ATRIBUTES
 * ( added parentheses)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   05 Oct 1999 17:20:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Attribute flag for Statistics
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   Oct 04 1999 09:52:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Fix for lock while going into run mode and Point scripting fix.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   Sep 27 1999 19:37:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed RESET_ALL_OBJECTS to support friends.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   Sep 24 1999 09:49:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Reset_All_Message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   24 Sep 1999 08:50:56   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  32
 * SCR Title:  Port Blob Analyzer from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed space3s from SV_VIEWABLE|SV_PUBLISHABLE|SV_ARCHIVABLE|SV_SELECTABLE_FOR_EQUATION|SV_EMBEDABLE of #define SV_ALL_ATTRIBUTES.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   23 Sep 1999 16:25:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added message SVM_DESTROY_FRIEND_OBJECT.
 * Revised DestroyFriends to send 
 * SVM_DESTROY_FRIEND_OBJECT instead of
 * SVM_DESTROY_CHILD_OBJECTS
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   Sep 23 1999 13:53:54   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  37
 * SCR Title:  Version 3.00 Beta 3 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New version specific changes to Serialize methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   22 Sep 1999 18:35:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to fix restoration of friends from script
 * (Added method to adjust friend aliases)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   Sep 20 1999 15:36:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new broadcast message router.
 * Added new messages: Create_All_Closed_Objects.
 *      Notify_About_Creation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   Sep 16 1999 12:38:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added Product Key Struct and other from SVPPQ.h.
 * Added GetAncestor(ObjectType) and IsDescendantOf( PAncestorObject)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   Sep 13 1999 13:29:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added message to get input interface list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   Sep 10 1999 17:37:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed SetObjectOwner() to virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   04 Sep 1999 17:24:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to save and restore uObjectAttributesAllowed and
 * uObjectAttributesSet via ObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   02 Sep 1999 08:44:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Add Message Qualifiers to better control routing of messages.
 * Add new messsages for drawing/figures.
 * Fixed Bug in CloseObject (Friends recurrsion)
 * Add published attribute and defaults on construction of SVObjectClass..
 * Added GetObjectColor and GetObjectState
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Aug 1999 19:31:42   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced Input Interface Handling.
 * ( GETFIRST - Message Handler )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Aug 1999 19:15:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SV_IS_KIND_OF not to call SV_VALIDATE_OBJECT.
 * Revised SVM_CREATE_ALL_OBJECTS to route to Children only.
 * Added Scripting support for Freinds Architecure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Aug 1999 12:58:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added SetupDialog() Function to SVObjectClass and Message.
 * This gives every derived class the possibility to support his own Setup Dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Aug 1999 09:06:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Add capability to script freind links.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   Aug 25 1999 22:50:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised Validate method to be OnValidate method.
 *  ( OnValidate - local scope validation).
 * Added Validate method (Routes to all owned objects)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Aug 25 1999 16:42:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SV_ALL_ATTRIBUTES define.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Aug 1999 11:36:12   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added SVObjectInfoArrayClass.
 * Added Friend Mechanism.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Aug 25 1999 10:47:32   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Correct check in file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Aug 20 1999 19:42:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Added attributes for output (results) picker
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 20 1999 16:54:22   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  James A. Brown
 * Change Description:  
 *   Added SVErrorClass  msvError to SVObjectClass.
 * 
 * Changed header comment block to new standard which includes Tracker keywords.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
