//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : AutoBuild
//* .File Name       : $Workfile:   AutoBuild.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.28  $
//* .Check In Date   : $Date:   28 Jan 2015 11:14:06  $
//******************************************************************************

#pragma once

// change the FALSE to TRUE for autoincrement of build number
#define INCREMENT_VERSION TRUE

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )

// FILEVER lay out is as follows: a,b,c,d
//    a is the major version number
//    b is the minor version number
//    c is the beta version number - if c equals 0 or is greater than 254 then the application is marked as Released
//    d is unused

#define FILEVER        7,20,6,0
#define PRODUCTVER     FILEVER
#define STRFILEVER     SV_STRINGIZE( FILEVER )
#define STRPRODUCTVER  SV_STRINGIZE( PRODUCTVER )

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\AutoBuild\AutoBuild.h_v  $
 * 
 *    Rev 1.28   28 Jan 2015 11:14:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 7.10 Release(Beta 256)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   26 Jan 2015 10:03:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 7.10 Release(Beta 255)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   23 Jan 2015 14:32:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 020
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   23 Jan 2015 11:12:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 019
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   20 Jan 2015 10:48:02   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 018
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   19 Jan 2015 11:44:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 017
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   15 Jan 2015 10:58:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 016
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   13 Jan 2015 11:08:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 015
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   12 Jan 2015 17:35:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 014
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   09 Jan 2015 17:29:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 013
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   05 Jan 2015 09:21:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 012
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Dec 2014 11:02:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 011
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Dec 2014 17:05:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 010
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   17 Dec 2014 07:01:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to SVO 7.10 Beta 009
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   12 Dec 2014 09:49:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 7.10 Beta 008
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Dec 2014 16:55:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to 7.10 Beta 007. James Bond eddition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   04 Dec 2014 10:45:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.10 beta 006
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Dec 2014 06:44:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Change to 7.10 Beta 005
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Nov 2014 11:19:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.10 beta 004
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Nov 2014 12:24:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.10 beta 003
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Nov 2014 10:18:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.10 beta 001
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Nov 2014 08:56:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 7.00 Released (beta 255)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Oct 2014 08:24:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.00 beta 011
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Oct 2014 10:48:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  952
 * SCR Title:  Run/Reject Server Should Respond to the GetVersion Command
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version to Beta 10
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Oct 2014 16:59:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version number to beta 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Oct 2014 17:21:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.00 beta 008
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Oct 2014 11:20:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.00 beta 007
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Oct 2014 11:35:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to version 7.00 beta 006
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Oct 2014 21:26:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved to AutoBuild folder to allow sharing between SVObserver and RunRejectServer.
 * Updated to version 7.00 beta 005
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   15 Oct 2014 18:23:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated to version 7.00 beta 004
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   15 Oct 2014 08:30:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated to  version 7.0 Beta 3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   14 Oct 2014 07:01:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 7.00 Beta 002
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   25 Sep 2014 14:10:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Change Version to 7.00 Beta 001
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   19 Sep 2014 10:15:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 6.30 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 6.30 Beta 002
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   15 Sep 2014 16:39:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 6.30 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed version to 6.30 Beta 001
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   09 Sep 2014 09:31:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Released (beta 255)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   08 Sep 2014 10:45:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 028
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   03 Sep 2014 16:17:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to SVO 6.20 Beta 027
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   26 Aug 2014 14:43:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to SVO 6.20 Beta 026
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   20 Aug 2014 12:58:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 25
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   14 Aug 2014 18:35:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised beta number to 24
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   04 Aug 2014 08:12:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 023
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   18 Jul 2014 10:09:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   version to SVO 6.20 Beta 022
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   11 Jul 2014 07:49:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   version to 6.20 Beta 021
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   10 Jul 2014 11:57:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 020
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   23 Jun 2014 13:59:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 019
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   19 Jun 2014 06:52:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   version to 6.20 Beta 018
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   18 Jun 2014 14:22:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   change version to 6.20 Beta 017
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   16 Jun 2014 11:42:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   change version to 6.20 beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   03 Jun 2014 14:04:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 015
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   23 May 2014 07:21:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 14.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   20 May 2014 06:54:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update version to 6.20 Beta 013
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   14 May 2014 12:51:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 beta 12
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   29 Apr 2014 06:26:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Version to 6.20 Beta 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   14 Mar 2014 12:38:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 beta 010
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   28 Feb 2014 10:01:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 009
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   05 Feb 2014 09:59:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update version to 6.20 Beta 008
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   29 Jan 2014 11:10:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 007
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   23 Jan 2014 10:15:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 006
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   15 Jan 2014 09:59:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   03 Dec 2013 07:20:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.20 Beta 004
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   05 Nov 2013 09:44:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed beta to 6.20 Beta 003
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   30 Oct 2013 12:51:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 002
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   24 Sep 2013 14:51:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  865
 * SCR Title:  Update SVObserver Version Number for Fletch Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Change version to 6.20 Beta 001.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   21 Aug 2013 08:45:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.10 Release.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   15 Aug 2013 06:19:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   changed version to 6.10 Beta 031
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   09 Aug 2013 12:56:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.10 Beta 0029
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   08 Aug 2013 07:17:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.10 Beta 028
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   31 Jul 2013 07:26:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 610 Beta 027
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 Jul 2013 14:49:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version to 6.10 Beta 026
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   12 Jul 2013 09:04:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 025
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   11 Jul 2013 08:24:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Change Version to 6.10 Beta 024
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   09 Jul 2013 13:28:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   09 Jul 2013 10:39:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 023
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Jul 2013 07:32:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 022.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   27 Jun 2013 12:32:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update to version 6.10 Beta 022
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   05 Jul 2013 15:01:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   14 Jun 2013 09:43:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 021
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   05 Jul 2013 09:10:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   06 Jun 2013 11:40:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version to 6.10 Beta 20
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Jun 2013 19:22:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   31 May 2013 11:08:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated version to 6.10 Beta 19
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   12 Jun 2013 15:19:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   23 May 2013 06:47:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 610 beta 018
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Jun 2013 15:25:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   20 May 2013 06:53:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 017
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 May 2013 19:45:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   14 May 2013 11:48:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 610 beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 May 2013 16:29:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   08 May 2013 10:33:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  826
 * SCR Title:  Fix Memory Leaks in I/O Adjustment Dialogs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated SVObserver version number to 6.10 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 May 2013 12:15:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   07 May 2013 13:45:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 014
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 May 2013 13:26:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   01 May 2013 11:11:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 012
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 May 2013 16:01:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   30 Apr 2013 09:18:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed version to 6.10 Beta 012
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 May 2013 08:20:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   22 Apr 2013 09:55:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 610 Beta 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 May 2013 20:05:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   16 Apr 2013 15:39:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   10 Apr 2013 11:21:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:12:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   05 Apr 2013 12:35:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.10 Beta 008.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   27 Mar 2013 14:47:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to 6.10 beta 7
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   25 Mar 2013 13:38:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated to 6.10 beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   20 Mar 2013 14:13:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.01 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   19 Mar 2013 14:48:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version to 6.10 Beta 4
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   18 Mar 2013 13:24:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version to 6.10 Beta 3
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   15 Mar 2013 13:07:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version to 6.10 beta 2
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   07 Mar 2013 08:49:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised version number to 6.10 Beta 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   05 Mar 2013 17:04:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version number to beta 25
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   01 Mar 2013 10:17:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  816
 * SCR Title:  Color Camera Light Reference Crash
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version to beta 24
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   01 Mar 2013 09:31:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to 6.05 Beta 23
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   27 Feb 2013 11:18:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to beta 22
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   18 Feb 2013 13:26:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   14 Feb 2013 11:05:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver vresion number to 6.05 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   12 Feb 2013 12:08:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 019.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   11 Feb 2013 14:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   21 Jan 2013 11:53:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Updated version beta number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Jan 2013 16:49:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   11 Jan 2013 15:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 015.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   10 Jan 2013 15:00:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  803
 * SCR Title:  Update SVObserver to version 6.01
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   11 Oct 2012 11:14:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.05 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   04 Oct 2012 11:32:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version 6.05 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   28 Sep 2012 14:51:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   27 Sep 2012 18:37:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.05 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   25 Sep 2012 15:31:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.05 Beta 009.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Sep 2012 18:37:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 008.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   12 Sep 2012 17:14:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.05 Beta 007.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   11 Sep 2012 12:30:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver versoin number to 6.05 Beta 006.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   06 Sep 2012 10:51:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 004.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Sep 2012 16:29:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   04 Sep 2012 16:01:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 002.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Aug 2012 09:49:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  786
 * SCR Title:  Fix Problems with IEEE 1394 Light Reference and LUT Camera Parameters
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.05 Beta 001.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.11   10 Jan 2013 08:20:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  803
 * SCR Title:  Update SVObserver to version 6.01
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to 6.01 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.10   09 Jan 2013 12:45:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to 6.01 beta 18
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.9   08 Jan 2013 11:45:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.01 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.8   07 Jan 2013 14:59:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   upped to 6.01 beta 16
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.7   02 Jan 2013 15:11:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Upped to beta 6.01 beta 15
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.6   13 Dec 2012 08:51:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated SVObserver version to 6.01 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.5   05 Dec 2012 11:49:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to 6.01 beta 11
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.4   30 Nov 2012 09:38:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  800
 * SCR Title:  Fix Blob Array Result index string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed version to 6.01 Beta 010
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.3   08 Nov 2012 15:53:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  790
 * SCR Title:  Fix property page in the configuration assistant when dealing with GD4A Color
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   changed to 6.1 beta 7
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.2   01 Nov 2012 13:48:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to 6.0.1 beta 6
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.1   11 Oct 2012 08:56:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.01 Beta 005.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12.1.0   01 Oct 2012 10:43:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  793
 * SCR Title:  Fix PPQ Memory Consumption Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.01 Beta 003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 Aug 2012 11:56:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change version to 6.00 Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   23 Aug 2012 11:08:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed version to 6.00 Beta 96.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Aug 2012 11:19:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed beta to 6.00 Beta 094
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Aug 2012 17:09:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.00 Beta 092.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Aug 2012 14:42:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.00 Beta 090.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   14 Aug 2012 15:58:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 088.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Aug 2012 14:33:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.00 Beta 086.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2012 16:19:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 084.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Aug 2012 16:01:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 082.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2012 10:49:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number 6.00 Beta 080.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Aug 2012 16:09:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version to 6.00 Beta 078.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Aug 2012 13:11:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver version number to 6.00 Beta 076.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jul 2012 12:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new functionality to use the resource version information instead of data found in SVObserver.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
