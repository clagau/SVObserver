// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDLLToolLoadLibraryClass
// * .File Name       : $Workfile:   SVDLLToolLoadLibraryClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   21 Oct 2013 08:35:56  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <io.h>
#include "SVDLLToolLoadLibraryClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVSystemLibrary/SVVersionInfo.h"

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

	SVDLLToolLoadLibraryClass::SVDLLToolLoadLibraryClass()
	{
	}

	SVDLLToolLoadLibraryClass::~SVDLLToolLoadLibraryClass()
	{
		Close();
	}

	HRESULT  SVDLLToolLoadLibraryClass::InitInputValueDef(SvStl::MessageContainer& rE)
	{
		HRESULT Result{ S_OK };
		m_pfnGetInputValueDefinitionsEx = (GetInputValueDefinitionsPtrEx)::GetProcAddress(m_hmHandle, "SVGetInputValueDefinitionsEx");
		m_pfnDestroyInputValueDefinitionStructuresEx = (DestroyInputValueDefinitionStructuresPtrEx)::GetProcAddress(m_hmHandle, "SVDestroyInputValueDefinitionStructuresEx");
		if (m_pfnGetInputValueDefinitionsEx && m_pfnDestroyInputValueDefinitionStructuresEx)
		{
			m_bUseInputEx = true;
		}
		else
		{
			m_pfnGetInputValueDefinitions = (GetInputValueDefinitionsPtr)::GetProcAddress(m_hmHandle, "SVGetInputValueDefinitions");
			m_pfnDestroyInputValueDefinitionStructures = (DestroyInputValueDefinitionStructuresPtr)::GetProcAddress(m_hmHandle, "SVDestroyInputValueDefinitionStructures");
			if (nullptr == m_pfnGetInputValueDefinitions)
			{
				m_pfnGetInputValueDefinitions = (GetInputValueDefinitionsPtr)::GetProcAddress(m_hmHandle, "GetInputValueDefinitions");
			}
			if (nullptr == m_pfnDestroyInputValueDefinitionStructures)
			{
				m_pfnDestroyInputValueDefinitionStructures = (DestroyInputValueDefinitionStructuresPtr)::GetProcAddress(m_hmHandle, "DestroyInputValueDefinitionStructures");
			}
			if (m_pfnGetInputValueDefinitions && m_pfnDestroyInputValueDefinitionStructures)
			{
				m_bUseInputEx = false;
			}
			else
			{
				Result = -12350;
				if (!m_pfnGetInputValueDefinitions)
					rE.addMessage(SvStl::MessageData(static_cast<DWORD> (-12359), SvStl::Tid_GetProcAddressError_GetInputValueDefinitions), false);
				if (!m_pfnDestroyInputValueDefinitionStructures)
					rE.addMessage(SvStl::MessageData(static_cast<DWORD> (-12360), SvStl::Tid_GetProcAddressError_DestroyInputValueDefinitionStructures), false);


			}

		}

		return Result;
	}

	HRESULT  SVDLLToolLoadLibraryClass::InitResultValueDef(SvStl::MessageContainer& rE)
	{
		HRESULT Result{ S_OK };
		m_pfnDestroyResultValueDefinitionStructuresEx = (DestroyResultValueDefinitionStructuresPtrEx)::GetProcAddress(m_hmHandle, "SVDestroyResultValueDefinitionStructuresEx");
		m_pfnGetResultValueDefinitionsEx = (GetResultValueDefinitionsPtrEx)::GetProcAddress(m_hmHandle, "SVGetResultValueDefinitionsEx");
		if (m_pfnDestroyResultValueDefinitionStructuresEx && m_pfnGetResultValueDefinitionsEx)
		{
			m_bUseResultEx = true;
		}
		else
		{
			m_pfnGetResultValueDefinitions = (GetResultValueDefinitionsPtr)::GetProcAddress(m_hmHandle, "SVGetResultValueDefinitions");
			m_pfnDestroyResultValueDefinitionStructures = (DestroyResultValueDefinitionStructuresPtr)::GetProcAddress(m_hmHandle, "SVDestroyResultValueDefinitionStructures");
			if (nullptr == m_pfnGetResultValueDefinitions)
			{
				m_pfnGetResultValueDefinitions = (GetResultValueDefinitionsPtr)::GetProcAddress(m_hmHandle, "GetResultValueDefinitions");
			}
			if (nullptr == m_pfnDestroyResultValueDefinitionStructures)
			{
				m_pfnDestroyResultValueDefinitionStructures = (DestroyResultValueDefinitionStructuresPtr)::GetProcAddress(m_hmHandle, "DestroyResultValueDefinitionStructures");
			}
			if (m_pfnDestroyResultValueDefinitionStructures && m_pfnGetResultValueDefinitions)
			{
				m_bUseResultEx = false;
			}
			else
			{
				Result = -12350;
				rE.setMessage(static_cast<DWORD> (Result), SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
				if (!m_pfnDestroyInputValueDefinitionStructures)
				{
					rE.addMessage(SvStl::MessageData(static_cast<DWORD> (-12360), SvStl::Tid_GetProcAddressError_DestroyInputValueDefinitionStructures), false);
				}
				if (!m_pfnDestroyResultValueDefinitionStructures)
				{
					rE.addMessage(SvStl::MessageData(static_cast<DWORD> (-12361), SvStl::Tid_GetProcAddressError_DestroyResultValueDefinitionStructures), false);
				}
			}
		}
		if (Result == S_OK)
		{
			if (m_bUseResultEx)
			{
				m_pfnGetResultTableDefinitionsEx = (GetResultTableDefinitionsPtrEx)::GetProcAddress(m_hmHandle, "GetResultTableDefinitionsEx");
				m_pfnDestroyResultTableDefinitionStructuresEx = (DestroyResultTableDefinitionStructuresPtrEx)::GetProcAddress(m_hmHandle, "DestroyResultTableDefinitionStructuresEx");
			}
			else
			{
				m_pfnGetResultTableDefinitions = (GetResultTableDefinitionsPtr)::GetProcAddress(m_hmHandle, "GetResultTableDefinitions");
				m_pfnDestroyResultTableDefinitionStructures = (DestroyResultTableDefinitionStructuresPtr)::GetProcAddress(m_hmHandle, "DestroyResultTableDefinitionStructures");
			}
		}
		return Result;
	}

	//@TODO [Arvid][10.10][8.3.2021] Diese Methode ist _erheblich_ zu lang!
	HRESULT SVDLLToolLoadLibraryClass::Open(const std::string& rDllPath, std::vector<std::string>& rStatusMsgs)
	{
		std::filesystem::path fullPath(rDllPath);

		m_dllName = fullPath.filename().string();

		HRESULT Result(S_OK);

		SvStl::MessageContainer e;
		if (nullptr != m_hmHandle)
		{
			return Result;
		}

		rStatusMsgs.reserve(25);
		if (_access(rDllPath.c_str(), 0) == 0)
		{
			// Check bitness
			if (CheckBitness(rDllPath.c_str()) != ImageFileMachineAMD64)
			{
				std::string Message = std::format(_T("{} is not 64 Bit!"), rDllPath.c_str());
				rStatusMsgs.emplace_back(Message);
			}
		}
		rStatusMsgs.emplace_back(_T("Attempting LoadLibrary"));
		m_hmHandle = ::LoadLibrary(rDllPath.c_str());
		Sleep(0);
		if (nullptr == m_hmHandle)	// can't load library
		{
			Result = -12378;
			e.setMessage(0, SvStl::Tid_ToolLoadError, SvStl::SourceFileParams(StdMessageParams));
		}
		if (Result == S_OK) //checkResultValueDefinition
		{
			Result = InitResultValueDef(e);
		}
		if (Result == S_OK)
		{
			Result = InitInputValueDef(e);
		}

		if (Result == S_OK)
		{
			m_pfnSimpleTest = (SimpleTestPtr)::GetProcAddress(m_hmHandle, "SVSimpleTest");
			m_pfnGetToolName = (GetToolNamePtr)::GetProcAddress(m_hmHandle, "SVGetToolName");
			m_pfnGetToolVersion = (GetToolVersionPtr)::GetProcAddress(m_hmHandle, "SVGetToolVersion");
			m_pfnRunTool = (RunToolPtr)::GetProcAddress(m_hmHandle, "SVRunTool");
			m_pfnStartup = (StartupPtr)::GetProcAddress(m_hmHandle, "SVStartup");
			m_pfnShutDown = (ShutDownPtr)::GetProcAddress(m_hmHandle, "SVShutDown");
			m_pfnInitializeRun = (InitializeRunPtr)::GetProcAddress(m_hmHandle, "SVInitializeRun");
			m_pfnUninitializeRun = (UninitializeRunPtr)::GetProcAddress(m_hmHandle, "SVUninitializeRun");
			m_pfnSetInputValues = (SetInputValuesPtr)::GetProcAddress(m_hmHandle, "SVSetInputValues");
			m_pfnGetInputImageInformation = (GetInputImageInformationPtr)::GetProcAddress(m_hmHandle, "SVGetInputImageInformation");
			m_pfnDestroyInputImageInformationPtr = (DestroyInputImageInformationPtr)::GetProcAddress(m_hmHandle, "SVDestroyInputImageInformation");
			m_pfnGetResultValues = (GetResultValuesPtr)::GetProcAddress(m_hmHandle, "SVGetResultValues");
			m_pfnGetMessageString = (GetMessageStringPtr)::GetProcAddress(m_hmHandle, "SVGetErrorMessageString");
			m_pfnValidateValueParameter = (ValidateValueParameterPtr)::GetProcAddress(m_hmHandle, "SVValidateValueParameter");
			m_pfnGetNumberOfInputImages = (GetNumberOfInputImagesPtr)::GetProcAddress(m_hmHandle, "SVGetNumberOfInputImages");
			m_pfnSetMILInputImages = (SetMILInputImagesPtr)::GetProcAddress(m_hmHandle, "SVSetMILInputImages");
			m_pfnSetMILResultImages = (SetMILResultImagesPtr)::GetProcAddress(m_hmHandle, "SVSetMILResultImages");
			m_pfnSetHBITMAPInputImages = (SetHBITMAPInputImagesPtr)::GetProcAddress(m_hmHandle, "SVSetHBITMAPInputImages");
			m_pfnGetHBITMAPResultImages = (GetHBITMAPResultImagesPtr)::GetProcAddress(m_hmHandle, "SVGetHBITMAPResultImages");
			m_pfnGetResultImageDefinitions = (GetResultImageDefinitionsPtr)::GetProcAddress(m_hmHandle, "SVGetResultImageDefinitions");
			m_pfnDestroyImageDefinitionStructure = (DestroyImageDefinitionStructurePtr)::GetProcAddress(m_hmHandle, "SVDestroyImageDefinitionStructure");
			m_pfnGetResultTables = (GetResultTablesPtr)::GetProcAddress(m_hmHandle, "GetResultTables");
			m_pfnGetResultTablesMaxRowSize = (GetResultTablesMaxRowSizePtr)::GetProcAddress(m_hmHandle, "GetResultTablesMaxRowSize");
			m_pfnGetResultValuesMaxArraySize = (GetResultValuesMaxArraySizePtr)::GetProcAddress(m_hmHandle, "GetResultValuesMaxArraySize");

			// Backwards compatibility
			if (nullptr == m_pfnSimpleTest) m_pfnSimpleTest = (SimpleTestPtr)::GetProcAddress(m_hmHandle, "SimpleTest");
			if (nullptr == m_pfnGetToolName) m_pfnGetToolName = (GetToolNamePtr)::GetProcAddress(m_hmHandle, "GetToolName");
			if (nullptr == m_pfnGetToolVersion) m_pfnGetToolVersion = (GetToolVersionPtr)::GetProcAddress(m_hmHandle, "GetToolVersion");
			if (nullptr == m_pfnRunTool) m_pfnRunTool = (RunToolPtr)::GetProcAddress(m_hmHandle, "RunTool");
			if (nullptr == m_pfnStartup) m_pfnStartup = (StartupPtr)::GetProcAddress(m_hmHandle, "Startup");
			if (nullptr == m_pfnShutDown) m_pfnShutDown = (ShutDownPtr)::GetProcAddress(m_hmHandle, "ShutDown");
			if (nullptr == m_pfnInitializeRun) m_pfnInitializeRun = (InitializeRunPtr)::GetProcAddress(m_hmHandle, "InitializeRun");
			if (nullptr == m_pfnUninitializeRun) m_pfnUninitializeRun = (UninitializeRunPtr)::GetProcAddress(m_hmHandle, "UninitializeRun");
			if (nullptr == m_pfnSetInputValues) m_pfnSetInputValues = (SetInputValuesPtr)::GetProcAddress(m_hmHandle, "SetInputValues");
			if (nullptr == m_pfnGetResultValues) m_pfnGetResultValues = (GetResultValuesPtr)::GetProcAddress(m_hmHandle, "GetResultValues");
			if (nullptr == m_pfnGetMessageString) m_pfnGetMessageString = (GetMessageStringPtr)::GetProcAddress(m_hmHandle, "GetErrorMessageString");
			if (nullptr == m_pfnValidateValueParameter) m_pfnValidateValueParameter = (ValidateValueParameterPtr)::GetProcAddress(m_hmHandle, "ValidateValueParameter");

			if (nullptr == m_pfnGetNumberOfInputImages) m_pfnGetNumberOfInputImages = (GetNumberOfInputImagesPtr)::GetProcAddress(m_hmHandle, "GetNumberOfInputImages");
			if (nullptr == m_pfnSetMILInputImages) m_pfnSetMILInputImages = (SetMILInputImagesPtr)::GetProcAddress(m_hmHandle, "SetMILInputImages");
			if (nullptr == m_pfnSetMILResultImages) m_pfnSetMILResultImages = (SetMILResultImagesPtr)::GetProcAddress(m_hmHandle, "SetMILResultImages");
			if (nullptr == m_pfnSetHBITMAPInputImages) m_pfnSetHBITMAPInputImages = (SetHBITMAPInputImagesPtr)::GetProcAddress(m_hmHandle, "SetHBITMAPInputImages");
			if (nullptr == m_pfnGetHBITMAPResultImages) m_pfnGetHBITMAPResultImages = (GetHBITMAPResultImagesPtr)::GetProcAddress(m_hmHandle, "GetHBITMAPResultImages");
			if (nullptr == m_pfnGetResultImageDefinitions) m_pfnGetResultImageDefinitions = (GetResultImageDefinitionsPtr)::GetProcAddress(m_hmHandle, "GetResultImageDefinitions");
			if (nullptr == m_pfnDestroyImageDefinitionStructure) m_pfnDestroyImageDefinitionStructure = (DestroyImageDefinitionStructurePtr)::GetProcAddress(m_hmHandle, "DestroyImageDefinitionStructure");

			if (nullptr == m_pfnGetTransformationDefinitions)
			{
				m_pfnGetTransformationDefinitions = (GetTransformationDefinitionsPtr)::GetProcAddress(m_hmHandle, "GetTransformationDefinitions");
			}
			if (nullptr == m_pfnDestroyTransformationDefinitions)
			{
				m_pfnDestroyTransformationDefinitions = (DestroyTransformationDefinitionsPtr)::GetProcAddress(m_hmHandle, "DestroyTransformationDefinitions");
			}

			if ((
				nullptr != m_pfnSimpleTest &&
				nullptr != m_pfnGetToolName &&
				nullptr != m_pfnGetToolVersion &&
				nullptr != m_pfnRunTool &&
				nullptr != m_pfnStartup &&
				nullptr != m_pfnShutDown &&
				nullptr != m_pfnInitializeRun &&
				nullptr != m_pfnUninitializeRun &&
				nullptr != m_pfnSetInputValues &&
				nullptr != m_pfnGetResultValues &&
				nullptr != m_pfnGetMessageString &&
				nullptr != m_pfnValidateValueParameter &&
				nullptr != m_pfnGetNumberOfInputImages &&
				nullptr != m_pfnGetResultImageDefinitions &&
				nullptr != m_pfnDestroyImageDefinitionStructure)
				)
			{
				if (m_pfnSetMILResultImages && m_pfnSetMILInputImages
					&& !m_pfnSetHBITMAPInputImages && !m_pfnGetHBITMAPResultImages)
				{
					m_bUseMil = true;
				}
				else if (!m_pfnSetMILResultImages && !m_pfnSetMILInputImages
					&& m_pfnSetHBITMAPInputImages && m_pfnGetHBITMAPResultImages)
				{
					m_bUseMil = false;
				}
				else	// error in MIL/HBITMAP definitions
				{
					Result = -12372;
					e.setMessage(0, SvStl::Tid_ToolLoadError_ImageDef, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));

					// none are defined
					if (!m_pfnSetMILResultImages && !m_pfnSetMILInputImages
						&& !m_pfnSetHBITMAPInputImages && !m_pfnGetHBITMAPResultImages)
					{
						e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12373), SvStl::Tid_ToolLoadError_NoImageDef), false);
					}
					else if (m_pfnSetMILResultImages && m_pfnSetMILInputImages
						&& m_pfnSetHBITMAPInputImages && m_pfnGetHBITMAPResultImages)
					{
						e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12375), SvStl::Tid_ToolLoadError_BothSetsImagesDef), false);
					}
					else if ((m_pfnSetMILResultImages && (m_pfnGetHBITMAPResultImages || m_pfnSetHBITMAPInputImages))
						|| (m_pfnSetHBITMAPInputImages && (m_pfnSetMILInputImages || m_pfnSetMILResultImages)))
					{
						e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12376), SvStl::Tid_ToolLoadError_BothImagesDef), false);
					}
					else
					{
						e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12374), SvStl::Tid_ToolLoadError_InFunctionDef), false);
					}

				}// end else	// error in MIL/HBITMAP definitions

				if (S_OK == Result)
				{

					rStatusMsgs.emplace_back(_T("Attempting SimpleTest"));

					try
					{
						long version = SvSyl::getSvoVersionAsUnsignedLong() >> 8;
						long versionShort = (version >> 8) * 100;
						versionShort += version & 0xFF;
						long lTest = m_pfnSimpleTest(versionShort, 2); //the first parameter is the version number of the SVOberver (820 means 8.20).This makes it possible for an external DLL to know the (interface) version of the SVObserver calling it
						if (lTest != 2)
						{
							Result = -12349;
							e.setMessage(0, SvStl::Tid_ToolLoadError_SimpleTest, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
						}
					}
					catch (...)
					{
						Result = -12384;
						e.setMessage(0, SvStl::Tid_ToolLoadError_SimpleTestException, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
					}

				}
				if (S_OK == Result)
				{
					rStatusMsgs.emplace_back(_T("Attempting Startup"));
					try
					{
						Result = m_pfnStartup();
						if (S_OK != Result)
						{
							e.setMessage(0, SvStl::Tid_ToolLoadError_DllStartup, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
						}
					}
					catch (...)
					{
						Result = -12385;
						e.setMessage(0, SvStl::Tid_ToolLoadError_DllStartupException, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
					}
				}
				// Get Tool Name
				if (S_OK == Result)
				{
					rStatusMsgs.emplace_back(_T("Attempting GetToolName"));
					BSTR bstName = nullptr;
					try
					{
						Result = m_pfnGetToolName(&bstName);
						if (S_OK != Result)
						{
							e.setMessage(0, SvStl::Tid_ToolLoadError_GetToolName, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
						}
						else
						{
							rStatusMsgs.emplace_back(SvUl::createStdString(_bstr_t(bstName)));
						}
					}
					catch (...)
					{
						Result = -12386;
						e.setMessage(0, SvStl::Tid_ToolLoadError_GetToolNameException, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
					}

				}
				// Get Tool Version
				if (S_OK == Result)
				{
					rStatusMsgs.emplace_back(_T("Attempting GetToolVersion"));
					long lTmp = 0;
					try
					{
						Result = m_pfnGetToolVersion(&lTmp);
						if (S_OK != Result)
						{
							e.setMessage(0, SvStl::Tid_ToolLoadError_GetToolVersion, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
						}
						else
						{
							std::string Version = std::format(_T("Version {:d}"), lTmp);
							rStatusMsgs.emplace_back(Version);
						}
					}
					catch (...)
					{
						Result = -12387;
						e.setMessage(0, SvStl::Tid_ToolLoadError_GetToolVersionException, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));
					}
				}

			}
			else	// get proc address failure
			{
				Result = -12350;
				e.setMessage(0, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), static_cast<DWORD> (Result));

				if (!m_pfnSimpleTest)                      e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12351), SvStl::Tid_GetProcAddressError_SimpleTest), false);
				if (!m_pfnGetToolName)                     e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12352), SvStl::Tid_GetProcAddressError_GetToolName), false);
				if (!m_pfnGetToolVersion)                  e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12353), SvStl::Tid_GetProcAddressError_GetToolVersion), false);
				if (!m_pfnRunTool)                         e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12354), SvStl::Tid_GetProcAddressError_RunTool), false);
				if (!m_pfnStartup)                         e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12355), SvStl::Tid_GetProcAddressError_Startup), false);
				if (!m_pfnShutDown)                        e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12356), SvStl::Tid_GetProcAddressError_ShutDown), false);
				if (!m_pfnInitializeRun)                   e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12357), SvStl::Tid_GetProcAddressError_InitializeRun), false);
				if (!m_pfnUninitializeRun)                 e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12358), SvStl::Tid_GetProcAddressError_UninitializeRun), false);
				if (!m_pfnSetInputValues)                  e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12362), SvStl::Tid_GetProcAddressError_SetInputValues), false);
				if (!m_pfnGetResultValues)                 e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12363), SvStl::Tid_GetProcAddressError_GetResultValues), false);
				if (!m_pfnGetMessageString)                e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12364), SvStl::Tid_GetProcAddressError_GetMessageString), false);
				if (!m_pfnValidateValueParameter)          e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12365), SvStl::Tid_GetProcAddressError_ValidateValueParameter), false);
				if (!m_pfnGetResultValueDefinitions)       e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12366), SvStl::Tid_GetProcAddressError_GetResultValueDefinitions), false);
				if (!m_pfnGetNumberOfInputImages)          e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12367), SvStl::Tid_GetProcAddressError_GetNumberOfInputImages), false);
				if (!m_pfnGetResultImageDefinitions)       e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12368), SvStl::Tid_GetProcAddressError_GetResultImageDefinitions), false);
				if (!m_pfnDestroyImageDefinitionStructure) e.addMessage(SvStl::MessageData(static_cast<DWORD> (-12369), SvStl::Tid_GetProcAddressError_DestroyImageDefinitionStructure), false);
			}
		}// end if ( nullptr != m_hmHandle )



		if (S_OK != Result)
		{
			Close();
			throw e;
		}

		return Result;
	}

	bool SVDLLToolLoadLibraryClass::UseMil()
	{
		return m_bUseMil;
	}

	bool  SVDLLToolLoadLibraryClass::UseTableOutput() const
	{
		if (m_bUseResultEx)
		{
			return (m_pfnGetResultTableDefinitionsEx != nullptr && m_pfnDestroyResultTableDefinitionStructuresEx != nullptr);
		}
		else
		{
			return(m_pfnGetResultTableDefinitions != nullptr && m_pfnDestroyResultTableDefinitionStructures != nullptr);
		}

	}
	bool SVDLLToolLoadLibraryClass::HasTransformationMatrix(int& rImageIndexIn, int& rImageIndexOut, int& rResutMatrixIndex)
	{
		rImageIndexIn = rImageIndexOut = rResutMatrixIndex = -1;
		if (m_pfnGetTransformationDefinitions == nullptr || m_pfnDestroyTransformationDefinitions == nullptr)
		{
			return false;
		}
		long size;
		TransformDefs* pTransformdefs {nullptr};
		if (S_OK == m_pfnGetTransformationDefinitions(&size, &pTransformdefs))
		{
			rImageIndexIn = pTransformdefs[0].imageinput;
			rImageIndexOut = pTransformdefs[0].imageoutput;
			rResutMatrixIndex = pTransformdefs[0].resultindex;
			if (m_pfnDestroyTransformationDefinitions)
			{
				m_pfnDestroyTransformationDefinitions(pTransformdefs);
			}
			return true;
		}
		return false;
	}
		
	
		
	

	bool  SVDLLToolLoadLibraryClass::UseResultTablesMaxRowSize() const
	{
		return  (m_pfnGetResultTablesMaxRowSize != nullptr);
	}
	bool  SVDLLToolLoadLibraryClass::UseResultValuesMaxArraySize() const
	{
		return  (m_pfnGetResultValuesMaxArraySize != nullptr);
	}


	bool SVDLLToolLoadLibraryClass::IsHandleNull()
	{
		if (nullptr == m_hmHandle)
		{
			return true;
		}
		return false;
	}


	HRESULT SVDLLToolLoadLibraryClass::Close()
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_hmHandle)
		{
			if (nullptr != m_pfnShutDown)
			{
				try
				{
					l_hrOk = m_pfnShutDown();
				}
				catch (...)
				{
					l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
				}
				l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
			}

			if (::FreeLibrary(m_hmHandle))
			{
				// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
				Sleep(0);
				m_hmHandle = nullptr;
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}


		m_bUseMil = false;
		m_bUseResultEx = false;
		m_bUseInputEx = false;
		m_pfnSimpleTest = nullptr;
		m_pfnGetToolName = nullptr;
		m_pfnGetToolVersion = nullptr;
		m_pfnRunTool = nullptr;
		m_pfnStartup = nullptr;
		m_pfnShutDown = nullptr;
		m_pfnInitializeRun = nullptr;
		m_pfnUninitializeRun = nullptr;
		m_pfnGetInputValueDefinitions = nullptr;
		m_pfnGetInputValueDefinitionsEx = nullptr;
		m_pfnDestroyInputValueDefinitionStructures = nullptr;
		m_pfnDestroyInputValueDefinitionStructuresEx = nullptr;
		m_pfnDestroyResultValueDefinitionStructures = nullptr;
		m_pfnDestroyResultValueDefinitionStructuresEx = nullptr;
		m_pfnSetInputValues = nullptr;
		m_pfnGetInputImageInformation = nullptr;
		m_pfnDestroyInputImageInformationPtr = nullptr;
		m_pfnGetResultValues = nullptr;
		m_pfnGetMessageString = nullptr;
		m_pfnValidateValueParameter = nullptr;
		m_pfnGetResultValueDefinitions = nullptr;
		m_pfnGetResultValueDefinitionsEx = nullptr;
		m_pfnDestroyResultValueDefinitionStructures = nullptr;
		m_pfnDestroyResultValueDefinitionStructuresEx = nullptr;
		m_pfnGetNumberOfInputImages = nullptr;
		m_pfnSetMILInputImages = nullptr;
		m_pfnSetHBITMAPInputImages = nullptr;
		m_pfnGetHBITMAPResultImages = nullptr;
		m_pfnSetMILResultImages = nullptr;
		m_pfnGetResultImageDefinitions = nullptr;
		m_pfnDestroyImageDefinitionStructure = nullptr;
		m_pfnGetResultTableDefinitions = nullptr;
		m_pfnGetResultTableDefinitionsEx = nullptr;
		m_pfnDestroyResultTableDefinitionStructures = nullptr;
		m_pfnDestroyResultTableDefinitionStructuresEx = nullptr;
		m_pfnGetResultTables = nullptr;
		m_pfnGetResultTablesMaxRowSize = nullptr;
		m_pfnGetResultValuesMaxArraySize = nullptr;
		m_pfnGetTransformationDefinitions  = nullptr;
		m_pfnDestroyTransformationDefinitions = nullptr;


		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetToolName(BSTR* pbstrToolName)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetToolName)
		{
			try
			{
				l_hrOk = m_pfnGetToolName(pbstrToolName);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetToolVersion(long* plVersionNumber)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetToolVersion)
		{
			try
			{
				l_hrOk = m_pfnGetToolVersion(plVersionNumber);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::RunTool(uint32_t toolId, long* plStatus)
	{
		HRESULT result = S_FALSE;

		if (nullptr != m_pfnRunTool)
		{
			try
			{
				result = m_pfnRunTool(GUID{ toolId }, plStatus);
			}
			catch (...)
			{
				result = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			result = (result == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : result;

			Log_Assert(S_OK == result);

			if (S_OK != result)
			{
				SvDef::StringVector msgList;
				msgList.push_back(std::format(_T("{:#x}ld"), result));
				msgList.push_back(m_dllName);
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(static_cast<DWORD> (result), SvStl::Tid_RunTool_Exception, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		return result;
	}

	HRESULT SVDLLToolLoadLibraryClass::Startup()
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnStartup)
		{
			try
			{
				l_hrOk = m_pfnStartup();
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::ShutDown()
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnShutDown)
		{
			try
			{
				l_hrOk = m_pfnShutDown();
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::InitializeRun(uint32_t toolId, long lImageArraySize, SVImageDefinitionStruct* paStructs, 
		long lValueArraySize, VARIANT* pavInputValues)
	{
		HRESULT result = S_FALSE;

		if (nullptr != m_pfnInitializeRun)
		{
			try
			{
				result = m_pfnInitializeRun(GUID{ toolId }, lImageArraySize, paStructs, lValueArraySize, pavInputValues);
			}
			catch (...)
			{
				result = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			result = (result == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : result;

			Log_Assert(S_OK == result);

			if (S_OK != result)
			{

				SvDef::StringVector msgList;
				msgList.push_back(std::format(_T("{:#x}ld"), result));
				msgList.push_back(m_dllName);
				
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(static_cast<DWORD> (result), SvStl::Tid_SVInitializeRun_Exception, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		return result;
	}

	HRESULT SVDLLToolLoadLibraryClass::UninitializeRun(uint32_t toolId)
	{
		HRESULT l_hrOk = S_FALSE;
		if (nullptr != m_pfnUninitializeRun)
		{
			try
			{
				l_hrOk = m_pfnUninitializeRun(GUID{ toolId });
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetInputValueDefinitions(long* plArraySize, InputValueDefinitionStructEx** ppaStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetInputValueDefinitionsEx)
		{
			try
			{
				l_hrOk = m_pfnGetInputValueDefinitionsEx(plArraySize, ppaStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}



	HRESULT SVDLLToolLoadLibraryClass::GetInputValueDefinitions(long* plArraySize, InputValueDefinitionStruct** ppaStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetInputValueDefinitions)
		{
			try
			{
				l_hrOk = m_pfnGetInputValueDefinitions(plArraySize, ppaStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}


	HRESULT SVDLLToolLoadLibraryClass::DestroyInputValueDefinitionStructures(InputValueDefinitionStruct* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyInputValueDefinitionStructures)
		{
			try
			{
				l_hrOk = m_pfnDestroyInputValueDefinitionStructures(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::DestroyInputValueDefinitionStructures(InputValueDefinitionStructEx* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyInputValueDefinitionStructuresEx)
		{
			try
			{
				l_hrOk = m_pfnDestroyInputValueDefinitionStructuresEx(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}


	HRESULT SVDLLToolLoadLibraryClass::DestroyResultValueDefinitionStructures(ResultValueDefinitionStruct* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyResultValueDefinitionStructures)
		{
			try
			{
				l_hrOk = m_pfnDestroyResultValueDefinitionStructures(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}
	HRESULT SVDLLToolLoadLibraryClass::DestroyResultValueDefinitionStructures(ResultValueDefinitionStructEx* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyResultValueDefinitionStructuresEx)
		{
			try
			{
				l_hrOk = m_pfnDestroyResultValueDefinitionStructuresEx(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}


	HRESULT SVDLLToolLoadLibraryClass::SetInputValues(uint32_t toolId, long lArraySize, VARIANT* paInputValues)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnSetInputValues)
		{
			try
			{
				l_hrOk = m_pfnSetInputValues(GUID{ toolId }, lArraySize, paInputValues);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetInputImageInformation(std::vector<InputImageInformationStruct>* pVector)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetInputImageInformation)
		{
			try
			{
				InputImageInformationStruct* ppIIISs;
				long ArraySize;
				l_hrOk = m_pfnGetInputImageInformation(&ArraySize, &ppIIISs);
				if (ArraySize != static_cast<long>(pVector->size()))
				{
					return E_FAIL;
				}

				for(int i =0; i < ArraySize; i++)
				{
					pVector->at(i).AllowedImageTypes = ppIIISs[i].AllowedImageTypes;
					pVector->at(i).DisplayName = ppIIISs[i].DisplayName.copy(true);
					pVector->at(i).HelpText = ppIIISs[i].HelpText.copy(true);
				}
				if (m_pfnDestroyInputImageInformationPtr)
				{
					m_pfnDestroyInputImageInformationPtr(ppIIISs);
				}
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}
		else
		{	// the external DLL we are using does not provide SVGetInputImageInformation:
			// so we assume it is an old DLL and can only handle black-and-white Images
			int i = 0;
			for (auto& rIiis : *pVector)
			{
				rIiis = InputImageInformationStruct();
				rIiis.DisplayName = std::format(_T("Image {:2d}"), ++i).c_str();
				rIiis.HelpText = "No HelpText available!";
				rIiis.allowBlackAndWhite();
			}
			l_hrOk = S_OK;
		}

		return l_hrOk;
	}



	HRESULT SVDLLToolLoadLibraryClass::GetResultValues(uint32_t toolId, long lArraySize, VARIANT* paResultValues)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultValues)
		{
			try
			{
				l_hrOk = m_pfnGetResultValues(GUID{ toolId }, lArraySize, paResultValues);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetMessageString(unsigned long ulErrorNumber, BSTR* pbstrErrorMessage) const
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetMessageString)
		{
			try
			{
				l_hrOk = m_pfnGetMessageString(ulErrorNumber, pbstrErrorMessage);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::ValidateValueParameter(uint32_t toolId, long lParameterNumber, VARIANT vParameterValue)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnValidateValueParameter)
		{
			try
			{
				l_hrOk = m_pfnValidateValueParameter(GUID{ toolId }, lParameterNumber, vParameterValue);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetResultValueDefinitions(long* plArraySize, ResultValueDefinitionStruct** ppaResultValues)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultValueDefinitions)
		{
			try
			{
				l_hrOk = m_pfnGetResultValueDefinitions(plArraySize, ppaResultValues);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}
	HRESULT SVDLLToolLoadLibraryClass::GetResultValueDefinitions(long* plArraySize, ResultValueDefinitionStructEx** ppaResultValues)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultValueDefinitionsEx)
		{
			try
			{
				l_hrOk = m_pfnGetResultValueDefinitionsEx(plArraySize, ppaResultValues);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}


	HRESULT SVDLLToolLoadLibraryClass::GetNumberOfInputImages(long* plNumberOfInputImages)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetNumberOfInputImages)
		{
			try
			{
				l_hrOk = m_pfnGetNumberOfInputImages(plNumberOfInputImages);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::SetMILInputImages(uint32_t toolId, long lArraySize, long* paMILhandles)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnSetMILInputImages)
		{
			try
			{
				l_hrOk = m_pfnSetMILInputImages(GUID{ toolId }, lArraySize, paMILhandles);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::SetHBITMAPInputImages(uint32_t toolId, HBITMAP* paHandles)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnSetHBITMAPInputImages)
		{
			try
			{
				l_hrOk = m_pfnSetHBITMAPInputImages(GUID{ toolId }, paHandles);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetHBITMAPResultImages(uint32_t toolId, long lArraySize, HBITMAP* paHandles)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetHBITMAPResultImages)
		{
			try
			{
				l_hrOk = m_pfnGetHBITMAPResultImages(GUID{ toolId }, lArraySize, paHandles);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}
		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::SetMILResultImages(uint32_t toolId, long lArraySize, long* paMILhandles)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnSetMILResultImages)
		{
			try
			{
				l_hrOk = m_pfnSetMILResultImages(GUID{ toolId }, lArraySize, paMILhandles);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::GetResultImageDefinitions(uint32_t toolId, long* plArraySize, SVImageDefinitionStruct** ppaImageDefinitions)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultImageDefinitions)
		{
			try
			{
				l_hrOk = m_pfnGetResultImageDefinitions(GUID{ toolId }, plArraySize, ppaImageDefinitions);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}



	HRESULT SVDLLToolLoadLibraryClass::DestroyImageDefinitionStructure(SVImageDefinitionStruct* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyImageDefinitionStructure)
		{
			try
			{
				l_hrOk = m_pfnDestroyImageDefinitionStructure(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	SVMachineTypeEnum SVDLLToolLoadLibraryClass::CheckBitness(LPCTSTR p_szFile)
	{
		//see http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
		//offset to PE header is always at 0x3C
		//PE header starts with "PE\0\0" =  0x50 0x45 0x00 0x00
		//followed by 2-byte machine type field (see document above for enum)
		FILE* fh = nullptr;
		SVMachineTypeEnum l_machineType = ImageFileUnknownMachine;

		errno_t err = fopen_s(&fh, p_szFile, "rb");
		if (err == 0)
		{
			fseek(fh, 0x3c, 0);	// seek to pe header start.
			_int32 peOffset;
			/*size_t bytes = */fread(&peOffset, 4, 1, fh); // read 4 bytes to get offset.
			/*int iRet = */fseek(fh, peOffset, 0);
			_int32 peHead = 0;
			fread(&peHead, 4, 1, fh);
			if (peHead == 0x00004550) // "PE\0\0", little-endian
			{
				fread(&l_machineType, 2, 1, fh);
			}
			fclose(fh);
		}
		return l_machineType;
	}

	HRESULT SVDLLToolLoadLibraryClass::getResultTableDefinitions(long* Size, ResultTableDefinitionStruct** ppaResultTableDefs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultTableDefinitions)
		{
			try
			{
				l_hrOk = m_pfnGetResultTableDefinitions(Size, ppaResultTableDefs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}
	HRESULT SVDLLToolLoadLibraryClass::getResultTableDefinitions(long* Size, ResultTableDefinitionStructEx** ppaResultTableDefs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultTableDefinitionsEx)
		{
			try
			{
				l_hrOk = m_pfnGetResultTableDefinitionsEx(Size, ppaResultTableDefs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::destroyResultTableDefinitionStructures(ResultTableDefinitionStruct* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyResultTableDefinitionStructures)
		{
			try
			{
				l_hrOk = m_pfnDestroyResultTableDefinitionStructures(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}

	HRESULT SVDLLToolLoadLibraryClass::destroyResultTableDefinitionStructures(ResultTableDefinitionStructEx* paStructs)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnDestroyResultTableDefinitionStructuresEx)
		{
			try
			{
				l_hrOk = m_pfnDestroyResultTableDefinitionStructuresEx(paStructs);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}



	HRESULT SVDLLToolLoadLibraryClass::getResultTables(uint32_t toolId, long lArraySize, VARIANT* paResultValues)
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultTables)
		{
			try
			{
				l_hrOk = m_pfnGetResultTables(GUID{ toolId }, lArraySize, paResultValues);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}


	HRESULT SVDLLToolLoadLibraryClass::getResultTablesMaxRowSize(uint32_t toolId, long  Size, int pRowSizes[])
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultTablesMaxRowSize)
		{
			try
			{
				l_hrOk = m_pfnGetResultTablesMaxRowSize(GUID{ toolId }, Size, pRowSizes);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}



	HRESULT SVDLLToolLoadLibraryClass::getResultValuesMaxArraySize(uint32_t toolId, long  Size, int pArraySizes[])
	{
		HRESULT l_hrOk = S_FALSE;

		if (nullptr != m_pfnGetResultValuesMaxArraySize)
		{
			try
			{
				l_hrOk = m_pfnGetResultValuesMaxArraySize(GUID{ toolId }, Size, pArraySizes);
			}
			catch (...)
			{
				l_hrOk = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL;
			}
			l_hrOk = (l_hrOk == ERROR_USER_EXCEPTION) ? SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL : l_hrOk;
		}

		return l_hrOk;
	}
} //namespace SvOp
