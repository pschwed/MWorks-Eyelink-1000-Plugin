/*
 *  EyelinkPlugin.cpp
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#include <MWorksCore/Plugin.h>

#include "EyelinkFactory.h"

using namespace mw;


class EyelinkPlugin : public Plugin {
	virtual void registerComponents(shared_ptr<ComponentRegistry> registry) {
        registry->registerFactory("iodevice/eyelink", new EyelinkFactory());
    }
};


extern "C" Plugin* getPlugin() {
    return new EyelinkPlugin();
}
