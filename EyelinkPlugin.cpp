/*
 *  EyelinkPlugin.cpp
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#include "EyelinkPlugin.h"
#include "EyelinkFactory.h"
#include "MWorksCore/ComponentFactory.h"
using namespace mw;

Plugin *getPlugin(){
    return new EyelinkPlugin();
}


void EyelinkPlugin::registerComponents(shared_ptr<mw::ComponentRegistry> registry) {

    registry->registerFactory(std::string("iodevice/eyelink"),
							  (ComponentFactory *)(new EyelinkFactory()));
}

