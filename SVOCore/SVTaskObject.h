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
#include "ObjectInterfaces/IValueObject.h"
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

class SVTaskObjectClass : public SVObjectAppClass, public SvOi::ITaskObject
{
	SV_DECLARE_CLASS( SVTaskObjectClass )

	friend class SVTaskObjectListClass; // For access to Run()
	friend class SVToolClass; // For access to Run()
	friend class SVToolSetClass; // For access to Run()

public:
	SVTaskObjectClass( LPCSTR LPSZObjectName );
	SVTaskObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECT );

	virtual ~SVTaskObjectClass();

	virtual bool resetAllObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual HRESULT IsInputImage( SVImageClass* p_psvImage );

	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const;
	virtual HRESULT GetImageExtent( SVImageExtentClass& rImageExtent );
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent );
	virtual HRESULT SetImageExtentToParent( );
	virtual HRESULT SetImageExtentToFit( const SVImageExtentClass& rImageExtent );
	virtual HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList );
	virtual HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;


	void ResetPrivateInputInterface();
	
	virtual bool ConnectAllInputs() override;
	HRESULT ConnectToObject( SVInObjectInfoStruct* p_psvInputInfo, SVObjectClass* pNewObject );

	virtual bool IsValid() const override;

	virtual bool CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual bool CloseObject() override;
	void Disconnect();
	virtual bool DisconnectObjectInput(SVInObjectInfoStruct* pInObjectInfo) override;
	virtual HRESULT GetOutputList(SVOutputInfoListClass& p_rOutputInfoList ) const override;
	
	virtual HRESULT DisconnectInputsOutputs(SVObjectPtrVector& rListOfObjects);
	virtual HRESULT HideInputsOutputs(SVObjectPtrVector& rListOfObjects);

	virtual void SetObjectDepth( int NewObjectDepth ) override;
	virtual void SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual bool SetImageDepth( long lDepth ) override;

	virtual void SetInvalid() override;
	virtual void SetDisabled() override;

	bool RegisterEmbeddedObject( SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID );
	bool RegisterEmbeddedObject( SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR newString );
	bool RegisterEmbeddedObject( SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset );
	bool RegisterEmbeddedObject( SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR strName, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset );
	bool RegisterInputObject( SVInObjectInfoStruct* PInObjectInfo, const SVString& p_rInputName );

	HRESULT GetOutputListFiltered( SVObjectReferenceVector& rvecObjects, UINT uiAttributes = SvOi::SV_NO_ATTRIBUTES, bool bAND = true ); /* true means AND, false means OR */

	HRESULT GetNonToolsetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const;

	virtual HRESULT IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo ); 

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	HRESULT FindNextInputImageInfo(SVInObjectInfoStruct*& p_rpsvFoundInfo, const SVInObjectInfoStruct* p_psvLastInfo = nullptr);

	//************************************
	//! Clears the task set message list 
	//************************************
	void clearTaskMessages( ) { m_ResetErrorMessages.clear(); m_RunErrorMessages.clear(); };
	void clearRunErrorMessages( ) { m_RunErrorMessages.clear(); };

	bool IsErrorMessageEmpty() const { return m_ResetErrorMessages.empty() && m_RunErrorMessages.empty(); };
	bool IsRunErrorMessageEmpty() { return m_RunErrorMessages.empty(); };

	/// Preparing to go offline. Is used e.g. by the Archive Tool.
	virtual void goingOffline() {};

	virtual HRESULT SetValuesForAnObject( const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject ) override;

	/// Set the flag, that the first friend (is normally the conditional task, if it is a tool) should be skipped in runFriends.
	void setSkipFirstFriendFromRun() { m_bSkipFirstFriend = true; };

#pragma region virtual method (ITaskObject)
	virtual SvOi::ISelectorItemVectorPtr GetSelectorList(SvOi::IsObjectInfoAllowed func, UINT Attribute, bool WholeArray) const override;
	virtual void GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries) override;
	virtual void GetInputs(SvUl::InputNameGuidPairList& rList, const SVObjectTypeInfoStruct& typeInfo = SVObjectTypeInfoStruct(SVNotSetObjectType), SVObjectTypeEnum objectTypeToInclude = SVNotSetObjectType ) override;
	virtual HRESULT ConnectToObject(const SVString& rInputName, const SVGUID& rNewID, SVObjectTypeEnum objectType = SVNotSetObjectType) override;
	virtual bool IsObjectValid() const override;
	virtual const SvStl::MessageContainerVector& getResetErrorMessages() const override {return m_ResetErrorMessages;};
	virtual const SvStl::MessageContainerVector& getRunErrorMessages() const override {return m_RunErrorMessages;};
	virtual SvStl::MessageContainerVector getErrorMessages() const override;
	virtual SvStl::MessageContainerVector validateAndSetEmmeddedValues(const SvOi::SetValueObjectPairVector& rValueVector, bool shouldSet) override;
	virtual void ResolveDesiredInputs(const SvOi::SVInterfaceVector& rDesiredInputs) override;
	//************************************
	//! Get the first task message
	//! \return a const reference to the first task message
	//************************************
	virtual SvStl::MessageContainer getFirstTaskMessage() const override;
	virtual void getSpecialImageList(std::vector<SVString>& rList) const override {};
	virtual bool getSpecialImage(const SVString& rName, SvOi::MatroxImageSmartHandlePtr& rImagePtr) const override { return false; };
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

	bool RegisterEmbeddedObjectAsClass( SVObjectClass* PEmbeddedObject, const GUID& REmbeddedID, LPCTSTR newObjectName );

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

	HRESULT GetImageList( SVImageListClass& p_rImageList, UINT uiAttributes = SvOi::SV_NO_ATTRIBUTES, bool bAND = true );

	virtual HRESULT RegisterSubObject( SVObjectClass* pObject ) override;
	virtual HRESULT UnregisterSubObject( SVObjectClass* pObject ) override;

	virtual HRESULT CollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray );

	void AddEmbeddedObject( SVObjectClass* PObject );
	void RemoveEmbeddedObject( SVObjectClass* pObjectToRemove);
	SVObjectClass* GetEmbeddedValueObject( GUID classguid );

	virtual HRESULT ResetObjectInputs() override;

protected:
	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual bool Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	virtual HRESULT GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine );
	virtual HRESULT UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine );
	HRESULT UpdateOverlayColor( SVExtentMultiLineStruct& p_rMultiLine );
	HRESULT UpdateOverlayName( SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents );
	virtual HRESULT onCollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructVector& p_MultiLineArray );

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual void addDefaultInputObjects( SVInputInfoListClass* PInputListToFill = nullptr );

	void hideEmbeddedObject( SVObjectClass& RObjectToHide );

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );
	bool runFriends( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );

	virtual bool resetAllOutputListObjects( SvStl::MessageContainerVector *pErrorMessages=nullptr );
	
private:
		HRESULT LocalInitialize();

protected:
	SvOi::IValueObjectPtrSet m_ValueObjectSet;
	SVImageClassPtrSet		m_ImageObjectSet;
	SVInputInfoListClass	m_InputObjectList;

	// Embedded Object:
	SVBoolValueObjectClass  m_isObjectValid;	//	Embedded
	SVDWordValueObjectClass m_statusTag;
	SVDWordValueObjectClass m_statusColor;
	bool m_bSkipFirstFriend; //if true first friend will not be "run" by "runFriends". Is used for conditionalTask, because it will be run before the normal run separately.

	//////////////////////////////////////////////////////////////////////////////////////////////
	// .Description : Contains pointer to SVObjectClass items, but doesn't owns these
	//				: SVObjectClass items. That means it doesn't construct
	//				: or destruct the items! 
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Only SVTaskObjectListClass and SVTaskObjectClass can have Embedded Objects
	// Embedded Objects can be SVObjectClass Objects
	//////////////////////////////////////////////////////////////////////////////////////////////
	SVObjectPtrVector m_embeddedList;

	// Input Interface List
	// Used to register your input interface...
	// Counterpart to embeddedList, which is some kind of 
	// outputInterfaceList.
	SVInputInfoListClass m_inputInterfaceList;

	bool m_bUseOverlays;

	SvStl::MessageContainerVector m_ResetErrorMessages;  ///The list of task messages
	SvStl::MessageContainerVector m_RunErrorMessages;  ///The list of task messages
};	// end class SVTaskObjectClass

