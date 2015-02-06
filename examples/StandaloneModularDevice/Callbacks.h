// ----------------------------------------------------------------------------
// Callbacks.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef CALLBACKS_H
#define CALLBACKS_H
#include "JsonParser.h"
#include "ModularDevice.h"
#include "Constants.h"
#include "Controller.h"


namespace callbacks
{
void setDisplayVariable1Callback();

void setInteractiveVariable1Callback();

void getInteractiveVariable1Callback();

void setInteractiveVariable2Callback();

void getInteractiveVariable2Callback();
}
#endif
