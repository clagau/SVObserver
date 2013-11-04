﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.296
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace SVImportTransform.Properties {
    using System;
    
    
    /// <summary>
    ///   A strongly-typed resource class, for looking up localized strings, etc.
    /// </summary>
    // This class was auto-generated by the StronglyTypedResourceBuilder
    // class via a tool like ResGen or Visual Studio.
    // To add or remove a member, edit your .ResX file then rerun ResGen
    // with the /str option, or rebuild your VS project.
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("System.Resources.Tools.StronglyTypedResourceBuilder", "4.0.0.0")]
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
    [global::System.Runtime.CompilerServices.CompilerGeneratedAttribute()]
    internal class Resources {
        
        private static global::System.Resources.ResourceManager resourceMan;
        
        private static global::System.Globalization.CultureInfo resourceCulture;
        
        [global::System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal Resources() {
        }
        
        /// <summary>
        ///   Returns the cached ResourceManager instance used by this class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Resources.ResourceManager ResourceManager {
            get {
                if (object.ReferenceEquals(resourceMan, null)) {
                    global::System.Resources.ResourceManager temp = new global::System.Resources.ResourceManager("SVImportTransform.Properties.Resources", typeof(Resources).Assembly);
                    resourceMan = temp;
                }
                return resourceMan;
            }
        }
        
        /// <summary>
        ///   Overrides the current thread's CurrentUICulture property for all
        ///   resource lookups using this strongly typed resource class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Globalization.CultureInfo Culture {
            get {
                return resourceCulture;
            }
            set {
                resourceCulture = value;
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to &lt;xsl:stylesheet version=&quot;1.0&quot; xmlns:xsl=&quot;http://www.w3.org/1999/XSL/Transform&quot;&gt;
        ///  &lt;xsl:output method=&quot;xml&quot; indent=&quot;yes&quot; encoding=&quot;utf-8&quot; omit-xml-declaration=&quot;no&quot;/&gt;
        ///  &lt;!-- &lt;xsl:strip-space elements=&quot;*&quot;/&gt; --&gt;
        ///  &lt;xsl:param name=&quot;Filename&quot; select=&quot;&apos;file://replacements.xml&apos;&quot;/&gt;
        ///  &lt;xsl:variable name=&quot;vReps&quot; select=&quot;document($Filename) &quot; /&gt;
        ///  &lt;xsl:template match=&quot;node()|@*&quot;&gt;
        ///    &lt;xsl:copy&gt;
        ///      &lt;xsl:apply-templates select=&quot;node()|@*&quot;/&gt;
        ///    &lt;/xsl:copy&gt;
        ///  &lt;/xsl:template&gt;
        ///  &lt;xsl:template match=&quot;text()&quot;&gt;
        ///  [rest of string was truncated]&quot;;.
        /// </summary>
        internal static string apply_replacements {
            get {
                return ResourceManager.GetString("apply_replacements", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to &lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot;?&gt; 
        ///&lt;xsl:stylesheet version=&quot;1.0&quot; xmlns:xsl=&quot;http://www.w3.org/1999/XSL/Transform&quot; 
        ///xmlns:msxsl=&quot;urn:schemas-microsoft-com:xslt&quot;   
        ///xmlns:js=&quot;urn:custom-javascript&quot;
        ///xmlns:svr1=&quot;x-schema:#SVR00001&quot;
        ///exclude-result-prefixes=&quot;msxsl js&quot;&gt; 
        ///  &lt;xsl:output method=&quot;xml&quot; indent=&quot;yes&quot; encoding=&quot;utf-8&quot; omit-xml-declaration=&quot;no&quot;/&gt;
        ///  
        ///  &lt;msxsl:script language=&quot;JavaScript&quot; implements-prefix=&quot;js&quot;&gt; 
        ///  &lt;![CDATA[
        ///  function GenerateGUID() { 
        ///    return (&quot;{&quot; +new ActiveXObject(&quot;Scr [rest of string was truncated]&quot;;.
        /// </summary>
        internal static string generate_replacements {
            get {
                return ResourceManager.GetString("generate_replacements", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to &lt;xsl:stylesheet version=&quot;1.0&quot; xmlns:xsl=&quot;http://www.w3.org/1999/XSL/Transform&quot; xmlns:svr1=&quot;x-schema:#SVR00001&quot;&gt;
        ///  &lt;xsl:output method=&quot;xml&quot; indent=&quot;yes&quot; encoding=&quot;utf-8&quot; omit-xml-declaration=&quot;no&quot;/&gt;
        ///
        ///  &lt;!-- &lt;xsl:strip-space elements=&quot;*&quot;/&gt; --&gt;
        ///  &lt;xsl:param name=&quot;ReplaceThis&quot; select=&quot;//svr1:NODE[@Name=&apos;Inspection Process&apos;]/svr1:DATA[@Name=&apos;ObjectName&apos;] | //svr1:NODE[@Name=&apos;SVIPDoc&apos;]/svr1:NODE[@Name=&apos;IPDocViews&apos;]/svr1:NODE[@Name=&apos;Views&apos;]/svr1:NODE[@Name=&apos;SVImageViewScroll&apos;]/svr1:DATA[@Name=&apos;ImageName&apos;] | //s [rest of string was truncated]&quot;;.
        /// </summary>
        internal static string replace_inspection_name {
            get {
                return ResourceManager.GetString("replace_inspection_name", resourceCulture);
            }
        }
    }
}
