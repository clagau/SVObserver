//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionTransform
//* .File Name       : $Workfile:   SVInspectionTransform.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:34:22  $
//******************************************************************************
using System;
using System.IO;
using System.Xml;
using System.Xml.Xsl;
using System.Net;
using System.Runtime.InteropServices;

namespace SVImportTransform
{
    public class SVInspectionTransform
    {
        private static readonly string outDirectory = @"c:/run";
        private static readonly string errFilename = outDirectory + @"/transform.error.log";

        #region Transform
        public static long Transform(string inFilename, string outFilename, string inspectionName)
        {
            if (!System.IO.Directory.Exists(outDirectory))
            {
                System.IO.Directory.CreateDirectory(outDirectory);
            }
            string replacementFilename = outDirectory + @"/replacements.xml";
            string tmpFilename = outDirectory + @"/tmp." + inspectionName + ".xml";
            
            long retVal = GenerateGuids(inFilename, replacementFilename, inspectionName);
            if (retVal == 0)
            {
                retVal = ReplaceInspectionName(inFilename, tmpFilename, inspectionName);
            }
            if (retVal == 0)
            {
                retVal = ReplaceGuids(tmpFilename, replacementFilename, outFilename);
            }
            //cleanup
            if (System.IO.File.Exists(replacementFilename))
            {
                System.IO.File.Delete(replacementFilename);
            }
            if (System.IO.File.Exists(tmpFilename))
            {
                System.IO.File.Delete(tmpFilename);
            }
            return retVal;
        }
        #endregion

        #region Generate Guids
        private static long GenerateGuids(string filename, string guidFilename, string newInspectionName)
        {
            long retVal = 0;
            XmlReaderSettings readerSettings = new XmlReaderSettings();
            readerSettings.ConformanceLevel = ConformanceLevel.Fragment;
            readerSettings.IgnoreWhitespace = true;
            readerSettings.IgnoreComments = true;

            XmlWriterSettings writerSettings = new XmlWriterSettings();
            writerSettings.Indent = true;// false;

            try
            {
                XmlWriter writer = XmlWriter.Create(guidFilename, writerSettings);

                try
                {
                    XmlReader reader = XmlReader.Create(filename, readerSettings);

                    // Create an XmlUrlResolver with default credentials.
                    XmlUrlResolver resolver = new XmlUrlResolver();
                    resolver.Credentials = CredentialCache.DefaultCredentials;

                    // Create Transform
                    XslCompiledTransform xslt = new XslCompiledTransform();
                    XsltSettings xsltSettings = new XsltSettings();
                    xsltSettings.EnableScript = true;
                    xsltSettings.EnableDocumentFunction = false;

                    XsltArgumentList argList = new XsltArgumentList();
                    argList.AddParam("InspectionName", "", newInspectionName);

                    string resName = SVImportTransform.Properties.Resources.generate_replacements;
                    try
                    {
                        StringReader stringReader = new StringReader(resName);
                        XmlReader xsltReader = XmlReader.Create(stringReader);
                        xslt.Load(xsltReader, xsltSettings, resolver);

                        try
                        {
                            xslt.Transform(reader, argList, writer);
                        }
                        catch (Exception e)
                        {
                            retVal = -1;
                            using (StreamWriter errLog = File.AppendText(errFilename))
                            {
                                Log(errLog, "Error applying transform");
                                Log(errLog, e.Message);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        retVal = -2;
                        using (StreamWriter errLog = File.AppendText(errFilename))
                        {
                            Log(errLog, "Error opening XSLT transform file - " + resName);
                            Log(errLog, e.Message);
                        }
                    }

                    reader.Close();
                }
                catch (Exception e)
                {
                    retVal = -3;
                    using (StreamWriter errLog = File.AppendText(errFilename))
                    {
                        Log(errLog, "Error opening inspection import file - " + filename);
                        Log(errLog, e.Message);
                    }
                }
                writer.Close();
            }
            catch (Exception e)
            {
                retVal = -4;

                using (StreamWriter errLog = File.AppendText(errFilename))
                {
                    Log(errLog, "Error creating replacement guid file - " + guidFilename);
                    Log(errLog, e.Message);
                }
            }
            return retVal;
        }
        #endregion

        #region ReplaceGuids
        private static long ReplaceGuids(string filename, string replacementsFilename, string outFilename)
        {
            long retVal = 0;
            XmlReaderSettings readerSettings = new XmlReaderSettings();
            readerSettings.ConformanceLevel = ConformanceLevel.Fragment;
            readerSettings.IgnoreWhitespace = true;
            readerSettings.IgnoreComments = true;

            XmlWriterSettings writerSettings = new XmlWriterSettings();
            writerSettings.Indent = true;// false;

            // Create an XmlUrlResolver with default credentials.
            XmlUrlResolver resolver = new XmlUrlResolver();
            resolver.Credentials = CredentialCache.DefaultCredentials;

            // Create Transform
            XslCompiledTransform xslt = new XslCompiledTransform();

            XsltSettings xsltSettings = new XsltSettings();
            xsltSettings.EnableScript = false;
            xsltSettings.EnableDocumentFunction = true;

            XsltArgumentList argList = new XsltArgumentList();
            argList.AddParam("Filename", "", "file://" + replacementsFilename);

            try
            {
                XmlWriter writer = XmlWriter.Create(outFilename, writerSettings);
                try
                {
                    XmlReader reader = XmlReader.Create(filename, readerSettings);
                    string resName = SVImportTransform.Properties.Resources.apply_replacements;

                    try
                    {
                        StringReader stringReader = new StringReader(resName);
                        XmlReader xsltReader = XmlReader.Create(stringReader);
                        xslt.Load(xsltReader, xsltSettings, resolver);

                        try
                        {
                            xslt.Transform(reader, argList, writer, resolver);
                        }
                        catch (Exception e)
                        {
                            retVal = -1;

                            using (StreamWriter errLog = File.AppendText(errFilename))
                            {
                                Log(errLog, "Error applying transform");
                                Log(errLog, e.Message);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        retVal = -2;
                        using (StreamWriter errLog = File.AppendText(errFilename))
                        {
                            Log(errLog, "Error loading transform file - " + resName);
                            Log(errLog, e.Message);
                        }
                    }
                    reader.Close();
                }
                catch (Exception e)
                {
                    retVal = -3;
                    using (StreamWriter errLog = File.AppendText(errFilename))
                    {
                        Log(errLog, "Error opening inspection import file - " + filename);
                        Log(errLog, e.Message);
                    }
                }
                writer.Close();
            }
            catch (Exception e)
            {
                retVal = -4;
                using (StreamWriter errLog = File.AppendText(errFilename))
                {
                    Log(errLog, "Error creating replacement file - " + outFilename);
                    Log(errLog, e.Message);
                }
            }

            return retVal;
        }
        #endregion

        #region Replace Inspection Name
        private static long ReplaceInspectionName(string filename, string outFilename, string newInspectionName)
        {
            long retVal = 0;
            XmlReaderSettings readerSettings = new XmlReaderSettings();
            readerSettings.ConformanceLevel = ConformanceLevel.Fragment;
            readerSettings.IgnoreWhitespace = true;
            readerSettings.IgnoreComments = true;

            XmlWriterSettings writerSettings = new XmlWriterSettings();
            writerSettings.Indent = true;// false;

            // Create an XmlUrlResolver with default credentials.
            XmlUrlResolver resolver = new XmlUrlResolver();
            resolver.Credentials = CredentialCache.DefaultCredentials;

            // Create Transform
            XslCompiledTransform xslt = new XslCompiledTransform();

            XsltSettings xsltSettings = new XsltSettings();
            xsltSettings.EnableScript = false;
            xsltSettings.EnableDocumentFunction = false;

            XsltArgumentList argList = new XsltArgumentList();
            //argList.AddParam("ReplaceThis", "", oldInspectionName +".");
            argList.AddParam("With", "", newInspectionName + ".");

            try
            {
                XmlWriter writer = XmlWriter.Create(outFilename, writerSettings);
                try
                {
                    XmlReader reader = XmlReader.Create(filename, readerSettings);
                    string resName = SVImportTransform.Properties.Resources.replace_inspection_name;
                    try
                    {
                        StringReader stringReader = new StringReader(resName);
                        XmlReader xsltReader = XmlReader.Create(stringReader);
                        xslt.Load(xsltReader, xsltSettings, resolver);

                        try
                        {
                            xslt.Transform(reader, argList, writer, resolver);
                        }
                        catch (Exception e)
                        {
                            retVal = -1;
                            using (StreamWriter errLog = File.AppendText(errFilename))
                            {
                                Log(errLog, "Error applying transform");
                                Log(errLog, e.Message);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        retVal = -2;
                        using (StreamWriter errLog = File.AppendText(errFilename))
                        {
                            Log(errLog, "Error opeing XSLT transform file - " + resName);
                            Log(errLog, e.Message);
                        }
                    }

                    reader.Close();
                }
                catch (Exception e)
                {
                    retVal = -3;
                    using (StreamWriter errLog = File.AppendText(errFilename))
                    {
                        Log(errLog, "Error opening file to transform - " + filename);
                        Log(errLog, e.Message);
                    }
                }
                writer.Close();
            }
            catch (Exception e)
            {
                retVal = -4;
                using (StreamWriter errLog = File.AppendText(errFilename))
                {
                    Log(errLog, "Error opening output file - " + outFilename);
                    Log(errLog, e.Message);
                }
            }
            return retVal;
        }
        #endregion

        #region Log
        private static void Log(TextWriter w, string logMessage)
        {
            w.Write("\r\nLog Entry : ");
            w.WriteLine("{0} {1}", DateTime.Now.ToLongTimeString(), DateTime.Now.ToLongDateString());
            w.WriteLine("  :");
            w.WriteLine("  :{0}", logMessage);
            w.WriteLine ("-------------------------------");
            // Update the underlying file.
            w.Flush();

            System.Diagnostics.Trace.WriteLine(logMessage);
        }
        #endregion
    }
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImportTransform\SVInspectionTransform.cs_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:34:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 16:59:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/