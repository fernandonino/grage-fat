#!/bin/bash

#
# Preparation
#
echo " =====> 1. Comenzando la preparacion en el directorio raiz del usuario..."
cd
if [ ! -d installers ] ; then
	mkdir installers
	echo " =====> Directorio installers creado satisfactoriamente."
else
	echo " =====> El directorio installers ya existe."
fi

echo " =====> 1. Comenzando la preparacion en el directorio raiz del usuario..."
cd
if [ ! -d hudson_work ] ; then
	mkdir hudson_work
	echo " =====> Directorio installers creado satisfactoriamente."
else
	echo " =====> El directorio installers ya existe."
fi

#
# CUnit
#
	echo " =====> 2. Descarganzo CUnit"
	wget http://sourceforge.net/projects/cunit/files/CUnit/2.1-2/CUnit-2.1-2-src.tar.bz2
	if [ $? -eq 0 ] ; then
	    echo " =====> CUnit descargado satisfactoriamente."
	fi

	echo " =====> 2.1 Descomprimiendo CUnit..."
	tar xfvj CUnit-2.1-2-src.tar.bz2
	echo " =====> 2.2 Compilando e instalado CUnit..."
	cd CUnit-2.1-2
		./configure && make && make install
		if [ $? -eq 0 ] ; then
			echo "CUnit instalado satisfactoriamente"
		fi
		make distclean-all
		cd ..
	echo " =====> 2.3 Removiendo carpeta tempoarl de CUnit"
	rm -r CUnit-2.1-2
	echo " =====> 2.4 Moviendo tarball a installers"
	mv CUnit-2.1-2-src.tar.bz2 installers

#
# JDK
#
	echo " =====> 3 Descargando JDK..."
	wget http://download.oracle.com/otn-pub/java/jdk/6u27-b07/jdk-6u27-linux-i586.bin
	if [ $? -eq 0 ] ; then
	    echo " =====> JDK descargado satisfactoriamente."
	fi

	echo " =====> 3.1 Moviendo tarball a installers"
	mv jdk-6u27-linux-i586.bin installers


#
# Tomcat
#
	echo " =====> 4 Descargando Tomcat..."
	wget http://apache.dattatec.com/tomcat/tomcat-6/v6.0.33/bin/apache-tomcat-6.0.33.tar.gz
	if [ $? -eq 0 ] ; then
	    echo " =====> Tomcat descargado satisfactoriamente."
	fi

	echo " =====> 4.1 Descomprimiendo Tomcat..."
	tar xzvf apache-tomcat-6.0.33.tar.gz	
	if [ $? -eq 0 ] ; then
	    echo " =====> Tomcat descomprimiedo satisfactoriamente."
	fi

	echo " =====> 4.2 Moviendo tarball a installers"
	mv apache-tomcat-6.0.33.tar.gz installers

#
# Maven
#
	echo " =====> 5 Descargando Maven..."
	wget http://apache.dattatec.com//maven/binaries/apache-maven-2.2.1-bin.tar.gz
	if [ $? -eq 0 ] ; then
	    echo " =====> Maven descargado satisfactoriamente."
	fi

	tar xzvf apache-maven-2.2.1-bin.tar.gz
	echo " =====> 5.1 Descomprimiendo Maven..."
	if [ $? -eq 0 ] ; then
	    echo " =====> Maven descomprimiedo satisfactoriamente."
	fi

	echo " =====> 5.2 Moviendo tarball a installers"
	mv apache-maven-2.2.1-bin.tar.gz installers

#
# Hudson
#
	echo " =====> 6 Descargando Hudson..."
	wget http://hudson-ci.org/downloads/war/2.1.0/hudson.war
	if [ $? -eq 0 ] ; then
	    echo " =====> Hudson descargado satisfactoriamente."
	fi

	echo " =====> 6.1 Copiando Hudson al directorio webapps de Tomcat..."
	cp hudson.war apache-tomcat-6.0.33/webapps/

	echo " =====> 6.2 Moviendo hudson.war a installers"
	mv hudson.war installers

#
# Configuration
#

	echo " =====> 7 Definiendo las variables de entorno..."
	echo "export HUDSON_HOME=\"/home/grage/hudson_work\"" >> .profile
	echo "export JAVA_HOME=\"/home/grage/jdk1.6.0_26\"" >> .profile
	echo "export JAVA_OPTS=\"-D\${HUDSON_HOME} -Xmx512m\"" >> .profile
	echo "export M2_HOME=\"/home/grage/apache-maven-2.2.1\"" >> .profile
	echo "export CATALINA_HOME=\"/home/grage/apache-tomcat-6.0.33\"" >> .profile
	echo " " >> .profile
	echo "export PATH=\"\$PATH:\$JAVA_HOME/bin\"" >> .profile
	echo "export PATH=\"\$PATH:\$M2_HOME/bin\"" >> .profile
	echo "export PATH=\"\$PATH:\$CATALINA_HOME/bin\"" >> .profile

	echo " =====> 7 Variables de entorno definidas"

	echo " "
	echo "Queda pendiente la configuracion de los paquetes."
	echo " "
