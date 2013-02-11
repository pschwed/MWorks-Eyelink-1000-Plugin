/*
 *  EyelinkPlugin.cpp
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#include <MWorksCore/Plugin.h>
#include <MWorksCore/StandardComponentFactory.h>

#include "Eyelink.h"

using namespace mw;


class EyelinkPlugin : public Plugin {
	virtual void registerComponents(shared_ptr<ComponentRegistry> registry) {
        registry->registerFactory<StandardComponentFactory, Eyelink>();
    }
};


MW_SYMBOL_PUBLIC
extern "C" Plugin* getPlugin() {
    return new EyelinkPlugin();
}
