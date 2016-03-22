//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommand.h
//* .File Name       : $Workfile:   SVCommand.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Feb 2014 08:05:20  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVActiveXLockStruct.h"	// Added by ClassView
#include "SVInfoStructs.h"

// BRW - SVImageCompression has been deprecated.
#ifndef _WIN64
class SVImageCompressionClass;
#endif
class SVValueObjectClass;
class SVInspectionProcess;

enum SVIMCOMMANDSRV_VERSION
{
	SIAC_VERSION_UNKNOWN,
	SIAC_VERSION_01_00,
	SIAC_VERSION_01_10,
	SIAC_VERSION_01_20,
	SIAC_VERSION_02_00
};// end enum

struct StreamDataStruct
{
    SVValueObjectClass *pValueObject;
	SVGUID m_InspectionID;
    CString strValueName;
	long arrayIndex;

	StreamDataStruct()
	: pValueObject( NULL ), m_InspectionID(), strValueName(), arrayIndex( 0 ) {}
};

struct PacketDataStruct
{
    SVValueObjectClass *pValueObject;
	CString strValue;
	long lState;

	PacketDataStruct()
	{
		pValueObject = NULL;
		strValue.Empty();
		lState = PRODUCT_NOT_INSPECTED;
	}// end ctor
};

struct ProductDataStruct
{
    long lProductCount;
	long lCallbackCount;
	SVVector< PacketDataStruct > arPacketData;

	ProductDataStruct()
	{
		lProductCount	= -1;
		lCallbackCount	= 0;
		arPacketData.RemoveAll();
	}// end ctor
};

#include "SVObserverCP.h"
#define SERVER_COMPRESSION_POOL_SIZE 5

/**
@SVObjectName COM Command Server

@SVObjectOverview This object process the COM commands that are sent from the SIAC software.  This object services the ability the get state information about the application, load configurations, save configurations, get parameter and image data, set parameter and image data, and create or modify OCR font files.

@SVObjectOperations This object presents operations to perform the following functions: Get Application State, Load Configuration, Save Configuration, Get Parameter Data, Get Image Data, Set Parameter Data, Set Image Data, Stream Parameter Data, Collect Condition Image and Parameter Data, and Create and Modify OCR Font Files.

*/
class ATL_NO_VTABLE CSVCommand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSVCommand, &CLSID_SVCommand>,
	public IDispatchImpl<ISVCommand, &IID_ISVCommand, &LIBID_SVObserver>,
	public IConnectionPointContainerImpl<CSVCommand>,
	public CProxy_ISVCommandEvents< CSVCommand >,
	public CProxy_ISVCommandObserverEvents< CSVCommand >
{
public:
	CSVCommand();
	
DECLARE_REGISTRY_RESOURCEID(IDR_SVCOMMAND)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSVCommand)
	COM_INTERFACE_ENTRY(ISVCommand)
	COM_INTERFACE_ENTRY2(IDispatch, ISVCommand)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CSVCommand)
	CONNECTION_POINT_ENTRY(IID__ISVCommandEvents)
	CONNECTION_POINT_ENTRY(IID__ISVCommandObserverEvents)
END_CONNECTION_POINT_MAP()

// ISVCommand
public:
	//Registers a callback function to be used by the remote 
	//command processor to deliver commands and parameters to 
	//SVObserver.
	//
	//Parameters:
//	STDMETHOD (RegisterCallback)();

	//Saves the configuration file supplied and initiates 
	//loading it into SVObserver.
	//
	//Parameters:
	//
	//XMLConfig - A BSTR containing the configuration file as 
	//well-formed XML.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (PutSVIMConfig)(BSTR XMLConfig, BSTR* pXMLError);

	//Retrieves the name of the currently loaded 
	//configuration from SVObserver.
	//
	//Parameters:
	//
	//pszConfigFileName - Pointer to a BSTR that will receive 
	//the fully qualified name of the configuration.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMConfigName)(BSTR * pszXMLData, BSTR* pXMLError);

	//Retreive a file from the SVIM.
	//
	//Parameters:
	//
	//szSourceFile - a BSTR containing the name of the file 
	//to retreive from the SVIM.
	//
	//pszXMLDestFile - Pointer to a BSTR that will receive 
	//the file data as well-formed XML
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMFile)(BSTR * pszXMLData, BSTR* pXMLError);

	//Saves a file on the SVIM.
	//
	//Parameters:
	//
	//szXMLSourceFile - a BSTR containing the file to save in 
	//well-formed XML format.
	//
	//szDestFile - A BSTR containing the name of the file 
	//that is to be saved.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (PutSVIMFile)(BSTR szXMLData, BSTR* pXMLError);

	//Sets the SVIM state to online of offline
	//
	//Parameters:
	//
	//bSVIMState - boolean that indicates if the SVIM is to 
	//be set Online (TRUE) or Offline (FALSE).
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SetSVIMState)(BSTR szXMLData, BSTR* pXMLError);

	//Retrieves the currently loaded configuration from 
	//SVObserver.
	//
	//Parameters:
	//
	//pXMLConfig - Pointer to a BSTR that will receive the 
	//configuration from SVObserver in well-formed XML format.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMConfig)(BSTR* pXMLConfig, BSTR* pXMLError);

	//Causes SVObserver to load the specified configuration.
	//
	//Parameters:
	//
	//szConfigFileName - a BSTR containing the name of the 
	//configuration to load.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (LoadSVIMConfig)(BSTR szXMLData, BSTR* pXMLError);

	//Retrieve the current inspection results from SVObserver
	//
	//Parameters:
	//
	//pXMLInspectionResults - Pointer to a BSTR that will 
	//receive the inspection results as well-formed XML.
	//
	//pXMLError - Pointer to a BSTR that will receive error 
	//information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (GetSVIMInspectionResults)(BSTR* pszXMLData, BSTR* pXMLError);

	//Retrieves the current operating state of the SVIM.
	//
	//Parameters:
	//
	//pXMLState - Pointer to a BSTR variable where the SVIM 
	//state will be stored as a well formed XML document.
	//
	//pXMLError - Pointer to a BSTR variable where the error 
	//information will be stored as a well formed XML 
	//document.
	//
	//Return Values:
	//
	//S_OK  if the function was successfully called. If 
	//the function returns S_OK, the pscSVIMError value 
	//can be checked to determine if the function 
	//executed successfully.
	//
	//Other - see COM documentation.
	STDMETHOD(GetSVIMState)(/*[out]*/ BSTR *pXMLState, /*[out]*/ BSTR *pXMLError);

//  The following group of functions are the same as above but they
//  do NOT use XML to wrap each function call. These functions also
//  break large files up into small blocks to decrease memory use.

	//Unpacks the configuration file supplied from PutSVIMFile and initiates loading it into SVObserver.
	//
	//Parameters:
	//
	//bstrConfigFilename - A BSTR containing the filename of the configuration file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVPutSVIMConfig)(long lOffset, long lBlockSize, BSTR *bstrFileData, BOOL bLastFlag);

	//Retrieves the name of the currently loaded configuration from SVObserver.
	//
	//Parameters:
	//
	//bstrConfigFilename - Pointer to a BSTR that will receive the fully qualified name of the configuration.
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - a BSTR that contains the file data block
	//
	//bLastFlag - BOOLEAN to determine last block of file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMConfigName)(BSTR *bstrConfigFilename);

	//Retrieve a file from the SVIM.
	//
	//Parameters:
	//
	//bstrSourceFile - a BSTR containing the name of the file to retreive from the SVIM.
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - Pointer to a BSTR that will receive the file data block
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMFile)(BSTR bstrSourceFile, long lOffset, long *lBlockSize, BSTR *bstrFileData,long *lLastPacketFlag);

	//Saves a file on the SVIM.
	//
	//Parameters:
	//
	//bstrDestFile - a BSTR containing the name of the file to write to the SVIM.
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - a BSTR that contains the file data block
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVPutSVIMFile)(BSTR bstrDestFile, long lOffset, long lBlockSize, BSTR *bstrFileData);

	//Sets the SVIM state to online of offline
	//
	//Parameters:
	//
	//bSVIMState - boolean that indicates if the SVIM is to be set Online (TRUE) or Offline (FALSE).
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVSetSVIMState)(unsigned long ulSVIMState);

	//Packs the configuration into the configuration file supplied to be read with GetSVIMFile
	//
	//Parameters:
	//
	//bstrConfigFilename - A BSTR containing the filename of the configuration file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMConfig)(long lOffset, long *lBlockSize, BSTR *bstrFileData, BOOL *bLastFlag);

	//Causes SVObserver to load the specified configuration.
	//
	//Parameters:
	//
	//bstrConfigFilename - A BSTR containing the filename of the configuration file
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrFileData - a BSTR that contains the file data block
	//
	//bLastFlag - BOOLEAN to determine last block of file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVLoadSVIMConfig)(BSTR bstrConfigFilename);

	//Retrieve the current inspection results from SVObserver
	//
	//Parameters:
	//
	//bstrInspection - BSTR that contains the name of the inpection document to get results from.
	//
	//bstrXMLResults - Pointer to a BSTR that will receive the inspection results as well-formed XML.
	//
	//bstrXMLError - Pointer to a BSTR that will receive error information as well-formed XML
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD (SVGetSVIMInspectionResults)(BSTR bstrInspection, BSTR *bstrXMLResults);

	//Retrieves the current operating state of the SVIM.
	//
	//Parameters:
	//
	//ulState - Pointer to a unsigned long where the SVIM state will be stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMState)(unsigned long *ulState);

	//Sets Svim mode.
	//
	//Parameters:
	//
	//lSVIMMode - unsigned long has the SVIM mode to be set.
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVSetSVIMMode)(unsigned long lSVIMMode);

	//Gets Svim mode.
	//
	//Parameters:
	//
	//p_plSVIMMode - Pointer to an unsigned long where the new SVIM mode is stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHODIMP SVGetSVIMMode(unsigned long* p_plSVIMMode);

	//Retrieves the current server version of the COM subsystem of SVObserver
	//
	//Parameters:
	//
	//ulVersion - Pointer to an unsigned long where the server version value will be stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMVersion)(unsigned long *ulVersion);

	//Retrieves the current number of times that SVObserver has gone offline
	//Remote users can use this to determine if the SVIM has potentially been modified
	//Then they can call SVGetSVIMConfigPrint and compare the configuration to a known one
	//
	//Parameters:
	//
	//ulVersion - Pointer to an unsigned long where the offline count will be stored
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMOfflineCount)(unsigned long *ulOfflineCount);

	//Retrieves the current configuration print data for the loaded configuration
	//
	//Parameters:
	//
	//lOffset - BYTE offset from the beginning of the file for this block
	//
	//lBlockSize - Size for this block of data
	//
	//bstrConfigPrint - Pointer to a BSTR where the config print data will be stored
	//
	//bLastFlag - BOOLEAN to determine last block of file
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetSVIMConfigPrint)(long lOffset, long *lBlockSize, BSTR *bstrConfigPrint, BOOL *bLastFlag);

	//Retrieves a list of images from the last inspection(s) that was run
	//
	//Parameters:
	//
	//saImageNames - A VARIANT holding a SAFEARRAY of BSTRs that contains a list of the names of the images to get
	//
	//saImages - A Pointer to a VARIANT holding a SAFEARRAY of BSTRs that contains a list of the bits of the images
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetImageList)(SAFEARRAY* psaImageNames, long lCompression, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts);

	//Runs the specified Inspection Process (IP). The correct Inputs and Source
	// Images must already contain the correct values for this inspection.
	//
	//Parameters:
	//
	//bstrNameOfIPToRun -  BSTR containing the name of the process to be 
	// executed.
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVRegisterStream)(SAFEARRAY* psaName, VARIANT vtInterface, SAFEARRAY** ppsaStatus);

	STDMETHOD(SVRunOnce)(BSTR bstrNameOfIPToRun);
	
	STDMETHOD(SVSetSourceImage)(BSTR bstrImageName, BSTR bstrImage);
	
	STDMETHOD(SVSetInputs)(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY** ppsaStatus);

    STDMETHOD(SVUnRegisterStream)(VARIANT vtInterface);

	STDMETHOD(SVGetProductDataList)(long lProcessCount, SAFEARRAY* psaNames, SAFEARRAY** ppsaData, SAFEARRAY** ppsaStatus);

	STDMETHOD(SVGetProductImageList)(long lProcessCount, SAFEARRAY* psaNames, long lCompression, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays, SAFEARRAY** ppsaStatus);

    STDMETHOD(SVGetDataList)(SAFEARRAY* psaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus, SAFEARRAY** ppsaProcCounts);

	STDMETHOD(SVGetLUT)(BSTR bstrCameraName, SAFEARRAY** ppaulLUTTable);

	STDMETHOD(SVSetLUT)(BSTR bstrCameraName, SAFEARRAY* paulLUTTable);

	STDMETHOD(SVSetImageList)(SAFEARRAY *psaNames, SAFEARRAY *psaImages, SAFEARRAY **ppsaStatus);

    STDMETHOD(SVSetToolParameterList)(SAFEARRAY* psaNames, SAFEARRAY* psaValues, SAFEARRAY** ppsaStatus);

	STDMETHOD(SVUnlockAllImages)();

	STDMETHOD(SVUnlockImage)(long lindex);

	STDMETHOD(SVGetLockedImage)(long lindex, long lCompression, BSTR *bsImage, BSTR *bsOverlay);

	STDMETHOD(SVLockImage)(long lProcessCount, long lindex, BSTR bsName);
	//Retrieves a count of the number of Remote Inputs that are available
	//
	//Parameters:
	//
	//lCount - A Pointer to a long that will contain the returned count
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVGetRemoteInputCount)(long *lCount);

	//Sets a Remote Input at the specified index to the specified value
	//
	//Parameters:
	//
	//lIndex - A long that contains the index of the Remote Input to set
	//
	//bstrValue - A BSTR that contains the value to set in the Remote Input
	//
	//Return Values:
	//
	//S_OK indicates success.
	STDMETHOD(SVSetRemoteInput)(long lIndex, VARIANT vtValue);


	// Conditional History functions
	STDMETHOD(SVSetConditionalHistoryProperties)(BSTR bstrInspectionName, SAFEARRAY*  psaNames,  SAFEARRAY*  psaValues,  SAFEARRAY** ppsaStatus);
	STDMETHOD(SVGetConditionalHistoryProperties)(BSTR bstrInspectionName, SAFEARRAY** ppsaNames, SAFEARRAY** ppsaValues, SAFEARRAY** ppsaStatus);
	STDMETHOD(SVSetConditionalHistoryList)(BSTR bstrInspectionName, SAFEARRAY*  psaValueNames,  SAFEARRAY*  psaImageNames,  SAFEARRAY*  psaConditionalNames,  SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaConditionalStatus);
	STDMETHOD(SVGetConditionalHistoryList)(BSTR bstrInspectionName, SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaConditionalStatus);
	STDMETHOD(SVGetConditionalHistoryAndClear)  (BSTR bstrInspectionName, long lCompression,
		SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays,
		SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaValues,
		SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaConditionalValues,
		SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaConditionalStatus,
		SAFEARRAY** ppsaProcessCount );
	STDMETHOD(SVGetMostRecentConditionalHistory)(BSTR bstrInspectionName, long lCompression,
		SAFEARRAY** ppsaImageNames, SAFEARRAY** ppsaImages, SAFEARRAY** ppsaOverlays,
		SAFEARRAY** ppsaValueNames, SAFEARRAY** ppsaValues,
		SAFEARRAY** ppsaConditionalNames, SAFEARRAY** ppsaConditionalValues,
		SAFEARRAY** ppsaImageStatus, SAFEARRAY** ppsaValueStatus, SAFEARRAY** ppsaConditionalStatus,
		long* ppsaProcessCount );





	// New functions to support Matrox Font Aalyzers - // Stage 1 runtime only, no setup.
	STDMETHOD(SVConnectFont)(long* lFontIdentifier);

	STDMETHOD(SVDisconnectFont)(long lFontIdentifier);

	STDMETHOD(SVGetCellSize)(long lFontIdentifier, long* plWidth, long* plHeight, long* plSpacing);

	STDMETHOD(SVSetCellSize)(long lFontIdentifier, long lWidth, long lHeight);

	STDMETHOD(SVGetSearchSettings)(long lFontIdentifier, long* plLength, double* pdStringAcceptance, double* pdCharacterAcceptance, BSTR* pbstrInvalidCharacter, BSTR* pbstrSearchSpeed);

	STDMETHOD(SVSetSearchSettings)(long lFontIdentifier, long lLength, double dStringAcceptance, double dCharacterAcceptance, BSTR bstrInvalidCharacter, BSTR bstrSearchSpeed);

	STDMETHOD(SVGetPositionVariance)(long lFontIdentifier, double* pdXDirection, double* pdYDirection);
 
	STDMETHOD(SVSetPositionVariance)(long lFontIdentifier, double dXDirection, double dYDirection);

	STDMETHOD(SVGetSearchAngleSettings)(long lFontIdentifier, double* pdSearchAngle, long* plInterpolation, double* pdDeltaPositive, double* pdDeltaNegative);

	STDMETHOD(SVSetSearchAngleSettings)(long lFontIdentifier, double dSearchAngle, long lInterpolation, double dDeltaPositive, double dDeltaNegative);

	STDMETHOD(SVGetCharacterEnhancementSettings)(long lFontIdentifier, long* plMorphIterations, double* pdThickenCharacters, long* plBrokenCharacters, long* plTouchingCharacters);

	STDMETHOD(SVSetCharacterEnhancementSettings)(long lFontIdentifier, long lMorphIterations, double dThickenCharacters, long lBrokenCharacters, long lTouchingCharacters);

	STDMETHOD(SVLoadFont)(long lFontIdentifier, BSTR bstrFontFile, BSTR bstrFontControls, BSTR bstrFontConstraints);

	STDMETHOD(SVSaveFont)(long lFontIdentifier, BSTR* bstrFontFile, BSTR* bstrFontControls, BSTR* bstrFontConstraints);

	STDMETHOD(SVCalibrateFont)(long lFontIdentifier, BSTR bstrCalibrateString, BSTR bstrCalibrateImage, double dXMinSize, double dXMaxSize, double dXStepSize, double dYMinSize, double dYMaxSize, double dYStepSize );

	STDMETHOD(SVReadString)(long lFontIdentifier, BSTR* bstrFoundString, BSTR bstrReadImage, double* dMatchScore );

	STDMETHOD(SVVerifyString)(long lFontIdentifier, BSTR bstrVerifyString, BSTR bstrVerifyImage, double* dMatchScore );

	STDMETHOD(SVGetConstraints)(long lFontIdentifier, long* plPosition, long* plType, BSTR* pbstrValues);

	STDMETHOD(SVSetConstraints)(long lFontIdentifier, long lPosition, long lType, BSTR bstrValues);

	STDMETHOD(SVCreateNew)(long lFontIdentifier, BSTR bstrImage);

	STDMETHOD(SVSetTrainingImage)(long lFontIdentifier, BSTR bstrImage);

	STDMETHOD(SVAddCharacter)(long lFontIdentifier, long lXPosition, long lYPosition, long lHeight, long lWidth, BSTR bstrLabel, long *lCharID);

	STDMETHOD(SVAddCharacters)(long lFontIdentifier, BSTR bstrLabelList);

	STDMETHOD(SVDeleteCharacters)(long lFontIdentifier, SAFEARRAY* psaCharIds);

	STDMETHOD(SVThresholdImage)(long lFontIdentifier, BSTR bstrImage, long lThreshold, BSTR* pbstrThresholdImage);

	STDMETHOD(SVGetFontCharacterCount)(long lFontIdentifier, long* plCharacterCount);

	STDMETHOD(SVGetFontCharacter)(long lFontIdentifier, long lIndex, BSTR* pbstrLabelImage);

	STDMETHOD(SVGetStringLength)(long p_lFontIdentifier, double* p_pdLength );

	STDMETHOD(SVSetStringLength)(long p_lFontIdentifier, double p_dLength );

	STDMETHOD(SVLoadFontImage)(long p_lFontIdentifier, BSTR bstrFontImage );

	STDMETHOD(SVGetFontCharacterList)(long p_lFontIdentifier, BSTR *bstrCharacterList, SAFEARRAY** ppsaCharIds);

	STDMETHOD(SVGetExpectedTargetCharacterSize)(long p_lFontIdentifier, long *plWidth, long *plHeight);

	STDMETHOD(SVGetFontCharacterSize)(long p_lFontIdentifier, long *plWidth, long *plHeight);
		// **********************************************************************************
		//////// *********** Transfer Definition List Functions ************** //////////////
		// **********************************************************************************

	STDMETHOD(SVGetTransferValueDefinitionList)(BSTR p_bstrInspectionName, 
														  long* p_plVersion, 
														  VARIANT* p_pvData);

	STDMETHOD(SVGetTransferImageDefinitionList)(BSTR p_bstrInspectionName, 
														  long* p_plVersion, 
														  VARIANT* p_pvData);

  STDMETHOD(SVConstructCommand)( long p_CommandType, ISVRemoteCommand **p_ppCommand );

  STDMETHOD(SVIsAvailiable)();

public:
	static BOOL ResetStreamingDataAndLockedImages();

//protected:
//  ImageToBSTR () - If alCompression = 0, then image compression will not 
//  be used.  More importantly the apCompressionObject will not be used, so if
//  alCompression = 0, then it is acceptable for apCompressionObject to equal 
//  NULL.	
	static HRESULT ImageToBSTR( SVImageInfoClass &rImageInfo, SVSmartHandlePointer ImageHandle, BSTR *pbstr
						// BRW - SVImageCompression has been deprecated.
#ifndef _WIN64
		, long alCompression, SVImageCompressionClass *apCompressionObject);
#else
		);
#endif

	static HRESULT SafeImageToBSTR( SVImageClass *p_pImage, SVImageIndexStruct p_svIndex, BSTR *pbstr
						// BRW - SVImageCompression has been deprecated.
#ifndef _WIN64
		, long alCompression, SVImageCompressionClass *apCompressionObject);
#else
		);
#endif
	static HRESULT BSTRToImage(bool bCreateNew, BSTR bstr, SVImageInfoClass& rImageInfo,SVSmartHandlePointer &rImageHandle);
	static SVMatroxBuffer CreateImageFromBSTR( BSTR bstrImage );
public:
    static HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
    static HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
protected:
    static HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv);

    static DWORD WINAPI SVStreamDataThread(LPVOID lpParam);

	virtual HRESULT StoreEventObserver( DWORD dwCookie, CComPtr< CSVCommand > p_pObserver );
	virtual HRESULT ReleaseEventObserver( DWORD dwCookie, CComPtr< CSVCommand > p_pObserver );

	HRESULT StreamingDataCallback( const SVInspectionCompleteInfoStruct& p_rData );
	HRESULT RebuildStreamingDataList();

    IStream *m_pStream;

private:
	static SVVector< SVActiveXLockStruct > m_aSVActXLock;
	static SVVector< StreamDataStruct* > m_arStreamList;
	static SVVector< ProductDataStruct* > m_arProductList;
	static SVVector< SVInspectionProcess* > m_arInspections;
	static CStringList m_cslInspectionNames;

    static volatile BOOL m_bRunStreamData;
    static volatile BOOL m_bRegisteredStream;
    static volatile DWORD m_dwStreamDataProcessId;
	static volatile long m_lLastStreamedProduct;
    static volatile HANDLE m_hStopStreamEvent;
    static volatile HANDLE m_hStreamingThread;
	static CRITICAL_SECTION m_hProductCritSect;
	static bool m_bCriticalSectionInitialized;
};// end class CSVCommand

OBJECT_ENTRY_AUTO( __uuidof(SVCommand), CSVCommand ) 

		