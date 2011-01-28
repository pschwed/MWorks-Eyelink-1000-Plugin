/*
 *  EyelinkFactory.cpp
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#include "EyelinkFactory.h"

#include <boost/regex.hpp>
#include "MWorksCore/ComponentRegistry.h"
#include "MWorksCore/GenericData.h"
#include "MWorksCore/MWorksTypes.h"

using namespace mw;

boost::shared_ptr<mw::Component> EyelinkFactory::createObject(std::map<std::string, std::string> parameters,
                                                               mw::ComponentRegistry *reg) {
	const char *TAG = "tag";
	const char *RX = "eye_rx";
	const char *RY = "eye_ry";
	const char *LX = "eye_lx";
	const char *LY = "eye_ly";
	const char *EX = "eye_x";
	const char *EY = "eye_y";
	const char *EZ = "eye_z";
	const char *H_RX = "href_rx";
	const char *H_RY = "href_ry";
	const char *H_LX = "href_lx";
	const char *H_LY = "href_ly";
	const char *P_RX = "pupil_rx";
	const char *P_RY = "pupil_ry";
	const char *P_LX = "pupil_lx";
	const char *P_LY = "pupil_ly";
	const char *P_R = "pupil_size_r";
	const char *P_L = "pupil_size_l";
	const char *EYE_DIST = "eye_dist";
	const char *Z_DIST = "z_dist";
	const char *EYE_TIME = "eye_time";
	const char *UPDATE_PERIOD = "data_interval";
	const char *IP = "tracker_ip";
	
	REQUIRE_ATTRIBUTES(parameters, 
					   TAG,
					   RX,
					   RY,
					   LX,
					   LY,
					   EX,
					   EY,
					   EZ,
					   H_RX,
					   H_RY,
					   H_LX,
					   H_LY,
					   P_RX,
					   P_RY,
					   P_LX,
					   P_LY,
					   P_R,
					   P_L,
					   EYE_DIST,
					   Z_DIST,
					   EYE_TIME,
					   UPDATE_PERIOD,
					   IP);
	
	MWorksTime update_period = reg->getNumber(parameters.find(UPDATE_PERIOD)->second);
	
	string tracker_ip = reg->getValueForAttribute(IP, parameters);
			
	boost::shared_ptr<mw::Variable> eye_rx = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[RX].empty()) {
		eye_rx = reg->getVariable(parameters.find(RX)->second);	
		checkAttribute(eye_rx, 
					   parameters.find("reference_id")->second, 
					   RX, 
					   parameters.find(RX)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_ry = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[RY].empty()) {
		eye_ry = reg->getVariable(parameters.find(RY)->second);	
		checkAttribute(eye_ry, 
					   parameters.find("reference_id")->second, 
					   RY, 
					   parameters.find(RY)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_lx = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[LX].empty()) {
		eye_lx = reg->getVariable(parameters.find(LX)->second);	
		checkAttribute(eye_lx, 
					   parameters.find("reference_id")->second, 
					   LX, 
					   parameters.find(LX)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_ly = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[LY].empty()) {
		eye_ly = reg->getVariable(parameters.find(LY)->second);	
		checkAttribute(eye_ly, 
					   parameters.find("reference_id")->second, 
					   LY, 
					   parameters.find(LY)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_x = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[EX].empty()) {
		eye_x = reg->getVariable(parameters.find(EX)->second);	
		checkAttribute(eye_x, 
					   parameters.find("reference_id")->second, 
					   EX, 
					   parameters.find(EX)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_y = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[EY].empty()) {
		eye_y = reg->getVariable(parameters.find(EY)->second);	
		checkAttribute(eye_y, 
					   parameters.find("reference_id")->second, 
					   EY, 
					   parameters.find(EY)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_z = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));	
	if(!parameters[EZ].empty()) {
		eye_z = reg->getVariable(parameters.find(EZ)->second);	
		checkAttribute(eye_z, 
					   parameters.find("reference_id")->second, 
					   EZ, 
					   parameters.find(EZ)->second);
	}

	boost::shared_ptr<mw::Variable> href_rx = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[H_RX].empty()) {
		href_rx = reg->getVariable(parameters.find(H_RX)->second);	
		checkAttribute(href_rx, 
					   parameters.find("reference_id")->second, 
					   H_RX, 
					   parameters.find(H_RX)->second);
	}
	
	boost::shared_ptr<mw::Variable> href_ry = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[H_RY].empty()) {
		href_ry = reg->getVariable(parameters.find(H_RY)->second);	
		checkAttribute(href_ry, 
					   parameters.find("reference_id")->second, 
					   H_RY, 
					   parameters.find(H_RY)->second);
	}
	
	boost::shared_ptr<mw::Variable> href_lx = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[H_LX].empty()) {
		href_lx = reg->getVariable(parameters.find(H_LX)->second);	
		checkAttribute(href_lx, 
					   parameters.find("reference_id")->second, 
					   H_LX, 
					   parameters.find(H_LX)->second);
	}
	
	boost::shared_ptr<mw::Variable> href_ly = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[H_LY].empty()) {
		href_ly = reg->getVariable(parameters.find(H_LY)->second);	
		checkAttribute(href_ly, 
					   parameters.find("reference_id")->second, 
					   H_LY, 
					   parameters.find(H_LY)->second);
	}
	
	boost::shared_ptr<mw::Variable> pupil_rx = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[P_RX].empty()) {
		pupil_rx = reg->getVariable(parameters.find(P_RX)->second);	
		checkAttribute(pupil_rx, 
					   parameters.find("reference_id")->second, 
					   P_RX, 
					   parameters.find(P_RX)->second);
	}
	
	boost::shared_ptr<mw::Variable> pupil_ry = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[P_RY].empty()) {
		pupil_ry = reg->getVariable(parameters.find(P_RY)->second);	
		checkAttribute(pupil_ry, 
					   parameters.find("reference_id")->second, 
					   P_RY, 
					   parameters.find(P_RY)->second);
	}
	
	boost::shared_ptr<mw::Variable> pupil_lx = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[P_LX].empty()) {
		pupil_lx = reg->getVariable(parameters.find(P_LX)->second);	
		checkAttribute(pupil_lx, 
					   parameters.find("reference_id")->second, 
					   P_LX, 
					   parameters.find(P_LX)->second);
	}
	
	boost::shared_ptr<mw::Variable> pupil_ly = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[P_LY].empty()) {
		pupil_ly = reg->getVariable(parameters.find(P_LY)->second);	
		checkAttribute(pupil_ly, 
					   parameters.find("reference_id")->second, 
					   P_LY, 
					   parameters.find(P_LY)->second);
	}
	
	boost::shared_ptr<mw::Variable> pupil_size_r = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[P_R].empty()) {
		pupil_size_r = reg->getVariable(parameters.find(P_R)->second);	
		checkAttribute(pupil_size_r, 
					   parameters.find("reference_id")->second, 
					   P_R, 
					   parameters.find(P_R)->second);
	}
	
	boost::shared_ptr<mw::Variable> pupil_size_l = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[P_L].empty()) {
		pupil_size_l = reg->getVariable(parameters.find(P_L)->second);	
		checkAttribute(pupil_size_l, 
					   parameters.find("reference_id")->second, 
					   P_L, 
					   parameters.find(P_L)->second);
	}
	
	boost::shared_ptr<mw::Variable> eye_dist = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[EYE_DIST].empty()) {
		eye_dist = reg->getVariable(parameters.find(EYE_DIST)->second);	
		checkAttribute(eye_dist, 
					   parameters.find("reference_id")->second, 
					   EYE_DIST, 
					   parameters.find(EYE_DIST)->second);
	}

	boost::shared_ptr<mw::Variable> z_dist = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_FLOAT, 0)));
	if(!parameters[Z_DIST].empty()) {
		z_dist = reg->getVariable(parameters.find(Z_DIST)->second);	
		checkAttribute(z_dist, 
					   parameters.find("reference_id")->second, 
					   Z_DIST, 
					   parameters.find(Z_DIST)->second);
	}
	

	
	boost::shared_ptr<mw::Variable> eye_time = boost::shared_ptr<mw::Variable>(new mw::ConstantVariable(Datum(M_INTEGER, 0)));	
	if(!parameters[EYE_TIME].empty()) {
		eye_time = reg->getVariable(parameters.find(EYE_TIME)->second);	
		checkAttribute(eye_time, 
					   parameters.find("reference_id")->second, 
					   EYE_TIME, 
					   parameters.find(EYE_TIME)->second);
	}
	
	
	boost::shared_ptr <mw::Scheduler> scheduler = mw::Scheduler::instance(true);
	
	//std::string tagname(parameters["tag"]);
		
	boost::shared_ptr <mw::Component> new_eyelink = boost::shared_ptr<mw::Component>(new Eyelink(scheduler,
																						eye_rx, 
																						eye_ry,
																						eye_lx,
																						eye_ly,
																						eye_x,
																						eye_y,
																						eye_z,
																						href_rx,
																						href_ry,
																						href_lx,
																						href_ly,
																						pupil_rx,
																						pupil_ry,
																						pupil_lx,
																						pupil_ly,
																						pupil_size_r,
																						pupil_size_l,
																						eye_dist,
																						z_dist,
																						eye_time,
																						update_period,
																						tracker_ip));
	return new_eyelink;
}
