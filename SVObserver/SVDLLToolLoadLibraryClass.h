// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolLoadLibraryClass
// * .File Name       : $Workfile:   SVDLLToolLoadLibraryClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   21 Oct 2013 08:21:26  $
// ******************************************************************************

#ifndef SVDLLToolLOADLIBRARYCLASS_H
#define SVDLLToolLOADLIBRARYCLASS_H

#pragma warning (push)
#pragma warning (disable : 4290)

#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVDLLToolDefinitionStructs.h"
#include "SVLoki/functor.h"

struct SVImageDefinitionStruct;

enum SVMachineTypeEnum
{
	ImageFileUnknownMachine = 0x0,
	ImageFileMachineAM33 = 0x1d3,
	ImageFileMachineAMD64 = 0x8664,
	ImageFileMachineARM = 0x1c0,
	ImageFileMachineEBC = 0xebc,
	ImageFileMachineI386 = 0x14c,
	ImageFileMachineIA64 = 0x200,
	ImageFileMachineM32R = 0x9041,
	ImageFileMachineMIPS16 = 0x266,
	ImageFileMachineMIPSFPU = 0x366,
	ImageFileMachineMIPSFPU16 = 0x466,
	ImageFileMachinePowerPC = 0x1f0,
	ImageFileMachinePowerPCFP = 0x1f1,
	ImageFileMachineR4000 = 0x166,
	ImageFileMachineSH3 = 0x1a2,
	ImageFileMachineSH3DSP = 0x1a3,
	ImageFileMachineSH4 = 0x1a6,
	ImageFileMachineSH5 = 0x1a8,
	ImageFileMachineThumb = 0x1c2,
	ImageFileMachineWCEMIPSV2 = 0x169
};

typedef long    (__stdcall *SimpleTestPtr) (long lValue1, long lValue2);
typedef HRESULT (__stdcall *GetToolNamePtr) (BSTR* pbstrToolName);
typedef HRESULT (__stdcall *GetToolVersionPtr) (long* plVersionNumber);
typedef HRESULT (__stdcall *RunToolPtr) (GUID tool, long* plStatus);
typedef HRESULT (__stdcall *StartupPtr) ();
typedef HRESULT (__stdcall *ShutDownPtr) ();
typedef HRESULT (__stdcall *InitializeRunPtr) ( GUID tool, long lImageArraySize, SVImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues);
typedef HRESULT (__stdcall *UninitializeRunPtr) ( GUID tool );
typedef HRESULT (__stdcall *GetInputValueDefinitionsPtr) (long* plArraySize, InputValueDefinitionStruct** ppaStructs);
typedef HRESULT (__stdcall *DestroyInputValueDefinitionStructuresPtr) (InputValueDefinitionStruct* paStructs);
typedef HRESULT (__stdcall *SetInputValuesPtr) (GUID tool, long lArraySize, VARIANT* paInputValues);
typedef HRESULT (__stdcall *GetResultValuesPtr) (GUID tool, long lArraySize, VARIANT* paResultValues);
typedef HRESULT (__stdcall *GetMessageStringPtr) (unsigned long ulErrorNumber, BSTR* pbstrErrorMessage);
typedef HRESULT (__stdcall *ValidateValueParameterPtr) (GUID tool, long lParameterNumber, VARIANT vParameterValue);
typedef HRESULT (__stdcall *GetResultValueDefinitionsPtr) (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
typedef HRESULT (__stdcall *DestroyResultValueDefinitionStructuresPtr) (ResultValueDefinitionStruct* paStructs);
typedef HRESULT (__stdcall *GetNumberOfInputImagesPtr) (long* plNumberOfInputImages);
typedef HRESULT (__stdcall *SetMILInputImagesPtr) (GUID tool, long lArraySize, long* paMILhandles);
typedef HRESULT (__stdcall *SetHBITMAPInputImagesPtr) (GUID tool, HBITMAP* paHandles);
typedef HRESULT (__stdcall *GetHBITMAPResultImagesPtr) (GUID tool, long lArraySize, HBITMAP* paHandles);
typedef HRESULT (__stdcall *SetMILResultImagesPtr) (GUID tool, long lArraySize, long* paMILhandles);
typedef HRESULT (__stdcall *GetResultImageDefinitionsPtr) (GUID tool, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions);
typedef HRESULT (__stdcall *DestroyImageDefinitionStructurePtr) ( SVImageDefinitionStruct* paStructs);

typedef Loki::Functor<void, TYPELIST_1(const CString&) > SVDllLoadLibraryCallback;
typedef Loki::DefaultFunctor< void, TYPELIST_1( const CString& ) > SVDllLoadLibraryCallbackDefault;

class SVDLLToolLoadLibraryClass  
{
public:
	SVDLLToolLoadLibraryClass();
	~SVDLLToolLoadLibraryClass();

	HRESULT Open( LPCTSTR p_szLibrary, SVDllLoadLibraryCallback fnNotifyProgress = SVDllLoadLibraryCallbackDefault() );
	HRESULT Close();
	bool IsOpen();

	HRESULT GetToolName (BSTR* pbstrToolName);
	HRESULT GetToolVersion (long* plVersionNumber);
	HRESULT RunTool (GUID tool, long* plStatus);
	HRESULT Startup ();
	HRESULT ShutDown ();
	HRESULT InitializeRun ( GUID tool, long lImageArraySize, SVImageDefinitionStruct* paStructs, long lValueArraySize, VARIANT* pavInputValues);
	HRESULT UninitializeRun ( GUID tool );
	HRESULT GetInputValueDefinitions (long* plArraySize,InputValueDefinitionStruct** ppaStructs);
	HRESULT DestroyInputValueDefinitionStructures (InputValueDefinitionStruct* paStructs);
	HRESULT SetInputValues (GUID tool, long lArraySize, VARIANT* paInputValues);
	HRESULT GetResultValues (GUID tool, long lArraySize, VARIANT* paResultValues);
	HRESULT GetMessageString (unsigned long ulErrorNumber,BSTR* pbstrErrorMessage);
	HRESULT ValidateValueParameter (GUID tool, long lParameterNumber, VARIANT vParameterValue);
	HRESULT GetResultValueDefinitions (long* plArraySize, ResultValueDefinitionStruct** ppaResultValues);
	HRESULT DestroyResultValueDefinitionStructures (ResultValueDefinitionStruct* paStructs);
	HRESULT GetNumberOfInputImages (long* plNumberOfInputImages);
	HRESULT SetMILInputImages (GUID tool, long lArraySize, long* paMILhandles);
	HRESULT SetHBITMAPInputImages (GUID tool, HBITMAP* paHandles);
	HRESULT GetHBITMAPResultImages (GUID tool, long lArraySize, HBITMAP* paHandles);
	HRESULT SetMILResultImages (GUID tool, long lArraySize, long* paMILhandles);
	HRESULT GetResultImageDefinitions (GUID tool, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions);
	HRESULT DestroyImageDefinitionStructure ( SVImageDefinitionStruct* paStructs);
	bool UseMil();
	BOOL IsHandleNull();
private:
	HMODULE m_hmHandle;

	bool m_bUseMil;

	SimpleTestPtr m_pfnSimpleTest;
	GetToolNamePtr m_pfnGetToolName;
	GetToolVersionPtr m_pfnGetToolVersion;
	RunToolPtr m_pfnRunTool;
	StartupPtr m_pfnStartup;
	ShutDownPtr m_pfnShutDown;
	InitializeRunPtr m_pfnInitializeRun;
	UninitializeRunPtr m_pfnUninitializeRun;
	GetInputValueDefinitionsPtr m_pfnGetInputValueDefinitions;
	DestroyInputValueDefinitionStructuresPtr m_pfnDestroyInputValueDefinitionStructures;
	SetInputValuesPtr m_pfnSetInputValues;
	GetResultValuesPtr m_pfnGetResultValues;
	GetMessageStringPtr m_pfnGetMessageString;
	ValidateValueParameterPtr m_pfnValidateValueParameter;
	GetResultValueDefinitionsPtr m_pfnGetResultValueDefinitions;
	DestroyResultValueDefinitionStructuresPtr m_pfnDestroyResultValueDefinitionStructures;
	GetNumberOfInputImagesPtr m_pfnGetNumberOfInputImages;
	SetMILInputImagesPtr m_pfnSetMILInputImages;
	SetHBITMAPInputImagesPtr m_pfnSetHBITMAPInputImages;
	GetHBITMAPResultImagesPtr m_pfnGetHBITMAPResultImages;
	SetMILResultImagesPtr m_pfnSetMILResultImages;
	GetResultImageDefinitionsPtr m_pfnGetResultImageDefinitions;
	DestroyImageDefinitionStructurePtr m_pfnDestroyImageDefinitionStructure;
	SVMachineTypeEnum CheckBitness( LPCTSTR p_szFile );

};

#include "SVDLLToolLoadLibraryClass.inl"

#pragma warning (pop)
#endif // _SVDLLToolLOADLIBRARYCLASS_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDLLToolLoadLibraryClass.h_v  $
 * 
 *    Rev 1.2   21 Oct 2013 08:21:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check for Bitness on External Tool DLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:16:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   29 Apr 2013 14:10:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:18:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   24 May 2010 14:52:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.  Updated thread creation in PPQ and version number 5.32 Beta 002.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Dec 2009 09:01:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix the include and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Sep 2009 10:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Jun 2008 13:32:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   fixed problem with the invalid box not showing up when an external tool is first added.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jul 2007 11:47:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2007 12:27:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2004 12:40:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  417
 * SCR Title:  Create Wrapper Class for External Tool DLL
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
