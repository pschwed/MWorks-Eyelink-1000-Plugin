/*
 *  EyelinkPlugin.h
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */
#ifndef Eyelink_PLUGIN_H_
#define Eyelink_PLUGIN_H_

#include <MWorksCore/Plugin.h>
using namespace mw;

extern "C"{
    Plugin *getPlugin();
}

class EyelinkPlugin : public Plugin {
    
	virtual void registerComponents(shared_ptr<mw::ComponentRegistry> registry);	
};


#endif