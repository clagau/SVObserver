//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectImpl
//* .File Name       : $Workfile:   SVValueObjectImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Nov 2013 07:42:24  $
//******************************************************************************

#ifndef SVVALUEOBJECTIMPL_H
#define SVVALUEOBJECTIMPL_H

#include "SVBoolValueObjectClass.h"
#include "SVByteValueObjectClass.h"
#include "SVCharValueObjectClass.h"
#include "SVDoubleValueObjectClass.h"
#include "SVDPointValueObjectClass.h"
#include "SVDWordValueObjectClass.h"
#include "SVEnumerateValueObjectClass.h"
#include "SVFileNameValueObjectClass.h"
#include "SVInt64ValueObjectClass.h"
#include "SVLongValueObjectClass.h"
#include "SVPointValueObjectClass.h"
#include "SVStringValueObjectClass.h"
#include "SVStaticStringValueObjectClass.h"
#include "SVTimerValueObjectClass.h"
#include "SVVariantValueObjectClass.h"

#endif // _SVVALUEOBJECTIMPL_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVValueObjectImpl.h_v  $
 * 
 *    Rev 1.1   11 Nov 2013 07:42:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SVStaticStringValueObjectClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:18:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Apr 2008 14:32:14   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Mar 2008 12:58:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added TypeName to value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2007 15:07:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   19 Jan 2006 08:55:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed object attributes for Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jan 2006 14:56:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetNormalizedValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Nov 2005 13:16:14   ebeyeler
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
 *    Rev 1.2   11 Nov 2005 14:27:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CompareWithCurrentValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Aug 2005 15:06:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made slight speed improvements to ValueObject. Really need /Og to get huge speed increases.
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
 * 
 *    Rev 3.24   02 Aug 2005 10:42:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   be less strict on SetArrayValues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   01 Aug 2005 12:02:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SVVectorObject with SVValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   29 Jul 2005 13:37:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new attributes and methods to track required reset functionality when values are changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   19 Jul 2005 14:44:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   14 Jul 2005 12:38:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to bool on SetDefaultValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   30 Jun 2005 15:12:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   22 Jun 2005 08:46:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implement array value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   17 Feb 2005 14:32:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support the new createObject and reset Methodology.
 * Added ResetObject(, CreateArrays(, DestroyArrays(
 * Removed SetObjectDepth(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   16 Feb 2005 15:25:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   27 Jul 2004 15:18:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added long GetDefaultType() to SVVariantValueObjectClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   07 Jul 2004 10:15:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up clutter
 * added SVValueObjectUnitTest
 * added support for Variant
 * added support for char
 * added GetLastSetIndex()
 * made CopyLastSetValue, GetLastSetIndex(int&) non-virtual
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   02 Apr 2004 12:06:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new Set & Get methods for bool's
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   04 Dec 2003 13:17:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   17 Sep 2003 13:02:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   17 Jun 2003 15:13:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Apr 2003 17:29:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   10 Apr 2003 10:14:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  271
 * SCR Title:  Adding timer features for Tool Execution times.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a new SetValue and new GetValue with the type being __int64
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Jan 2003 14:11:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a SetValue that takes a double value, to the SVLongValueObjectClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Dec 2002 15:22:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   typedef SVArrayClass <SVValueObjectClass*, SVValueObjectClass*> SVValueObjectArray;
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   13 Nov 2002 12:32:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  289, 272, 226
 * SCR Title:  Extending the SVValueObjectClass Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   The following apply to the class SVValueObject and its derivatives, SVBoolValueObjectClass, SVByteValueObjectClass, SVDWordValueObjectClass, SVLongValueObjectClass, SVDoubleValueObjectClass, SVPointValueObjectClass, SVStringValueObjectClass, SVFileNameValueObjectClass.
 * 
 * The SetValue(type) functions were changed to SetValue(bucket, type)
 * 
 * The SetValue(bucket, type&) functions were changed to be aware of the buckets.
 * 
 * The GetValue(type&) functions were changed to call GetValue(m_nLastSetIndex, type&)
 * 
 * The GetValue(bucket, type&) functions were changed to be aware of the buckets.
 * 
 * Extra GetValue functions were added in each class to support uniform conversions of type.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   13 Jun 2001 10:44:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVStringValueObjectClass::SetValue, 
 * SVStringValueObjectClass::SetObjectValue, both SVFileNameValueObjectClass::SVFileNameValueObjectClass constructors, 
 * ~SVFileNameValueObjectClass, SVFileNameValueObjectClass::SetValue,
 * SVFileNameValueObjectClass::SetDefaultValue, SVFileNameValueObjectClass::SetObjectValue,
 * SVFileNameValueObjectClass::GetObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2001 20:20:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2001 17:37:38   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified SVStringValueObjectClass to use the SVFileNameClass and the SVFileNameManagerClass is the object refers to a filename.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:52:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Fixed bugs in SVByteValueObjectClass.
 * Fixed SVValueObjectClass to handle Byte Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 14:00:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Sep 1999 13:43:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetDefaultValue Method for all classes
 * derived from SVValueObjectClass.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 1999 08:35:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to set defaults in the PPQDataLinkInfoStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Aug 1999 16:40:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Overridden GetValue() for CString& and double&.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
