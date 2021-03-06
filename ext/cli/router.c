
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2013 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          Rack Lin <racklin@gmail.com>                                  |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/exception.h"
#include "kernel/array.h"
#include "kernel/string.h"
#include "kernel/object.h"

/**
 * Phalcon\CLI\Router
 *
 * <p>Phalcon\CLI\Router is the standard framework router. Routing is the
 * process of taking a command-line arguments and
 * decomposing it into parameters to determine which module, task, and
 * action of that task should receive the request</p>
 *
 *<code>
 *	$router = new Phalcon\CLI\Router();
 *	$router->handle(array(
 *		'module' => 'main',
 *		'task' => 'videos',
 *		'action' => 'process'
 *	));
 *	echo $router->getTaskName();
 *</code>
 *
 */


/**
 * Phalcon\CLI\Router initializer
 */
PHALCON_INIT_CLASS(Phalcon_CLI_Router){

	PHALCON_REGISTER_CLASS(Phalcon\\CLI, Router, cli_router, phalcon_cli_router_method_entry, 0);

	zend_declare_property_null(phalcon_cli_router_ce, SL("_dependencyInjector"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_module"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_task"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_action"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_params"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_defaultModule"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_defaultTask"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_defaultAction"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_cli_router_ce, SL("_defaultParams"), ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_class_implements(phalcon_cli_router_ce TSRMLS_CC, 1, phalcon_di_injectionawareinterface_ce);

	return SUCCESS;
}

/**
 * Phalcon\CLI\Router constructor
 */
PHP_METHOD(Phalcon_CLI_Router, __construct){


	phalcon_update_property_empty_array(phalcon_cli_router_ce, this_ptr, SL("_params") TSRMLS_CC);
	
	phalcon_update_property_empty_array(phalcon_cli_router_ce, this_ptr, SL("_defaultParams") TSRMLS_CC);
	
}

/**
 * Sets the dependency injector
 *
 * @param Phalcon\DiInterface $dependencyInjector
 */
PHP_METHOD(Phalcon_CLI_Router, setDI){

	zval *dependency_injector;

	phalcon_fetch_params(0, 1, 0, &dependency_injector);
	
	phalcon_update_property_this(this_ptr, SL("_dependencyInjector"), dependency_injector TSRMLS_CC);
	
}

/**
 * Returns the internal dependency injector
 *
 * @return Phalcon\DiInterface
 */
PHP_METHOD(Phalcon_CLI_Router, getDI){


	RETURN_MEMBER(this_ptr, "_dependencyInjector");
}

/**
 * Sets the name of the default module
 *
 * @param string $moduleName
 */
PHP_METHOD(Phalcon_CLI_Router, setDefaultModule){

	zval *module_name;

	phalcon_fetch_params(0, 1, 0, &module_name);
	
	phalcon_update_property_this(this_ptr, SL("_defaultModule"), module_name TSRMLS_CC);
	
}

/**
 * Sets the default controller name
 *
 * @param string $taskName
 */
PHP_METHOD(Phalcon_CLI_Router, setDefaultTask){

	zval *task_name;

	phalcon_fetch_params(0, 1, 0, &task_name);
	
	phalcon_update_property_this(this_ptr, SL("_defaultTask"), task_name TSRMLS_CC);
	
}

/**
 * Sets the default action name
 *
 * @param string $actionName
 */
PHP_METHOD(Phalcon_CLI_Router, setDefaultAction){

	zval *action_name;

	phalcon_fetch_params(0, 1, 0, &action_name);
	
	phalcon_update_property_this(this_ptr, SL("_defaultAction"), action_name TSRMLS_CC);
	
}

/**
 * Handles routing information received from command-line arguments
 *
 * @param array $arguments
 */
PHP_METHOD(Phalcon_CLI_Router, handle){

	zval *arguments = NULL, *module_name = NULL, *task_name = NULL, *action_name = NULL;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 0, 1, &arguments);
	
	if (!arguments) {
		PHALCON_INIT_VAR(arguments);
		array_init(arguments);
	} else {
		PHALCON_SEPARATE_PARAM(arguments);
	}
	
	if (Z_TYPE_P(arguments) != IS_ARRAY) { 
		PHALCON_THROW_EXCEPTION_STR(phalcon_cli_router_exception_ce, "Arguments must be an Array");
		return;
	}
	
	PHALCON_INIT_VAR(module_name);
	
	PHALCON_INIT_VAR(task_name);
	
	PHALCON_INIT_VAR(action_name);
	
	/** 
	 * Check for a module
	 */
	if (phalcon_array_isset_string(arguments, SS("module"))) {
		PHALCON_OBS_NVAR(module_name);
		phalcon_array_fetch_string(&module_name, arguments, SL("module"), PH_NOISY);
		phalcon_array_unset_string(&arguments, SS("module"), PH_SEPARATE);
	}
	
	/** 
	 * Check for a task
	 */
	if (phalcon_array_isset_string(arguments, SS("task"))) {
		PHALCON_OBS_NVAR(task_name);
		phalcon_array_fetch_string(&task_name, arguments, SL("task"), PH_NOISY);
		phalcon_array_unset_string(&arguments, SS("task"), PH_SEPARATE);
	}
	
	/** 
	 * Check for an action
	 */
	if (phalcon_array_isset_string(arguments, SS("action"))) {
		PHALCON_OBS_NVAR(action_name);
		phalcon_array_fetch_string(&action_name, arguments, SL("action"), PH_NOISY);
		phalcon_array_unset_string(&arguments, SS("action"), PH_SEPARATE);
	}
	
	phalcon_update_property_this(this_ptr, SL("_module"), module_name TSRMLS_CC);
	phalcon_update_property_this(this_ptr, SL("_task"), task_name TSRMLS_CC);
	phalcon_update_property_this(this_ptr, SL("_action"), action_name TSRMLS_CC);
	phalcon_update_property_this(this_ptr, SL("_params"), arguments TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Returns proccesed module name
 *
 * @return string
 */
PHP_METHOD(Phalcon_CLI_Router, getModuleName){


	RETURN_MEMBER(this_ptr, "_module");
}

/**
 * Returns proccesed task name
 *
 * @return string
 */
PHP_METHOD(Phalcon_CLI_Router, getTaskName){


	RETURN_MEMBER(this_ptr, "_task");
}

/**
 * Returns proccesed action name
 *
 * @return string
 */
PHP_METHOD(Phalcon_CLI_Router, getActionName){


	RETURN_MEMBER(this_ptr, "_action");
}

/**
 * Returns proccesed extra params
 *
 * @return array
 */
PHP_METHOD(Phalcon_CLI_Router, getParams){


	RETURN_MEMBER(this_ptr, "_params");
}

