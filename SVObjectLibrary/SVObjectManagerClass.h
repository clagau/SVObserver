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
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"
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

	typedef std::map<std::string, SVGUID> RootNameChildMap;

	typedef std::deque<std::string> SVSubjectDataNameDeque;

	static SVObjectManagerClass& Instance();

	virtual ~SVObjectManagerClass();

	SVObjectManagerStateEnum GetState() const;
	HRESULT SetState( SVObjectManagerStateEnum State );

	const SVGUID GetChildRootObjectID( const std::string& rRootChild ) const;

	template< typename SVObjectTypeName >
	HRESULT GetRootChildObject( SVObjectTypeName*& rpObject, const std::string& rRootChild );

	//For backward compatibility
	template< typename SVObjectTypeName >
	HRESULT GetConfigurationObject( SVObjectTypeName*& rpObject );

	HRESULT ConstructRootObject( const SVGUID& rClassID );
	HRESULT DestroyRootObject();
	void setRootChildID(const std::string& rRootChild, const SVGUID& rUniqueID);

	//! Translates the dotted name if the name start needs to be replaced
	//! \param rName [in][out] dotted name to translate is done in place
	void TranslateDottedName(std::string& rName) const;

	HRESULT ConstructObject( const SVGUID& rClassID, GUID& rObjectID );
	HRESULT ConstructObject( const SVGUID& rClassID, SVObjectClass*& rpObject );

	template< typename SVObjectTypeName >
	HRESULT ConstructObject( const SVGUID& rClassID, SVObjectTypeName*& rpObject );

	HRESULT GetObjectByIdentifier( const SVGUID& rObjectID, SVObjectClass*& rpObject ) const;

	HRESULT GetObjectByDottedName( const std::string& rFullName, GUID& rObjectID ) const;
	HRESULT GetObjectByDottedName( const std::string& rFullName, SVObjectClass*& rpObject ) const;
	HRESULT GetObjectByDottedName( const std::string& rFullName, SVObjectReference& rObjectRef ) const;

	template< typename SVObjectTypeName >
	HRESULT GetObjectByDottedName( const std::string& rFullName, SVObjectTypeName*& rpObject );

	bool CreateUniqueObjectID( SVObjectClass* pObject );
	bool OpenUniqueObjectID( SVObjectClass* pObject );
	bool CloseUniqueObjectID( SVObjectClass* pObject );
	bool ChangeUniqueObjectID( SVObjectClass* pObject, const SVGUID& rNewGuid );

	SVObjectClass* GetObject( const SVGUID& rGuid ) const;
	SVObjectClass* GetObjectCompleteName( LPCTSTR Name );
	SVObjectReference GetObjectReference( LPCTSTR Name );

	SVGUID GetObjectIdFromCompleteName( LPCTSTR Name );

	void getObjectsOfType(SVObjectPtrVectorInserter Inserter, SvDef::SVObjectTypeEnum ObjectType, SvDef::SVObjectSubTypeEnum ObjectSubType=SvDef::SVNotSetSubObjectType) const;

	std::string GetCompleteObjectName( const SVGUID& rGuid );

	HRESULT SubmitCommand( const SVGUID& rObjectID, const SVCommandTemplatePtr& rCommandPtr );
	HRESULT SubmitCommand( SVObjectClass& rObject, const SVCommandTemplatePtr& rCommandPtr );

	template< typename SVNotifyData >
	HRESULT Notify( const SVGUID& rObjectID, SVNotifyData& rData );

	template< typename ObjectVisitor >
	HRESULT VisitElements( ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID=GUID_NULL );

	HRESULT InsertObserver( SVObserverNotificationFunctorPtr pFunctor, long& rCookie );
	HRESULT EraseObserver( long p_Cookie );

	HRESULT GetObserverDataNames( long Cookie, SVSubjectDataNameDeque& rSubjectDataNames ) const;
	HRESULT GetObserverDataNames( const SVGUID& rObserverID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObserverSubject( const std::string& rSubjectDataName, const SVGUID& rObserverID, GUID& rSubjectID ) const;
	HRESULT GetObserverSubject( const std::string& rSubjectDataName, long Cookie, GUID& rSubjectID ) const;

	HRESULT GetObserverIds( const std::string& rSubjectDataName, const SVGUID& rSubjectID, SVGuidSet& rObserverIds );

	HRESULT AttachObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT AttachObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie );

	HRESULT EnableObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT EnableObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie );

	HRESULT DisableObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT DisableObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie );

	HRESULT DetachObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVGUID& rObserverID );
	HRESULT DetachObserver( const std::string& rSubjectDataName, const SVGUID& rSubjectID, long Cookie );
	HRESULT DetachObservers( const std::string& rSubjectDataName, const SVGUID& rSubjectID );

	HRESULT DetachSubjectsAndObservers( const SVGUID& rObjectID );

	template< typename SVDataType >
	HRESULT UpdateObserver( const SVGUID& rObserverID, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObserver( long Cookie, const SVDataType& rData );

	template< typename SVDataType >
	HRESULT UpdateObservers( const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVDataType& rData );

	HRESULT DisconnectObjects( const SVGUID& rSource, const SVGUID& rDestination );

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
	HRESULT connectDependency( const SVGUID& rSource, const SVGUID& rDestination, SvOl::JoinType Type );

	//! Disconnects two objects with a certain join type
	//! \param rSource [in] reference to the source object ID
	//! \param rDestination [in] reference to the destination object ID
	//! \param Type [in] the dependency type
	//! \returns S_OK on success
	HRESULT disconnectDependency( const SVGUID& rSource, const SVGUID& rDestination, SvOl::JoinType Type );

	HRESULT RegisterSubObject( const SVGUID& rSubObjectID );
	HRESULT UnregisterSubObject( const SVGUID& rSubObjectID );

	// Return the first object which the required object type.
	//! \param rSourceId [in] reference to the source object ID
	/// \param rObjectTypeInfo [in] The required object type.
	/// \returns IObjectClass* Pointer to the object, if not found nullptr.
	SvOi::IObjectClass* getFirstObject(const SVGUID& rSourceId, const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo) const;

	/// Connect inputs to a object
	/// \param rSourceId [in] reference to the source object ID
	/// \param pObjectInInfo [in] object input info
	/// \returns bool
	bool ConnectObjectInput( const SVGUID& rSourceId, SVInObjectInfoStruct* pObjectInInfo );

	/// Disconnect input from a object
	/// \param rSourceId [in] reference to the source object ID
	/// \param pObjectInInfo [in] object input info
	/// \returns bool
	bool DisconnectObjectInput( const SVGUID& rSourceId, SVInObjectInfoStruct* pObjectInInfo );

	//! Create Bucket for value object
	//! \param rpBucket [in/out] reference to the Bucket shared pointer
	//! \param RDefault [in] reference to the default value
	//! \param NumberOfBuckets [in] the number of buckets
	//! \returns bool
	template<typename DataType>
	bool createBucket( std::unique_ptr<std::vector<DataType>>& rpBucket, const DataType& rDefault, int NumberOfBuckets );

protected:
	typedef std::map<std::string, SVGUID> SVSubjectDataNameSubjectIDMap;

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

	typedef std::map< SVGUID, long > SVSubjectEnabledObserverMap;
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
		SVGUID m_ObjectUID;
		SVObjectClass* m_pObject;

		SVSubjectDataNameSubjectIDMap m_SubjectIDs;
		SVSubjectDataNameObserverMap m_DataNameSubjectObservers;

		SVUniqueObjectEntryStruct()
		: m_ObjectUID(), m_pObject( nullptr ), m_SubjectIDs(), m_DataNameSubjectObservers() {}
	};

	typedef std::shared_ptr< SVUniqueObjectEntryStruct > SVUniqueObjectEntryStructPtr;
	typedef std::map< GUID, SVUniqueObjectEntryStructPtr > SVUniqueObjectEntryMap;

	SVObjectManagerClass();

	HRESULT GetSubjectDataNames( const SVGUID& rSubjectID, SVSubjectDataNameDeque& rSubjectDataNames ) const;

	HRESULT GetObservers( const std::string& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers );
	HRESULT GetObservers( const std::string& rSubjectDataName, const SVGUID& rSubjectID, SVSubjectEnabledObserverMap& rObservers, SVSubjectEnabledCookieMap& rObserverCookies );

	SVCookieEntryStructPtr GetCookieEntry( long Cookie ) const;

	SVGUID GetSubjectID( const std::string& rSubjectDataName, SVUniqueObjectEntryStructPtr pObjectEntry ) const;
	SVGUID GetSubjectID( const std::string& rSubjectDataName, SVCookieEntryStructPtr pCookieEntry ) const;

	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const SVGUID& rGuid ) const;
	SVUniqueObjectEntryStructPtr getUniqueObjectEntry( const std::string& rName ) const;

	HRESULT DetachSubjects( long Cookie );
	HRESULT DetachSubjects( const SVGUID& rObserverID );

	HRESULT DetachObservers( const SVGUID& rSubjectID );

	long m_State;

	mutable SVCriticalSection m_Lock;

	SVCookieEntryMap		m_CookieEntries;
	SVUniqueObjectEntryMap	m_UniqueObjectEntries;
	RootNameChildMap		m_RootNameChildren;
	SvDef::TranslateMap			m_TranslationMap;

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

