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
#include "SVImageClass.h"
#include "SVProtobuf/SVO-Enum.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVInputInfoListClass.h"
#include "SVObjectLibrary/SVObjectInfoArrayClass.h"
#include "SVObjectAppClass.h"
#include "SVValueObjectLibrary/SVDWordValueObjectClass.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvTo
{
	class SVToolClass;
	class LoopTool;
}


namespace SvVol
{
	class LinkedValue;
}


class SVOutputInfoListClass;
class RunStatus;
class SVToolSet;

namespace SvIe
{
struct SVExtentPropertyInfoStruct;

class SVTaskObjectClass : public SVObjectAppClass, public SvOi::ITaskObject
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS(SVTaskObjectClass)

	friend class SVTaskObjectListClass; // For access to Run()
	friend class SvTo::SVToolClass; // For access to Run()
	friend class SvTo::LoopTool; // For access to Run()
	friend class SVToolSet; // For access to Run()
public:
	explicit SVTaskObjectClass(LPCSTR LPSZObjectName);
	SVTaskObjectClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECT);

	virtual ~SVTaskObjectClass();

	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual bool isInputImage(uint32_t imageId) const override;

	virtual bool DoesObjectHaveExtents() const;
	const SVImageExtentClass& GetImageExtent() const { return m_imageExtent; }
	virtual HRESULT SetImageExtent(const SVImageExtentClass& rImageExtent);
	virtual HRESULT SetImageExtentToParent();
	virtual HRESULT SetImageExtentToFit(const SVImageExtentClass& rImageExtent);
	virtual HRESULT updateImageExtent() {return S_OK;}
	virtual HRESULT GetPropertyInfo(SvPb::SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo) const;


	void ResetPrivateInputInterface();

	virtual bool ConnectAllInputs() override;
	HRESULT ConnectToObject(SvOl::SVInObjectInfoStruct* p_psvInputInfo, SVObjectClass* pNewObject);

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool CloseObject() override;
	void Disconnect();
	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pInObjectInfo) override;
	virtual HRESULT GetOutputList(SVOutputInfoListClass& p_rOutputInfoList) const override;

	virtual HRESULT DisconnectInputsOutputs(SVObjectPtrVector& rListOfObjects);
	virtual HRESULT HideInputsOutputs(SVObjectPtrVector& rListOfObjects);

	virtual void SetDisabled() override;

	bool RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID);
	bool RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR newString);
	bool RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset);
	bool RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR strName, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset);
	/// Moved an embedded-object in the embedded-list to a new position.
	/// \param pToMoveObject [in] This object should moved.
	/// \param pPosObject [in] The other object will be moved before this object.
	void MovedEmbeddedObject(SVObjectClass* pToMoveObject, SVObjectClass* pPosObject);
	bool RegisterInputObject(SvOl::SVInObjectInfoStruct* PInObjectInfo, const std::string& p_rInputName);

	HRESULT GetOutputListFiltered(SVObjectReferenceVector& rvecObjects, UINT uiAttributes = SvPb::noAttributes, bool bAND = true); /* true means AND, false means OR */

	virtual HRESULT IsAuxInputImage(const SvOl::SVInObjectInfoStruct* p_psvInfo) const;

	virtual HRESULT GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0) const override;
	HRESULT FindNextInputImageInfo(SvOl::SVInObjectInfoStruct*& p_rpsvFoundInfo, const SvOl::SVInObjectInfoStruct* p_psvLastInfo = nullptr) const;

	//************************************
	//! Clears the task set message list 
	//************************************
	void clearTaskMessages() { m_ResetErrorMessages.clear(); m_RunErrorMessages.clear(); };
	void clearRunErrorMessages() { m_RunErrorMessages.clear(); };

	void removeTaskMessage(DWORD MessageCode, SvStl::MessageTextEnum AdditionalTextId);
	void addResetErrorMessage(SvStl::MessageContainer& rErrorMessage);

	bool IsRunErrorMessageEmpty() { return m_RunErrorMessages.empty(); };

	virtual HRESULT SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject) override;

	/// Set the flag, that the first friend (is normally the conditional task, if it is a tool) should be skipped in runFriends.
	void setSkipFirstFriendFromRun() { m_bSkipFirstFriend = true; };

	/// This method destroys all friends on the friends list owned by this object.  All other objects are disconnected from this object.
	virtual void DestroyFriends() override;
	void DestroyFriend(SVObjectClass* pObject);

	//************************************
	//! this function returns a pointer to the friendobject which fit the ObjectType, if any. Otherwise it returns nullptr. 
	//! \param rObjectType [in]
	//! \returns SVObjectClass*
	//************************************
	SVObjectClass* GetFriend(const SvDef::SVObjectTypeInfoStruct& rObjectType) const;
	
	/// This method returns a reference to the friends list attribute of this object.
	const SVObjectInfoArrayClass& GetFriendList() const { return m_friendList;	};

	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType) const override;
	
#pragma region virtual method (ITaskObject)
	virtual void GetInputs(SvUl::InputNameObjectIdPairList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo = SvDef::SVObjectTypeInfoStruct(SvPb::SVNotSetObjectType), SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVNotSetObjectType, bool shouldExcludeFirstObjectName = false, int maxNumbers = 0) override;
	virtual HRESULT ConnectToObject(const std::string& rInputName, uint32_t newID, SvPb::SVObjectTypeEnum objectType = SvPb::SVNotSetObjectType) override;
	virtual const SvStl::MessageContainerVector& getResetErrorMessages() const override { return m_ResetErrorMessages; };
	virtual const SvStl::MessageContainerVector& getRunErrorMessages() const override { return m_RunErrorMessages; };
	virtual SvStl::MessageContainerVector getErrorMessages() const override;
	virtual SvStl::MessageContainerVector validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet) override;
	virtual SvStl::MessageContainerVector setEmbeddedDefaultValues(const SvOi::SetValueStructVector& rValueVector) override;
	virtual void ResolveDesiredInputs(const SvDef::SVObjectTypeInfoVector& rDesiredInputs) override;
	//************************************
	//! Get the first task message
	//! \return a const reference to the first task message
	//************************************
	virtual SvStl::MessageContainer getFirstTaskMessage() const override;
	virtual SvDef::StringVector getSpecialImageList() const override { return {}; };
	virtual bool getSpecialImage(const std::string& , SvOi::SVImageBufferHandlePtr& ) const override { return false; };
	virtual std::vector<uint32_t> getEmbeddedList() const override;
	virtual bool isErrorMessageEmpty() const override { return m_ResetErrorMessages.empty() && m_RunErrorMessages.empty(); };
	virtual bool AddFriend(uint32_t friendId, uint32_t addPreId = SvDef::InvalidObjectId) override;
	virtual void moveFriendObject(uint32_t objectToMoveId, uint32_t preObjectId = SvDef::InvalidObjectId) override;
	virtual void getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse&) const override {};
#pragma endregion virtual method (ITaskObject)

#pragma region Methods to replace processMessage
	virtual SVObjectClass* OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID) override;
	virtual void GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	bool RegisterEmbeddedObjectAsClass(SVObjectClass* PEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR newObjectName);

	/// This method return true if method ConnectAllObject has to ask friends to connect this input info
	/// \param rInfo [in] input info for the connection.
	/// \param rPOwner [in,out] The method can change the owner if required.
	/// \returns bool
	virtual bool hasToAskFriendForConnection(const SvDef::SVObjectTypeInfoStruct& , SVObjectClass*& ) const { return true; }

	/// calls RegisterEmbeddedObject(() twice to register a linked value referring to 'T' in one function call
	void registerEmbeddedLinkedValue(SvVol::LinkedValue* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, SvPb::EmbeddedIdEnum embeddedLinkID, int StringResourceID, _variant_t defaultValue);

public:
	// Get the local object color...
	virtual DWORD GetObjectColor() const override;
	void GetInputObjects(SvOl::SVInputInfoListClass& RInputObjectList);
	virtual void GetAllInputObjects();
	virtual void Persist(SvOi::IObjectWriter& rWriter) override;
	void PersistFriends(SvOi::IObjectWriter& rWriter);
	void PersistInputs(SvOi::IObjectWriter& rWriter);
	void PersistEmbeddeds(SvOi::IObjectWriter& rWriter);

	void GetPrivateInputList(SvOl::SVInputInfoListClass& RInputInterface) const;

	HRESULT GetImageList(SVImageClassPtrVector& p_rImageList, UINT uiAttributes = SvPb::noAttributes, bool bAND = true);

	virtual HRESULT RegisterSubObject(SVObjectClass* pObject) override;
	virtual HRESULT UnregisterSubObject(SVObjectClass* pObject) override;
	virtual void setEditModeFreezeFlag(bool flag) override;
	virtual void copiedSavedImage(SvTrc::ITriggerRecordRWPtr pTr) override;
	virtual void goingOffline() override;

	virtual HRESULT CollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray);
	/// Collect all ShapeGroups for this overlay in the protoBuf-message and return it.
	/// \param pImage [in] Image for with the overlay should be collected.
	/// \param rOverlay [in,out] Protobuf Message.
	virtual void collectOverlays(const SVImageClass* pImage, SvPb::Overlay& rOverlay) const;

	void AddEmbeddedObject(SVObjectClass* pObject);
	void RemoveEmbeddedObject(SVObjectClass* pObjectToRemove);
	SVObjectClass* GetEmbeddedValueObject(SvPb::EmbeddedIdEnum embeddedID);

	virtual HRESULT ResetObjectInputs() override;

	/// This method will be called if a embeddedId has be changed.
	/// \param pOwnerObject [in] The owner of the changed object.
	/// \param rOldEmbeddedID [in] Old embeddedId
	/// \param rNewEmbeddedID [in] New embeddedId
	virtual void OnEmbeddedIDChanged(const SVObjectClass* , SvPb::EmbeddedIdEnum , SvPb::EmbeddedIdEnum ) {};

protected:
	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual bool Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);

	virtual HRESULT GetDrawInfo(SVExtentMultiLineStruct& p_rMultiLine);
	virtual HRESULT UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine);
	HRESULT UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine);
	HRESULT UpdateOverlayName(SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents);
	virtual HRESULT onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector& p_MultiLineArray);
	/// Add a overlay group if available to the protoBuf-message.
	/// \param pImage [in] Image for with the overlay should be collected.
	/// \param rOverlay [in,out] Protobuf Message.
	virtual void addOverlayGroups(const SVImageClass* , SvPb::Overlay& ) const {};

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual void addDefaultInputObjects(SvOl::SVInputInfoListClass* PInputListToFill = nullptr);

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);
	bool runFriends(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);

	virtual bool resetAllOutputListObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr);

private:
	HRESULT LocalInitialize();

protected:
	SvOi::IValueObjectPtrSet m_ValueObjectSet;
	SvOl::SVInputInfoListClass	m_InputObjectList;

	// Embedded Object:
	SvVol::SVDWordValueObjectClass m_statusTag;
	SvVol::SVDWordValueObjectClass m_statusColor;
	bool m_bSkipFirstFriend; //if true first friend will not be "run" by "runFriends". Is used for conditionalTask, because it will be run before the normal run separately.
	//Contains a list of friend objects, which will be informed about certain actions or messages this object is doing/processing.
	SVObjectInfoArrayClass m_friendList;

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
	SvOl::SVInputInfoListClass m_inputInterfaceList;

	SVImageExtentClass m_imageExtent;	//NOTE! this object is directly accessed by m_toolExtent (SVToolClass) via reference

	bool m_bUseOverlays;

	SvStl::MessageContainerVector m_ResetErrorMessages;  ///The list of task messages
	SvStl::MessageContainerVector m_RunErrorMessages;  ///The list of task messages

private:
	//The next two parameter are used only for the method FindNextInputImageInfo to save the input-list to avoid to rebuild it every run.
	mutable SvOl::SVInputInfoListClass m_svToolInputList;
	mutable long m_lLastToolInputListIndex = -1;
};

} //namespace SvIe
