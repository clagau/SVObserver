// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionFileSelectSheet
// * .File Name       : $Workfile:   SVRegressionFileSelectSheet.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   04 Sep 2014 12:44:38  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "svobserver.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVRegressionFileSelectDlg.h"
#include "SVCameraImageTemplate.h"
#include "SVVirtualCamera.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSVRegressionFileSelectSheet, CPropertySheet)

CSVRegressionFileSelectSheet::CSVRegressionFileSelectSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSVRegressionFileSelectSheet::CSVRegressionFileSelectSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CSVRegressionFileSelectSheet::~CSVRegressionFileSelectSheet()
{
	int iPageCnt = GetPageCount();
	CSVRegressionFileSelectDlg *pPage;

	for (int i = iPageCnt-1; i >= 0; i-- )
	{
		pPage = (CSVRegressionFileSelectDlg*) GetPage(i);
		RemovePage(i);
		if ( pPage )
			delete pPage;
	}

	int iStructCnt = static_cast<int>(m_pRegressionList->GetCount());
}


BEGIN_MESSAGE_MAP(CSVRegressionFileSelectSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSVRegressionFileSelectSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVRegressionFileSelectSheet message handlers

void CSVRegressionFileSelectSheet::CreatePages(CList<RegressionTestStruct*,RegressionTestStruct*>*RegessionList, SVVirtualCameraPtrSet CameraList )
{
	m_pRegressionList = RegessionList;
	
	SVVirtualCameraPtrSet::iterator iter;

	SVString PrevName( _T("Blank") );
	for ( iter = CameraList.begin(); iter != CameraList.end(); ++iter )
	{
		if ( 0 != SvUl_SF::CompareNoCase( PrevName, SVString((*iter)->GetName()) ) )
		{
			CSVRegressionFileSelectDlg *pPage = new CSVRegressionFileSelectDlg((*iter)->GetName());
			AddPage(pPage);
			PrevName = (*iter)->GetName();
		}
	}
	
	int iPageCnt = GetPageCount();

	for ( int i = 1; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);
	
}


void CSVRegressionFileSelectSheet::OnOK()
{
	int iNumPages = GetPageCount();

	if ( !m_pRegressionList->IsEmpty() )
	{
		ClearRegressionList();
	}

	for ( int i = 0; i < iNumPages; i++ )
	{
		RegressionTestStruct *pStruct = new RegressionTestStruct;
		CSVRegressionFileSelectDlg *pPage = (CSVRegressionFileSelectDlg*)GetPage(i);
		if ( (pPage) && (pPage->GetSafeHwnd()) )
		{
			if ( pPage->GetFileSelectType() != RegNone )
			{
				pStruct->iFileMethod = pPage->GetFileSelectType();
				pStruct->Camera = pPage->GetPageName();
				pStruct->FirstFile = pPage->GetSelectedFile();
				pStruct->FileMask = MakeFileNameMask(pStruct->FirstFile );
				pStruct->bDone = false;
				m_pRegressionList->AddTail(pStruct);
			}
		}
		//check to make sure that if using file list that all are of same size.
	}

	switch (ValidateList())
	{
		case SelectGood:
		{
			FillFileList();
			for ( int i = 0; i < iNumPages; i++ )
			{
				CSVRegressionFileSelectDlg *pPage = (CSVRegressionFileSelectDlg*)GetPage(i);
				if ( (pPage) && (pPage->GetSafeHwnd()) )
				{
					pPage->OnOK();
				}
			}

			break;
		}
		case SelectionBadList:
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_WrongListSize, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10188 ); 
			ClearRegressionList();
			return;
		}
		case SelectionInvalid:
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_NoFileSelected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10189, SV_GUID_NULL, MB_YESNO ); 
			if ( IDYES == result )
			{
				return;
			}
			else
			{
				ClearRegressionList();
				EndDialog(IDCANCEL);
				return;
			}
		}
		case SelectionEmptyList:
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_WrongFormat, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10190, SV_GUID_NULL, MB_YESNO ); 
			if ( IDYES == result )
			{
				ClearRegressionList();
				return;
			}
			else
			{
				ClearRegressionList();
				EndDialog(IDCANCEL);
				return;
			}
		}
		case SelectionInvalidMask:
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_WrongFormat, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10191, SV_GUID_NULL, MB_YESNO ); 
			if ( IDYES == result )
			{
				return;
			}
			else
			{
				ClearRegressionList();
				EndDialog(IDCANCEL);
				return;
			}
		}
		case SelectionNoFiles:
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_NoFilesSelected, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10192, SV_GUID_NULL, MB_YESNO ); 
			if ( IDYES == result )
			{
				return;
			}
			else
			{
				ClearRegressionList();
				EndDialog(IDCANCEL);
				return;
			}

		}
		case SelectionFileNotExist:
		{
			SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
			INT_PTR result = Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_RegressionTest_FileNotExist, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10193, SV_GUID_NULL, MB_YESNO ); 
			if ( IDYES == result )
			{
				return;
			}
			else
			{
				ClearRegressionList();
				EndDialog(IDCANCEL);
				return;
			}

		}
	}//end switch...


	EndDialog(IDOK);
}

SVString CSVRegressionFileSelectSheet::MakeFileNameMask( const SVString& rFileName )
{
	SVString Result;
	size_t FileNameLength = rFileName.size();

	//check to see if last 4 chars = .bmp 
	if( 0 == SvUl_SF::CompareNoCase( SvUl_SF::Right(rFileName, 4), SVString( _T(".bmp") ) ) )
	{
		Result = SvUl_SF::Left( rFileName, FileNameLength - 4 );
	
		size_t Pos = Result.rfind('_');
		if ( SVString::npos != Pos )
		{
			Result = SvUl_SF::Left( rFileName, Pos+1 );
			Result = Result + _T("*.bmp");
		}
	}

	return Result;
}
	
RegressionFileSelectCode CSVRegressionFileSelectSheet::ValidateList()
{
	BOOL bRet = TRUE;
	RegressionFileSelectCode l_ReturnCode = SelectGood;

	long l_lTotalNumFiles = 0;

	int iCount = static_cast<int>(m_pRegressionList->GetCount());
	long lListCountSize = 0;

	for ( int i = 0; i < iCount; i++ )
	{
		RegressionTestStruct *pStruct;// = new RegressionTestStruct;

		POSITION pos = m_pRegressionList->FindIndex(i);
		if ( pos )
		{
			pStruct = m_pRegressionList->GetAt(pos);

			if ( pStruct->iFileMethod == RegSingleFile )
			{
				//check to see if a selection has been made...
				if ( pStruct->FirstFile.empty() )
				{
					return SelectionInvalid;
				}
				else 
				{
					//check to make sure name is a file that exists
					if ( 0 == ::_access( pStruct->FirstFile.c_str(), 0 ) )
					{
						l_lTotalNumFiles++;
					}
					else
					{
						return SelectionFileNotExist;
					}
				}
				
			}

			if ( pStruct->iFileMethod == RegFileList )
			{
				if( !pStruct->FileMask.empty() )
				{
					long lNmbFiles = GetNumberOfFilesMatchingMask(pStruct->FileMask);
					l_lTotalNumFiles += lNmbFiles;

					if ( 0  < lNmbFiles )
					{
						//check number of files...
						if ( lListCountSize == 0 )
						{
							lListCountSize = lNmbFiles;
						}
						else // check to make sure that size matches the others
						{
							if ( lNmbFiles != lListCountSize )
							{
								bRet = FALSE;
								l_ReturnCode = SelectionBadList;

							}
						}
					}
					else
					{
						bRet = FALSE;
						l_ReturnCode = SelectionInvalidMask;
					}
				}
				else
				{
					bRet = FALSE;
					l_ReturnCode = SelectionEmptyList;
				}
			}
		}
	}

	if ( bRet )
	{
		//check to see if total num of files is > 0
		if ( l_lTotalNumFiles == 0 )
		{
			bRet = FALSE;
			l_ReturnCode = SelectionNoFiles;
		}
		else
		{
			l_ReturnCode = SelectGood;
		}
	}

	return l_ReturnCode;
}

long CSVRegressionFileSelectSheet::GetNumberOfFilesMatchingMask(const SVString& rMask)
{
	long lNumCount = 0;
	CFileFind fileFinder;

	BOOL bFound = fileFinder.FindFile(rMask.c_str());

	while ( bFound )
	{
		lNumCount++;
		bFound = fileFinder.FindNextFile();
	}


	return lNumCount;
}

void CSVRegressionFileSelectSheet::ClearRegressionList()
{
	int iCount = static_cast<int>(m_pRegressionList->GetCount());
	for ( int i = iCount-1; i >= 0; i-- )
	{
		POSITION pos = m_pRegressionList->FindIndex(i);
		if ( pos )
		{
			m_pRegressionList->RemoveAt(pos);
		}
	}
}

void CSVRegressionFileSelectSheet::FillFileList()
{
	int iCnt = static_cast<int>(m_pRegressionList->GetCount());

	for ( int i = 0; i <= iCnt; i++ )
	{
		RegressionTestStruct *pStruct;

		POSITION pos = m_pRegressionList->FindIndex(i);

		if ( pos )
		{
			pStruct = m_pRegressionList->GetAt(pos);

			CFileFind fileFinder;

			if ( pStruct->iFileMethod == RegSingleFile )
			{
				pStruct->stdVectorFile.push_back(pStruct->FirstFile);
				SVStringVector::iterator iter;

				iter = std::find(pStruct->stdVectorFile.begin(),pStruct->stdVectorFile.end(),pStruct->FirstFile);

				//set the structs starting and current position to the start
				pStruct->stdIteratorStart = iter;
				pStruct->stdIteratorCurrent = pStruct->stdIteratorStart;
				m_pRegressionList->SetAt(pos,pStruct);
				continue;
			}

			BOOL bFound = fileFinder.FindFile(pStruct->FileMask.c_str());

			while ( bFound )
			{
				bFound = fileFinder.FindNextFile();
				pStruct->stdVectorFile.push_back( SVString(fileFinder.GetFilePath()) );
			}
			std::sort(pStruct->stdVectorFile.begin(),pStruct->stdVectorFile.end());
			//find the starting position
			SVStringVector::iterator iter;

			iter = std::find(pStruct->stdVectorFile.begin(),pStruct->stdVectorFile.end(),pStruct->FirstFile);

			//set the structs starting and current position to the start
			pStruct->stdIteratorStart = iter;
			pStruct->stdIteratorCurrent = pStruct->stdIteratorStart;
			m_pRegressionList->SetAt(pos,pStruct);
		}
	}

}

BOOL CSVRegressionFileSelectSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	RegressionTestStruct *pTmpStruct;

	int iPageCnt = GetPageCount();

	for ( int i = 0; i <= iPageCnt; i++ )
	{
		SetActivePage(i);
	}
	SetActivePage(0);

	CSVRegressionFileSelectDlg *pPage;
	int iRegListCnt = static_cast<int>(m_pRegressionList->GetCount());
	if ( iRegListCnt > 0 )
	{
		for ( int l_iCount = 0; l_iCount <= iPageCnt-1; l_iCount++ )
		{
			pPage = (CSVRegressionFileSelectDlg*)GetPage(l_iCount);
			if ( pPage )
			{
				SVString TmpName = pPage->GetPageName();
				
				bool l_bFound = false;
				for ( int iPos = 0; iPos <= iRegListCnt && !l_bFound; iPos++ )
				{
					POSITION pos = m_pRegressionList->FindIndex(iPos);
					if ( pos )
					{
						pTmpStruct = m_pRegressionList->GetAt(pos);
						if ( pTmpStruct->Camera == TmpName )
						{
							pPage->SetRegressionData(pTmpStruct);
							l_bFound = true;
						}
					}
				}
			}
		}
	}

	return bResult;
}

