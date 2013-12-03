//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVView
//* .File Name       : $Workfile:   SVView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:39:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVView.h"
#include "SVClassInfoStruct.h"

#include "SVImageClass.h"
#include "SVImageListClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : Class SVToolWindow
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

BEGIN_MESSAGE_MAP(SVToolWindow, CWnd)
	//{{AFX_MSG_MAP(SVToolWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolWindow::SVToolWindow()
{

}

SVToolWindow::~SVToolWindow()
{
}





// Moved SVDlgImageClass to SVDlgImage.h and SVDlgImage.cpp, RO_20Mar2000





//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : Class SVSliderCtrl
//* Note(s)    : // e.g. Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

BEGIN_MESSAGE_MAP(SVSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(SVSliderCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SVSliderCtrl::SVSliderCtrl()
{
	isClicked = FALSE;
}


SVSliderCtrl::~SVSliderCtrl()
{
}






//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : Class SVAvailableAnalyzerListComboBoxClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

BEGIN_MESSAGE_MAP(SVAvailableAnalyzerListComboBoxClass, CComboBox)
	//{{AFX_MSG_MAP(SVAvailableAnalyzerListComboBoxClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVAvailableAnalyzerListComboBoxClass::SVAvailableAnalyzerListComboBoxClass()
{
}

SVAvailableAnalyzerListComboBoxClass::~SVAvailableAnalyzerListComboBoxClass()
{

}

int SVAvailableAnalyzerListComboBoxClass::init( SVClassInfoStructListClass* L, int currentSelection )
{
	ResetContent();

	int index;
	int selection = LB_ERR;
	if( L )
	{
		for( int i = 0; i < L->GetSize(); ++i )
		{
			index = AddString( L->GetAt( i ).ClassName );
			SetItemData( index, ( DWORD ) i );

			if (i == currentSelection)
				selection = currentSelection;
		}
	
		if( L->GetSize() <= 0 )
		{
			index = AddString( _T( "(No Analyzer Available)" ) );
			SetItemData( index, LB_ERR );
		}
		else
		{
			index = AddString( _T( "(No Analyzer)" ) );
			SetItemData( index, LB_ERR );
		}
	
		// The List is sorted - so find the item to be selected
		if( selection == LB_ERR )
		{
			selection = 0;
		}
		else
		{
			for (int i = 0;i < GetCount(); i++ )
			{
				int item = static_cast<int>(GetItemData( i ));
				if( item == currentSelection )
				{
					selection = i;
					break;
				}
			}
		}
		SetCurSel( selection );

		return static_cast<int>(GetItemData( GetCurSel() ));
	}
	return LB_ERR;
}

void SVAvailableAnalyzerListComboBoxClass::add (LPCTSTR ClassName)
{
	long index = AddString( ClassName );
	SetItemData( index, LB_ERR );
	SetCurSel(index);

}

void SVAvailableAnalyzerListComboBoxClass::remove(LPCTSTR ClassName)
{
	int iIndex = FindString(0,ClassName);
	
	if ( iIndex != LB_ERR )
	{
		DeleteString(iIndex);
	}
}








//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : Class SVAvailableSourceImageListComboBoxClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

BEGIN_MESSAGE_MAP(SVAvailableSourceImageListComboBoxClass, CComboBox)
	//{{AFX_MSG_MAP(SVAvailableSourceImageListComboBoxClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVAvailableSourceImageListComboBoxClass::SVAvailableSourceImageListComboBoxClass()
{
}

SVAvailableSourceImageListComboBoxClass::~SVAvailableSourceImageListComboBoxClass()
{
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:23.11.1998 RO			BugFix, CurrentSelection is now correct!
////////////////////////////////////////////////////////////////////////////////
int SVAvailableSourceImageListComboBoxClass::init( SVImageListClass* pImageList, SVImageClass* pCurrentSelectedImage )
{
	// Clear list...
	ResetContent();
	
	// Generate new list...
	if( pImageList && pImageList->GetSize() > 0 )
	{
		for( int i = 0; i < pImageList->GetSize(); ++i )
		{
			SVImageClass* pImage = pImageList->GetAt( i );
			if( pImage )
			{
				const SVObjectTypeInfoStruct& rObjectTypeInfo = pImage->GetObjectInfo().ObjectTypeInfo;
				CString strName;
				switch( rObjectTypeInfo.SubType )
				{
					case SVRGBMainImageObjectType:	// RGBMain image - Not selectable
						break;
						
					case SVMainImageObjectType:	// Main image
						if( pImage->GetOwner() )
						{
							strName = pImage->GetOwner()->GetName();
							strName += _T( ".Image1" );
						}
						break;
						
					default:
					{
						SVImageInfoClass imageInfo = pImage->GetImageInfo();

						if( imageInfo.GetOwner() )
						{
							/*
							strName = imageInfo.POwnerTool->GetName();
							
							// Handle Multiple Image outputs...
							
							CString imageName = pImage->GetName();
							strName += _T( " " );
							strName += imageName;
							//*/
							strName = pImage->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType );
						}

						break;
					}// end default:
				}// end switch( rObjectTypeInfo.SubType )

				if( !strName.IsEmpty() )
				{
					SetItemData( AddString( strName ), ( DWORD ) i );
				}
				else
				{
				}
			}// end if( pImage )
			else
			{
			}
		}// end for( int i = 0; i < pImageList->GetSize(); ++i )
		
		// Find out the current selection...
		int index  = 0;
		for( int j = 0; j < GetCount(); ++j )
		{
			if( ( index = ( int ) GetItemData( j ) ) >= 0 && index < pImageList->GetSize() && ( (DWORD) index ) != LB_ERR )
			{
				if( pImageList->GetAt( index ) == pCurrentSelectedImage )
				{
					SetCurSel( j );
					return index;
				}
			}
		}
	}// end if( pImageList && pImageList->GetSize() > 0 )
	
	
	// Error
	if( GetCount() == 0 )
		SetItemData( AddString( _T( "(No Image Available)" ) ), LB_ERR );
	SetCurSel( -1 );
	return -1;
}






//** EOF **


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVView.cpp_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:39:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:20:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   05 Mar 2013 13:40:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added the add & remove method, to add/remove an analyzer name to the combo box
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   25 Feb 2011 12:27:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   17 Feb 2005 14:53:08   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changes to support new extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   12 Jul 2004 12:05:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     cleaned up code
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   22 Apr 2003 17:30:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   31 Jan 2003 09:37:36   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated init method to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   20 Nov 2002 15:01:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Remove SVSystem.h
   Remove SVEvent.h
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Oct 2001 17:01:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  235
   SCR Title:  Image source list is truncated on color system
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVAvailableSourceImageListComboBoxClass::init to add fully qualified image names back to the tool level.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Oct 2001 17:01:40   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  235
   SCR Title:  Image source list is truncated on color system
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVAvailableSourceImageListComboBoxClass::init to add fully qualified image names back to the tool level.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   06 Sep 2000 16:37:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   23 Mar 2000 16:53:40   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed SVDlgImageClass (now in SVDlgImage.cpp/h)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   03 Mar 2000 13:03:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  112
   SCR Title:  Expose Extents of Images
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use SVImageInfoClass methods to get image extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:32:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed reference to SVToolSetViewClass and added header.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/

