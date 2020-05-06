//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManagerClass
//* .File Name       : $Workfile:   SVObjectManagerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   13 Nov 2014 10:01:18  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <memory>

#include "SVClsids.h"
#include "SVObjectReference.h"
#include "SVObserverNotificationFunctor.h"
#include "SVObjectClass.h"
#include "JoinType.h"

#pragma endregion Includes


#pragma region Declarations
class SVObjectClass;

#pragma endregion Declarations

class SVObjectManagerClass
{
public:
	enum SVObjectManagerStateEnum
	{
		Unknown   = 0,
		ReadOnly  = 1,
		ReadWrite = 2,
	};

	typedef std::map<std::string, uint32_t> RootNameChildMap;

	typedef std::deque<std::string> SVSubjectDataNameDeque;

	static SVObjectManagerClass& Instance();

	virtual ~SVObjectManagerClass();

	
	SVObjectManagerClass(const SVObjectManagerClass& rObject) = delete;
	const SVObjectManagerClass& operator=(const SVObjectManagerClass& rObject) = delete;
	
	SVObjectManagerStateEnum GetState() const;
	HRESULT SetState( SVObjectManagerStateEnum State );

	uint32_t GetChildRootObjectID( const std::string& rRootChild ) const;

	template< typename SVObjectTypeName >
	HRESULT GetRootChildObject( SVObjectTypeName*& rpObject, const std::string& rRootChild );

	//For backward compatibility
	template< typename SVObjectTypeName >
	HRESULT GetConfigurationObject( SVObjectTypeName*& rpObject );

	HRESULT ConstructRootObject(SvPb::ClassIdEnum classID);
	HRESULT DestroyRootObject();
	void setRootChildID(const SVObjectClass* pObject);

	//! Translates the dotted name if the name start needs to be replaced
	//! \param rName [in][out] dotted name to translate is done in place
	void TranslateDottedName(std::string& rName) const;

	HRESULT ConstructObject(SvPb::ClassIdEnum classID, uint32_t& rObjectID);
	HRESULT ConstructObject(SvPb::ClassIdEnum classID, SVObjectClass*& rpObject);

	template< typename SVObjectTypeName >
	HRESULT ConstructObject(SvPb::ClassIdEnum classID, SVObjectTypeName*& rpObject);

	HRESULT GetObjectByIdentifier(uint32_t objectID, SVObjectClass*& rpObject ) const;

	HRESULT GetObjectByDottedName( const std::string& rFullName, uint32_t& rObjectID ) const;
	HRESULT GetObjectByDottedName( const std::string& rFullName, SVObjectClass*& rpObject ) const;
	HRESULT GetObjectByDottedName( const std::string& rFullName, SVObjectReference& rObjectRef ) const;

	template< typename SVObjectTypeName >
	HRESULT GetObjectByDottedName( const std::string& rFullName, SVObjectTypeName*& rpObject );

	bool CreateObjectID( SVObjectClass* pObject );
	uint32_t getNextObjectId();

	/// Return the current next objectId without increase the number.
	/// \returns uint32_t
	uint32_t getCurrentNextObjectId() { return m_nextObjectId; };
	void fitFirstObjectId() { m_firstObjectId = m_nextObjectId + 1; }
	void fitNextObjectId(uint32_t usedObjectId);
	/// If the removedObjectId the highest used objectId, reduce NextObjectId.
	/// \param removedObjectId [in]
	void reduceNextObjectId(uint32_t removedObjectId);
	void setDeletedFlag(bool flag) { m_addToDeletedList = flag; };
	void resetNextObjectId();
	bool OpenUniqueObjectID( SVObjectClass* pObject );
	bool CloseUniqueObjectID( SVObjectClass* pObject );
	bool ChangeUniqueObjectID( SVObjectClass* pObject, uint32_t newObjectId );

	SVObjectClass* GetObject(uint32_t objectId) const;
	SVObjectClass* GetObjectCompleteName( LPCTSTR Name );
	SVObjectReference GetObjectReference( LPCTSTR Name );

	uint32_t GetObjectIdFromCompleteName( LPCTSTR Name );

	void getObjectsOfType(SVObjectPtrVectorInserter Inserter, SvPb::SVObjectTypeEnum ObjectType, SvPb::SVObjectSubTypeEnum ObjectSubType= SvPb::SVNotSetSubObjectType) const;

	std::string GetCompleteObjectName(uint32_t objectId);

	template< typename ObjectVisitor >
	HRESULT VisitElements( ObjectVisitor& rVisitor, uint32_t startingObjectID=InvalidObjectId );

	HRESULT InsertObserver( SVObserverNotificationFunctorPtr pFunctor, long& rCookie );
	HRESULT EraseObserver( long p_Cookie );

	HRESULT GetObserverDataNames( long Cookie, SVSubjectDataNameDeque& rSubjectDataNames ) const;
	HRESULT GetObserverDataNames(uint32_t observerID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	uint32_t getObserverSubject( const std::string& rSubjectDataName, uint32_t observerID ) const;
	uint32_t getObserverSubject( const std::string& rSubjectDataName, long Cookie ) const;

	HRESULT GetObserverIds( const std::string& rSubjectDataName, uint32_t subjectID, std::set<uint32_t>& rObserverIds );

	HRESULT AttachObserver( const std::string& rSubjectDataName, uint32_t subjectID, uint32_t observerID );
	HRESULT AttachObserver( const std::string& rSubjectDataName, uint32_t subjectID, long Cookie );

	HRESULT EnableObserver( const std::string& rSubjectDataName, uint32_t subjectID, uint32_t observerID );
	HRESULT EnableObserver( const std::string& rSubjectDataName, uint32_t subjectID, long Cookie );

	HRESULT DisableObserver( const std::string& rSubjectDataName, uint32_t subjectID, uint32_t observerID );
	HRESULT DisableObserver( const std::string& rSubjectDataName, uint32_t subjectID, long Cookie );

	HRESULT DetachObserver( const std::string& rSubjectDataName, uint32_t subjectID, uint32_t observerID );
	HRESULT DetachObserver( const std::string& rSubjectDataName, uint32_t subjectID, long Cookie );
	HRESULT DetachObservers( const std::string& rSubjectDataName, uint32_t subjectID );

	HRESULT DetachSubjectsAndObservers( uint32_t objectID );

	template< typename SVDataType >
	HRESULT UpdateObserver( uint32_t observerID, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObserver( long Cookie, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObservers( const std::string& rSubjectDataName, uint32_t subjectID, const SVDataType& rData );

	HRESULT DisconnectObjects( uint32_t source, uint32_t destination );

	///Resets all the indicators, which are used to determine if the display should be updated
	void ClearAllIndicator();

	long GetShortPPQIndicator() const;
	void ClearShortPPQIndicator();
	void IncrementShortPPQIndicator();
	void DecrementShortPPQIndicator();

	long GetProductIndicator() const;
	void ClearProductIndicator();
	void IncrementProductIndicator();
	void DecrementProductIndicator();
	void AdjustProductIndicator( long Amount );

	long GetPendingImageIndicator() const;
	void ClearPendingImageIndicator();
	void IncrementPendingImageIndicator();
	void DecrementPendingImageIndicator();
	void AdjustPendingImageIndicator( long Amount );

	long GetInspectionIndicator() const;
	void ClearInspectionIndicator();
	void IncrementInspectionIndicator();
	void DecrementInspectionIndicator();
	void AdjustInspectionIndicator( long Amount );

	long GetNextFrameRate( long LastFrameRate );

	long GetFileSequenceNumber() const;

	HRESULT getTreeList(const std::string& rPath, SVObjectReferenceVector& rObjectList, UINT AttributesAllowedFilter) const;

	// This method, Shutdown, is only meant to be called by the main application class and no other
	// It used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call this method anywhere else, as described via this comment
	void Shutdown();	// This method is only meant to be called by the main application class

	//! Connects two objects with a certain join type
	//! \param rSource [in] reference to the source object ID
	//! \param rDestination [in] reference to the destination object ID
	//! \param Type [in] the dependency type
	//! \returns S_OK on success
	HRESULT connectDependency( uint32_t source, uint32_t destination, SvOl::JoinType Type );

	//! Disconnects two objects with a certain join type
	//! \param rSource [in] reference to the source object ID
	//! \param rDestination [in] reference to the destination object ID
	//! \param Type [in] the dependency type
	//! \returns S_OK on success
	HRESULT disconnectDependency( uint32_t source, uint32_t destination, SvOl::JoinType Type );

	HRESULT RegisterSubObject(uint32_t subObjectID );
	HRESULT UnregisterSubObject(uint32_t subObjectID);

	// Return the first object which the required object type.
	//! \param rSourceId [in] reference to the source object ID
	/// \param rObjectTypeInfo [in] The required object type.
	/// \returns IObjectClass* Pointer to the object, if not found nullptr.
	SvOi::IObjectClass* getFirstObject(uint32_t sourceId, const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const;

	/// Connect inputs to a object
	/// \param rSourceId [in] reference to the source object ID
	/// \param pObjectInInfo [in] object input info
	/// \returns bool
	bool ConnectObjectInput( uint32_t sourceId, SvOl::SVInObjectInfoStruct* pObjectInInfo );

	/// Disconnect input from a object
	/// \param rSourceId [in] reference to the source object ID
	/// \param pObjectInInfo [in] object input info
	/// \returns bool
	bool DisconnectObjectInput( uint32_t sourceId, SvOl::SVInObjectInfoStruct* pObjectInInfo );

	///return Mutex  
	std::recursive_mutex* GetMutex() { return  &m_Mutex; }


protected:
	typedef std::map<std::string, uint32_t> SVSubjectDataNameSubjectIDMap;

	struct SVCookieEntryStruct
	{
		long m_Cookie;
		SVObserverNotificationFunctorPtr m_pFunctor;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;

		SVCookieEntryStruct()
		: m_Cookie( 0 ), m_pFunctor( nullptr ), m_SubjectIDs() {}

		SVCookieEntryStruct( long Cookie, SVObserverNotificationFunctorPtr pFunctor )
		: m_Cookie( Cookie ), m_pFunctor( pFunctor ), m_SubjectIDs() {}
	};

	typedef std::shared_ptr< SVCookieEntryStruct > SVCookieEntryStructPtr;
	typedef std::map< long, SVCookieEntryStructPtr > SVCookieEntryMap;

	typedef std::set< long > SVSubjectCookieSet;

	typedef std::map< uint32_t, long > SVSubjectEnabledObserverMap;
	typedef std::map< long, long > SVSubjectEnabledCookieMap;

	struct SVSubjectObserverStruct
	{
		SVSubjectEnabledObserverMap m_SubjectObservers;
		SVSubjectEnabledCookieMap m_SubjectCookies;

		SVSubjectObserverStruct()
			: m_SubjectObservers(), m_SubjectCookies() {}
	};

	typedef std::map<std::string, SVSubjectObserverStruct> SVSubjectDataNameObserverMap;

	struct SVUniqueObjectEntryStruct
	{
		int32_t m_ObjectID = 0;
		SVObjectClass* m_pObject;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;
		SVSubjectDataNameObserverMap m_DataNameSubjectObservers;

		SVUniqueObjectEntryStruct()
		: m_pObject( nullptr ), m_SubjectIDs(), m_DataNameSubjectObservers() {}
	};

	typedef std::shared_ptr< SVUniqueObjectEntryStruct > SVUniqueObjectEntryStructPtr;
	typedef std::unordered_map< uint32_t, SVUniqueObjectEntryStructPtr > SVUniqueObjectEntryMap;

	SVObjectManagerClass();

	HRESULT GetSubjectDataNames( uint32_t subjectID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObservers( const std::string& rSubjectDataName, uint32_t subjectID, SVSubjectEnabledObserverMap& rObservers );
	HRESULT GetObservers( const std::string& rSubjectDataName, uint32_t subjectID, SVSubjectEnabledObserverMap& rObservers, SVSubjectEnabledCookieMap& rObserverCookies );

	SVCookieEntryStructPtr GetCookieEntry( long Cookie ) const;

	uint32_t GetSubjectID( const std::string& rSubjectDataName, SVUniqueObjectEntryStructPtr pObjectEntry ) const;
	uint32_t GetSubjectID( const std::string& rSubjectDataName, SVCookieEntryStructPtr pCookieEntry ) const;

	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( uint32_t objectId ) const;
	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const std::string& rName ) const;

	HRESULT DetachSubjects( long Cookie );
	HRESULT DetachSubjects( uint32_t observerID );

	HRESULT DetachObservers( uint32_t subjectID );

	long m_State;

	//@Todo[MEC][8.20] [15.05.2019] probaly after some code cleanup std::mutex would be enough  
	mutable std::recursive_mutex m_Mutex;
	SVCookieEntryMap		m_CookieEntries;
	SVUniqueObjectEntryMap	m_UniqueObjectEntries;
	std::set<uint32_t>		m_deletedObjectIdSet;
	//@TODO[gra][10.00][06,05,2020]: Needed to set this to false as it caused problems with certain tests
	bool					m_addToDeletedList = false;
	RootNameChildMap		m_RootNameChildren;
	SvDef::TranslateMap		m_TranslationMap;

	long m_ShortPPQIndicator;
	long m_ProductIndicator;
	long m_PendingImageIndicator;
	long m_InspectionIndicator;

	long m_LastFrameRate;

	long m_ObserverCookie;

	long m_FileSequenceNumber;
	uint32_t m_nextObjectId = ObjectIdEnum::FirstPossibleObjectId;	
	uint32_t m_firstObjectId = ObjectIdEnum::FirstPossibleObjectId;
};

#include "SVObjectManagerClass.inl"

