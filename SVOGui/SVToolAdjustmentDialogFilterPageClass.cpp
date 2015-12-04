//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogFilterPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogFilterPageClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   24 Oct 2014 11:45:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogFilterPageClass.h"

#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/ISVFilter.h"
#include "ObjectInterfaces/ISVImage.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ObjectDefines.h"
#include "SVCustomFilterDlg.h"
#include "SVRankingFilterDlg.h"
#include "SVThinningFilterDlg.h"
#include "SVThickeningFilterDlg.h"
#include "Custom2FilterDlg.h"
#include "SVWatershedFilterDlg.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SvOi;

namespace Seidenader
{
	namespace SVOGui
	{
		BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogFilterPageClass, CPropertyPage)
			//{{AFX_MSG_MAP(SVToolAdjustmentDialogFilterPageClass)
			ON_BN_CLICKED(IDC_BUTTON4, OnButtonProperties)
			ON_BN_CLICKED(IDC_BUTTON3, OnButtonClearAll)
			ON_BN_CLICKED(IDC_BUTTON2, OnButtonDeleteCurrentFilter)
			ON_BN_CLICKED(IDC_BUTTON1, OnButtonInsertNewFilter)
			ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		SVToolAdjustmentDialogFilterPageClass::SVToolAdjustmentDialogFilterPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID) 
		: CPropertyPage(SVToolAdjustmentDialogFilterPageClass::IDD)
		, availableFilters( SvOi::createSVClassInfoStructList() )
		, m_rTool(*(dynamic_cast<SvOi::ITaskObjectListClass *>(SvOi::getObject(rTaskObjectID))))
		, m_pUnaryImageOperatorList(nullptr)
		{
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVUnaryImageOperatorListObjectType;

			// Get The UnaryImageOperatorList
			IObjectClass *pTmpObject = m_rTool.GetFirstObject(info);
			m_pUnaryImageOperatorList = dynamic_cast<ITaskObjectListClass*>(pTmpObject);

			if( m_pUnaryImageOperatorList )
			{
				info.ObjectType = SVFilterObjectType;
				m_pUnaryImageOperatorList->getAvailableObjects(*availableFilters, info);
			}
		}

		SVToolAdjustmentDialogFilterPageClass::~SVToolAdjustmentDialogFilterPageClass()
		{
			deleteSVClassInfoStructList(availableFilters);
		}

		HRESULT SVToolAdjustmentDialogFilterPageClass::SetInspectionData()
		{
			HRESULT l_hrOk = S_OK;

			UpdateData( TRUE ); // get data from dialog

			if( l_hrOk == S_OK )
			{
				l_hrOk = m_rTool.RunOnce( &m_rTool );
			}

			UpdateData( FALSE );

			return l_hrOk;
		}
		
		bool SVToolAdjustmentDialogFilterPageClass::setImages()
		{
			ISVImage* pImage = m_rTool.getFirstImage();
			// Set dialog image...
			dialogImage.setImage(pImage);
			dialogImage.Refresh();

			return (nullptr != pImage);
		}

		void SVToolAdjustmentDialogFilterPageClass::refresh()
		{
			// Populate filter list box and run filter...
			filterListBox.init( m_pUnaryImageOperatorList );

			SetInspectionData();

			// Refresh dialog image...
			setImages();

		}

		void SVToolAdjustmentDialogFilterPageClass::OnSelchangeList1() 
		{
			int index	= filterListBox.GetCurSel();
			if( index != LB_ERR ) 
			{
				if( nullptr != m_pUnaryImageOperatorList )
				{ 
					IObjectClass* pOperator = reinterpret_cast<IObjectClass*>(filterListBox.GetItemData( index ));

					m_btnProperties.EnableWindow( SV_IS_KIND_OF( pOperator, ICustomFilter ) 
						|| SV_IS_KIND_OF( pOperator, ICustom2Filter )
						|| SV_IS_KIND_OF( pOperator, IRankingFilter )
						|| SV_IS_KIND_OF( pOperator, IThinningFilter )
						|| SV_IS_KIND_OF( pOperator, IThickeningFilter )
						|| SV_IS_KIND_OF( pOperator, IWatershedFilter ) );
				}// end if
			}// end if
		}// end OnSelchangeList1

		void SVToolAdjustmentDialogFilterPageClass::DoDataExchange(CDataExchange* pDX)
		{
			CPropertyPage::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(SVToolAdjustmentDialogFilterPageClass)
			DDX_Control(pDX, IDC_BUTTON4, m_btnProperties);
			DDX_Control(pDX, IDC_BUTTON1, insertFilter);
			DDX_Control(pDX, IDC_LIST1, filterListBox);
			DDX_Control(pDX, IDC_DIALOGIMAGE, dialogImage);
			DDX_Control(pDX, IDC_COMBO1, availableFilterListBox);
			//}}AFX_DATA_MAP
		}

		BOOL SVToolAdjustmentDialogFilterPageClass::OnSetActive() 
		{

			setImages();
			return CPropertyPage::OnSetActive();
		}

		BOOL SVToolAdjustmentDialogFilterPageClass::OnInitDialog() 
		{
			CPropertyPage::OnInitDialog();

			m_btnProperties.EnableWindow( FALSE );

			dialogImage.AddTab(_T("Tool Result"));  

			if( nullptr != m_pUnaryImageOperatorList)
			{

				bool res = setImages();
				if(res)
				{
					availableFilterListBox.init( availableFilters );

					UpdateData( FALSE );
					// Populate filter list box and run filter...
					filterListBox.init( m_pUnaryImageOperatorList );

					SetInspectionData();
					return TRUE;
				}
			}

			// Not valid call...
			if( GetParent() )
				GetParent()->SendMessage( WM_CLOSE );
			else
				SendMessage( WM_CLOSE );

			return TRUE;  // return TRUE unless you set the focus to a control
			// EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
		}


		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonInsertNewFilter
		// -----------------------------------------------------------------------------
		// .Description : Insert New Filter in Current Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :27.05.1997 RO			First Implementation
		//	:20.03.2000 RO			Revised to use new refresh() to update dialog.
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonInsertNewFilter() 
		{ 
			int sourceIndex = availableFilterListBox.GetCurSel();
			if( sourceIndex != LB_ERR )
			{
				sourceIndex = ( int )availableFilterListBox.GetItemData( sourceIndex );
				if(  sourceIndex != -1 )
				{
					int destinyIndex	= filterListBox.GetCurSel();

					if( destinyIndex == LB_ERR ) // First Entryitem
						destinyIndex = 0;

					// Construct and Create the Filter Class Object

					// Rebuild the List of selected Filters
					if ( nullptr != m_pUnaryImageOperatorList )
					{
						// create a new filter and add it the List (before the Threshold object)
						// and now Instantiate a new Object
						ISVFilter* pFilter = dynamic_cast<ISVFilter*> (SvOi::ConstructObject( availableFilters->GetClassID( sourceIndex )) );

						if( nullptr != pFilter )
						{
							// Need to add before the Threshold Object
							m_pUnaryImageOperatorList->InsertAt( destinyIndex, *pFilter );

							// And last - Create (initialize) it

							//if( ! pFilter->IsCreated() )
							if( ! pFilter->is_Created() )
							{
								// SEJ 
								// And finally try to create the child object...
								if( m_pUnaryImageOperatorList->CreateChildObject( *pFilter, SVMFResetObject ) != SVMR_SUCCESS )
								{
									AfxMessageBox("Creation of Filter Failed");

									// What should we really do here ??? SEJ

									// Remove it from the Tool TaskObjectList ( Destruct it )
									GUID objectID = pFilter->GetUniqueObjectID();
									if( objectID != SVInvalidGUID )
										m_pUnaryImageOperatorList->Delete( objectID );
									else
										delete pFilter;
								}
							}
						}
					}

					// Refresh Dialog...
					refresh();
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonClearAll
		// -----------------------------------------------------------------------------
		// .Description : Clear Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :27.05.1997 RO			First Implementation
		//	:20.03.2000 RO			Revised to use new refresh() to update dialog.
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonClearAll() 
		{
			// For all Items in the Selected (Instantiated) Filter list

			// Delete them
			if( nullptr != m_pUnaryImageOperatorList )
			{
				CWaitCursor l_cwcMouse;

				bool l_bReset = false;

				// remove all filter items (instantiated)
				for( int i = m_pUnaryImageOperatorList->GetSize()-1; i >= 0; --i )
				{
					ISVFilter *l_pFilter = dynamic_cast<ISVFilter*>( m_pUnaryImageOperatorList->GetInterfaceAt( i ) );

					if( l_pFilter != NULL )
					{
						l_bReset |= l_pFilter->shouldResetInspection();

						// Close, Disconnect and Delete it
						m_pUnaryImageOperatorList->DestroyChildObject( *l_pFilter, 0);
					}
				}

				if( l_bReset )
				{
					// Refresh Dependencies
					IInspectionProcess* pIPClass = dynamic_cast<IInspectionProcess*>(m_rTool.GetAncestorInterface(SVInspectionObjectType));

					pIPClass->SetDefaultInputs();
					pIPClass->resetAllObjects();
				}

				filterListBox.SetCurSel( -1 );
			}

			// Refresh Dialog...
			refresh();
		}

		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonDeleteCurrentFilter
		// -----------------------------------------------------------------------------
		// .Description : Delete Current Filter in Current Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :27.05.1997 RO			First Implementation
		//	:20.03.2000 RO			Revised to use new refresh() to update dialog.
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonDeleteCurrentFilter() 
		{
			int index	= filterListBox.GetCurSel();
			if( index != LB_ERR ) 
			{
				if( nullptr != m_pUnaryImageOperatorList )
				{ 
					ISVFilter *l_pFilter = dynamic_cast<ISVFilter*>( (IObjectClass *)filterListBox.GetItemData( index ) );

					if( l_pFilter != NULL )
					{
						CWaitCursor l_cwcMouse;

						DWORD l_Context = 0;

						if( l_pFilter->shouldResetInspection() )
						{
							l_Context = SVMFSetDefaultInputs | SVMFResetInspection;
						}

						// Close, Disconnect and Delete it
						m_pUnaryImageOperatorList->DestroyChildObject( *l_pFilter, l_Context);
					}

					// Do not delete empty item '(No Filter)'
					if( index == filterListBox.GetCount() - 1 )
					{
						// Do not delete end marker '--------------------------'
						filterListBox.DeleteString( --index ); 
					}
					else
					{
						filterListBox.DeleteString( index ); 
					}
				}

				// Refresh Dialog...
				refresh();
			}
		}


		////////////////////////////////////////////////////////////////////////////////
		// .Title       : OnButtonProperties
		// -----------------------------------------------------------------------------
		// .Description : Change properties of the Current custom Filter in Filter Table
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		////////////////////////////////////////////////////////////////////////////////
		void SVToolAdjustmentDialogFilterPageClass::OnButtonProperties() 
		{
			int index	= filterListBox.GetCurSel();
			if( index != LB_ERR ) 
			{
				if( nullptr != m_pUnaryImageOperatorList )
				{ 
					IObjectClass* l_pOperator = reinterpret_cast<IObjectClass*>(filterListBox.GetItemData( index ));

					ICustomFilter *l_pCustomFilter = dynamic_cast<ICustomFilter*>( l_pOperator );
					ICustom2Filter *l_pCustom2Filter = dynamic_cast<ICustom2Filter *>( l_pOperator );
					IRankingFilter *l_pRankingFilter = dynamic_cast<IRankingFilter*>( l_pOperator );
					IThinningFilter *l_pThinningFilter = dynamic_cast<IThinningFilter*>( l_pOperator );
					IThickeningFilter *l_pThickeningFilter = dynamic_cast<IThickeningFilter*>( l_pOperator );
					IWatershedFilter *l_pWatershedFilter = dynamic_cast<IWatershedFilter*>( l_pOperator );

					if( l_pCustomFilter != NULL )
					{
						SVCustomFilterDlg l_svDlg( *l_pCustomFilter, this );
						l_svDlg.DoModal();
					}// end if
					if( l_pCustom2Filter != NULL )
					{
						Custom2FilterDlg l_svDlg( *l_pCustom2Filter, this );
						l_svDlg.DoModal();
					}// end if
					else if( l_pRankingFilter != NULL )
					{
						SVRankingFilterDlg l_svDlg( *l_pRankingFilter, this );
						l_svDlg.DoModal();
					}// end else if
					else if( l_pThinningFilter != NULL )
					{
						SVThinningFilterDlg l_svDlg( *l_pThinningFilter, this );
						l_svDlg.DoModal();
					}// end if
					else if( l_pThickeningFilter != NULL )
					{
						SVThickeningFilterDlg l_svDlg( *l_pThickeningFilter, this );
						l_svDlg.DoModal();
					}// end if
					else if( l_pWatershedFilter != NULL )
					{
						SVWatershedFilterDlg l_svDlg( *l_pWatershedFilter, this );
						l_svDlg.DoModal();
					}// end if
				}// end if

				// Refresh Dialog...
				refresh();
			}
		}
	}  //end namespace SVOGUI
}  //end namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAdjustmentDialogFilterPageClass.cpp_v  $
 * 
 *    Rev 1.4   24 Oct 2014 11:45:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added Custom2 Filter Dialog
 * Method Changed: OnButtonProperties
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Jun 2014 18:29:24   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 14:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:21:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:31:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jul 2012 17:48:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Apr 2011 16:41:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVDlgImageClass to change the method init to UpdateImageInfo to make it trackable throughout the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2010 13:45:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jul 2009 13:09:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Aug 2005 13:34:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Aug 2005 08:37:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   28 Jul 2005 13:26:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     make fill options only available with shape masks
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   19 Jul 2005 14:49:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented new value object Get/Set
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   19 Jul 2005 10:25:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Cosmetic changes for the Mask dialogs
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   27 Jun 2005 09:54:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  497
   SCR Title:  Remove Freeze Set from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed Freeze Tool from configurable tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   22 Jun 2005 12:37:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     value objects use SVHRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   22 Jun 2005 10:36:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes use accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   19 May 2005 15:02:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented Shape Mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   18 May 2005 14:10:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added support for mask draw criteria combo.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   09 Mar 2005 13:13:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  478
   SCR Title:  Add a new Perspective Warp Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Allow the PerspectiveWarp Tool to be added from the add menu.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   09 Mar 2005 12:29:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  477
   SCR Title:  Add Cylindrical warp Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added SVToolCylindricalObjectType and #include
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   18 Feb 2005 11:38:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed method of tracking values objects from full object manager rebuild to a value object registration methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   18 Feb 2005 11:10:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added new linear tool to allowable tools to configure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   18 Feb 2005 07:26:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changes to support new Reset methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   16 Feb 2005 15:21:30   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   20 Oct 2004 15:15:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  458
   SCR Title:  Deleting a filter on a tool on certain configuration causes a Dr. Watson
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problem with deleting filters.  
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   12 Oct 2004 12:58:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  448
   SCR Title:  Fix External Tool Change DLL functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added support for ISVPropertyPageDialog interface
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   07 Sep 2004 09:56:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  446
   SCR Title:  Add new filters to SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Add  filters button properties for Watershed, Thinning, and Thickening. OnSelChangeList1() and OnButtonProperties()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   10 Aug 2004 16:18:34   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed Bug in External Tool where output images still show up in image lists generated by the SVToolAdjustmentDialogImagePageClass.  Added check to uObjectAttributesAllowed not SV_HIDDEN.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   21 Jul 2004 09:00:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added CREATE_ALL_CLOSED_OBJECTS message to SVToolAdjustmentDialogSheetClass::OnOK()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   12 Jul 2004 12:03:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implement External Tool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   04 Nov 2003 14:40:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  394
   SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   04 Nov 2003 14:12:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   07 Aug 2003 12:03:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  380
   SCR Title:  Add Image Mask option
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new methods for the new mask that is on the dialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   08 Jul 2003 10:15:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new code and classes for the new custom filters objects and dialogs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   22 Apr 2003 16:47:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   31 Jan 2003 08:59:58   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated OnEditMaskButton and OnInitDialog methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   20 Nov 2002 13:59:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     theSVObjectManager
    ValueObject Data Indexes
    Removed old Automation
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   30 Jan 2002 16:15:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified ::refresh and ::initMask and ::initThreshold to validate the toolset before blindly running it.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   03 Oct 2001 08:43:20   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  225
   SCR Title:  Color SVIM, Incorrect default image for tools
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Modified function, 
   SVToolAdjustmentDialogImagePageClass::OnInitDialog().
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   13 Jun 2001 10:24:28   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Revised file header.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   19 Apr 2001 20:24:52   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   06 Feb 2001 13:47:52   cSchmittinger
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  cSchmittinger;  Carl Schmittinger
   Change Description:  
     Modified source code to use new file management classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   01 Feb 2001 11:55:16   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Steve;  Stephen S. Steffan
   Change Description:  
     Modified files to use the SVFileNameClass and the SVFileNameManagerClass if applicable, if the object refers to a filename.  The SVFileNameManagerClass is used if the Object is a persistant file object, that is, if the file object is saved as part of the configuration data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16.1.0   16 May 2001 14:12:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  202
   SCR Title:  Operator, delete and cut should not be available in File Open/Save Dialogs
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed all occurences of CFileDialog to CSVFileDialog to enable the new security features and privileges
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   24 Oct 2000 15:51:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  187
   SCR Title:  Color SVIM Problem when Adding Monochrome Tools
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     These changes remove the ability to add a monochrome tool to the tool set until a color tool gets added.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   06 Sep 2000 16:37:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   14 Jun 2000 16:22:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  159
   SCR Title:  Incorrect placement of Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnSelchangedCombo1 to Send the message
   SVM_IMAGE_SOURCE_CHANGED to the tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 May 2000 16:06:00   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  145
   SCR Title:  Half field images and rotation
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved SVToolAdjustmentDialogTwoImagePageClass to
   it's own header and implementation file (
   SVToolAdjustmentDialogTwoImagePage.h and 
   SVToolAdjustmentDialogTwoImagePage.cpp
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   26 Apr 2000 14:57:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnSelchangeCurrentAnalyzer to Send 
   SVM_CONNECT_ALL_INPUTS message to the Tool, so
   the friends get their connections as well.
   Revised destructor for 
   SVToolAdjustmentDialogSpecialPropertiesPageClass
   to delete propertyArray.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   23 Mar 2000 16:48:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added new LUT Dialog page.
   Revised to work with new In Place Image Operator List.
   Revised to run Tool instead or OperatorList.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   03 Mar 2000 16:03:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised addPages(): Polar Transform (Polar Unwarp) Tool
   now has a standard image source page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   Mar 01 2000 13:05:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised to support new polar transform tool ( Polar Unwrap ) and
   his new SVTADlgPolarTransformPage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   29 Feb 2000 14:28:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  106
   SCR Title:  Math Tool - List processing
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Close Button on caption.
   Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   Feb 24 2000 12:06:38   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  111
   SCR Title:  Integrate Conditional Tool Set/Tool Drawing
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added new general page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   21 Feb 2000 15:22:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  104
   SCR Title:  New Build Reference Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised 	SVToolAdjustmentDialogImagePageClass
   constructor to use the id as an argument.
   Added include files for
   SVToolImage.h
   SVTransformationTool.h
   SVTADlgTransformImagePage.h
   SVTADlgTranslationPage.h
   SVTADlgRotationPage.h
   SVTADlgTransformationLearnPage.h
   
   Revised SVToolAdjustmentDialogSheetClass::addPages()
   to add property pages for editing the Transformation Tool.
   
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   Feb 21 2000 10:53:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new freeze page and freeze tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   07 Feb 2000 22:51:10   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Changed to handle new Page of the Acq. Tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Jan 14 2000 15:48:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  85
   SCR Title:  Version 3.04 Beta 1 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fix memory leak for PMaskEditorCtrl pointer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Jan 03 2000 14:41:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  73
   SCR Title:  Mask Editor Mil 5.0 to Mil 6.0 repairs.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes so bitmap image is created when necessary.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:36:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed reference to SVToolSetViewClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:46:08   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.40   Nov 17 1999 15:06:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     RO change for image arithmetic.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.39   Nov 17 1999 07:25:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added check for unique results archive file paths.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.38   Nov 10 1999 12:38:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Documents marked as changed when Dialog OK buttons pressed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.37   Nov 09 1999 10:47:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove Gage Tool Image possible image selection for tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.36   08 Nov 1999 10:58:22   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use IDS_INVALID_FORMULA (string table).
   Revised to use IDS_INVALID_CONDITION_FORMULA 
   (string table).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.35   Oct 28 1999 11:31:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  52
   SCR Title:  Version 3.00 Beta 14 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Tool/Tool set conditional dialog adjustment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.34   19 Oct 1999 17:42:42   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  48
   SCR Title:  Threshold Page Formula
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved SVTADlgThresholdPageClass to it's own file(s)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.33   14 Oct 1999 12:06:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised not to recreate the same analyzer when picked
   from the dropdown list of available analyzers
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.32   08 Oct 1999 16:29:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Corrected problem in SVToolAdjustmentDialogAnalyzerPageClass
   which would crash when no analyzer selected and the
   publish button was pressed.
   Added call to SVToolAdjustmentDialogPassFailPageClass::
   OnOK()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.31   08 Oct 1999 10:58:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Call to SVIPDoc::SetDefaultInputs() after sending
   SVM_DESTROY_CHILD_OBJECT message to refresh
   dependencies.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.30   07 Oct 1999 17:48:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Call to SVToolAdjustmentStatisticsPageClass::OnOk()
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.29   05 Oct 1999 13:15:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  39
   SCR Title:  Integrate Statistics Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added entry in AddPages for SVStatisticsToolClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.28   Oct 04 1999 10:04:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Image source fix for profile and others.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.27   29 Sep 1999 13:14:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Disabled and Hid IDCANCEL button.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.25   24 Sep 1999 19:04:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed call to RemoveExternalDependencies
   (Fix to restore inputs via script)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.24   Sep 24 1999 13:58:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Mask Editor OCX access is corrected.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.23   Sep 24 1999 10:39:06   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  4
   SCR Title:  Add "Load image" option
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Load Image Tool changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.22   Sep 23 1999 13:59:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Bug fixes in threshold edit control code for threshold values.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.21   Sep 22 1999 18:12:58   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added a SVDoubleValue for auto thresholding value on Threshold page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.20   Sep 20 1999 15:35:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Image source Page Bug fix..
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   20 Sep 1999 11:01:14   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Reincorporate changes from version 1.16.
   Corrected bug - restore pointer to current analyzer after 
   calling setupDialog and Result Dialog
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   Sep 20 1999 09:51:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixes in image source page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   Sep 15 1999 17:24:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  15
   SCR Title:  Integrate archive tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added 'page' for Archive tool adjustments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   15 Sep 1999 17:11:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for SVUserMaskOperatorClass Embedded Objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Sep 14 1999 17:11:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added Build Reference Page.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   14 Sep 1999 17:06:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added equation validation in OnOk method
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   14 Sep 1999 13:21:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to set pointer to SVIPDoc in SVChildrenSetupDialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   Sep 13 1999 15:22:56   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove unnececessary DDX_Control line for dialogImage.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Sep 13 1999 14:01:10   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Ported TwoImagePage
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   11 Sep 1999 19:54:04   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed old math tool adjust dialogs includes.
   Added include for SVFormulaEditorPage.
   Added Conditional dialog page to all tools.
   Revised Dialog pages for the MathTool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Sep 10 1999 18:46:16   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added profile pages.
   Modified Analyzer page to support ObjectTypes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   08 Sep 1999 09:20:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed include of SVPublishResultDialog.
   Revised OnPublishButton to refresh Published List.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   07 Sep 1999 08:21:32   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised all calls to Run() to new Run(RunStatus)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   30 Aug 1999 20:11:58   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  14
   SCR Title:  Integrate luminance analyzer into window tool.
   Checked in by:  Jim;  James A. Brown
   Change Description:  
     Integrating Pixel Analyzer;
   Modified OnPublishButton() to use new result picker.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   28 Aug 1999 14:27:56   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Replaced DoDetailDialog() call with SetupDialog() call.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   25 Aug 1999 12:21:16   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Removed old Multiple Analyzer Page.
   Adapted new object type enumerations into SVToolAdjustmentDialogSheet::addPage().
   
   /////////////////////////////////////////////////////////////////////////////////////
*/

