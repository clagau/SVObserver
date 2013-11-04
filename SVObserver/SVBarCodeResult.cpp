//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeResultClass
//* .File Name       : $Workfile:   SVBarCodeResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 12:15:52  $
//******************************************************************************

// SVBarCodeResult.cpp: implementation of the SVBarCodeResultClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVBarCodeResult.h"
#include "SVGlobal.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS (SVBarCodeResultClass, SVBarCodeResultClassGuid);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVBarCodeResultClass::SVBarCodeResultClass (BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID)
  : SVStringResultClass(BCreateDefaultTaskList, POwner, StringResourceID)
{
	m_pBuffer = NULL;
	m_pIndexTable = NULL;
	m_lTotalBytes = 0;

	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVResultBarCodeObjectType;

  RegisterEmbeddedObject(
		&msv_bUseSingleMatchString, 
	  SVBCUseSingleMatchStringGuid, 
	  IDS_OBJECTNAME_BC_USE_SINGLE_MATCH_STRING,
		false, SVResetItemOwner );

  RegisterEmbeddedObject(
		&msv_bUseMatchStringFile, 
		SVBCUseMatchStringFileGuid, 
		IDS_OBJECTNAME_BC_USE_MATCH_STRING_FILE,
		false, SVResetItemOwner );

  RegisterEmbeddedObject(
		&msv_szMatchStringFileName, 
	  SVBCMatchStringFileNameObjectGuid, 
	  IDS_OBJECTNAME_BC_MATCH_STRING_FILE_NAME,
		false, SVResetItemOwner );

  RegisterEmbeddedObject(
		&msv_lMatchStringLine, 
		SVBCMatchStringLineGuid, 
		IDS_OBJECTNAME_BC_MATCH_STRING_LINE,
		false, SVResetItemNone );

  RegisterEmbeddedObject(
		&m_dReadScore,
		SVBCMatchScoreGuid,
		IDS_OBJECTNAME_BC_READ_SCORE,
		false, SVResetItemNone );

	msv_bUseSingleMatchString.SetDefaultValue( TRUE, TRUE );
  msv_szMatchStringFileName.SetDefaultValue(_T(""), TRUE);
	msv_lMatchStringLine.SetDefaultValue( 0, TRUE );
	msv_bUseMatchStringFile.SetDefaultValue( FALSE, TRUE );
	m_dReadScore.SetDefaultValue( ( double ) -1.0, TRUE );
  // Specify which string SVResultString should require
	m_SVInputStringObjectInfo.SetInputObjectType( SVBarCodeObjectGuid );

	// Identify our input type needs
	m_SVRegExpressionObjectInfo.SetInputObjectType( SVRegExpressionObjectGuid, SVStringValueObjectType );
	m_SVRegExpressionObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_SVRegExpressionObjectInfo, _T( "BarCodeResultString" ) );

	// Register Embedded Objects
//	RegisterEmbeddedObject( &m_szValue, SVStringResultObjectGuid, IDS_OBJECTNAME_VALUE );

}

SVBarCodeResultClass::~SVBarCodeResultClass()
{
	if(m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}

	if(m_pIndexTable)
	{
		delete m_pIndexTable;
		m_pIndexTable = NULL;
	}
}

BOOL SVBarCodeResultClass::CreateObject(SVObjectLevelCreateStruct *PCreateStructure)
{
	BOOL bOk = FALSE;

	if( SVStringResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = getInputString() && getRegExpression() && (LoadMatchStringFile() == S_OK);
		if (bOk)
		{
			getRegExpression()->ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
		}
	}

	msv_bUseSingleMatchString.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_szMatchStringFileName.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	msv_lMatchStringLine.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msv_bUseMatchStringFile.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_dReadScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	isCreated = bOk;

	return bOk;
}

BOOL SVBarCodeResultClass::CloseObject()
{
  return SVStringResultClass::CloseObject();
}

SVStringValueObjectClass* SVBarCodeResultClass::getInputString()
{
	if( m_SVInputStringObjectInfo.IsConnected() && m_SVInputStringObjectInfo.GetInputObjectInfo().PObject )
		return ( SVStringValueObjectClass* ) m_SVInputStringObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

SVStringValueObjectClass* SVBarCodeResultClass::getRegExpression()
{
	if( m_SVRegExpressionObjectInfo.IsConnected() && m_SVRegExpressionObjectInfo.GetInputObjectInfo().PObject )
		return ( SVStringValueObjectClass* ) m_SVRegExpressionObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

BOOL SVBarCodeResultClass::OnValidate()
{
	BOOL bRetVal = m_SVRegExpressionObjectInfo.IsConnected();

	bRetVal = bRetVal && m_SVRegExpressionObjectInfo.GetInputObjectInfo().PObject != NULL;
	bRetVal = bRetVal && SVStringResultClass::OnValidate();

	BOOL bLoad = FALSE;

	msv_bUseMatchStringFile.GetValue( bLoad );

	if ( bLoad )
	{
		bRetVal = m_lTotalBytes > 0;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

BOOL SVBarCodeResultClass::onRun(SVRunStatusClass &RRunStatus)
{
  if( SVStringResultClass::onRun( RRunStatus ) )
  {
	  if( this->m_bFailedToRead )
	  {
		  RRunStatus.SetWarned();
		  return TRUE;
	  }

	SVStringValueObjectClass* pValue = getInputString();

    if (pValue->IsValid())
    {
			BOOL bLoad = FALSE;

      CString szInputString;
      pValue->GetValue(szInputString);
 
			msv_bUseMatchStringFile.GetValue( bLoad );
      if ( bLoad )
      {
				long lLine = CheckStringInTable( szInputString );
				msv_lMatchStringLine.SetValue( RRunStatus.m_lResultDataIndex, lLine );
				if ( 0 < lLine )
				{
	        RRunStatus.SetPassed();
				}
				else
				{
					RRunStatus.SetFailed();
				}
      }
      else
      {
				CString szRegExpression;

				SVStringValueObjectClass* pRegExp = getRegExpression();

		    pRegExp->GetValue(szRegExpression);

				if (szRegExpression.IsEmpty() || !szInputString.Compare((LPCTSTR) szRegExpression))
				{
					RRunStatus.SetPassed();
				}
				else
				{
					RRunStatus.SetFailed();
				}
      }
    }
    else
    {
      RRunStatus.SetFailed();
    }
    return TRUE;
  }
	RRunStatus.SetInvalid();
  SetInvalid ();
	return FALSE;
}

DWORD SVBarCodeResultClass::processMessage(DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext)
{
	DWORD dwResult = 0L;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS :
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				dwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				dwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return (dwResult | SVStringResultClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}

HRESULT SVBarCodeResultClass::ResetObject()
{
	HRESULT l_hrOk = SVStringResultClass::ResetObject();

	if( LoadMatchStringFile() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVBarCodeResultClass::LoadMatchStringFile()
{
	HRESULT hrRet = S_OK;
	BOOL bOk = TRUE;
	BOOL bLoad = FALSE;

	if ( m_pBuffer != NULL )
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}

	m_lTotalBytes = 0;

	msv_bUseMatchStringFile.GetValue( bLoad );
	if ( bLoad )
	{
		CString csFileName;
		CFile matchFile;

		// Check to see if the file exists..
		//
		msv_szMatchStringFileName.GetValue( csFileName );

		bOk = ! csFileName.IsEmpty() && SVFileExists( csFileName );
		if ( bOk )
		{
			//
			// Open and read the Bar Code match string from the file.
			//
			try
			{
				bOk = matchFile.Open( csFileName, CFile::modeRead | CFile::shareDenyNone);
				if ( bOk )
				{
					unsigned long ulLength = static_cast<unsigned long>(matchFile.GetLength());

					if ( 0 < ulLength )
					{
						m_pBuffer = new char[ulLength + 1];
						bOk = m_pBuffer != NULL;
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
						bOk = FALSE;
						//set tool to be invalid
					   /* SVToolClass *l_pTool = (SVToolClass*) GetAncestor(SVToolObjectType);
					   if ( l_pTool )
					   {
						   l_pTool->SetInvalid();
					   }
					   */
					}
				}
				else
				{
					hrRet = FALSE;
				}
			}
			catch ( ... )
			{
				hrRet = S_FALSE;
				bOk = FALSE;
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
         int nCharCount = strlen(pData);
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

int SVBarCodeResultClass::CheckStringInTable(CString MatchString)
{
   int nReturnIndex = -1;
//   m_dFactor = ((double)(m_lHighValue - m_lLowValue)) / (double)m_nTotalCount;

   long  lIndexValue = 0;
   int   nCharCount = MatchString.GetLength();

   for (int i = 0; i < nCharCount; i++)
   {
      int   nValue = MatchString.GetAt(i) - 0x20;
      lIndexValue += nValue * nValue * (i + 1);
   }

// if Index value is out of range, definitely there won't be a match in the file.
   if(lIndexValue >= m_lLowValue && lIndexValue <= m_lHighValue)
   {
      int nActualIndex = (int)((((double)(lIndexValue - m_lLowValue)) / m_dFactor) * 9.5);
// Check whether the string at this location is the matchString.

      while(m_pIndexTable[nActualIndex] != 0) 
      {
         char *pData = m_pDataArr[m_pIndexTable[nActualIndex] - 1];

         if(MatchString  == (CString)pData)
         {
            nReturnIndex = m_pIndexTable[nActualIndex];
            break;
         }
         nActualIndex++;

// if we reach the end of the hash table, start from the begining( starting from 3rd position) 
         if(nActualIndex >= m_nTotalCount * 10 )
            nActualIndex = 2;
      }
   }

   return  nReturnIndex;

}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBarCodeResult.cpp_v  $
 * 
 *    Rev 1.1   13 May 2013 12:15:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   02 May 2013 11:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:42:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   12 Jul 2012 14:37:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   18 Jun 2012 17:51:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   27 Jan 2011 10:47:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   04 Mar 2008 09:57:46   JHanebac
 * Project:  SVObserver
 * Change Request (SCR) nbr:  620
 * SCR Title:  Barcode Analyzer property changes
 * Checked in by:  jHanebac;  Jack Hanebach
 * Change Description:  
 *   Changed properties of match string and match string file name to read/write.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   22 Jun 2007 10:35:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   21 Jun 2007 11:56:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   20 Oct 2005 14:19:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle a reset problem with the multi-result match string file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   29 Jul 2005 12:03:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   22 Jun 2005 09:16:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   27 Apr 2005 10:08:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  465
 * SCR Title:  Fix Bug in Bar Code Match String Files and OCR Analyzer Match String Files
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add code to prevent divide by zero on Build Hash Table.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Feb 2005 13:12:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Sep 2004 11:15:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  444
 * SCR Title:  Change Bar Code Analyzer Pass / Fail Result
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Constructor, CreateObject and OnRun to support new ValueObject ReadScore, and variable m_bFailedToRead flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Jul 2004 10:38:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  431
 * SCR Title:  Fix problem with using match string files that are zero elements
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   put in check to make sure that if using a match string file that the number of elements is above 0.  if not, the inspection will not be allowed to go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2003 16:54:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Dec 2002 18:56:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
