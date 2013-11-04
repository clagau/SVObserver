<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:svr1="x-schema:#SVR00001">
  <xsl:output method="xml" indent="yes" encoding="utf-8" omit-xml-declaration="no"/>

  <!-- <xsl:strip-space elements="*"/> -->
  <xsl:param name="ReplaceThis" select="//svr1:NODE[@Name='Inspection Process']/svr1:DATA[@Name='ObjectName'] | //svr1:NODE[@Name='SVIPDoc']/svr1:NODE[@Name='IPDocViews']/svr1:NODE[@Name='Views']/svr1:NODE[@Name='SVImageViewScroll']/svr1:DATA[@Name='ImageName'] | //svr1:NODE[@Name='SVIPDoc']/svr1:NODE[@Name='IPDocViews']/svr1:NODE[@Name='Views']/svr1:NODE[@Name='SVImageView']/svr1:DATA[@Name='ImageName']" />
  <xsl:param name="With"/>
  <xsl:variable name="InpectionNamePrefix" select="concat($ReplaceThis, '.')"/>
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:variable name="vNewVal">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="." />
        <xsl:with-param name="replace" select="$InpectionNamePrefix" />
        <xsl:with-param name="by" select="$With" />
      </xsl:call-template>
    </xsl:variable>

    <xsl:copy-of select="$vNewVal"/>
  </xsl:template>

  <!--   <xsl:include href="string-replace-all.xslt"/> -->

  <xsl:template name="string-replace-all">
    <xsl:param name="text" />
    <xsl:param name="replace" />
    <xsl:param name="by" />
    <xsl:choose>
      <xsl:when test="contains($text, $replace)">
        <xsl:value-of select="substring-before($text,$replace)" />
        <xsl:value-of select="$by" />
        <xsl:call-template name="string-replace-all">
          <xsl:with-param name="text"
          select="substring-after($text,$replace)" />
          <xsl:with-param name="replace" select="$replace" />
          <xsl:with-param name="by" select="$by" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

