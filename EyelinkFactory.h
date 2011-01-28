/*
 *  EyelinkFactory.h
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */


#ifndef Eyelink_FACTORY_H
#define Eyelink_FACTORY_H

#include "Eyelink.h"

#include "MWorksCore/ComponentFactory.h"
using namespace mw;

class EyelinkFactory : public ComponentFactory {
	virtual boost::shared_ptr<mw::Component> createObject(std::map<std::string, std::string> parameters,
                                                   mw::ComponentRegistry *reg);
};

#endif
