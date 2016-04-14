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
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVObjectLibrary/SVInputInfoListClass.h"
#include "SVObjectAppClass.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "SVValueObjectImpl.h"
#include "SVImageClass.h"
#include "SVImageListClass.h"
#include "SVExtentPropertiesInfoStruct.h"
#include "SVObjectLibrary/SVObjectListClass.h"
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVTaskObjectValueInterface.h"
#include "ObjectSelectorLibrary/SelectorItemVector.h"
#pragma endregion Includes

class SVLineClass;
class SVIPDoc;

class SVTaskObjectClass : virtual public SvOi::ITaskObject, public SVObjectAppClass  
{
	SV_DECLARE_CLASS( SVTaskObjectClass )

	friend class SVInspectionProcess;
	friend class SVTaskObjectListClass;
	friend class SVToolSetClass;

public:
	SVTaskObjectClass( LPCSTR LPSZObjectName );
	SVTaskObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTASKOBJECT );

	virtual ~SVTaskObjectClass();

	virtual HRESULT ResetObject();

	virtual BOOL ReInit();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const;
	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent );
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList );
	virtual HRESULT GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;

	virtual HRESULT GetOutputRectangle( RECT &l_roRect );
	virtual HRESULT NormalizePoint( POINT l_oPoint, POINT &l_roOutputPoint );

	void ResetPrivateInputInterface();
	
	virtual BOOL ConnectAllInputs();
	virtual HRESULT ConnectToImage( SVInObjectInfoStruct* p_psvInputInfo, SVImageClass* p_psvNewImage ); // SEJ - why is this virtual ?

	virtual BOOL IsValid();

	// Routing Version of Validate
	// Validates Local Scope and all owned objects
	virtual BOOL Validate();

	// Non Routing Version of Validate
	// validates only Local Scope
	virtual BOOL OnValidate();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual BOOL CloseObject();
	virtual void Disconnect();
	virtual BOOL DisconnectInput(SVInObjectInfoStruct* pInObjectInfo);
	virtual HRESULT GetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const;
	virtual HRESULT DisconnectInputsOutputs(SVObjectVector& rListOfObjects);
	virtual HRESULT HideInputsOutputs(SVObjectVector& rListOfObjects);

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual BOOL SetImageDepth( long lDepth );

	virtual BOOL SetEmbeddedObjectValue( const GUID& REmbeddedID, SVObjectAttributeClass* PDataObject );
	virtual void SetInvalid();
	virtual void SetDisabled();

	virtual BOOL RegisterEmbeddedObject( SVImageClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, int p_iStringResourceID );
	virtual BOOL RegisterEmbeddedObject( SVValueObjectClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, int p_iStringResourceID, bool p_bResetAlways, SVResetItemEnum p_eRequiredReset );
	virtual BOOL RegisterEmbeddedObject( SVValueObjectClass* p_psvEmbeddedObject, const GUID& p_rguidEmbeddedID, int p_iStringResourceID, bool p_bResetAlways, SVResetItemEnum p_eRequiredReset, LPCTSTR p_pszTypeName );
	virtual BOOL RegisterInputObject( SVInObjectInfoStruct* PInObjectInfo, const SVString& p_rInputName );

	HRESULT GetOutputListFiltered(std::vector<SVValueObjectReference>& rvecObjects, UINT uiAttributes = SV_NO_ATTRIBUTES, bool bAND = true ); /* true means AND, false means OR */

	HRESULT GetNonToolsetOutputList( SVOutputInfoListClass& p_rOutputInfoList ) const;

	virtual HRESULT IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo ); 

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;
	HRESULT FindNextInputImageInfo(SVInObjectInfoStruct*& p_rpsvFoundInfo, const SVInObjectInfoStruct* p_psvLastInfo = nullptr);
	
#pragma region virtual method (ITaskObject)
	virtual HRESULT AddInputRequestMarker() override;
	virtual HRESULT RunOnce(IObjectClass* pTool = nullptr) override;
	virtual SvOi::ISelectorItemVectorPtr GetSelectorList(SvOi::IsObjectInfoAllowed func, UINT Attribute, bool WholeArray) const override;
	virtual SvOi::DependencyList GetDependents(bool bImagesOnly, SVObjectTypeEnum nameToObjectType) const override;
	virtual void GetConnectedImages(SvUl::InputNameGuidPairList& rList, int maxEntries) override;
	virtual HRESULT ConnectToImage(const SVString& rInputName, const SVGUID& rNewID) override;
	virtual bool IsObjectValid() const override;
#pragma endregion virtual method (ITaskObject)

protected:

	SVInputInfoListClass m_svToolInputList;
	long m_lLastToolInputListIndex;

	virtual BOOL RegisterEmbeddedObjectAsClass( SVObjectClass* PEmbeddedObject, const GUID& REmbeddedID, int NewStringResourceID );

public:
	// Get the local object color...
	virtual DWORD GetObjectColor() const override;
	// Get the local object state...
	virtual DWORD GetObjectState() const override;
	virtual void GetInputObjects( SVInputInfoListClass& RInputObjectList );
	virtual void GetAllInputObjects();
	virtual void Persist(SVObjectWriter& writer);
	virtual void PersistFriends(SVObjectWriter& rWriter);
	virtual void PersistInputs(SVObjectWriter& rWriter);
	virtual void PersistEmbeddeds(SVObjectWriter& rWriter);

	void GetPrivateInputList( SVInputInfoListClass& RInputInterface );
	HRESULT GetDependentsList( SVObjectListClass& rListOfDependents, bool bOnlyImageDependencies = false );

	// the first object is the dependent, the second is the specific object being used
	HRESULT GetDependentsList( SVObjectPairVector& rListOfDependents, bool bOnlyImageDependencies = false );
	HRESULT GetDependentsList( const SVObjectVector& rListOfObjects, SVObjectPairVector& rListOfDependents );

	HRESULT GetImageList( SVImageListClass& p_rImageList, UINT uiAttributes = SV_NO_ATTRIBUTES, bool bAND = true );

	virtual HRESULT RegisterSubObject( SVValueObjectClass* p_pValueObject );
	virtual HRESULT RegisterSubObject( SVImageClass* p_pImageObject );
	virtual HRESULT UnregisterSubObject( SVValueObjectClass* p_pValueObject );
	virtual HRESULT UnregisterSubObject( SVImageClass* p_pImageObject );

	virtual HRESULT RegisterSubObjects( SVTaskObjectClass *p_psvOwner, SVObjectClassPtrArray &p_rsvEmbeddedList );
	virtual HRESULT UnregisterSubObjects( SVTaskObjectClass *p_psvOwner );

	virtual HRESULT CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	virtual void AddEmbeddedObject( SVObjectClass* PObject );
	virtual void RemoveEmbeddedObject( SVObjectClass* pObjectToRemove);
	virtual SVValueObjectClass* GetEmbeddedValueObject( GUID classguid );

	virtual HRESULT ResetObjectInputs();

protected:
	// Direct Method Call
	// NOTE:
	// Use onRun() to implement your special updating!
	// Override this only if you have to reroute the call!
	// NEVER call base class Run()! 
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

	virtual HRESULT GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine );
	virtual HRESULT UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine );
	virtual HRESULT UpdateOverlayColor( SVExtentMultiLineStruct& p_rMultiLine );
	virtual HRESULT UpdateOverlayName( SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents );
	virtual HRESULT onCollectOverlays( SVImageClass* p_Image, SVExtentMultiLineStructCArray& p_MultiLineArray );

	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	virtual void addDefaultInputObjects( BOOL BCallBaseClass = FALSE, SVInputInfoListClass* PInputListToFill = NULL );

	virtual void hideEmbeddedObject( SVObjectClass& RObjectToHide );

	// Called by Run()
	// NOTE:
	// Override this if you want to implement your own special run.
	// Don't forget to call base class onRun() first.
	// He will call OnValidate() for you!
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual BOOL runFriends( SVRunStatusClass& RRunStatus );

	virtual DWORD_PTR	processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual DWORD_PTR	OutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual DWORD_PTR	FriendOutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual DWORD_PTR	EmbeddedOutputListProcessMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:

	SVValueObjectClassPtrSet        m_svValueObjectSet;
	SVImageClassPtrSet              m_svImageObjectSet;

	SVInputInfoListClass            m_InputObjectList;

	// Embedded Object:
	SVBoolValueObjectClass          isObjectValid;	//	Embedded
	SVDWordValueObjectClass         statusTag;
	SVDWordValueObjectClass         statusColor;


	// Embedded Object List - SEJ July 7,1999
	//////////////////////////////////////////////////////////////////////////////////////////////
	// .Description : Contains pointer to SVObjectClass items, but doesn't owns this
	//				: SVObjectClass items. That means it doesn't construct
	//				: or destruct the items! 
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Only SVTaskObjectListClass and SVTaskObjectClass can have Embedded Objects
	// Embedded Objects can be SVObjectClass Objects
	//////////////////////////////////////////////////////////////////////////////////////////////
	SVObjectClassPtrArray           embeddedList;


	// Input Interface List
	// Used to register your input interface...
	// Counterpart to embeddedList, which is some kind of 
	// outputInterfaceList.
	SVInputInfoListClass            inputInterfaceList;

	bool                            m_bUseOverlays;

	SVTaskObjectValueInterface      m_taskObjectValueInterface;  ///< this parameter is needed for the interface implementation from RunOnce and AddInputRequestMarker

private:
	HRESULT LocalInitialize();
};	// end class SVTaskObjectClass

