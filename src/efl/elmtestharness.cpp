#include <Elementary.h>

#include "elmtestharness.h"

ElmTestHarness::ElmTestHarness()
	: TestHarness::TestHarness()
	, eventType_(ecore_event_type_new())
	, handler_(NULL)
{
	return;
}

void ElmTestHarness::run()
{
	// TODO: Add a run timeout just in case
	// a bug gets introduced into idler stuff.

	ecore_idler_add(idleSetup, this);
	elm_run();
}

/*static*/
Eina_Bool ElmTestHarness::idleSetup(void* data)
{
	ElmTestHarness* harness = static_cast<ElmTestHarness*>(data);
	harness->handler_ = ecore_event_handler_add(
		harness->eventType_,
		doSetup,
		data
	);
	ecore_event_add(harness->eventType_, NULL, NULL, NULL);

	return ECORE_CALLBACK_CANCEL;
}

/*static*/
Eina_Bool ElmTestHarness::doSetup(void* data, int, void*)
{
	ElmTestHarness* harness = static_cast<ElmTestHarness*>(data);

	ecore_event_handler_del(harness->handler_);

	// TODO: wrap this with try-catch
	harness->setup();

	Application::yield();

	ecore_idler_add(idleStep, data);

	return ECORE_CALLBACK_DONE;
}

/*static*/
Eina_Bool ElmTestHarness::idleStep(void* data)
{
	ElmTestHarness* harness = static_cast<ElmTestHarness*>(data);

	harness->handler_ = ecore_event_handler_add(
		harness->eventType_,
		doStep,
		data
	);
	ecore_event_add(harness->eventType_, NULL, NULL, NULL);

	return ECORE_CALLBACK_CANCEL;
}

/*static*/
Eina_Bool ElmTestHarness::doStep(void* data, int, void*)
{
	ElmTestHarness* harness = static_cast<ElmTestHarness*>(data);

	ecore_event_handler_del(harness->handler_);

	if (harness->haveStep()) {
		harness->runNextStep();
		Application::yield();
		ecore_idler_add(idleStep, data);
	} else {
		ecore_idler_add(idleTeardown, data);
	}

	return ECORE_CALLBACK_DONE;
}

/*static*/
Eina_Bool ElmTestHarness::idleTeardown(void* data)
{
	ElmTestHarness* harness = static_cast<ElmTestHarness*>(data);

	harness->handler_ = ecore_event_handler_add(
		harness->eventType_,
		doTeardown,
		data
	);
	ecore_event_add(harness->eventType_, NULL, NULL, NULL);

	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool ElmTestHarness::doTeardown(void* data, int, void*)
{
	ElmTestHarness* harness = static_cast<ElmTestHarness*>(data);

	ecore_event_handler_del(harness->handler_);

	harness->handler_ = NULL;

	// TODO: wrap this with try-catch
	harness->teardown();

	Application::yield();

	elm_exit();

	return ECORE_CALLBACK_DONE;
}
