//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectImpl
//* .File Name       : $Workfile:   SVValueObjectImpl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:18:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVValueObjectImpl.h"

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVValueObjectImpl.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:18:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Apr 2008 14:31:56   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   13 Mar 2008 12:52:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added default Type Names for Timer, Int64, and DPoint value objects to support value definition list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   07 Mar 2008 12:58:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added TypeName to value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   21 Jun 2007 15:07:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   30 Jan 2006 06:59:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified SVBoolValueObjectClass::GetNormalizedValue to trim beginning and ending spaces off the input string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   19 Jan 2006 08:55:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed object attributes for Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   17 Jan 2006 14:56:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetNormalizedValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   17 Jan 2006 09:53:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Conditional History attributes
 * added SVScalarValue accessor functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Nov 2005 13:16:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVEnumerateValueObjectClass::ToNumber
 * fixed SVEnumerateValueObjectClass::GetEnumerator
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   11 Nov 2005 14:27:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CompareWithCurrentValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   12 Sep 2005 12:38:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed bug in VariantValueObject SetType with single array element 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   08 Sep 2005 11:47:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed problematic ASSERTs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Sep 2005 12:29:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem with the VARIANT Value Object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Sep 2005 14:46:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized for speed
 * - only store multidimentional buckets if array size != 1
 * - inlined functions (ArraySize) etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Aug 2005 14:07:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVValueObjectReference test code
 * added logging to debugger in Release mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Aug 2005 15:43:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   tool out debugging code in SVValueObjectUnitTest::Test
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Aug 2005 15:06:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made slight speed improvements to ValueObject. Really need /Og to get huge speed increases.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   26 Aug 2005 15:02:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed merge error with SVFileNameValueObjectClass::SetValueAt
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Aug 2005 14:19:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed SetValueAt with string parameter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Aug 2005 15:16:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Split Value Object base functionality into own file and moved all other value objects to another single file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   01 Aug 2005 12:01:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     replaced SVVectorObject with SVValueObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   29 Jul 2005 13:37:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new attributes and methods to track required reset functionality when values are changed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   28 Jul 2005 15:27:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     in SetArraySize, use m_uDefaultObjectAttributesSet
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   27 Jul 2005 15:24:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     removed the improper isCreated assignment
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   27 Jul 2005 08:12:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed the isObjectValid and isCreated flags
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   26 Jul 2005 12:27:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed loading problem for SVFileNameValueObjects (SetObjectValue)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   19 Jul 2005 14:44:04   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed Get/Set
   fixed Variant merge error - SVRemoveEscapedSpecialCharacters
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   14 Jul 2005 12:38:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed object script to store full array of last set bucket, instead of first element in array of bucket 0 and 1
   changed BOOL to bool on SetDefaultValue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   22 Jun 2005 08:46:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implement array value object
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   17 Feb 2005 14:32:54   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modifications to support the new createObject and reset Methodology.
   Added ResetObject(, CreateArrays(, DestroyArrays(
   Removed SetObjectDepth(
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   15 Feb 2005 15:06:14   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  468
   SCR Title:  Fix Variant Object Script
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified SVVariantValueObjectClass::SetObjectValue to remove special characters when the variant is a BSTR
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   06 Aug 2004 12:11:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  416
   SCR Title:  Add Variant Value Object
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     fixed parser to treat floating point as US locale
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   27 Jul 2004 15:18:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  416
   SCR Title:  Add Variant Value Object
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added long GetDefaultType() to SVVariantValueObjectClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   07 Jul 2004 10:15:10   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  416
   SCR Title:  Add Variant Value Object
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     cleaned up clutter
   added SVValueObjectUnitTest
   added support for Variant
   added support for char
   added GetLastSetIndex()
   made CopyLastSetValue, GetLastSetIndex(int&) non-virtual
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   02 Apr 2004 12:06:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  411
   SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new Set & Get methods for bool's
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   04 Dec 2003 13:17:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Sep 2003 13:02:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  384
   SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   17 Jun 2003 15:13:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   22 Apr 2003 17:29:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   14 Jan 2003 14:11:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added a SetValue that takes a double value, to the SVLongValueObjectClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   18 Dec 2002 08:26:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  299
   SCR Title:  SIAC needs to be able to request Source Images
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   09 Dec 2002 20:21:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added SV_PRINTABLE attribute flag as a default.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   13 Nov 2002 12:32:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  289, 272, 226
   SCR Title:  Extending the SVValueObjectClass Object
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     The following apply to the class SVValueObject and its derivatives, SVBoolValueObjectClass, SVByteValueObjectClass, SVDWordValueObjectClass, SVLongValueObjectClass, SVDoubleValueObjectClass, SVPointValueObjectClass, SVStringValueObjectClass, SVFileNameValueObjectClass.
   
   The SetValue(type) functions were changed to SetValue(bucket, type)
   
   The SetValue(bucket, type&) functions were changed to be aware of the buckets.
   
   The GetValue(type&) functions were changed to call GetValue(m_iLastSetIndex, type&)
   
   The GetValue(bucket, type&) functions were changed to be aware of the buckets.
   
   Extra GetValue functions were added in each class to support uniform conversions of type.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
