<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="utf-8" omit-xml-declaration="no"/>
  <!-- <xsl:strip-space elements="*"/> -->
  <xsl:param name="Filename" select="'file://replacements.xml'"/>
  <xsl:variable name="vReps" select="document($Filename) " />
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>
  <xsl:template match="text()">
    <xsl:variable name="vNewVal" select= "$vReps/elem[@name=name(current()/..)] /replace[this = current()] /with/text()"/>
    <xsl:copy-of select= "$vNewVal | self::text()[not($vNewVal)]"/>
  </xsl:template>
</xsl:stylesheet>