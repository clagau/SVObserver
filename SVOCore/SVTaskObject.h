//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObject
//* .File Name       : $Workfile:   SVTaskObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   17 Jul 2014 20:39:34  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "ObjectInterfaces/ITaskObject.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVObjectLibrary/SVInputInfoListClass.h"
#include "SVObjectAppClass.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "SVValueObjectLibrary/SVValueObject.h"
#include "SVObjectLibrary/SVObjectListClass.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "ObjectSelectorLibrary/SelectorItemVector.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVImageListClass.h"
#include "SVExtentPropertiesInfoStruct.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVImageClass;

typedef std::set<SVImageClass *> SVImageClassPtrSet;

enum ValidationLevelEnum
{
	AllParameters,					// level 3
	RemotelyAndInspectionSettable,  // level 2
	InspectionSettable				// level 1
};

class SVTaskObjectClass : public SVObjectAppClass, public SvOi::ITaskObject
{
	SV_DECLARE_CLASS( SVTaskObjectClass )

	friend class SVTaskObjectListClass; // For access to Run()
	friend class SVToolSetClass; // For access to Run()

public:
	SVTaskObjectClass( LPCSTR LPSZObjectName );
	SVTaskObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECT );

	virtual ~SVTaskObjectClass();

	virtual bool resetAllObjects( bool shouldNotifyFriends, bool silentReset ) override;
	virtual HRESULT ResetObject() override;

	virtual HRESULT IsInputImage( SVImageClass* p_psvImage );

	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const;
	virtual HRESULT GetImageExtent( SVImageExtentClass& p_rsvImageExtent );
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList );
	virtual HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;


	void ResetPrivateInputInterface();
	
	virtual bool ConnectAllInputs() override;
	HRESULT ConnectToObject( SVInObjectInfoStruct* p_psvInputInfo, SVObjectClass* pNewObject );

	virtual BOOL IsValid() override;

	// Routing Version of Validate
	// Validates Local Scope and all owned objects
	virtual BOOL Validate() override;

	// Non Routing Version of Validate
	// validates only Local Scope
	virtual BOOL OnValidate() override;

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct ) override;
	virtual BOOL CloseObject() override;
	void Disconnect();
	virtual bool DisconnectObjectInput(SVInObjectInfoStruct* pInObjectInfo) override;
	virtual HRESULT GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const;
	virtual HRESULT DisconnectInputsOutputs(SVObjectVector& rListOfObjects);
	virtual HRESULT HideInputsOutputs(SVObjectVector& rListOfObjects);

	virtual BOOL SetObjectDepth( int NewObjectDepth ) override;
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual BOOL SetImageDepth( long lDepth ) override;

	virtual void SetInvalid() override;
	virtual void SetDisabled() override;

	BOOL RegisterEmbeddedObject( SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID );
	BOOL RegisterEmbeddedObject( SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR newString );
	BOOL RegisterEmbeddedObject( SVValueObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID, bool p_bResetAlways, SVResetItemEnum p_eRequiredReset, LPCTSTR p_pszTypeName = nullptr );
	BOOL RegisterEmbeddedObject( SVValueObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR strName, bool p_bResetAlways, SVResetItemEnum p_eRequiredReset, LPCTSTR p_pszTypeName = nullptr );
	BOOL RegisterInputObject( SVInObjectInfoStruct* PInObjectInfo, const SVString& p_rInputName );

	HRESULT GetOutputListFiltered(std::vector<SVValueObjectReference>& rvecObjects, UINT uiAttributes = SV_NO_ATTRIBUTES, bool bAND = true ); /* true means AND, false means OR */

	HRESULT GetNonToolsetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const;

	virtual HRESULT IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo ); 

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	HRESULT FindNextInputImageInfo(SVInObjectInfoStruct*& p_rpsvFoundInfo, const SVInObjectInfoStruct* p_psvLastInfo = nullptr);
	
	//************************************
	//! Adds a task message to the list
	//! \param rMessage [in] reference to the message container
	//************************************
	void addTaskMessage( const SvStl::MessageContainer& rMessage ) { m_TaskMessages.push_back( rMessage ); };

	//************************************
	//! Clears the task set message list 
	//************************************
	void clearTaskMessages( ) { m_TaskMessages.clear(); };

	/// Preparing to go offline. Is used e.g. by the Archive Tool.
	virtual void goingOffline() {};

	virtual HRESULT SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject ) override;

#pragma region virtual method (ITaskObject)
	virtual SvOi::ISelectorItemVectorPtr GetSelectorList(SvOi::IsObjectInfoAllowed func, UINT Attribute, bool WholeArray) const override;
	virtual SvOi::DependencyList GetDependents(bool bImagesOnly, SVObjectTypeEnum nameToObjectType) const override;
	virtual void GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries) override;
	virtual void GetInputs(SvUl::InputNameGuidPairList& rList, const SVObjectTypeInfoStruct& typeInfo = SVObjectTypeInfoStruct(SVNotSetObjectType), SVObjectTypeEnum objectTypeToInclude = SVNotSetObjectType ) override;
	virtual HRESULT ConnectToObject(const SVString& rInputName, const SVGUID& rNewID, SVObjectTypeEnum objectType = SVNotSetObjectType) override;
	virtual bool IsObjectValid() const override;
	virtual const SvStl::MessageContainerVector& getTaskMessages() const override {return m_TaskMessages;};
	virtual SvStl::MessageContainerVector validateAndSetEmmeddedValues(const SvOi::SetValuePairVector& valueVector, bool shouldSet) override;
	virtual void ResolveDesiredInputs(const SvOi::SVInterfaceList& rDesiredInputs) override;
	//************************************
	//! Get the first task message
	//! \return a const reference to the first task message
	//************************************
	virtual SvStl::MessageContainer getFirstTaskMessage() const override;
#pragma endregion virtual method (ITaskObject)

#pragma region Methods to replace processMessage
	virtual SVObjectClass* OverwriteEmbeddedObject(const GUID& uniqueID, const GUID& rEmbeddedID) override;
	virtual void GetInputInterface(SVInputInfoListClass& rInputList, bool bAlsoFriends) const override;
	virtual void DestroyFriend(SVObjectClass* pObject) override;
	virtual SvOi::IObjectClass* getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	SVInputInfoListClass m_svToolInputList;
	long m_lLastToolInputListIndex;

	BOOL RegisterEmbeddedObjectAsClass( SVObjectClass* PEmbeddedObject, const GUID& REmbeddedID, LPCTSTR newObjectName );

	/// This method return true if method ConnectAllObject has to ask friends to connect this input info
	/// \param rInfo [in] input info for the connection.
	/// \param rPOwner [in,out] The method can change the owner if required.
	/// \returns bool
	virtual bool hasToAskFriendForConnection( const SVObjectTypeInfoStruct& rInfo, SVObjectClass*& rPOwner ) const { return true; }

public:
	// Get the local object color...
	virtual DWORD GetObjectColor() const override;
	// Get the local object state...
	virtual DWORD GetObjectState() const override;
	void GetInputObjects( SVInputInfoListClass& RInputObjectList );
	virtual void GetAllInputObjects();
	virtual void Persist(SVObjectWriter& writer) override;
	void PersistFriends(SVObjectWriter& rWriter);
	void PersistInputs(SVObjectWriter& rWriter);
	void PersistEmbeddeds(SVObjectWriter& rWriter);

	void GetPrivateInputList( SVInputInfoListClass& RInputInterface ) const;
	HRESULT GetDependentsList( SVObjectListClass& rListOfDependents, bool bOnlyImageDependencies = false );

	// the first object is the dependent, the second is the specific object being used
	HRESULT GetDependentsList( SVObjectPairVector& rListOfDependents, bool bOnlyImageDependencies = false );
	HRESULT GetDependentsList( const SVObjectVector& rListOfObjects, SVObjectPairVector& rListOfDependents );

	HRESULT GetImageList( SVImageListClass& p_rImageList, UINT uiAttributes = SV_NO_ATTRIBUTES, bool bAND = true );

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject ) override;
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject ) override;

	virtual HRESULT CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	void AddEmbeddedObject( SVObjectClass* PObject );
	void RemoveEmbeddedObject( SVObjectClass* pObjectToRemove);
	SVValueObjectClass* GetEmbeddedValueObject( GUID classguid );

	virtual HRESULT ResetObjectInputs() override;

protected:
	/// Validate the Parameter of this object and call depending of the level the sub methods.
	/// \param validationLevel [in] InspectionSettable calls only ValidateInspectionSettableParameters, RemotelyAndInspectionSettable calls also ValidateRemotelySettableParameters 
	///										and AllParameters also ValidateOfflineParameters.
	/// \returns bool
	bool OnValidateParameter (ValidationLevelEnum validationLevel);
	/// Check parameter which can be changed online if they are valid.
	/// \returns bool
	virtual bool ValidateInspectionSettableParameters ();
	/// Check parameter which can be remotely settable if they are valid.
	/// \returns bool
	virtual bool ValidateRemotelySettableParameters ();
	/// Check parameter which can only set offline if they are valid.
	/// \returns bool
	virtual bool ValidateOfflineParameters ();

	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

	virtual HRESULT GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine );
	virtual HRESULT UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine );
	HRESULT UpdateOverlayColor( SVExtentMultiLineStruct& p_rMultiLine );
	HRESULT UpdateOverlayName( SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents );
	virtual HRESULT onCollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray& p_MultiLineArray );

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual void addDefaultInputObjects( BOOL BCallBaseClass = FALSE, SVInputInfoListClass* PInputListToFill = nullptr );

	void hideEmbeddedObject( SVObjectClass& RObjectToHide );

	HRESULT RegisterSubObject( SVValueObjectClass* p_pValueObject );
	HRESULT RegisterSubObject( SVImageClass* p_pImageObject );
	HRESULT UnregisterSubObject( SVValueObjectClass* p_pValueObject );
	HRESULT UnregisterSubObject( SVImageClass* p_pImageObject );

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	// He will call OnValidate() for you!
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	BOOL runFriends( SVRunStatusClass& RRunStatus );

	virtual bool resetAllOutputListObjects( bool shouldNotifyFriends, bool silentReset );

protected:
	SVValueObjectClassPtrSet m_svValueObjectSet;
	SVImageClassPtrSet       m_svImageObjectSet;
	SVInputInfoListClass     m_InputObjectList;

	// Embedded Object:
	SVBoolValueObjectClass  m_isObjectValid;	//	Embedded
	SVDWordValueObjectClass m_statusTag;
	SVDWordValueObjectClass m_statusColor;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// .Description : Contains pointer to SVObjectClass items, but doesn't owns these
	//				: SVObjectClass items. That means it doesn't construct
	//				: or destruct the items! 
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Only SVTaskObjectListClass and SVTaskObjectClass can have Embedded Objects
	// Embedded Objects can be SVObjectClass Objects
	//////////////////////////////////////////////////////////////////////////////////////////////
	SVObjectClassPtrArray m_embeddedList;

	// Input Interface List
	// Used to register your input interface...
	// Counterpart to embeddedList, which is some kind of 
	// outputInterfaceList.
	SVInputInfoListClass m_inputInterfaceList;

	bool m_bUseOverlays;

private:
	HRESULT LocalInitialize();

	SvStl::MessageContainerVector m_TaskMessages;  ///The list of task messages
};	// end class SVTaskObjectClass

