<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes"/>
	<xsl:variable name="cunitCount" select="count(//CUNIT_RUN_TEST_RECORD)"/>
	<xsl:variable name="cunitFailureCount" select="count(//CUNIT_RUN_TEST_FAILURE)"/>
	<xsl:variable name="cunitSuccessCount" select="count(//CUNIT_RUN_TEST_SUCCESS)"/>
	<xsl:template match="/">
		<TestRun>
			<FailedTests>
				<xsl:apply-templates select="//CUNIT_RUN_TEST_FAILURE"/>
			</FailedTests>
			<SuccessfulTests>
				<xsl:apply-templates select="//CUNIT_RUN_TEST_SUCCESS"/>
			</SuccessfulTests>
			<Statistics>
				<xsl:element name="Tests"><xsl:value-of select="$cunitCount"/></xsl:element>
				<xsl:element name="FailuresTotal"><xsl:value-of select="$cunitFailureCount"/></xsl:element>
				<xsl:element name="Errors"><xsl:value-of select="$cunitFailureCount"/></xsl:element><!-- Este falta ver como obtengo los errores. -->
				<xsl:element name="Failures"><xsl:value-of select="$cunitFailureCount"/></xsl:element>												
			</Statistics>
		</TestRun>
	</xsl:template>
	<xsl:template name="failures" match="//CUNIT_RUN_TEST_FAILURE">
		<FailedTest>
			<xsl:attribute name="id">
				<xsl:value-of select="position()" />
			</xsl:attribute>
			<xsl:element name="Name">
				<xsl:value-of select="normalize-space(TEST_NAME)"/>
			</xsl:element>
			<xsl:element name="FailureType"/>
			<Location>
				<xsl:element name="File">
					<xsl:value-of select="normalize-space(FILE_NAME)"/>
				</xsl:element>
				<xsl:element name="Line">
					<xsl:value-of select="normalize-space(LINE_NUMBER)"/>
				</xsl:element>
			</Location>
			<xsl:element name="Message">
				<xsl:value-of select="normalize-space(CONDITION)"/>
			</xsl:element>
		</FailedTest>
	</xsl:template>
	<xsl:template name="success" match="//CUNIT_RUN_TEST_SUCCESS">
		<Test>
			<xsl:attribute name="id">
				<xsl:value-of select="position() + $cunitFailureCount" />
			</xsl:attribute>		
			<xsl:element name="Name">
				<xsl:value-of select="normalize-space(TEST_NAME)"/>
			</xsl:element>
		</Test>
	</xsl:template>
	<xsl:template match="text()|@*"/>
</xsl:stylesheet>
