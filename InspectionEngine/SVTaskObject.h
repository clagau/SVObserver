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

class SVOutputInfoListClass;
class SVRunStatusClass;
class SVToolSetClass;

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
	friend class SVToolSetClass; // For access to Run()
public:
	SVTaskObjectClass(LPCSTR LPSZObjectName);
	SVTaskObjectClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECT);

	virtual ~SVTaskObjectClass();

	virtual bool resetAllObjects(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual bool isInputImage(const SVGUID& rImageGuid) const override;

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

	bool RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID);
	bool RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR newString);
	bool RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, int StringResourceID, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset);
	bool RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, const GUID& rGuidEmbeddedID, LPCTSTR strName, bool p_bResetAlways, SvOi::SVResetItemEnum eRequiredReset);
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

	void removeTaskMessage(long MessageCode, SvStl::MessageTextEnum AdditionalTextId);


	bool IsRunErrorMessageEmpty() { return m_RunErrorMessages.empty(); };

	/// Preparing to go offline. Is used e.g. by the Archive Tool.
	virtual void goingOffline() {};

	virtual HRESULT SetValuesForAnObject(const GUID& rAimObjectID, SVObjectAttributeClass* pDataObject) override;

	/// Set the flag, that the first friend (is normally the conditional task, if it is a tool) should be skipped in runFriends.
	void setSkipFirstFriendFromRun() { m_bSkipFirstFriend = true; };

	
#pragma region virtual method (ITaskObject)
	virtual int GetObjectSelectorList(SvOi::IsObjectInfoAllowed pFunctor, std::vector<SvPb::TreeItem>& rTreeItems ) const override;
	virtual std::vector<SvPb::TreeItem> GetSelectorList(SvOi::IsObjectInfoAllowed pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum objectType) const override;
	virtual void GetInputImages(SvUl::InputNameGuidPairList& rList, int maxEntries) override;
	virtual void GetInputs(SvUl::InputNameGuidPairList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo = SvDef::SVObjectTypeInfoStruct(SvPb::SVNotSetObjectType), SvPb::SVObjectTypeEnum objectTypeToInclude = SvPb::SVNotSetObjectType, bool shouldExcludeFirstObjectName = false) override;
	virtual HRESULT ConnectToObject(const std::string& rInputName, const SVGUID& rNewID, SvPb::SVObjectTypeEnum objectType = SvPb::SVNotSetObjectType) override;
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
	virtual bool getSpecialImage(const std::string& rName, SvOi::SVImageBufferHandlePtr& rImagePtr) const override { return false; };
	virtual SVGuidVector getEmbeddedList() const override;
	virtual bool isErrorMessageEmpty() const override { return m_ResetErrorMessages.empty() && m_RunErrorMessages.empty(); };
#pragma endregion virtual method (ITaskObject)

#pragma region Methods to replace processMessage
	virtual SVObjectClass* OverwriteEmbeddedObject(const GUID& uniqueID, const GUID& rEmbeddedID) override;
	virtual void GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const override;
	virtual void DestroyFriend(SVObjectClass* pObject) override;
	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends = true, const SvOi::IObjectClass* pRequestor = nullptr) const override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

protected:
	bool RegisterEmbeddedObjectAsClass(SVObjectClass* PEmbeddedObject, const GUID& REmbeddedID, LPCTSTR newObjectName);

	/// This method return true if method ConnectAllObject has to ask friends to connect this input info
	/// \param rInfo [in] input info for the connection.
	/// \param rPOwner [in,out] The method can change the owner if required.
	/// \returns bool
	virtual bool hasToAskFriendForConnection(const SvDef::SVObjectTypeInfoStruct& rInfo, SVObjectClass*& rPOwner) const { return true; }

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

	virtual HRESULT CollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray);
	/// Collect all ShapeGroups for this overlay in the protoBuf-message and return it.
	/// \param pImage [in] Image for with the overlay should be collected.
	/// \param rOverlay [in,out] Protobuf Message.
	virtual void collectOverlays(const SVImageClass* pImage, SvPb::Overlay& rOverlay) const;

	void AddEmbeddedObject(SVObjectClass* pObject);
	void RemoveEmbeddedObject(SVObjectClass* pObjectToRemove);
	SVObjectClass* GetEmbeddedValueObject(GUID classguid);

	virtual HRESULT ResetObjectInputs() override;

	/// This method will be called if a embeddedId has be changed.
	/// \param pOwnerObject [in] The owner of the changed object.
	/// \param rOldEmbeddedID [in] Old embeddedId
	/// \param rNewEmbeddedID [in] New embeddedId
	virtual void OnEmbeddedIDChanged(const SVObjectClass* pOwnerObject, const SVGUID& rOldEmbeddedID, const SVGUID& rNewEmbeddedID) {};

protected:
	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual bool Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);

	virtual HRESULT GetDrawInfo(SVExtentMultiLineStruct& p_rMultiLine);
	virtual HRESULT UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine);
	HRESULT UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine);
	HRESULT UpdateOverlayName(SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents);
	virtual HRESULT onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector& p_MultiLineArray);
	/// Add a overlay group if available to the protoBuf-message.
	/// \param pImage [in] Image for with the overlay should be collected.
	/// \param rOverlay [in,out] Protobuf Message.
	virtual void addOverlayGroups(const SVImageClass* pImage, SvPb::Overlay& rOverlay) const {};

	virtual SVObjectPtrDeque GetPreProcessObjects() const override;
	virtual SVObjectPtrDeque GetPostProcessObjects() const override;

	virtual void addDefaultInputObjects(SvOl::SVInputInfoListClass* PInputListToFill = nullptr);

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);
	bool runFriends(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr);

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
