#!/bin/sh

REPO_LOCATION=/opt/grage-repository


if [ -d ${REPO_LOCATION} ] ; then 
	echo "Eliminando repo actual"
	rm -r /opt/grage-repository ;

fi


echo "Creando repo"
mkdir ${REPO_LOCATION}

echo "Creando directorio de includes"
mkdir ${REPO_LOCATION}/includes

echo "Creando directorio de librerias"
mkdir ${REPO_LOCATION}/lib

echo "Creando directorio para las transformaciones"
mkdir ${REPO_LOCATION}/hudson/xslt --parents

echo "Creando directorio para los reportes"
mkdir ${REPO_LOCATION}/hudson/reports 

echo "Copiando el archivo de la transformacion XSLT"
cp $2 ${REPO_LOCATION}/hudson/xslt 

echo "Cambiando propietario del repositorio al usuario '$1'"
chown -R $1:$1 ${REPO_LOCATION}
