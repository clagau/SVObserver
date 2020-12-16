//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeResult
//* .File Name       : $Workfile:   SVBarCodeResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 10:19:40  $
//******************************************************************************

// SVBarCodeResult.cpp: implementation of the SVBarCodeResult class.
//
//////////////////////////////////////////////////////////////////////

#pragma region Includes
#include "stdafx.h"
#include "SVBarCodeResult.h"
#include "SVStatusLibrary/RunStatus.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVBarCodeResult, SvPb::BarCodeResultClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVBarCodeResult::SVBarCodeResult(SVObjectClass* POwner, int StringResourceID)
	: SVStringResult(POwner, StringResourceID)
	, m_lTotalBytes(0L)
	, m_nTotalCount(0)
	, m_pBuffer(nullptr)
	, m_lLowValue(-1L)
	, m_lHighValue(-1L)
	, m_pIndexTable(nullptr)
	, m_dFactor(1.0)
{

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultBarCodeObjectType;

	RegisterEmbeddedObject(
		&msv_bUseSingleMatchString,
		SvPb::BCUseSingleMatchStringEId,
		IDS_OBJECTNAME_BC_USE_SINGLE_MATCH_STRING,
		false, SvOi::SVResetItemOwner);

	RegisterEmbeddedObject(
		&msv_bUseMatchStringFile,
		SvPb::BCUseMatchStringFileEId,
		IDS_OBJECTNAME_BC_USE_MATCH_STRING_FILE,
		false, SvOi::SVResetItemOwner);

	RegisterEmbeddedObject(
		&msv_szMatchStringFileName,
		SvPb::BCMatchStringFileNameEId,
		IDS_OBJECTNAME_BC_MATCH_STRING_FILE_NAME,
		false, SvOi::SVResetItemOwner);

	RegisterEmbeddedObject(
		&msv_lMatchStringLine,
		SvPb::BCMatchStringLineEId,
		IDS_OBJECTNAME_BC_MATCH_STRING_LINE,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_dReadScore,
		SvPb::BCMatchScoreEId,
		IDS_OBJECTNAME_BC_READ_SCORE,
		false, SvOi::SVResetItemNone);

	msv_bUseSingleMatchString.SetDefaultValue(BOOL(true), true);
	msv_szMatchStringFileName.SetDefaultValue(_T(""), true);
	msv_lMatchStringLine.SetDefaultValue(0, true);
	msv_lMatchStringLine.setSaveValueFlag(false);
	msv_bUseMatchStringFile.SetDefaultValue(BOOL(false), true);
	m_dReadScore.SetDefaultValue(-1.0, true);
	m_dReadScore.setSaveValueFlag(false);
	// Specify which string SVResultString should require
	m_inputObject.SetInputObjectType(SvPb::SVNotSetObjectType, SvPb::SVNotSetSubObjectType, SvPb::BarCodeEId);
	m_inputObject.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	// Identify our input type needs
	m_SVRegExpressionInput.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVStringValueObjectType, SvPb::RegExpressionEId);
	registerInputObject(&m_SVRegExpressionInput, _T("BarCodeResultString"), SvPb::BarCodeResultEId);
	m_SVRegExpressionInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;
}

SVBarCodeResult::~SVBarCodeResult()
{
	if (m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}

	if (m_pIndexTable)
	{
		delete m_pIndexTable;
		m_pIndexTable = nullptr;
	}
}

bool SVBarCodeResult::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result = SVStringResult::CreateObject(rCreateStructure);
	Result = Result && nullptr != m_inputObject.getInput<SvVol::SVStringValueObjectClass>();
	SvVol::SVStringValueObjectClass* pRegExpression = m_SVRegExpressionInput.getInput<SvVol::SVStringValueObjectClass>();
	Result = Result && nullptr != pRegExpression;

	if (Result)
	{
		pRegExpression->SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	}

	msv_bUseSingleMatchString.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	msv_szMatchStringFileName.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	msv_lMatchStringLine.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	msv_bUseMatchStringFile.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);
	m_dReadScore.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_isCreated = Result;

	return Result;
}

bool SVBarCodeResult::onRun(RunStatus &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	if (__super::onRun(rRunStatus, pErrorMessages) && ValidateLocal(pErrorMessages))
	{
		if (this->m_bFailedToRead)
		{
			rRunStatus.SetWarned();
			return true;
		}

		SvVol::SVStringValueObjectClass* pValue = m_inputObject.getInput<SvVol::SVStringValueObjectClass>(true);

		if (nullptr != pValue && pValue->IsCreated())
		{
			BOOL bLoad = false;

			std::string InputString;
			pValue->GetValue(InputString);

			msv_bUseMatchStringFile.GetValue(bLoad);
			if (bLoad)
			{
				long lLine = CheckStringInTable(InputString);
				msv_lMatchStringLine.SetValue(lLine);
				if (0 < lLine)
				{
					rRunStatus.SetPassed();
				}
				else
				{
					rRunStatus.SetFailed();
				}
			}
			else
			{
				std::string RegExpression;
				SvVol::SVStringValueObjectClass* pRegExp = m_SVRegExpressionInput.getInput<SvVol::SVStringValueObjectClass>(true);
				pRegExp->GetValue(RegExpression);

				if (RegExpression.empty() || !InputString.compare(RegExpression.c_str()))
				{
					rRunStatus.SetPassed();
				}
				else
				{
					rRunStatus.SetFailed();
				}
			}
		}
		else
		{
			rRunStatus.SetFailed();
		}
		return true;
	}
	rRunStatus.SetInvalid();
	return false;
}

bool SVBarCodeResult::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVStringResult::ResetObject(pErrorMessages);

	m_SVRegExpressionInput.validateInput();

	if (S_OK != LoadMatchStringFile())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_BarCodeMatchStringLoadFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	return Result && ValidateLocal(pErrorMessages);
}

HRESULT SVBarCodeResult::LoadMatchStringFile()
{
	HRESULT Result = S_OK;
	
	if (nullptr != m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}

	m_lTotalBytes = 0;

	BOOL bLoad = false;
	msv_bUseMatchStringFile.GetValue(bLoad);
	if (bLoad)
	{
		std::string FileName;

		// Check to see if the file exists..
		//
		msv_szMatchStringFileName.GetValue(FileName);

		bool bOk = !FileName.empty() && 0 == _access(FileName.c_str(), 0);
		if (bOk)
		{
			//
			// Open and read the Bar Code match string from the file.
			//
			try
			{
				std::ifstream matchFile {FileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate};
				bOk = matchFile.is_open();
				if (bOk)
				{
					unsigned long ulLength = static_cast<unsigned long>(matchFile.tellg());
					matchFile.seekg(0, std::ios::beg);

					if (0 < ulLength)
					{
						m_pBuffer = new char[ulLength + 1];
						bOk = nullptr != m_pBuffer;
						if (bOk)
						{
							matchFile.read(m_pBuffer, ulLength);
							m_lTotalBytes = static_cast<long> (matchFile.gcount());
							m_pBuffer[m_lTotalBytes] = '\0';
						}
						else
						{
							Result = E_FAIL;
						}
					}

					matchFile.close();

					if (m_lTotalBytes > 0)
					{
						if (bOk)
						{
							bOk = BuildHashTable(m_pBuffer);
							if (!bOk)
							{
								Result = E_FAIL;
							}
						}
					}
					else
					{
						Result = E_FAIL;
					}
				}
				else
				{
					Result = E_FAIL;
				}
			}
			catch (...)
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

bool SVBarCodeResult::BuildHashTable(char *pBuffer)
{
	long lBufIndex = 0;
	bool  bRet = true;

	for (int i = 0; i < BC_MAX_ENTREES; i++)
	{
		m_pDataArr[i] = 0;
	}

	for (m_nTotalCount = 0;
		(m_nTotalCount < BC_MAX_ENTREES) && (lBufIndex < m_lTotalBytes);
		 m_nTotalCount++)
	{
		int nCharCount = 0;
		long lIndexValue = 0;
		// Assign each line into a char pointer array.
		m_pDataArr[m_nTotalCount] = &pBuffer[lBufIndex];

		while ((lBufIndex < m_lTotalBytes) && pBuffer[lBufIndex] != BC_CAR_RETURN)
		{
			// The index value is calculated as follows.
			// nValue = ASCII value of char at position index - ASCII of first displayable character
			// lIndexValue = Square of nValue * Position of the char from left.
			nCharCount++;
			int   nValue = pBuffer[lBufIndex++] - 0x20;
			lIndexValue += nValue * nValue * nCharCount;
		}
		if (lBufIndex < m_lTotalBytes)
		{
			pBuffer[lBufIndex++] = 0; // Carriage Return
			lBufIndex++; // skip the Line feed
		}
		if (!nCharCount) // If there is a blank line skip that one.
		{
			m_nTotalCount--;
			continue;
		}
		// We are calculating m_lLowValue and m_lHighValue because the string corresponding to
		// m_lLowValue should go to index 0 and the string for m_lHighValue should have the max index
		if (!m_nTotalCount)
			m_lLowValue = m_lHighValue = lIndexValue;

		else if (lIndexValue < m_lLowValue)
			m_lLowValue = lIndexValue;
		else if (lIndexValue > m_lHighValue)
			m_lHighValue = lIndexValue;
	}

	// Allocate a table that is 10 times the size of m_nTotalCount
	while (1)
	{
		if (m_pIndexTable)
			delete m_pIndexTable;
		// If there is only one entry in the file, make it work the old way.   

		m_pIndexTable = new short[m_nTotalCount * 10];
		if (!m_pIndexTable)
		{
			bRet = false;
			break;
		}
		memset(m_pIndexTable, 0, (size_t)(sizeof(short) * m_nTotalCount * 10));

		// Now calculate the hash table index for each enrty in the m_pDataArr

		m_dFactor = ((double)(m_lHighValue - m_lLowValue)) / (double)m_nTotalCount;

		for (short Count = 0; Count < m_nTotalCount; Count++)
		{
			char *pData = m_pDataArr[Count];
			int nCharCount = static_cast<int>(strlen(pData));
			long lIndexValue = 0;
			for (int i = 0; i < nCharCount; i++)
			{
				int   nValue = pData[i] - 0x20;
				lIndexValue += nValue * nValue * (i + 1);
			}
			// We are multiplying by 9.5 since we want to reserve some slots at the end of the table,
			// in case we get the max index for multiple strings.

			if (m_dFactor <= 0.0)
			{
				m_dFactor = 1.0;
			}
			double dActualIndex = (((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5;

			// Line number will be count  + 1
			InsertValueToTable(Count + 1, (int)dActualIndex);
		}
		break;
	}
	return bRet;
}

void SVBarCodeResult::InsertValueToTable(short nValue, int nIndex)
{
	// Check whether the location contains a data

	if (m_pIndexTable[nIndex] == 0)
		m_pIndexTable[nIndex] = nValue;
	else
	{
		//    
		++nIndex;
		while (m_pIndexTable[nIndex] != 0)
		{
			nIndex++;
			// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
			if (nIndex >= m_nTotalCount * 10)
				nIndex = 2;
		}

		m_pIndexTable[nIndex] = nValue;
	}
}

int SVBarCodeResult::CheckStringInTable(const std::string& rMatchString)
{
	int Result = -1;

	long  lIndexValue = 0;
	size_t nCharCount = rMatchString.size();

	if (0 == nCharCount)
	{
		return Result;
	}

	for (size_t i = 0; i < nCharCount; i++)
	{
		int nValue = rMatchString[i] - 0x20;
		lIndexValue += nValue * nValue * static_cast<int> (i + 1);
	}

	// if Index value is out of range, definitely there won't be a match in the file.
	if (lIndexValue >= m_lLowValue && lIndexValue <= m_lHighValue)
	{
		int nActualIndex = (int)((((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5);
		// Check whether the string at this location is the matchString.

		while (m_pIndexTable[nActualIndex] != 0)
		{
			TCHAR *pData = m_pDataArr[m_pIndexTable[nActualIndex] - 1];

			if (rMatchString == pData)
			{
				Result = m_pIndexTable[nActualIndex];
				break;
			}
			nActualIndex++;

			// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
			if (nActualIndex >= m_nTotalCount * 10)
			{
				nActualIndex = 2;
			}
		}
	}

	return  Result;
}

bool SVBarCodeResult::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if (nullptr == m_SVRegExpressionInput.getInput<SvVol::SVStringValueObjectClass>())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

} //namespace SvOp
