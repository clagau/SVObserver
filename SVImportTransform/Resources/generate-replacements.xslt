<?xml version="1.0" encoding="UTF-8"?> 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:msxsl="urn:schemas-microsoft-com:xslt"   
xmlns:js="urn:custom-javascript"
xmlns:svr1="x-schema:#SVR00001"
exclude-result-prefixes="msxsl js"> 
  <xsl:output method="xml" indent="yes" encoding="utf-8" omit-xml-declaration="no"/>
  
  <msxsl:script language="JavaScript" implements-prefix="js"> 
  <![CDATA[
  function GenerateGUID() { 
    return ("{" +new ActiveXObject("Scriptlet.TypeLib").GUID.substr(1, 36) +"}"); 
  }
  ]]> 
  </msxsl:script>
  <xsl:param name="InspectionName" select="'Inspection_9'" />
  <xsl:template match="/">
    <elem name="DATA">
      <xsl:for-each select="//svr1:DATA[@Name='UniqueReferenceID']">
        <xsl:call-template name="EmitReplace">
          <xsl:with-param name="OldValue" select="."/>
          <xsl:with-param name="NewValue" select="js:GenerateGUID()"/>
        </xsl:call-template>
      </xsl:for-each>
      <xsl:call-template name="EmitReplace">
        <xsl:with-param name="OldValue" select="//svr1:NODE[@Name='Inspection Process']/svr1:DATA[@Name='ObjectName']"/>
        <xsl:with-param name="NewValue" select="$InspectionName"/>
      </xsl:call-template>
    </elem>
  </xsl:template>

  <xsl:template name="EmitReplace">
    <xsl:param name="OldValue"/>
    <xsl:param name="NewValue"/>
    <replace>
      <this>
        <xsl:value-of select="$OldValue"/>
      </this>
      <with>
        <xsl:value-of select="$NewValue"/>
      </with>
    </replace>
  </xsl:template>
  
</xsl:stylesheet>