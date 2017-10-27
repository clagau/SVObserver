//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryStream
//* .File Name       : $Workfile:   SVMemoryStream.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 17:29:00  $
//******************************************************************************
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

#region Memory Stream that implements IStream
	/// <summary>
	/// COM IStream wrapper for a MemoryStream.
	/// CLR will make the class implement the IDispatch COM interface
	/// so COM objects can make calls to IMemoryStream methods
	/// </summary>
	[ClassInterface(ClassInterfaceType.AutoDispatch)] 
	public class SVMemoryStream : MemoryStream, IStream
	{
		#region Constructor
		public SVMemoryStream() : base() 
		{
		}
		#endregion

		#region Write String to Stream
		// convenience method for writing Strings to the stream
		public void Write(string s) 
		{
			System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
			byte[] pv = encoding.GetBytes(s);
			Write(pv, 0, pv.GetLength(0));
		}
		#endregion

		#region Clone
		// Implementation of the IStream interface
		public void Clone(out IStream ppstm) 
		{
			ppstm = null;
		}
		#endregion

		#region Read Bytes
		public void Read(byte[] pv, int cb, System.IntPtr pcbRead) 
		{
			long bytesRead = Read(pv, 0, cb);
			if (pcbRead != IntPtr.Zero) 
				Marshal.WriteInt64(pcbRead, bytesRead);
		}
		#endregion

		#region Write Bytes
		public void Write(byte[] pv, int cb, System.IntPtr pcbWritten) 
		{
			Write(pv, 0, cb); 
			if (pcbWritten != IntPtr.Zero) 
				Marshal.WriteInt64(pcbWritten, (Int64)cb);
		}
		#endregion

		#region Seek
		public void Seek(long dlibMove, int dwOrigin, System.IntPtr plibNewPosition) 
		{
			long pos = base.Seek(dlibMove, (SeekOrigin)dwOrigin);
			if (plibNewPosition != IntPtr.Zero) 
				Marshal.WriteInt64(plibNewPosition, pos);
		}
		#endregion

		#region Set Size
		public void SetSize(long libNewSize) { }
		#endregion

		#region Copy To
		public void CopyTo(IStream pstm, long cb, System.IntPtr pcbRead, System.IntPtr pcbWritten) {}
		#endregion

		#region Commit
		public void Commit(int grfCommitFlags) { }
		#endregion

		#region Lock Region
		public void LockRegion(long libOffset, long cb, int dwLockType) { }
		#endregion

		#region Revert
		public void Revert() { }
		#endregion

		#region Unlock Region
		public void UnlockRegion(long libOffset, long cb, int dwLockType) { }
		#endregion

		#region Stat
		public void Stat(out System.Runtime.InteropServices.ComTypes.STATSTG pstatstg, int grfStatFlag) 
		{
			pstatstg = new System.Runtime.InteropServices.ComTypes.STATSTG();
		}
		#endregion
	}
	#endregion

//Using IMemoryStream
//Writing a string into the stream and then having an unmanaged COM object read the stream. Don't forget to seek to the beginning of the stream before reading else you'll get nothing. 
/*
string result = email.body;
IMemoryStream ms = new IMemoryStream();
ms.Write(result);
ms.Seek(0, SeekOrigin.Begin); // return to start of stream ready for reading
mime.Load(ms); // transfer into a MimeSniffer object
*/
//Writing to the stream from an unmanaged COM object that takes an IStream pointer then reading it into a managed Bitmap object. 
/*
ms = new IMemoryStream();
attachment.Export(ms); // MimeSniffer.MimeBody.Export(Stream)
image = new Bitmap((Stream)ms);
*/

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVMemoryStream.cs_v  $
 * 
 *    Rev 1.0   14 May 2013 17:29:00   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Check-In
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * 
 *    Rev 1.0   15 Mar 2010 08:37:12   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/