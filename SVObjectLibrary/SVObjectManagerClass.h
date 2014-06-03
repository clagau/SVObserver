//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManagerClass
//* .File Name       : $Workfile:   SVObjectManagerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   02 Jun 2014 06:40:22  $
//******************************************************************************

#ifndef SVOBJECTMANAGERCLASS_H
#define SVOBJECTMANAGERCLASS_H

#pragma region Includes
#include <map>
#include <set>
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVMaterialsLibrary/SVMaterials.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVObjectReference.h"
#include "SVObserverNotificationFunctor.h"
#pragma endregion Includes

#pragma region Declarations
class SVObjectClass;
#pragma endregion Declarations

//
// SVObjectManagerClass
//
class SVObjectManagerClass
{
	friend class SVIPDoc;
	friend class SVIODoc;
	friend class SVToolClass;
	friend class SVObserverApp;

public:
	enum SVObjectManagerStateEnum
	{
		Unknown   = 0,
		ReadOnly  = 1,
		ReadWrite = 2,
	};

	enum RootChildObjectEnum
	{
		Root,
		Environment,
		Configuration,
	};
	typedef std::map< RootChildObjectEnum, SVGUID > RootEnumChildMap;
	typedef std::map< SVString, SVGUID > RootNameChildMap;
	typedef std::map< SVString, SVString > TranslateMap;

	typedef std::set< SVGUID > SVObserverIdSet;
	typedef std::deque< SVString > SVSubjectDataNameDeque;

	static SVObjectManagerClass& Instance();

	virtual ~SVObjectManagerClass();

	SVObjectManagerStateEnum GetState() const;
	HRESULT SetState( SVObjectManagerStateEnum p_State );

	const SVGUID GetChildRootObjectID(const RootChildObjectEnum RootChild) const;
	const SVGUID GetChildRootObjectID(const SVString& RootName) const;

	template< typename SVObjectTypeName >
	HRESULT GetRootChildObject( SVObjectTypeName*& rpObject, RootChildObjectEnum RootChild );

	//For backward compatibility
	template< typename SVObjectTypeName >
	HRESULT GetConfigurationObject( SVObjectTypeName*& rpObject );

	HRESULT ConstructRootObject( const SVGUID& rClassID );
	HRESULT DestroyRootObject();
	void setRootChildID(const RootChildObjectEnum RootChild, const SVGUID& rUniqueID);
	void Translation(SVString& Name);

	HRESULT ConstructObject( const SVGUID& rClassID, GUID& rObjectID );
	HRESULT ConstructObject( const SVGUID& rClassID, SVObjectClass*& rpObject );

	template< typename SVObjectTypeName >
	HRESULT ConstructObject( const SVGUID& rClassID, SVObjectTypeName*& rpObject );

	void DestructObject( const SVGUID& rClassID );

	HRESULT GetObjectByIdentifier( const SVGUID& rObjectID, SVObjectClass*& rpObject ) const;

	HRESULT GetObjectByDottedName( const SVString& rFullName, GUID& rObjectID ) const;
	HRESULT GetObjectByDottedName( const SVString& rFullName, SVObjectClass*& rpObject ) const;
	HRESULT GetObjectByDottedName( const SVString& rFullName, SVObjectReference& rReference ) const;

	template< typename SVObjectTypeName >
	HRESULT GetObjectByDottedName( const SVString& rFullName, SVObjectTypeName*& rpObject );

	BOOL CreateUniqueObjectID( SVObjectClass* PObject );
	BOOL OpenUniqueObjectID( SVObjectClass* PObject );
	BOOL CloseUniqueObjectID( SVObjectClass* PObject );
	BOOL ChangeUniqueObjectID( SVObjectClass* PObject, const SVGUID& RNewGuid );

	SVObjectClass* GetObject( const SVGUID& RGuid );
	//SVObjectClass* GetObject( LPCTSTR tszName );
	SVObjectClass* GetObjectCompleteName( LPCTSTR tszName );
	SVObjectReference GetObjectReference( const CString& strName );

	SVGUID GetObjectIdFromCompleteName( const SVString& rName );

	SVString GetCompleteObjectName( const SVGUID& RGuid );

	HRESULT SubmitCommand( const SVGUID& rObjectID, const SVCommandTemplatePtr& rCommandPtr );

	template< typename SVNotifyData >
	HRESULT Notify( const SVGUID& rObjectID, SVNotifyData& rData );

	template< typename ObjectVisitor >
	HRESULT VisitElements( ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID=GUID_NULL );

	HRESULT InsertObserver( SVObserverNotificationFunctorPtr p_FunctorPtr, long& rCookie );
	HRESULT EraseObserver( long p_Cookie );

	HRESULT GetObserverDataNames( long p_Cookie, SVSubjectDataNameDeque& rSubjectDataNames ) const;
	HRESULT GetObserverDataNames( const SVGUID& rObserverID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObserverSubject( const SVString& rSubjectDataName, const SVGUID& rObserverID, GUID& rSubjectID ) const;
	HRESULT GetObserverSubject( const SVString& rSubjectDataName, long p_Cookie, GUID& rSubjectID ) const;

	HRESULT GetObserverIds( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVObserverIdSet& rObserverIds );

	HRESULT AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID, const SVMaterials& rAttributes );
	HRESULT AttachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );

	HRESULT EnableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT EnableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );

	HRESULT DisableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT DisableObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );

	HRESULT DetachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT DetachObserver( const SVString& rSubjectDataName, const SVGUID& rSubjectID, long p_Cookie );
	HRESULT DetachObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID );

	HRESULT DetachSubjectsAndObservers( const SVGUID& rObjectID );

	template< typename SVDataType >
	HRESULT UpdateObserver( const SVGUID& rObserverID, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObserver( long p_Cookie, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, const SVDataType& rData );

	HRESULT DisconnectObjects( const SVGUID& rSource, const SVGUID& rDestination );

	SVIPDoc* GetIPDoc( const SVGUID& rIPGuid ) const;

	HRESULT RegisterIPDoc( const SVGUID& rIPGuid, SVIPDoc* p_pIPDoc );
	HRESULT UnregisterIPDoc( const SVGUID& rIPGuid );

	SVIODoc* GetIODoc( const SVGUID& rIPGuid ) const;

	HRESULT RegisterIODoc( const SVGUID& rIOGuid, SVIODoc* p_pIODoc );
	HRESULT UnregisterIODoc( const SVGUID& rIOGuid );

	long GetShortPPQIndicator() const;
	void ClearShortPPQIndicator();
	void IncrementShortPPQIndicator();
	void DecrementShortPPQIndicator();

	long GetProductIndicator() const;
	void ClearProductIndicator();
	void IncrementProductIndicator();
	void DecrementProductIndicator();
	void AdjustProductIndicator( long p_Amount );

	long GetPendingImageIndicator() const;
	void ClearPendingImageIndicator();
	void IncrementPendingImageIndicator();
	void DecrementPendingImageIndicator();
	void AdjustPendingImageIndicator( long p_Amount );

	long GetInspectionIndicator() const;
	void ClearInspectionIndicator();
	void IncrementInspectionIndicator();
	void DecrementInspectionIndicator();
	void AdjustInspectionIndicator( long p_Amount );

	long GetNextFrameRate( long p_LastFrameRate );

	long GetFileSequenceNumber() const;

	bool m_bIsColorSVIM;

protected:
	typedef std::map< SVString, SVGUID > SVSubjectDataNameSubjectIDMap;

	struct SVCookieEntryStruct
	{
		long m_Cookie;
		SVObserverNotificationFunctorPtr m_FunctorPtr;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;

		SVCookieEntryStruct()
		: m_Cookie( 0 ), m_FunctorPtr( NULL ), m_SubjectIDs() {}

		SVCookieEntryStruct( long p_Cookie, SVObserverNotificationFunctorPtr p_FunctorPtr )
		: m_Cookie( p_Cookie ), m_FunctorPtr( p_FunctorPtr ), m_SubjectIDs() {}
	};

	typedef SVSharedPtr< SVCookieEntryStruct > SVCookieEntryStructPtr;
	typedef std::map< long, SVCookieEntryStructPtr > SVCookieEntryMap;

	typedef std::set< SVGUID > SVSubjectObserverSet;
	typedef std::set< long > SVSubjectCookieSet;

	typedef std::map< SVGUID, long > SVSubjectEnabledObserverMap;
	typedef std::map< long, long > SVSubjectEnabledCookieMap;

	struct SVSubjectObserverStruct
	{
		SVSubjectEnabledObserverMap m_SubjectObservers;
		SVSubjectEnabledCookieMap m_SubjectCookies;

		SVSubjectObserverStruct()
			: m_SubjectObservers(), m_SubjectCookies() {}
	};

	typedef std::map< SVString, SVSubjectObserverStruct > SVSubjectDataNameObserverMap;

	struct SVUniqueObjectEntryStruct
	{
		SVGUID ObjectUID;
		SVObjectClass* PObject;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;
		SVSubjectDataNameObserverMap m_DataNameSubjectObservers;

		SVUniqueObjectEntryStruct()
		: ObjectUID(), PObject( NULL ), m_SubjectIDs(), m_DataNameSubjectObservers() {}
	};

	typedef SVSharedPtr< SVUniqueObjectEntryStruct > SVUniqueObjectEntryStructPtr;
	typedef std::map< GUID, SVUniqueObjectEntryStructPtr > SVUniqueObjectEntryMap;
	typedef std::map< GUID, SVIPDoc* > SVIPDocMap;
	typedef std::map< GUID, SVIODoc* > SVIODocMap;

	SVObjectManagerClass();

	void Shutdown();

	HRESULT GetSubjectDataNames( const SVGUID& rSubjectID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers );
	HRESULT GetObservers( const SVString& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers, SVSubjectEnabledCookieMap& rObserverCookies );

	SVCookieEntryStructPtr GetCookieEntry( long p_Cookie ) const;

	SVGUID GetSubjectID( const SVString& rSubjectDataName, SVUniqueObjectEntryStructPtr p_ObjectEntryPtr ) const;
	SVGUID GetSubjectID( const SVString& rSubjectDataName, SVCookieEntryStructPtr p_CookieEntryPtr ) const;

	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const SVGUID& RGuid ) const;
	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const CString& sName ) const;
	SVUniqueObjectEntryStructPtr getUniqueObjectEntryCompleteObjectName( const CString& sName ) const;

	HRESULT DetachSubjects( long p_Cookie );
	HRESULT DetachSubjects( const SVGUID& rObserverID );

	HRESULT DetachObservers( const SVGUID& rSubjectID );

	long m_State;

	mutable SVCriticalSection m_Lock;

	SVCookieEntryMap		m_CookieEntries;
	SVUniqueObjectEntryMap	m_UniqueObjectEntries;
	SVIPDocMap				m_IPDocs;
	SVIODocMap				m_IODocs;
	RootEnumChildMap		m_RootEnumChildren;
	RootNameChildMap		m_RootNameChildren;
	TranslateMap			m_TranslationMap;

	long m_ShortPPQIndicator;
	long m_ProductIndicator;
	long m_PendingImageIndicator;
	long m_InspectionIndicator;

	long m_LastFrameRate;

	long m_ObserverCookie;

	long m_FileSequenceNumber;

private:
	//Do not implement this method
	SVObjectManagerClass( const SVObjectManagerClass& rObject );

	//Do not implement this method
	const SVObjectManagerClass& operator=( const SVObjectManagerClass& rObject );
};

#include "SVObjectManagerClass.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObjectLibrary\SVObjectManagerClass.h_v  $
 * 
 *    Rev 1.3   02 Jun 2014 06:40:22   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed m_OnlineDisplay and its Get and Set methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Mar 2014 14:18:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed that a root object is created as the top object instead of a configuration object
 * Interface to insert Root child objects into ObjectManager (eg. Configuration and Environment objects)
 * Access to these objects via GUIDS and name.
 * Translation function to map Fully Qualified Names to their internal names
 * Method to access Configuration object
 * Changed parameter names to follow guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2014 16:15:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 * Changed parameter names to follow guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 16:57:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   11 Feb 2013 12:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update object manager to fix issues with subject/observer pattern functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   16 Jan 2013 15:55:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  Updated type of color flag and initialized it to false.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   01 Aug 2012 11:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   30 Jul 2012 12:32:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to change the functionality for the object visitor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   09 Jul 2012 13:51:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove display attributes and SVIM State functoinality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   02 Jul 2012 16:02:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   18 Jun 2012 17:31:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 May 2012 19:00:30   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Merge 5.43 changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Feb 2012 09:25:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Jan 2012 10:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to reduce unnecessary methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jan 2012 15:04:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to include the new notification functionality to managed object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.0   14 May 2012 13:36:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functionallity for display update rate and display timeout feature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Sep 2011 14:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a new method to adjust the processing indicator variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   05 Jul 2011 14:38:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to include a file sequence number used for the offline log files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Apr 2011 07:57:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Feb 2011 11:59:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Object manager to use new Critical Section class and updated Lockable Class to remove unused attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Feb 2011 14:42:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to allow for high speed multi-threaded access of the object manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Dec 2010 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Oct 2010 09:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Jun 2010 08:29:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 12:55:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jul 2009 08:35:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2007 13:27:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jul 2005 15:49:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVObjectReference (get by name)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:58:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   02 Jun 2003 15:06:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   22 Apr 2003 11:49:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   03 Jan 2003 15:29:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new methods for getting the correct object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   20 Nov 2002 09:37:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added #include lockable.h
 * Added friend class SVIPDoc
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   07 Aug 2001 15:14:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  208
 * SCR Title:  Improve configuration load time
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed TheObjectManager's internal object list from a array to a map to allow for quicker lookups
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 14:59:40   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:21:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Sep 20 1999 15:40:48   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new Broadcast Message Router.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
