<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
>

<xsl:output method="text"/>

<xsl:template match="xs:schema">
#ifndef GLITE_LB_JOB_ATTRS_H
#define GLITE_LB_JOB_ATTRS_H

#define GLITE_JP_LB_NS "http://egee.cesnet.cz/en/Schema/LB/Attributes"
#define GLITE_JP_LB_JDL_NS "http://egee.cesnet.cz/en/Schema/LB/Attributes:JDL"

<xsl:apply-templates select="xs:element" mode="define"/>
#define GLITE_JP_LB_CLASSAD_NS "http://jdl"

typedef enum _lb_attrs {
	attr_UNDEF,
<xsl:apply-templates select="xs:element" mode="enum"/>} lb_attrs;

static const char *lb_attrNames[] = {
	"UNDEFINED",
<xsl:apply-templates select="xs:element" mode="str"/>};

#endif /* GLITE_LB_JOB_ATTRS_H */
</xsl:template>

<xsl:template match="xs:element" mode="define">/** <xsl:value-of select="xs:documentation/text()"/> */
#define GLITE_JP_LB_<xsl:value-of select="@name"/>	GLITE_JP_LB_NS &quot;:<xsl:value-of select="@name"/>&quot;
</xsl:template>

<xsl:template match="xs:element" mode="enum">	attr_<xsl:value-of select="@name"/>, /** <xsl:value-of select="xs:documentation/text()"/> */
</xsl:template>

<xsl:template match="xs:element" mode="str">	GLITE_JP_LB_<xsl:value-of select="@name"/>,
</xsl:template>

</xsl:stylesheet>
