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

#include "stdafx.h"
#include "svobserver.h"
#include "SVRegressionFileSelectSheet.h"
#include "SVRegressionFileSelectDlg.h"
#include "SVCameraImageTemplate.h"
#include "SVVirtualCamera.h"


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
	
	//svImageManager->GetSize(lNumberOfCameras);
	

	SVVirtualCameraPtrSet::iterator iter;

	CString sPrevName = "Blank";
	for ( iter = CameraList.begin(); iter != CameraList.end(); ++iter )
	{
		if ( sPrevName.CompareNoCase( (*iter)->GetName() ) != 0 )
		{
			CSVRegressionFileSelectDlg *pPage = new CSVRegressionFileSelectDlg((*iter)->GetName());
			AddPage(pPage);
			sPrevName = (*iter)->GetName();
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
				pStruct->csCamera = pPage->GetPageName();
				pStruct->csFirstFile = pPage->GetSelectedFile();
				pStruct->csFileMask = MakeFileNameMask(pStruct->csFirstFile);
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
			AfxMessageBox("Selection Error:  Lists must be the same size.  Please select again.");
			ClearRegressionList();
			return;
		}
		case SelectionInvalid:
		{
			if ( AfxMessageBox("Selection Error: \"Use single file\" is selected for one of the cameras but a file was not selected. Select again?",MB_YESNO ) == IDYES )
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
			if ( AfxMessageBox("Selection Error: \"Use list of files\" selected, but the file name does not match\n the acceptable format (<file name>_<sequence number>.bmp). Select again?",MB_YESNO ) == IDYES )
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
			if ( AfxMessageBox("Selection Error: \"Use list of files\" selected, but the file name does not match\n the acceptable format (<file name>_<sequence number>.bmp). Select again?",MB_YESNO ) == IDYES )
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
			if ( AfxMessageBox("Selection Error:  \"No files\" selected.  Select again?",MB_YESNO ) == IDYES )
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
			if ( AfxMessageBox("Selection Error:  File does not exist.  Select again?",MB_YESNO ) == IDYES )
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

CString CSVRegressionFileSelectSheet::MakeFileNameMask(CString csFileName)
{
	CString sTempName(_T(""));
	int iFileNameLength = csFileName.GetLength();

	//check to see if last 4 chars = .bmp 
	if ( csFileName.Right(4).CompareNoCase(".bmp") == 0 )
	{
		sTempName = csFileName.Left(iFileNameLength-4);
	
		int iPos = sTempName.ReverseFind('_');
		if ( iPos > -1 )
		{
			sTempName = csFileName.Left(iPos+1);
			sTempName = sTempName +"*.bmp";
		}
	}

	return sTempName;
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
				if ( pStruct->csFirstFile.IsEmpty() )
				{
					return SelectionInvalid;
				}
				else 
				{
					//check to make sure name is a file that exists
					CFile TempFile;
					if ( TempFile.Open(pStruct->csFirstFile,CFile::modeRead) )
					{
						l_lTotalNumFiles++;
						TempFile.Close();
					}
					else
					{
						return SelectionFileNotExist;
					}
				}
				
			}

			if ( pStruct->iFileMethod == RegFileList )
			{
				if (!pStruct->csFileMask.IsEmpty())
				{
					CString sFullName;
					long lNmbFiles = GetNumberOfFilesMatchingMask(pStruct->csFileMask);
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

long CSVRegressionFileSelectSheet::GetNumberOfFilesMatchingMask(CString sMask)
{
	long lNumCount = 0;
	CFileFind fileFinder;

	BOOL bFound = fileFinder.FindFile(sMask);

	while ( bFound )
	{
		lNumCount++;
		bFound = fileFinder.FindNextFile();
	}


	return lNumCount;
}


BOOL CSVRegressionFileSelectSheet::ValidateMask(CString sMask)
{
	BOOL bRet = FALSE;

	CString sMaskStr;
	sMaskStr = MakeFileNameMask(sMask);

	if ( m_listMasks.IsEmpty() )
	{
		m_listMasks.AddTail(sMaskStr);
		m_listMasks.AddHead((CString)sMaskStr);
		bRet = TRUE;
	}
	else
	{
		POSITION pos = m_listMasks.Find(sMaskStr,NULL);
		if ( pos == NULL )
		{
			//add to list
			m_listMasks.AddTail((CString)sMaskStr);
			bRet = TRUE;
		}
		else //pos != null
		{
			bRet = FALSE;
		}
	}

	return bRet;
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
				pStruct->stdVectorFile.push_back(pStruct->csFirstFile);
				std::vector<CString>::iterator iter;

				iter = std::find(pStruct->stdVectorFile.begin(),pStruct->stdVectorFile.end(),pStruct->csFirstFile);

				//set the structs starting and current position to the start
				pStruct->stdIteratorStart = iter;
				pStruct->stdIteratorCurrent = pStruct->stdIteratorStart;
				m_pRegressionList->SetAt(pos,pStruct);
				continue;
			}

			BOOL bFound = fileFinder.FindFile(pStruct->csFileMask);

			while ( bFound )
			{
				bFound = fileFinder.FindNextFile();
				pStruct->stdVectorFile.push_back(fileFinder.GetFilePath());
			}
			std::sort(pStruct->stdVectorFile.begin(),pStruct->stdVectorFile.end());
			//find the starting position
			std::vector<CString>::iterator iter;

			iter = std::find(pStruct->stdVectorFile.begin(),pStruct->stdVectorFile.end(),pStruct->csFirstFile);

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
	CString sTmpName;

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
				sTmpName = pPage->GetPageName();
				
				bool l_bFound = false;
				for ( int iPos = 0; iPos <= iRegListCnt && !l_bFound; iPos++ )
				{
					POSITION pos = m_pRegressionList->FindIndex(iPos);
					if ( pos )
					{
						pTmpStruct = m_pRegressionList->GetAt(pos);
						if ( pTmpStruct->csCamera == sTmpName )
						{
							pPage->SetRegressionData(pTmpStruct);
							l_bFound = true;
						} // if pTmpSturct->csCamera
					} //if pos
				} // for int iPos
			} //if (pPage)
		}//for iCnt
	}

	return bResult;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRegressionFileSelectSheet.cpp_v  $
 * 
 *    Rev 1.3   04 Sep 2014 12:44:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  765
 * SCR Title:  Fix crash due to issue with selecting files for Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made changes to the error messages 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jul 2014 14:03:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  765
 * SCR Title:  Fix crash due to issue with selecting files for Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed OnOk to display more descriptive error messages and to clear the regression data if they do not want to re-select files.  Changed ValidateList to look for other invalid conditions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 07:12:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:41:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Oct 2012 10:53:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Sep 2011 13:13:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Apr 2011 15:31:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Dec 2010 13:35:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Nov 2010 16:30:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Dec 2005 15:29:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  527
 * SCR Title:  Fix problems with Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with the regression test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Aug 2005 09:29:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem of 2 Camera 1's showing up on a color SVIM.  Also fixed a problem if 2 cameras are attached to the inspection and camera 1 is selected for no files.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2005 11:20:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jul 2005 13:59:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code to save the current settings and will also save the regression file select path in the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2005 11:34:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
