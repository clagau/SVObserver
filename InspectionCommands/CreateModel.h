//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for Creating a model from input image of the analyzer.
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/IPatternAnalyzer.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxSimpleEnums.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class CreateModel
	{
	public:
		CreateModel(const CreateModel&) = delete;
		CreateModel& operator=(const CreateModel&) = delete;

		CreateModel(const SVGUID& rInstanceID, int posX, int posY, int modelWidth, int modelHeight, const std::string& rFileName)
			: m_InstanceID(rInstanceID)
			, m_posX(posX)
			, m_posY(posY)
			, m_modelWidth(modelWidth)
			, m_modelHeight(modelHeight)
			, m_FileName(rFileName)
		{};

		virtual ~CreateModel() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;
			m_messages.clear();
			SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
			SvOi::IPatternAnalyzer* pPatAnalyzer = dynamic_cast<SvOi::IPatternAnalyzer*>(pObject);

			if( nullptr != pPatAnalyzer && pPatAnalyzer->UpdateModelFromInputImage(m_posX, m_posY, m_modelWidth, m_modelHeight) )
			{
				SvOi::MatroxImageSmartHandlePtr imageHandle;
				SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(pObject);
				if ( nullptr != pTaskObject && pTaskObject->getSpecialImage(SvDef::PatternModelImageName, imageHandle)  && !imageHandle->empty() )
				{
					TCHAR FileExtension[_MAX_EXT];
					_tsplitpath(m_FileName.c_str(), nullptr, nullptr, nullptr, FileExtension);
					// Now save the Model Image buffer to a file
					SVMatroxFileTypeEnum FileFormatID = SVFileMIL; // Set as default.
					if ( 0 == SvUl::CompareNoCase( std::string(FileExtension), std::string( _T(".bmp") ) ) )
					{
						FileFormatID = SVFileBitmap;
					}
					if ( 0 == SvUl::CompareNoCase(std::string(FileExtension), std::string( _T(".tif") ) ) )
					{
						FileFormatID = SVFileTiff;
					}

					SVMatroxBuffer milBuffer;
					imageHandle->GetBuffer(milBuffer);
					HRESULT Code = SVMatroxBufferInterface::Export( milBuffer, m_FileName, FileFormatID );
				}
				else
				{
					hr = E_FAIL;
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, m_InstanceID );
					m_messages.push_back(Msg);
				}
			}
			else
			{
				hr = E_FAIL;
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, m_InstanceID );
				m_messages.push_back(Msg);
			}

			return hr;
		}
		bool empty() const { return false; }
		SvStl::MessageContainerVector getErrorMessages() {return m_messages; };

	private:
		SVGUID m_InstanceID;
		int m_posX;
		int m_posY;
		int m_modelWidth;
		int m_modelHeight;
		std::string m_FileName;
		SvStl::MessageContainerVector m_messages;
	};
} //namespace SvCmd
