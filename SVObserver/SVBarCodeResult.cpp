//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeResultClass
//* .File Name       : $Workfile:   SVBarCodeResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 10:19:40  $
//******************************************************************************

// SVBarCodeResult.cpp: implementation of the SVBarCodeResultClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVBarCodeResult.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVOCore/SVTool.h"

SV_IMPLEMENT_CLASS (SVBarCodeResultClass, SVBarCodeResultClassGuid);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVBarCodeResultClass::SVBarCodeResultClass (BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID)
  : SVStringResultClass(BCreateDefaultTaskList, POwner, StringResourceID)
{
	m_pBuffer = nullptr;
	m_pIndexTable = nullptr;
	m_lTotalBytes = 0;

	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVResultBarCodeObjectType;

  RegisterEmbeddedObject(
		&msv_bUseSingleMatchString, 
	  SVBCUseSingleMatchStringGuid, 
	  IDS_OBJECTNAME_BC_USE_SINGLE_MATCH_STRING,
		false, SvOi::SVResetItemOwner );

  RegisterEmbeddedObject(
		&msv_bUseMatchStringFile, 
		SVBCUseMatchStringFileGuid, 
		IDS_OBJECTNAME_BC_USE_MATCH_STRING_FILE,
		false, SvOi::SVResetItemOwner );

  RegisterEmbeddedObject(
		&msv_szMatchStringFileName, 
	  SVBCMatchStringFileNameObjectGuid, 
	  IDS_OBJECTNAME_BC_MATCH_STRING_FILE_NAME,
		false, SvOi::SVResetItemOwner );

  RegisterEmbeddedObject(
		&msv_lMatchStringLine, 
		SVBCMatchStringLineGuid, 
		IDS_OBJECTNAME_BC_MATCH_STRING_LINE,
		false, SvOi::SVResetItemNone );

  RegisterEmbeddedObject(
		&m_dReadScore,
		SVBCMatchScoreGuid,
		IDS_OBJECTNAME_BC_READ_SCORE,
		false, SvOi::SVResetItemNone );

	msv_bUseSingleMatchString.SetDefaultValue( TRUE, TRUE );
  msv_szMatchStringFileName.SetDefaultValue(_T(""), TRUE);
	msv_lMatchStringLine.SetDefaultValue( 0, TRUE );
	msv_lMatchStringLine.setSaveValueFlag(false);
	msv_bUseMatchStringFile.SetDefaultValue( FALSE, TRUE );
	m_dReadScore.SetDefaultValue( ( double ) -1.0, TRUE );
	m_dReadScore.setSaveValueFlag(false);
  // Specify which string SVResultString should require
	m_inputObjectInfo.SetInputObjectType( SVBarCodeObjectGuid );

	// Identify our input type needs
	m_SVRegExpressionObjectInfo.SetInputObjectType( SVRegExpressionObjectGuid, SVStringValueObjectType );
	m_SVRegExpressionObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_SVRegExpressionObjectInfo, _T( "BarCodeResultString" ) );
}

SVBarCodeResultClass::~SVBarCodeResultClass()
{
	if(m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}

	if(m_pIndexTable)
	{
		delete m_pIndexTable;
		m_pIndexTable = nullptr;
	}
}

BOOL SVBarCodeResultClass::CreateObject(SVObjectLevelCreateStruct *PCreateStructure)
{
	BOOL bOk = FALSE;

	if( SVStringResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = getInputString() && getRegExpression() && (S_OK == LoadMatchStringFile());
		if (bOk)
		{
			getRegExpression()->SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
		}
	}

	msv_bUseSingleMatchString.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_szMatchStringFileName.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	msv_lMatchStringLine.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msv_bUseMatchStringFile.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_dReadScore.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_isCreated = bOk;

	return bOk;
}

BOOL SVBarCodeResultClass::CloseObject()
{
  return SVStringResultClass::CloseObject();
}

SVStringValueObjectClass* SVBarCodeResultClass::getInputString()
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SVStringValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return nullptr;
}

SVStringValueObjectClass* SVBarCodeResultClass::getRegExpression()
{
	if( m_SVRegExpressionObjectInfo.IsConnected() )
	{
		return dynamic_cast<SVStringValueObjectClass*>  (m_SVRegExpressionObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return nullptr;
}

bool SVBarCodeResultClass::onRun(SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	if( __super::onRun( rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages) )
  {
	  if( this->m_bFailedToRead )
	  {
		  rRunStatus.SetWarned();
		  return TRUE;
	  }

	SVStringValueObjectClass* pValue = getInputString();

    if (pValue->IsValid())
    {
		BOOL bLoad = FALSE;

		SVString InputString;
		pValue->GetValue(InputString);

		msv_bUseMatchStringFile.GetValue( bLoad );
		if ( bLoad )
		{
			long lLine = CheckStringInTable( InputString );
			msv_lMatchStringLine.SetValue(lLine);
			if ( 0 < lLine )
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
			SVString RegExpression;
			SVStringValueObjectClass* pRegExp = getRegExpression();
		    pRegExp->GetValue(RegExpression);

				if (RegExpression.empty() || !InputString.compare(RegExpression))
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
  SetInvalid ();
	return false;
}

bool SVBarCodeResultClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVStringResultClass::ResetObject(pErrorMessages);

	if( S_OK != LoadMatchStringFile() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
	{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_BarCodeMatchStringLoadFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
	}
	}

	return Result && ValidateLocal(pErrorMessages);
}

HRESULT SVBarCodeResultClass::LoadMatchStringFile()
{
	HRESULT hrRet = S_OK;
	BOOL bOk = true;
	BOOL bLoad = false;

	if ( nullptr != m_pBuffer )
	{
		delete m_pBuffer;
		m_pBuffer = nullptr;
	}

	m_lTotalBytes = 0;

	msv_bUseMatchStringFile.GetValue( bLoad );
	if ( bLoad )
	{
		SVString FileName;
		CFile matchFile;

		// Check to see if the file exists..
		//
		msv_szMatchStringFileName.GetValue( FileName );

		bOk = ! FileName.empty() && SVFileExists( FileName.c_str() );
		if ( bOk )
		{
			//
			// Open and read the Bar Code match string from the file.
			//
			try
			{
				bOk = matchFile.Open( FileName.c_str(), CFile::modeRead | CFile::shareDenyNone);
				if ( bOk )
				{
					unsigned long ulLength = static_cast<unsigned long>(matchFile.GetLength());

					if ( 0 < ulLength )
					{
						m_pBuffer = new char[ulLength + 1];
						bOk = nullptr != m_pBuffer;
						if ( bOk )
						{
							m_lTotalBytes = matchFile.Read( m_pBuffer, ulLength );
							m_pBuffer[m_lTotalBytes] = '\0';
						}
						else
						{
							hrRet = S_FALSE;
						}
					}

					matchFile.Close();

					if ( m_lTotalBytes > 0 )
					{
						if ( bOk )
						{
							bOk = BuildHashTable( m_pBuffer );
							if ( !bOk )
							{
								hrRet = S_FALSE;
							}
						}
					}
					else
					{
						hrRet = S_FALSE;
						bOk = false;
					}
				}
				else
				{
					hrRet = S_FALSE;
				}
			}
			catch ( ... )
			{
				hrRet = S_FALSE;
				bOk = false;
			}
		}
	}

	return hrRet;
}

BOOL SVBarCodeResultClass::BuildHashTable(char *pBuffer)
{
   long lBufIndex = 0;
   BOOL  bRet = TRUE;

   for(int i = 0; i < BC_MAX_ENTREES; i++)
      m_pDataArr[i] = 0;

   for(m_nTotalCount = 0; 
         (m_nTotalCount < BC_MAX_ENTREES) && (lBufIndex < m_lTotalBytes);
          m_nTotalCount++)
   {
      int nCharCount = 0;
      long lIndexValue = 0;
// Assign each line into a char pointer array.
      m_pDataArr[m_nTotalCount] = &pBuffer[lBufIndex];

      while(pBuffer[lBufIndex] != BC_CAR_RETURN && (lBufIndex < m_lTotalBytes))
      {
// The index value is calculated as follows.
// nValue = ASCII value of char at position index - ASCII of first displayable character
// lIndexValue = Square of nValue * Position of the char from left.
         nCharCount++;
         int   nValue = pBuffer[lBufIndex++] - 0x20;
         lIndexValue += nValue * nValue * nCharCount;
      }
      if(lBufIndex < m_lTotalBytes)
      {
         pBuffer[lBufIndex++] = 0; // Carriage Return
         lBufIndex++; // skip the Line feed
      }
      if(!nCharCount) // If there is a blank line skip that one.
      {
         m_nTotalCount--;
         continue;
      }
// We are calculating m_lLowValue and m_lHighValue because the string corresponding to
// m_lLowValue should go to index 0 and the string for m_lHighValue should have the max index
      if(!m_nTotalCount)
         m_lLowValue = m_lHighValue = lIndexValue;

      else if(lIndexValue < m_lLowValue)
         m_lLowValue = lIndexValue; 
      else if(lIndexValue > m_lHighValue)
         m_lHighValue = lIndexValue; 
   }
   
// Allocate a table that is 10 times the size of m_nTotalCount
   while(1)
   {
      if(m_pIndexTable)
         delete m_pIndexTable;
// If there is only one entry in the file, make it work the old way.   

      m_pIndexTable = new short[m_nTotalCount * 10];
      if(!m_pIndexTable)
      {
         bRet = FALSE;
         break;
      }
      memset(m_pIndexTable, 0, (size_t)(sizeof(short) * m_nTotalCount * 10));

// Now calculate the hash table index for each enrty in the m_pDataArr

      m_dFactor = ((double)(m_lHighValue - m_lLowValue)) / (double)m_nTotalCount;

      for(short Count = 0; Count < m_nTotalCount ; Count++)
      {
         char *pData = m_pDataArr[Count];
         int nCharCount = static_cast< int >( strlen( pData ) );
         long lIndexValue = 0;
         for (int i = 0; i < nCharCount; i++)
         {
            int   nValue = pData[i] - 0x20;
            lIndexValue += nValue * nValue * (i + 1);
         }
// We are multiplying by 9.5 since we want to reserve some slots at the end of the table,
// in case we get the max index for multiple strings.

		 if( m_dFactor <= 0.0 )
		 {
			 m_dFactor = 1.0;
		 }
         double dActualIndex = (((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5;

// Line number will be count  + 1
         InsertValueToTable(Count + 1, (int) dActualIndex);
      }
      break;
   }
   return bRet;
}

void SVBarCodeResultClass::InsertValueToTable(short nValue, int nIndex)
{
// Check whether the location contains a data

   if(m_pIndexTable[nIndex] == 0)
      m_pIndexTable[nIndex] = nValue;
   else
   {
//    
      ++nIndex;
      while(m_pIndexTable[nIndex] != 0)
      {
         nIndex++;
// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
         if(nIndex >= m_nTotalCount * 10 )
            nIndex = 2;
      }
   
      m_pIndexTable[nIndex] = nValue;
   }
}

int SVBarCodeResultClass::CheckStringInTable( const SVString& rMatchString )
{
   int nReturnIndex = -1;

   long  lIndexValue = 0;
   size_t nCharCount = rMatchString.size();

   for( size_t i = 0; i < nCharCount; i++ )
   {
      int nValue = rMatchString[i] - 0x20;
      lIndexValue += nValue * nValue * static_cast<int> (i + 1);
   }

	// if Index value is out of range, definitely there won't be a match in the file.
   if(lIndexValue >= m_lLowValue && lIndexValue <= m_lHighValue)
   {
      int nActualIndex = (int)((((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5);
		// Check whether the string at this location is the matchString.

      while(m_pIndexTable[nActualIndex] != 0) 
      {
         TCHAR *pData = m_pDataArr[m_pIndexTable[nActualIndex] - 1];

         if(rMatchString  == pData)
         {
            nReturnIndex = m_pIndexTable[nActualIndex];
            break;
         }
         nActualIndex++;

			// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
         if(nActualIndex >= m_nTotalCount * 10 )
		 {
            nActualIndex = 2;
		 }
      }
   }

   return  nReturnIndex;
}

bool SVBarCodeResultClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if ( !m_SVRegExpressionObjectInfo.IsConnected() || nullptr == m_SVRegExpressionObjectInfo.GetInputObjectInfo().m_pObject )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	
	return true;
}
