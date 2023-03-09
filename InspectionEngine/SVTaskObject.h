//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVProtoBuf/SVO-Enum.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectAppClass.h"
#include "SVValueObjectLibrary/SVDWordValueObjectClass.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVStatusLibrary/MessageContainer.h"
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

namespace SvOl
{
	class InputObject;
}

class SVToolSet;

namespace SvIe
{
	class RunStatus;
	struct SVExtentPropertyInfoStruct;

class SVTaskObjectClass : public SvOl::SVObjectAppClass, public SvOi::ITaskObject
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

	friend class SVTaskObjectListClass; // For access to Run()
	friend class SvTo::SVToolClass; // For access to Run()
	friend class SvTo::LoopTool; // For access to Run()
	friend class SVToolSet; // For access to Run()
public:
	explicit SVTaskObjectClass(LPCSTR LPSZObjectName);
	SVTaskObjectClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECT);

	virtual ~SVTaskObjectClass();
	
	//add the errormessage if add is true and errormessagetext is different from last errormessagetext 
	//remove last errormessage if add is false and errormessagetext is last errormessagetext 
	void addOrRemoveResetErrorMessage(SvStl::MessageContainer& rErrorMessage, bool add);

	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr, int nResetDepth = 0) override;
	virtual bool isInputImage(uint32_t imageId) const override;

	virtual bool DoesObjectHaveExtents() const;
	virtual HRESULT SetImageExtentToParent();
	virtual HRESULT SetImageExtentToFit(const SVImageExtentClass& rImageExtent);
	virtual HRESULT updateImageExtent(bool /*init*/) { return S_OK; }
	virtual HRESULT GetPropertyInfo(SvPb::SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo) const;

	virtual void connectInput(SvOi::IObjectClass* pInput) override;
	void connectInput(SvOl::InputObject& rObject);

	virtual bool connectAllInputs() override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool CloseObject() override;
	virtual void disconnectAllInputs() override;
	virtual void getInputs(std::back_insert_iterator<std::vector<SvOl::InputObject*>> inserter) const;
	virtual void refreshAllObjects(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual void getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const override;
	virtual void fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter) override;

	virtual void SetDisabled() override;

	bool RegisterEmbeddedImage(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID);
	bool RegisterEmbeddedImage(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR newString);

	virtual SVObjectClass* overwriteInputObject(uint32_t uniqueId, SvPb::EmbeddedIdEnum embeddedId) override;
	/// Moved an embedded-object in the embedded-list to a new position.
	/// \param pToMoveObject [in] This object should moved.
	/// \param pPosObject [in] The other object will be moved before this object.
	void MovedEmbeddedObject(SVObjectClass* pToMoveObject, SVObjectClass* pPosObject);
	bool registerInputObject(SvOl::InputObject* pInputObject, const std::string& p_rInputName, SvPb::EmbeddedIdEnum embeddedId);

	virtual HRESULT GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0) const override;

	//************************************
	//! Clears the task set message list 
	//************************************
	void clearTaskMessages() { m_ResetErrorMessages.clear(); m_RunErrorMessages.clear(); };
	void clearRunErrorMessages() { m_RunErrorMessages.clear(); };

	void removeTaskMessage(DWORD MessageCode, SvStl::MessageTextEnum AdditionalTextId);

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
	const SVThreadSafeList<SVTaskObjectClass*>& GetFriendList() const { return m_friendList; };

	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;
	
#pragma region virtual method (ITaskObject)
	virtual void GetInputs(SvPb::InputDataList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo = SvDef::SVObjectTypeInfoStruct(SvPb::SVNotSetObjectType), SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVNotSetObjectType, bool shouldExcludeFirstObjectName = false, int maxNumbers = 0) override;
	virtual HRESULT ConnectToObject(const std::string& rInputName, uint32_t newID) override;
	virtual HRESULT connectToObject(const SvPb::ConnectToObjectRequest& rConnectData) override;
	virtual const SvStl::MessageContainerVector& getResetErrorMessages() const override { return m_ResetErrorMessages; };
	virtual const SvStl::MessageContainerVector& getRunErrorMessages() const override { return m_RunErrorMessages; };
	virtual SvStl::MessageContainerVector getErrorMessages() const override;
	virtual SvStl::MessageContainerVector validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet, SvOi::ResetParameter* pPar = nullptr) override;
	virtual void ResolveDesiredInputs(const SvDef::SVObjectTypeInfoVector& rDesiredInputs) override;
	//************************************
	//! Get the first task message
	//! \return a const reference to the first task message
	//************************************
	virtual SvStl::MessageContainer getFirstTaskMessage() const override;
	virtual SvDef::StringVector getSpecialImageList() const override { return {}; };
	virtual bool getSpecialImage(const std::string& , SvOi::SVImageBufferHandlePtr& ) const override { return false; };
	virtual bool getImage(SvPb::EmbeddedIdEnum embeddedId, SvOi::SVImageBufferHandlePtr& rImagePtr) const override;
	virtual std::vector<uint32_t> getEmbeddedList() const override;
	virtual void fillInputInList(::google::protobuf::RepeatedPtrField< SvPb::ValueObjectValues >& rList) const override;
	virtual bool isErrorMessageEmpty() const override { return m_ResetErrorMessages.empty() && m_RunErrorMessages.empty(); };
	virtual bool AddFriend(uint32_t friendId, uint32_t addPreId = SvDef::InvalidObjectId) override;
	bool AddFriend(SVTaskObjectClass* pFriend, uint32_t addPreId = SvDef::InvalidObjectId);
	virtual void moveFriendObject(uint32_t objectToMoveId, uint32_t preObjectId = SvDef::InvalidObjectId) override;
	virtual void getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse&) const override {};
	virtual SvPb::InspectionCmdResponse setAndSortEmbeddedValues(SvPb::SetAndSortEmbeddedValueRequest request) override { Log_Assert(false); return {}; };
	virtual void getInputData(const SvPb::GetInputDataRequest& request, SvPb::InspectionCmdResponse& rCmdResponse) const override;
#pragma endregion virtual method (ITaskObject)

#pragma region Methods to replace processMessage
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	
public:
	// Get the local object color...
	virtual DWORD GetObjectColor() const override;
	DWORD GetStatusTag() const;
	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;
	void PersistFriends(SvOi::IObjectWriter& rWriter) const;
	void PersistInputs(SvOi::IObjectWriter& rWriter) const;

	const std::vector<SvOl::InputObject*>& GetPrivateInputList() const { return m_inputs; };

	HRESULT GetImageList(SVImageClassPtrVector& p_rImageList);

	virtual HRESULT RegisterSubObject(SVObjectClass* pObject) override;
	virtual HRESULT UnregisterSubObject(SVObjectClass* pObject) override;
	virtual void setEditModeFreezeFlag(bool flag) override;
	virtual void copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr) override;
	virtual void goingOffline() override;

	virtual HRESULT CollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray);
	/// Collect all ShapeGroups for this overlay in the protoBuf-message and return it.
	/// \param pImage [in] Image for with the overlay should be collected.
	/// \param rOverlay [in,out] Protobuf Message.
	virtual void collectOverlays(const SVImageClass* pImage, SvPb::Overlay& rOverlay) const;

	SVObjectClass* GetEmbeddedValueObject(SvPb::EmbeddedIdEnum embeddedID);

	/// This method will be called if a embeddedId has be changed.
	/// \param pOwnerObject [in] The owner of the changed object.
	/// \param rOldEmbeddedID [in] Old embeddedId
	/// \param rNewEmbeddedID [in] New embeddedId
	virtual void OnEmbeddedIDChanged(const SVObjectClass* , SvPb::EmbeddedIdEnum , SvPb::EmbeddedIdEnum ) {};

	void setStatus(const RunStatus& state);
	void setStatus(DWORD color, DWORD state);
	virtual const SVImageExtentClass* GetImageExtentPtr() const { return nullptr; };
	virtual void  SetToolActiveFlag(bool ) { return; }
protected:
	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual bool Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);

	virtual void GetDrawInfo(SVExtentMultiLineStruct&) {};
	virtual void UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine);
	virtual void UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine) const;
	void UpdateOverlayName(SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents);
	virtual HRESULT onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector& p_MultiLineArray);
	/// Add a overlay group if available to the protoBuf-message.
	/// \param pImage [in] Image for with the overlay should be collected.
	/// \param rOverlay [in,out] Protobuf Message.
	virtual void addOverlayGroups(const SVImageClass* , SvPb::Overlay& ) const {};

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);
	bool runFriends(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);

	
private:
	HRESULT LocalInitialize();
	HRESULT setEmbeddedValue(const SvOi::SetValueStruct& rEntry, std::back_insert_iterator<SvStl::MessageContainerVector> inserter);
	HRESULT setEmbeddedValue(const SvOi::SetLinkedStruct& rEntry, std::back_insert_iterator<SvStl::MessageContainerVector> inserter);
	bool addFriend(SVTaskObjectClass& rFriend, uint32_t addPreId = SvDef::InvalidObjectId);

	
protected:
	SvOi::IValueObjectPtrSet m_ValueObjectSet;

	// Embedded Object:
	SvVol::SVDWordValueObjectClass m_statusTag;
	SvVol::SVDWordValueObjectClass m_statusColor;
	bool m_bSkipFirstFriend; //if true first friend will not be "run" by "runFriends". Is used for conditionalTask, because it will be run before the normal run separately.
	//Contains a list of friend objects, which will be informed about certain actions or messages this object is doing/processing.
	SVThreadSafeList<SVTaskObjectClass*> m_friendList;
	std::vector<SvVol::LinkedValue*> m_embeddedFormulaLinked;

	std::vector<SvOl::InputObject*> m_inputs;

	
	bool m_bUseOverlays;

	SvStl::MessageContainerVector m_ResetErrorMessages;  ///The list of task messages
	SvStl::MessageContainerVector m_RunErrorMessages;  ///The list of task messages
};

} //namespace SvIe
