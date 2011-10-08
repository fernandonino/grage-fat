#
# Linux Commons Makefile
#


## Rutas del repositorio comun
GRAGE_REPOSITORY=/opt/grage-repository
GRAGE_REPOSITORY_HUDSON=$(GRAGE_REPOSITORY)/hudson
GRAGE_REPOSITORY_LIB=$(GRAGE_REPOSITORY)/lib
GRAGE_REPOSITORY_INCLUDES=$(GRAGE_REPOSITORY)/includes
GRAGE_REPOSITORY_HUDSON_REPORTS=$(GRAGE_REPOSITORY_HUDSON)/reports
GRAGE_REPOSITORY_HUDSON_XSLT=$(GRAGE_REPOSITORY_HUDSON)/xslt

## El archivo de la transformacion
GRAGE_XSLT_TEMPLATE=$(GRAGE_REPOSITORY_HUDSON_XSLT)/template1.xsl

## nombres generales del projecto
GRAGE_PROJECT_NAME=grage-nipc
GRAGE_UNIT_TESTING_LIB=grage-linux-testing-unit
GRAGE_INTEGRATION_TESTING_LIB=grage-linux-testing-integration
##
##Compilacion de la aplicacin (app)
##
BUILD_DIR=build
BUILD_TARGET = build/lib$(GRAGE_PROJECT_NAME).a
BUILD_INCLUDES = ./app/includes
BUILD_SOURCE=app/src/*
BUILD_FLAGS = -w -g -c -o
BUILD_LIB_DEPS= -L$(GRAGE_REPOSITORY_LIB) -lgrage-commons

##
## Compilacion de los test de unidad (testing-unit)
##
TEST_UNIT_INCLUDES=./tests/unit/includes
TEST_UNIT_SOURCE=./tests/unit/src/*
TEST_UNIT_TARGET_DIR=target/unit

##
## Compilacion de los test de integracion (testing-integration)
##
TEST_INTEG_INCLUDES = ./tests/integration/includes
TEST_INTEG_SRC = ./tests/integration/src/*
TEST_INTEG_TARGET_DIR = target/integration

TEST_LAUNCHER=testLauncher-$(GRAGE_PROJECT_NAME)


#
# Comportamiento comun
#

build: objdir 
	ar rcs $(BUILD_TARGET) build/obj/*.o
	rm -rf build/obj
	
objdir:
	if [ -d build ]; then rm -rf build; fi
	mkdir -p build/obj
	gcc -I$(BUILD_INCLUDES) -I$(GRAGE_REPOSITORY_INCLUDES) $(BUILD_LIB_DEPS) $(BUILD_SOURCE) -w -g -c -lm
	mv *.o build/obj


#
# Instalacion de librerias y headers en un
# repositorio comun
#
install: build
	if [ -d $(GRAGE_REPOSITORY_INCLUDES) ]; then cp $(BUILD_INCLUDES)/*.h $(GRAGE_REPOSITORY_INCLUDES); fi
	if [ -d $(GRAGE_REPOSITORY_LIB) ]; then cp $(BUILD_TARGET) $(GRAGE_REPOSITORY_LIB); fi


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
	
	gcc $(TEST_UNIT_SOURCE) -I$(GRAGE_REPOSITORY_INCLUDES) -I$(TEST_UNIT_INCLUDES) -L$(GRAGE_REPOSITORY_LIB) -l$(GRAGE_UNIT_TESTING_LIB)\
	  -lgrage-commons -l$(GRAGE_PROJECT_NAME) -lm -lpthread -I/usr/local/include/CUnit -L/usr/local/lib -lcunit  \
	  -static -o $(TEST_UNIT_TARGET_DIR)/$(TEST_LAUNCHER)
	mv $(TEST_UNIT_TARGET_DIR)/$(TEST_LAUNCHER) $(TEST_UNIT_TARGET_DIR)/bin  


##
## Compilacion de los test de integracion
##
integration: clean-integration integration-build integration-run

integration-build:
	mkdir $(TEST_INTEG_TARGET_DIR) -p
	gcc $(TEST_INTEG_SRC) -I$(GRAGE_REPOSITORY_INCLUDES) -I$(TEST_INTEG_INCLUDES)\
	 -L$(GRAGE_REPOSITORY_LIB) -l$(GRAGE_INTEGRATION_TESTING_LIB) -l$(GRAGE_PROJECT_NAME)\
	  -lm -lpthread -static -o $(TEST_INTEG_TARGET_DIR)/$(TEST_LAUNCHER)

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
## Clean de test y reportes
##	
clean-build:
	if [ -d $(BUILD_DIR) ]; then rm -r $(BUILD_DIR); fi

clean-test:
	if [ -d $(TEST_UNIT_TARGET_DIR) ]; then rm -r $(TEST_UNIT_TARGET_DIR); fi

clean-integration:
	if [ -d $(TEST_INTEG_TARGET_DIR) ]; then rm -r $(TEST_INTEG_TARGET_DIR); fi

clean-target:
	if [ -d target ]; then rm -r target ; fi

clean-eclipse-Debug:
	if [ -d Debug ]; then rm -r Debug ; fi

clean:
	make clean-build
	make clean-test
	make clean-integration
	make clean-target
	make clean-eclipse-Debug	