//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectClass.h"
#include "JoinType.h"

#pragma endregion Includes


#pragma region Declarations
class SVObjectClass;

#pragma endregion Declarations

namespace ObIds
{
 constexpr uint32_t   WarningRegion   =  static_cast<uint32_t>( 0.9 * UINT_MAX);
 }

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

	
	void CheckObjectId(uint32_t objectId);
	
	/// Return the current next objectId without increase the number.
	/// \returns uint32_t
	uint32_t getCurrentNextObjectId() { return m_nextObjectId; };
	
	void fitNextObjectId(uint32_t usedObjectId);
	uint32_t   getGreatestUsedObjectId();
	
	void resetNextObjectId();
	bool OpenUniqueObjectID( SVObjectClass& rObject );
	bool CloseUniqueObjectID(const SVObjectClass& rObject );
	bool ChangeUniqueObjectID( SVObjectClass* pObject, uint32_t newObjectId );
	bool SwapUniqueObjectID(SVObjectClass& rFirstObject, SVObjectClass& rSecondObject);

	SVObjectClass* GetObject(uint32_t objectId) const;
	SVObjectClass* GetObjectCompleteName( LPCTSTR Name );
	SVObjectReference GetObjectReference( LPCTSTR Name );

	uint32_t GetObjectIdFromCompleteName( LPCTSTR Name );

	void getObjectsOfType(SVObjectPtrVectorInserter Inserter, SvPb::SVObjectTypeEnum ObjectType, SvPb::SVObjectSubTypeEnum ObjectSubType= SvPb::SVNotSetSubObjectType) const;

	std::string GetCompleteObjectName(uint32_t objectId);

	void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, uint32_t startingObjectID, bool addHidden = false, bool stopIfClosed = false);

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

	long GetPendingImageIndicator() const;
	void ClearPendingImageIndicator();
	void IncrementPendingImageIndicator();
	void DecrementPendingImageIndicator();
	void AdjustPendingImageIndicator( long Amount );

	long GetInspectionIndicator() const;
	void ClearInspectionIndicator();
	void IncrementInspectionIndicator();
	void DecrementInspectionIndicator();

	long GetNextFrameRate( long LastFrameRate );

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

	/// Used to output all objects listed in the object manager
	void listAllObjects();

	///return Mutex  
	std::recursive_mutex* GetMutex() { return  &m_Mutex; }


protected:
	SVObjectManagerClass();

	SVObjectClass* getUniqueObjectEntry( uint32_t objectId ) const;
	SVObjectClass* getUniqueObjectEntry( const std::string& rName ) const;

	std::tuple<HRESULT, SVObjectClass*, SVObjectNameInfo> getObjectByDottedName(const std::string& rFullName) const;

	long m_State;

	//@Todo[MEC][8.20] [15.05.2019] probaly after some code cleanup std::mutex would be enough  
	mutable std::recursive_mutex m_Mutex;

	typedef std::unordered_map< uint32_t, SVObjectClass*> SVUniqueObjectEntryMap;
	SVUniqueObjectEntryMap	m_UniqueObjectEntries;
	
	RootNameChildMap		m_RootNameChildren;
	SvDef::TranslateMap		m_TranslationMap;

	long m_ShortPPQIndicator;
	long m_ProductIndicator;
	long m_PendingImageIndicator;
	long m_InspectionIndicator;

	long m_LastFrameRate;

	std::atomic<uint32_t> m_nextObjectId = ObjectIdEnum::FirstPossibleObjectId;	
	bool m_AlreadyWarnedObjectIds {false};

	
};

std::string getCompleteObjectNameForId(uint32_t ownerId);


#include "SVObjectManagerClass.inl"

