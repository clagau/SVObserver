//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcess
//* .File Name       : $Workfile:   SVInspectionProcess.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.14  $
//* .Check In Date   : $Date:   12 Feb 2015 02:58:54  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/tokenizer.hpp>
//Moved to precompiled header: #include <boost/lexical_cast.hpp>
//Moved to precompiled header: #include <boost/type_traits.hpp>

#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVContainerLibrary/SVBiUniqueMap.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVDataManagerLibrary/SVDataManagerIndexArrayHandle.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectSubmitCommandFacade.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVSharedMemoryLibrary/SVSharedData.h"
#include "SVSharedMemoryLibrary/SVSharedInspectionWriter.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVConditionalHistory.h"
#include "SVImageBuffer.h" //SVImageOverlayClass; used for getting overlay data for the ActiveX
#include "SVInfoStructs.h"
#include "SVInspectionProcessResetStruct.h"
#include "SVInspectionProcessTransactions.h"
#include "SVPublishList.h"
#include "SVResetStruct.h"
#include "SVRGBMainImage.h"
#include "SVValueObjectReference.h"
#include "SVVirtualCamera.h"
#include "SVMonitorList.h"
#pragma endregion Includes

#pragma region Declarations
class SVCameraImageTemplate;
class SVConditionalClass;
class SVDisplayObject;
class SVIPDoc;
class SVPPQObject;
class SVToolSetClass;
class SVTaskObjectClass;
class SVResultListClass;
#pragma endregion Declarations

class SVInspectionProcess : 
	virtual public SvOi::IInspectionProcess,
	virtual public SVObjectClass,
	public SVObjectSubmitCommandFacade,
	public SVObserverTemplate< SVAddTool >,
	public SVObserverTemplate< SVDeleteTool >,
	public SVObserverTemplate< SVRenameObject >
{
	friend class SVCommandInspectionExtentUpdater;
	friend class SVConditionalHistory;

	SV_DECLARE_CLASS( SVInspectionProcess );

public:
	typedef SVBiUniqueMap< CString, SVValueObjectClass* >::type SVValueObjectMap;
	typedef SVTQueueObject< SVOutputRequestInfoStruct > SVOutputRequestQueue;
	typedef SVVector< SVPPQObject* > SVPPQObjectPtrVector;
	//************************************
	//! FunctionPointer as Argument for LooopOverTool
	//! negative return values indicate an error 
	//************************************
	typedef int  (*pToolFunc) (SVObjectClass *pObject );


	SVInspectionProcess( LPCSTR ObjectName );
	SVInspectionProcess( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVINSPECTIONOBJECT );
	virtual ~SVInspectionProcess();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );

	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;
	
	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual BOOL SetImageDepth( long lDepth );

	virtual void ResetName();
	virtual void SetName( const CString& StrString );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual HRESULT ResetObject();
	virtual BOOL OnValidate();
	virtual BOOL Validate();

	virtual HRESULT RegisterSubObject( SVValueObjectClass* p_pValueObject );
	virtual HRESULT RegisterSubObject( SVImageClass* p_pImageObject );
	virtual HRESULT UnregisterSubObject( SVValueObjectClass* p_pValueObject );
	virtual HRESULT UnregisterSubObject( SVImageClass* p_pImageObject );

	virtual HRESULT RegisterSubObjects( SVTaskObjectClass *p_psvOwner, SVObjectClassPtrArray &p_rsvEmbeddedList );
	virtual HRESULT UnregisterSubObjects( SVTaskObjectClass *p_psvOwner );

	virtual BOOL Run( SVRunStatusClass& RRunStatus );

	virtual HRESULT ObserverUpdate( const SVAddTool& p_rData );
	virtual HRESULT ObserverUpdate( const SVDeleteTool& p_rData );
	virtual HRESULT ObserverUpdate( const SVRenameObject& p_rData );

	void SetPPQIdentifier( const SVGUID& p_rPPQId );
	const SVGUID& GetPPQIdentifier() const;
	SVPPQObject* GetPPQ() const;

#pragma region virtual method (IInspectionProcess)
	virtual SvOi::IObjectClass* GetPPQInterface() const override;
	virtual void SetDefaultInputs() override;
	virtual SvOi::ISelectorItemVectorPtr GetPPQSelectorList( const UINT Attribute ) const override;
	virtual SvOi::ITaskObject* GetToolSetInterface() const override;
	virtual HRESULT RunOnce(SvOi::ITaskObject* pTask) override;
#pragma region virtual method (IInspectionProcess)

	bool IsCameraInInspection( const CString& p_rName ) const;

	bool IsResetStateSet( unsigned long p_State ) const;
	void AddResetState( unsigned long p_State );
	void RemoveResetState( unsigned long p_State );

	BOOL CreateInspection( LPCTSTR szDocName );

	// Runtime Functions
	BOOL CanGoOnline();
	BOOL CanRegressionGoOnline();
	BOOL GoOnline();
	BOOL GoOffline();

	BOOL CanProcess( SVProductInfoStruct *pProduct );
	HRESULT StartProcess( SVProductInfoStruct *pProduct );

	BOOL RebuildInspectionInputList();
	
	BOOL RemoveCamera(CString sCameraName);
	
	BOOL AddInputRequest( SVValueObjectReference p_svObjectRef, const _variant_t& p_rValue );
	BOOL AddInputRequestMarker();

	HRESULT AddInputImageRequest( SVImageClass* p_psvImage, BSTR& p_rbstrValue );
	HRESULT AddInputImageFileNameRequest( SVImageClass* p_psvImage, const SVString& p_rFileName );
	HRESULT AddInputImageRequest( SVInputImageRequestInfoStructPtr p_InRequestPtr );
	HRESULT AddInputImageRequestByCameraName(CString sCameraName, CString sFileName);

	HRESULT SetConditionalHistoryProperties( SVScalarValueVector& rmapProperties, bool bResetObject = true );
	HRESULT GetConditionalHistoryProperties( SVScalarValueVector& rmapProperties );
	HRESULT GetConditionalHistoryProperties( SVScalarValueMapType& rmapProperties );
	HRESULT SetConditionalHistoryList( std::vector <SVScalarValue>* pvecData, std::vector <SVScalarValue>* pvecImages, std::vector <SVScalarValue>* pvecConditionals, bool bResetObject = true );// parameters not const because status will be returned
	HRESULT GetConditionalHistoryList( std::vector <SVScalarValue>& rvecData, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals );
	HRESULT GetConditionalHistoryAndClear( std::vector < std::vector <SVScalarValue> >& rvecData, std::vector < std::vector <SVImageBufferStruct> >& rvecImages, std::vector < std::vector <SVScalarValue> >& rvecConditionals, std::vector<long>& rvecProcessCount );
	HRESULT GetMostRecentConditionalHistory( std::vector <SVScalarValue>& rvecData, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount );
	HRESULT BuildConditionalHistoryListAfterLoad();

	BOOL DisconnectToolSetMainImage();
	BOOL ConnectToolSetMainImage();

	BOOL GetNewDisableMethod();
	void SetNewDisableMethod( BOOL bNewDisableMethod ); 

	virtual long GetEnableAuxiliaryExtent() const override;
	virtual void SetEnableAuxiliaryExtent( long p_lEnableAuxiliaryExtents ) override;

	//new GetOverlay method for use with the ActiveX
	HRESULT CollectOverlayData(SVImageClass *p_pImage, SVImageOverlayClass *p_pOverlayData);
	HRESULT CollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray);

	SVProductInfoStruct LastProductGet( SVDataManagerLockTypeEnum p_LockType ) const;
	HRESULT LastProductUpdate( SVProductInfoStruct *p_psvProduct );

	HRESULT LastProductNotify();

	HRESULT GetNextAvailableIndexes( SVInspectionInfoStruct& p_rInspectionInfo, SVDataManagerLockTypeEnum p_LockType ) const;

	void DumpDMInfo( LPCTSTR p_szName ) const;

	SVToolSetClass* GetToolSet() const;
	SVResultListClass* GetResultList() const;

	SVImageClass* GetRGBMainImage();
	SVImageClass* GetHSIMainImage();

	long GetGageToolCount() const;

	void GageToolCountInc();
	void GageToolCountDec();

	void SetInvalid();

	HRESULT RebuildInspection();
	void ValidateAndInitialize( bool p_Validate, bool p_IsNew );
	void ClearResetCounts();
	void SetResetCounts( );

	SVPublishListClass& GetPublishList();

	LPCTSTR GetDeviceName() const;
	void SetDeviceName( LPCTSTR p_szDeviceName );

	BOOL IsColorInspectionDocument() const;

	int UpdateMainImagesByProduct( SVProductInfoStruct* p_psvProduct );
	bool IsColorCamera() const;


	LPCTSTR GetToolsetImage();
	void SetToolsetImage( CString sToolsetImage );

	SVCameraImageTemplate* GetToolSetMainImage();

	SVVirtualCamera* GetFirstCamera() const;
	SVVirtualCamera* GetFirstPPQCamera() const;
	HRESULT GetCameras( SVVirtualCameraPtrSet& p_rCameras ) const;
	HRESULT GetPPQCameras( SVVirtualCameraPtrSet& p_rCameras ) const;
	HRESULT GetCamerasForLut( SVVirtualCameraPtrSet& p_rCameras ) const;
	HRESULT GetCamerasForLightReference( SVVirtualCameraPtrSet& p_rCameras ) const;

	HRESULT AddSharedCamera( SVVirtualCamera* pCamera );

	HRESULT GetMainImages( const CString& p_rCameraName, SVCameraImagePtrSet& p_rMainImages ) const;

	HRESULT RemoveImage(SVImageClass* pImage);

	HRESULT UpdateSharedMemoryFilters( const SVMonitorList& p_rMonitorList );
	HRESULT UpdateSharedMemoryLastInspectedImages( const SVMonitorItemList& p_rImageList );
	virtual void Persist(SVObjectWriter& rWriter);

	long GetResultDataIndex() const;

	//************************************
	//! Get all active ppqVariables for the used inspection.
	//! \returns vector of objects
	//************************************
	SVObjectVector getPPQVariables() const;
	
	//************************************
	//! Check if the pValueObject is a active ppqVarable for the used inspection.
	//! \param pValueObject [in]
	//! \returns 
	//************************************
	bool IsEnabledPPQVariable(SVValueObjectClass* pValueObject);
	
	
	//************************************
	//! Check if the pValueObject is an inactive ppqVarable for the used inspection.
	//! \param pValueObject [in]
	//! \returns 
	//************************************
	bool IsDisabledPPQVariable(SVValueObjectClass* pValueObject);

	
	
	virtual DWORD GetObjectColor() const;

	
	
	
	//************************************
	//! calls pf for all object in Inspection 
	//! \param pf [in] function pointer 
	//! \param counter [out] sum over results from pf
	//! \returns bool true if all call from pf are not negativ
	//************************************
	bool   LoopOverTools(pToolFunc pf, int& counter ); 
	
	
	SVIOEntryStructVector m_PPQInputs;

	bool m_bForceOffsetUpdate; // Force Global Extent data to update

	CStringArray m_arViewedInputNames;

	 
protected:
	typedef std::map< SVString, SVGUID > SVFilterElementMap;

	struct SVSharedMemoryFilters
	{
		SVSharedMemoryFilters();

		void clear();
		
		SVFilterElementMap m_LastInspectedValues;
		SVFilterElementMap m_LastInspectedImages;
	};

#ifdef EnableTracking
	struct SVInspectionTrackingElement
	{
		typedef std::map< unsigned __int64, long > SVTimeCountMap;

		SVInspectionTrackingElement();
		SVInspectionTrackingElement( const SVInspectionTrackingElement& p_rObject );

		virtual ~SVInspectionTrackingElement();

		void clear();

		SVClock::SVTimeStamp m_StartTime;
		SVTimeCountMap m_Start;
		SVTimeCountMap m_End;
		SVTimeCountMap m_Duration;
	};

	struct SVInspectionTracking
	{
		typedef std::map< SVString, SVInspectionTrackingElement > SVEventTrackingMap;

		SVInspectionTracking();
		SVInspectionTracking( const SVInspectionTracking& p_rObject );

		virtual ~SVInspectionTracking();

		void clear();

		void SetStartTime();

		void EventStart( const SVString& p_rName );
		void EventEnd( const SVString& p_rName );

		SVClock::SVTimeStamp m_StartTime;
		SVEventTrackingMap m_EventCounts;
	};
#endif //EnableTracking

	typedef void ( CALLBACK * SVAPCSignalHandler )( DWORD_PTR );
	typedef boost::function<void ( bool& )> SVThreadProcessHandler;

	typedef SVTQueueObject< SVCommandTemplatePtr > SVCommandQueue;
	typedef SVTQueueObject< SVMonitorList > SVMonitorListQueue;
	typedef SVTQueueObject< SVMonitorItemList > SVImageNameDequeQueue;
	typedef SVTQueueObject< SVInputRequestInfoStructPtr > SVInputRequestQueue;
	typedef SVTQueueObject< SVInputImageRequestInfoStructPtr > SVInputImageRequestQueue;
	typedef SVTQueueObject< SVProductInfoStruct > SVProductQueue;
	typedef SVTQueueObject<SVInspectionTransactionStruct> SVTransactionQueueType;
	typedef SVSingleLockT<SVTransactionQueueType> SVTransactionQueueObjectLock;
	
	virtual HRESULT SubmitCommand( const SVCommandTemplatePtr& p_rCommandPtr );

	virtual SVObjectPtrDeque GetPreProcessObjects() const;
	virtual SVObjectPtrDeque GetPostProcessObjects() const;

	virtual SVObjectClass* UpdateObject( const GUID &friendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner );

	BOOL RunOnce( SVToolClass* p_psvTool = nullptr );
	BOOL RunInspection( long lResultDataIndex, SVImageIndexStruct svResultImageIndex, SVProductInfoStruct *pProduct, bool p_UpdateCounts = true );

	BOOL DestroyInspection();

	HRESULT InitializeRunOnce();

	HRESULT BuildValueObjectMap();

	HRESULT GetInspectionValueObject( const CString& p_strName, SVValueObjectReference& p_rRefObject );
	HRESULT GetInspectionImage( const CString& p_strName, SVImageClass*& p_rRefObject );
	HRESULT GetInspectionObject( const CString& p_strName, SVObjectReference& p_rRefObject );

	BOOL AddInputRequest( SVInputRequestInfoStructPtr p_pInRequest );

	BOOL RemoveAllInputRequests();

	BOOL ProcessInputRequests( long p_DataIndex, bool &p_rbForceOffsetUpdate );
	BOOL ProcessInputRequests( long p_DataIndex, SVResetItemEnum &p_reResetItem, SVStdMapSVToolClassPtrSVInspectionProcessResetStruct &p_svToolMap );
	BOOL ProcessInputImageRequests( SVProductInfoStruct *p_psvProduct );

	HRESULT ReserveNextResultImage( SVProductInfoStruct *p_pProduct, SVDataManagerLockTypeEnum p_eLockType, bool p_ClearOtherInspections = false );

	HRESULT LastProductCopySourceImagesTo( SVProductInfoStruct *p_psvProduct );

	HRESULT SetSourceImagesTo( SVProductInfoStruct *p_psvProduct );

	HRESULT RestoreCameraImages();

	template<typename T>
	HRESULT SetObjectArrayValues(SVValueObjectReference & object, int bucket, const CString & values, bool & reset);

	void SingleRunModeLoop( bool p_Refresh = false );

	HRESULT OnlyCopyForward( SVRunStatusClass& rRunStatus );

	HRESULT CreateResultImageIndexManager() const;

	static void CALLBACK APCThreadProcess( DWORD_PTR dwParam );

	void ThreadProcess( bool& p_WaitForEvents );

	HRESULT ProcessInspection( bool& p_rProcessed, SVProductInfoStruct& p_rProduct );
	HRESULT ProcessMonitorLists( bool& p_rProcessed );
	HRESULT ProcessLastInspectedImages( bool& p_rProcessed );
	HRESULT ProcessNotifyWithLastInspected( bool& p_rProcessed, long sharedSlotIndex );
	HRESULT ProcessConditionalHistory( bool& p_rProcessed );
	HRESULT ProcessCommandQueue( bool& p_rProcessed );

	SVGUID m_PPQId;

	// DataManager index handles
	mutable SVSmartIndexArrayHandlePtr m_pResultImageCircleBuffer;

	mutable SVAsyncProcedure< SVAPCSignalHandler, SVThreadProcessHandler > m_AsyncProcedure;
	long m_NotifyWithLastInspected;

	volatile long m_lInputRequestMarkerCount;
	SVInputRequestQueue m_InputRequests;
	SVInputImageRequestQueue m_InputImageRequests;

	long m_lGageToolCount;

	SVRGBMainImageClass m_rgbMainImageObject;

	SVCameraImagePtrSet m_CameraImages;

	// Published List
	SVPublishListClass m_publishList;

	// Run status of last tool set run, if any...
	SVRunStatusClass m_runStatus;

	// Inspection pointers
	SVToolSetClass* m_pCurrentToolset;

	volatile bool m_bInspecting;

	SVTransactionQueueType m_qTransactions;// don't AddTail directly; call AddTransaction

	// Inspection Queue
	SVProductQueue       m_qInspectionsQueue;

	// Map of All Value Objects
	SVValueObjectMap m_mapValueObjects;

	SVResetStruct m_svReset;

	SVValueObjectClassPtrSet m_svValueObjectSet;
	SVImageClassPtrSet m_svImageObjectSet;

	CString m_ToolSetCameraName;
	CString m_DeviceName;

#ifdef EnableTracking
	SVInspectionTracking m_InspectionTracking;
#endif

private:
	void Init();
	HRESULT AddTransaction( SVInspectionTransactionStruct& p_rMessage );
	HRESULT ProcessTransaction( SVInspectionTransactionStruct p_Message );
	HRESULT CollectConditionalHistoryData();

	HRESULT FindPPQInputObjectByName( SVObjectClass*& p_rpObject, LPCTSTR p_FullName ) const;

	void FillSharedData(long sharedSlotIndex, SVSharedData& rData, const SVFilterElementMap& rValues, const SVFilterElementMap& rImages, SVProductInfoStruct& rProductInfo, SeidenaderVision::SVSharedInspectionWriter& rWriter);

	DWORD m_dwCHTimeout;
	SVConditionalHistory  m_ConditionalHistory;

	SVCriticalSectionPtr m_LastRunLockPtr;
	bool m_LastRunProductNULL;
	SVProductInfoStruct m_svLastRunProduct;

	BOOL                m_bNewDisableMethod;
	long                m_lEnableAuxiliaryExtents;

	DWORD               m_dwThreadId;

	// JMS - this variable is only used for configuration conversion.
	SVConditionalClass* m_pToolSetConditional;

	SVCommandQueue m_CommandQueue;
	SVMonitorListQueue m_MonitorListQueue;
	SVImageNameDequeQueue m_ImageNameDequeQueue;
	SVSharedMemoryFilters m_SharedMemoryFilters;
};

typedef SVVector< SVInspectionProcess* > SVInspectionProcessArray;

namespace SVDetail
{
	typedef std::string String;

	template<typename T>
	struct ValueObjectTraits
	{
		typedef T value_type;
		typedef SVValueObjectClassImpl<T> object_type;
		inline static bool validate(const String &) { return true; }
	};

	template<>
	struct ValueObjectTraits<CFile>
	{
		typedef CString value_type;
		typedef SVValueObjectClassImpl<CString> object_type;
		inline static bool validate(const String & str)
		{
			CFileStatus rStatus;
			return CFile::GetStatus( str.c_str(), rStatus ) != 0 && 0L <= rStatus.m_size;
		}
	};

	template<typename T>
	inline T str2int(const String & str)
	{
		int base = (str[0] == '0' && toupper(str[1]) == 'X')?16:10;
		return static_cast<T>(_tcstol(str.c_str(), NULL, base));
	}

	template<typename T>
	inline T str2(const String & str)
	{
		if (boost::is_integral<T>::value)
			return str2int<T>(str);
		else
			return boost::lexical_cast<T>(str);
	}

	template<>
	inline CString str2<CString>(const String & str)
	{
		return str.c_str();
	}

	typedef boost::function<bool (const String &)> Validator;
}

template<typename T>
inline HRESULT Parse(std::vector<T> & vec, const CString & values, SVDetail::Validator validate)
{
	const TCHAR * escape = _T("\\");
	const TCHAR * separator = _T(",");
	const TCHAR * quote = _T("`\'");
	if (values.IsEmpty())
	{
		return S_FALSE;
	}
	typedef SVDetail::String String;
	typedef String::const_iterator Iter;
	typedef boost::escaped_list_separator<TCHAR> Sep;
	typedef boost::tokenizer<Sep, Iter, String> Tokenizer;
	String str = values;
	Sep sep(escape, separator, quote);
	Tokenizer tok(str, sep);
	try
	{
		for (Tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
		{
			if (validate(*it))
			{
				vec.push_back(SVDetail::str2<T>(*it));
			}
			else
			{
				return E_INVALIDARG;
			}
		}
	}
	catch(std::exception & )
	{
		return E_FAIL;
	}
	return S_OK;
}

template<typename T>
inline HRESULT SVInspectionProcess::SetObjectArrayValues(SVValueObjectReference & object, int bucket, const CString & values, bool & reset)
{
	ASSERT(object->IsCreated() && object->IsValid() && object->IsArray() && object.IsEntireArray());
	typedef typename SVDetail::ValueObjectTraits<T> Traits;
	typedef typename SVDetail::ValueObjectTraits<T>::value_type Type;
	typedef typename SVDetail::ValueObjectTraits<T>::object_type ObjectType;
	std::vector<Type> vec;
	ObjectType * vo = dynamic_cast<ObjectType *>(object.Object());
	if (vo)
	{
		HRESULT hr = Parse(vec, values, Traits::validate);
		if (S_OK == hr)
		{
			hr = object.SetArrayValues(1, vec);
		}
		if (S_OK == hr && bucket != 1)
		{
			hr = object.SetArrayValues(bucket, vec);
		}
		reset = S_OK == hr;
		return hr;
	}
	else
	{
		return TYPE_E_TYPEMISMATCH;
	}
}



