#
# Linux Commons Makefile
#

## Parametros a customizar para cada aplicacion
GRAGE_PROJECT_NAME=grage-pfs
BUILD_SOURCE=app/src/core/* ./app/src/*.c
BUILD_SOURCE_CONSOLE=app/src/console/*
BUILD_SOURCE_SERVICES=app/src/services/*


<zx<zx<zx<zx<zx<x

##
##Compilacion de la aplicacin (app)
##

GRAGE_REPOSITORY_HUDSON=$(GRAGE_REPOSITORY)/hudson
GRAGE_REPOSITORY_LIB=$(GRAGE_REPOSITORY)/lib
GRAGE_REPOSITORY_INCLUDES=$(GRAGE_REPOSITORY)/includes
GRAGE_REPOSITORY_HUDSON_REPORTS=$(GRAGE_REPOSITORY_HUDSON)/reports
GRAGE_REPOSITORY_HUDSON_XSLT=$(GRAGE_REPOSITORY_HUDSON)/xslt
GRAGE_XSLT_TEMPLATE=$(GRAGE_REPOSITORY_HUDSON_XSLT)/template1.xsl
GRAGE_TESTING_UNIT_LIB=grage-linux-testing-unit
GRAGE_TESTING_INTEGRATION_LIB=grage-linux-testing-integration
GRAGE_REPOSITORY=/opt/grage-repository

BUILD_DIR=build
BUILD_TARGET = build/bin/$(GRAGE_PROJECT_NAME)
BUILD_INCLUDES = -I./app/includes -I$(GRAGE_REPOSITORY_INCLUDES) 
BUILD_DEPS= -L$(GRAGE_REPOSITORY_LIB) -lgrage-commons -lgrage-nipc -lm -lpthread `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=22 -licui18n -licuuc -licudata
BUILD_LIB_DEPS= -L$(GRAGE_REPOSITORY_LIB) -lgrage-commons -lgrage-ppd -lgrage-praid \
		-lm -lpthread `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=22

TEST_UNIT_INCLUDES=./tests/unit/includes
TEST_UNIT_SOURCE=./tests/unit/src/*
TEST_UNIT_TARGET_DIR=target/unit
TEST_INTEG_INCLUDES = ./tests/integration/includes
TEST_INTEG_SRC = ./tests/integration/src/*
TEST_INTEG_TARGET_DIR = target/integration
TEST_LAUNCHER=testLauncher-$(GRAGE_PROJECT_NAME)

.PHONY: build tests

build:
	if [ -d build ]; then rm -rf build; fi
	mkdir build
	mkdir build/bin
	mkdir build/conf
	mkdir build/logs
	gcc $(BUILD_SOURCE) $(BUILD_SOURCE_SERVICES) $(BUILD_SOURCE_CONSOLE) $(BUILD_INCLUDES) $(BUILD_DEPS) -g -o $(BUILD_TARGET)
	cp app/conf/* build/conf
	cp app/conf/* /opt/grage-repository/conf
	
build-lib:
	if [ -d build ]; then rm -rf build; fi
	mkdir build
	mkdir build/bin
	mkdir build/conf
	mkdir build/logs
	gcc $(BUILD_SOURCE) $(BUILD_SOURCE_SERVICES) $(BUILD_SOURCE_CONSOLE) $(BUILD_INCLUDES) $(BUILD_LIB_DEPS) -o $(BUILD_TARGET)
	cp app/conf/* build/conf

	
##
## Generacion de los reportes
##
test-reports:
	xsltproc --stringparam suitename $(GRAGE_PROJECT_NAME)\
	 -o $(TEST_UNIT_TARGET_DIR)/$(GRAGE_PROJECT_NAME)-Hudson-Reports.xml\
	 $(GRAGE_XSLT_TEMPLATE) $(TEST_UNIT_TARGET_DIR)/$(GRAGE_PROJECT_NAME)-Results.xml 
	
	
##
## Instalacion de los reportes
##	
test-install:	 
	cp  $(TEST_UNIT_TARGET_DIR)/$(GRAGE_PROJECT_NAME)-Hudson-Reports.xml $(GRAGE_REPOSITORY_HUDSON_REPORTS)
	

##
## Compilacion de los test de unidad
##
test-build:
	mkdir $(TEST_UNIT_TARGET_DIR) -p
	mkdir $(TEST_UNIT_TARGET_DIR)/conf
	mkdir $(TEST_UNIT_TARGET_DIR)/logs
	mkdir $(TEST_UNIT_TARGET_DIR)/bin
	
	gcc $(TEST_UNIT_SOURCE) $(BUILD_SOURCE) $(BUILD_SOURCE_CONSOLE) $(BUILD_SOURCE_SERVICES) -I$(TEST_UNIT_INCLUDES) $(BUILD_INCLUDES) -l$(GRAGE_TESTING_UNIT_LIB)\
	  $(BUILD_DEPS) -I/usr/local/include/CUnit -L/usr/local/lib -lcunit -static -o $(TEST_UNIT_TARGET_DIR)/bin/$(TEST_LAUNCHER)
	  
	cp -r app/conf/* $(TEST_UNIT_TARGET_DIR)/conf

##
## Compilacion de los test de integracion
##
integration: clean-integration integration-build integration-run

integration-build:
	mkdir $(TEST_INTEG_TARGET_DIR) -p
	gcc $(TEST_INTEG_SRC) $(BUILD_SOURCE) $(BUILD_INCLUDES) -I$(TEST_INTEG_INCLUDES)\
	  $(BUILD_DEPS) -l$(GRAGE_TESTING_INTEGRATION_LIB)  -o $(TEST_INTEG_TARGET_DIR)/$(TEST_LAUNCHER)

integration-run:
	if [ -d $(TEST_INTEG_TARGET_DIR) ]; then ./$(TEST_INTEG_TARGET_DIR)/$(TEST_LAUNCHER); fi 


##
## Corrida completa de los tests
##
tests: clean-test test-build test-run test-reports test-install


##
## Ejecucion de los tests
##
test-run:
	if [ -d $(TEST_UNIT_TARGET_DIR) ]; then cd ./$(TEST_UNIT_TARGET_DIR)/bin && ./$(TEST_LAUNCHER) && cd ../../.. ; fi 
	if [ -d $(TEST_UNIT_TARGET_DIR) ]; then mv $(TEST_UNIT_TARGET_DIR)/bin/*.xml $(TEST_UNIT_TARGET_DIR); fi


##
## Clean de build, test y reportes
##
clean-build:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi
	
clean-test:
	if [ -d $(TEST_UNIT_TARGET_DIR) ]; then rm -rf $(TEST_UNIT_TARGET_DIR); fi
	
clean-integration:
	if [ -d $(TEST_INTEG_TARGET_DIR) ]; then rm -rf $(TEST_INTEG_TARGET_DIR); fi

clean-target:
	if [ -d target ]; then rm -rf target ; fi

clean-eclipse-Debug:
	if [ -d Debug ]; then rm -rf Debug ; fi

clean:
	make clean-build
	make clean-test
	make clean-integration
	make clean-target
	make clean-eclipse-Debug

