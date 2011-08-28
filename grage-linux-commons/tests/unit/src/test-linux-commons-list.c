/*
 * test-linux-commons-list.c
 *
 *  Created on: 22/08/2011
 *      Author: gonzalo
 */


#include <stdlib.h>
#include "linux-commons.h"
#include "linux-commons-list.h"
#include "linux-commons-strings.h"

#include "linux-testing-unit.h"


	/*
	 * Mocking
	 */

	typedef struct {
		int id;
		String vlaue;
	} SomethingTest;


	SomethingTest * mockSomethingTest(int id , String value){
		SomethingTest * s = malloc(sizeof(SomethingTest));
		s->id = id;
		s->vlaue = value;

		return s;
	}


	List mockList(){
		Boolean equalityCriteria(SomethingTest * a , SomethingTest * b){
			return ( a->id == b->id);
		}
		void listingCriteria(SomethingTest * a){
			printf("Listando %i %s \n " , a->id , a->vlaue);
		}
		return commons_list_buildList(
			(void (*)(Object))listingCriteria ,
			(Boolean (*)(Object, Object)) equalityCriteria ,
			commons_list_ORDER_ALWAYS_LAST);
	}





	/*
	 * prototipos
	 */

	void test_commons_list_buildList();
	void test_commons_list_buildNode();
	void test_commons_list_addNode();

	void test_commons_list_listAllNodes();
	void test_commons_list_removeNode();
	void test_commons_list_containsNode();
	void test_commons_list_getNodeByCriteria();
	void test_commons_list_buildStringsList();
	void test_commons_list_buildStringsListWithValues();
	void test_commons_list_formatToPreetyString();
	void test_commons_list_serialize();
	void test_commons_list_tokenize();
	void test_commons_list_deserialize();
	void test_commons_list_getSize();
	void test_commons_iterator_buildIterator();
	void test_commons_list_removeList();




	/*
	 * Configuracion de la suite
	 */

	Boolean linux_commons_list_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons List" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Test de la contruccion de una lista"
				, test_commons_list_buildList) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Contruccion listado de todos los nodos"
				, test_commons_list_listAllNodes) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test de la contruccion de un nodo"
					, test_commons_list_buildNode) == NULL)
				return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Agregado de un nodo"
				, test_commons_list_addNode) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Eliminacion de un nodo"
				, test_commons_list_removeNode) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Existencia de un nodo en la lista"
				, test_commons_list_containsNode) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Obtencion de un nodo de la lista por un criterio determinado"
				, test_commons_list_getNodeByCriteria) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Construccion de una lista a travez de parametros strings"
				, test_commons_list_buildStringsList) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Construccion de una lista a travez de parametros strings"
				, test_commons_list_buildStringsListWithValues) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Formateo de una lista de strings a un string bonito"
				, test_commons_list_formatToPreetyString) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Serializacion de los valores de una lista a un string"
				, test_commons_list_serialize) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Deserializacion de un string a una lista"
				, test_commons_list_deserialize) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Separacion de un string en una lista de tokens "
				, test_commons_list_tokenize) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Construccion de un Iterator"
				, test_commons_iterator_buildIterator) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "eliminacion de una lista liberando la memoria"
				, test_commons_list_removeList) == NULL)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}






	void test_commons_list_buildList(){

		List targetList = mockList();

		CU_ASSERT_PTR_NOT_NULL(targetList);
		CU_ASSERT_EQUAL(targetList->size , 0);

		commons_list_removeList(targetList , free );
	}





	void test_commons_list_buildNode(){

		SomethingTest * s = mockSomethingTest(32 , "hola");

		Node * node = commons_list_buildNode(s);

		CU_ASSERT_PTR_NOT_NULL(node);
		CU_ASSERT_PTR_NOT_NULL(node->data);

		SomethingTest * data = (SomethingTest*)node->data;

		CU_ASSERT_EQUAL(data->id, 32);
		CU_ASSERT_TRUE( commons_string_equals(data->vlaue, "hola"));

		free(data);
		free(node);

	}




	void test_commons_list_addNode(){
		SomethingTest * st = mockSomethingTest(1 , "probando agregar nodo");

		List list = mockList();

		commons_list_addNode(list , st);

		CU_ASSERT_EQUAL(list->size , 1);

		commons_list_removeList(list , free);
	}




	void test_commons_list_listAllNodes(){

		List list = mockList();

		commons_list_addNode(list , mockSomethingTest(1 , "probando agregar nodo 1"));
		commons_list_addNode(list , mockSomethingTest(2 , "probando agregar nodo 2"));
		commons_list_addNode(list , mockSomethingTest(3 , "probando agregar nodo 3"));
		commons_list_addNode(list , mockSomethingTest(4 , "probando agregar nodo 4"));

		commons_list_listAllNodes(list);

		commons_list_removeList(list , free);
	}





	void test_commons_list_removeNode(){
		List list = mockList();

		SomethingTest * aNode = mockSomethingTest(1 , "probando agregar nodo 1");
		commons_list_addNode(list , aNode);

		CU_ASSERT_EQUAL(list->size , 1);
		CU_ASSERT_PTR_NOT_NULL(list->elements);

		commons_list_removeNode(list , aNode , free);

		CU_ASSERT_EQUAL(list->size , 0);
		CU_ASSERT_PTR_NULL(list->elements);

	}





	void test_commons_list_containsNode(){

		List list = mockList();

		SomethingTest * st1 = mockSomethingTest(1,  "mock node 1");
		SomethingTest * st2 = mockSomethingTest(2,  "mock node 2");
		SomethingTest * st3 = mockSomethingTest(3,  "mock node 3");

		CU_ASSERT_FALSE(commons_list_containsNode(list , st3));

		commons_list_addNode(list , st1);
		commons_list_addNode(list , st2);

		CU_ASSERT_FALSE(commons_list_containsNode(list , st3));

		commons_list_addNode(list , st3);

		CU_ASSERT_TRUE(commons_list_containsNode(list , st3));

		commons_list_removeList(list , free);
	}





	void test_commons_list_getNodeByCriteria(){

		List list = mockList();

		SomethingTest * s1 = mockSomethingTest(1 , "value 1");
		SomethingTest * s2 = mockSomethingTest(2 , "value 2");

		commons_list_addNode(list , s1);
		commons_list_addNode(list , s2);

		Boolean searchByIdCriteria(SomethingTest * t){
			return ( t->id == 1);
		}

		SomethingTest * found1 = commons_list_getNodeByCriteria(list , searchByIdCriteria);

		CU_ASSERT_PTR_NOT_NULL(found1);
		CU_ASSERT_PTR_NOT_NULL(found1->vlaue);
		CU_ASSERT_TRUE( commons_string_equals("value 1" , found1->vlaue));
		CU_ASSERT_EQUAL( found1->id , 1);

		Boolean searchByValueCriteria(SomethingTest * t){
			return ( commons_string_equals( t->vlaue  , "value 2"));
		}

		SomethingTest * found2 = commons_list_getNodeByCriteria(list , searchByValueCriteria);

		CU_ASSERT_PTR_NOT_NULL(found2);
		CU_ASSERT_PTR_NOT_NULL(found2->vlaue);
		CU_ASSERT_TRUE( commons_string_equals("value 2" , found2->vlaue));
		CU_ASSERT_EQUAL( found2->id , 2);

		commons_list_removeList(list , free);
	}





	void test_commons_iterator_buildIterator(){
		List list = commons_list_buildStringsListWithValues(2 , "hola" , "chau");
		Iterator * ite = commons_iterator_buildIterator(list);

		CU_ASSERT_PTR_NOT_NULL(ite);

		CU_ASSERT_TRUE(commons_iterator_hasMoreElements(ite));
		CU_ASSERT_PTR_NOT_NULL(commons_iterator_next(ite));

		CU_ASSERT_TRUE(commons_iterator_hasMoreElements(ite));
		CU_ASSERT_PTR_NOT_NULL(commons_iterator_next(ite));

		CU_ASSERT_FALSE(commons_iterator_hasMoreElements(ite));


	}





	void test_commons_list_buildStringsList(){

		List list = commons_list_buildStringsList();

		commons_list_addNode(list , "Hola");
		commons_list_addNode(list , " ");
		commons_list_addNode(list , "Mundo.");
		commons_list_addNode(list , " ");
		commons_list_addNode(list , "Este ");
		commons_list_addNode(list , "es ");
		commons_list_addNode(list , "el ");
		commons_list_addNode(list , "mejor ");
		commons_list_addNode(list , "Grupo!!!");

		CU_ASSERT_EQUAL(list->size , 9 );
	}






	void test_commons_list_buildStringsListWithValues(){

		List list = commons_list_buildStringsListWithValues( 7 , "Hola" ,
				"mundo. " , "Este" , "es" , "el" , "mejor" , "grupo!" );

		CU_ASSERT_EQUAL(list->size , 7);
	}






	void test_commons_list_formatToPreetyString(){

		List list = commons_list_buildStringsListWithValues( 7 , "Hola" ,
				"mundo. " , "Este " , "es " , "el " , "mejor " , "grupo!" );

		String formated = commons_list_formatToPreetyString(list);
		String expected = "Hola, mundo., Este, es, el, mejor, grupo!";
		printf("%s\n" , formated);
		printf("%s\n" , expected);

		CU_ASSERT_TRUE(commons_string_equals(formated , expected));

	}






	void test_commons_list_serialize(){

		List list = commons_list_buildStringsListWithValues( 4 , "hola" ,
			"a" , "todos" , "lalala");

		String serialized = commons_list_serialize(list , "#");
		String expected = "hola#a#todos#lalala";

		CU_ASSERT_PTR_NOT_NULL(serialized);
		CU_ASSERT_TRUE(commons_string_equals(serialized , expected));
	}





	void test_commons_list_deserialize(){

		String serialized = "hola,a,todos,lalala";
		List list = commons_list_deserialize(serialized);

		CU_ASSERT_PTR_NOT_NULL(list);
		CU_ASSERT_EQUAL(list->size , 4);
	}





	void test_commons_list_tokenize(){

		String enter = "hola a todos esto es una puta prueba";

		List tokens = commons_list_tokenize(enter , ' ');

		CU_ASSERT_PTR_NOT_NULL(tokens);
		CU_ASSERT_EQUAL(tokens->size , 8);
	}




	void test_commons_list_getSize(){

		String enter = "hola a todos esto es una puta prueba";

		List tokens = commons_list_tokenize(enter , ' ');

		CU_ASSERT_PTR_NOT_NULL(tokens);
		CU_ASSERT_EQUAL(commons_list_getSize(tokens) , 8);

	}




	void test_commons_list_removeList(){

	}

