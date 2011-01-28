/*
 *  Eyelink.h
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#ifndef Eyelink_H_
#define Eyelink_H_

#include <MWorksCore/Plugin.h>
#include <MWorksCore/IODevice.h>

#include <eyelink_core/eyelink.h>
#include <eyelink_core/core_expt.h>

using namespace mw;

class Eyelink : public IODevice {

protected:
	boost::shared_ptr <Scheduler> scheduler;
    boost::shared_ptr <ScheduleTask> schedule_node;
	
	boost::shared_ptr <Variable> e_rx;
	boost::shared_ptr <Variable> e_ry;
	boost::shared_ptr <Variable> e_lx;
	boost::shared_ptr <Variable> e_ly;
	boost::shared_ptr <Variable> e_x;
	boost::shared_ptr <Variable> e_y;
	boost::shared_ptr <Variable> e_z;
	boost::shared_ptr <Variable> h_rx;
	boost::shared_ptr <Variable> h_ry;
	boost::shared_ptr <Variable> h_lx;
	boost::shared_ptr <Variable> h_ly;
	boost::shared_ptr <Variable> p_rx;
	boost::shared_ptr <Variable> p_ry;
	boost::shared_ptr <Variable> p_lx;
	boost::shared_ptr <Variable> p_ly;
	boost::shared_ptr <Variable> p_r;
	boost::shared_ptr <Variable> p_l;
	boost::shared_ptr <Variable> e_dist;
	boost::shared_ptr <Variable> z_dist;
	boost::shared_ptr <Variable> e_time;
	
	MWorksTime update_period;
	string tracker_ip;
		
	FSAMPLE evt;         // buffer to hold sample and event data
	char version_info[256];
	int tracker_version;
	bool stopped;
	int errors;
	
	// vars for coordinate transformation
	float iod;
	float p4321z;
	float p43x;
	float p43y;     
	float p21x;
	float p21y;
	float d4321;
	float d4343;
	float d2121;
	float denom;
	float numer;
	float mua;
	float mub;
	float pax;
	float pay;
	float paz;
	float pbx;
	float pby;
	float pbz;
	

public:
	Eyelink(const boost::shared_ptr <Scheduler> &a_scheduler,
			const boost::shared_ptr <Variable> rx,
			const boost::shared_ptr <Variable> ry,
			const boost::shared_ptr <Variable> lx,
			const boost::shared_ptr <Variable> ly,
			const boost::shared_ptr <Variable> gx,
			const boost::shared_ptr <Variable> gy,
			const boost::shared_ptr <Variable> gz,
			const boost::shared_ptr <Variable> hrx,
			const boost::shared_ptr <Variable> hry,
			const boost::shared_ptr <Variable> hlx,
			const boost::shared_ptr <Variable> hly,
			const boost::shared_ptr <Variable> prx,
			const boost::shared_ptr <Variable> pry,
			const boost::shared_ptr <Variable> plx,
			const boost::shared_ptr <Variable> ply,
			const boost::shared_ptr <Variable> pr,
			const boost::shared_ptr <Variable> py,
			const boost::shared_ptr <Variable> edist,
			const boost::shared_ptr <Variable> zdist,
			const boost::shared_ptr <Variable> etime,
			const MWorksTime update_time,
			const string trackerip);
	~Eyelink();

	virtual bool update();
	virtual bool startDeviceIO();
	virtual bool stopDeviceIO();
	
	virtual void addChild(std::map<std::string, std::string> parameters,
						  mw::ComponentRegistry *reg,
						  shared_ptr<mw::Component> child);
	
	shared_ptr<Eyelink> shared_from_this() { return dynamic_pointer_cast<Eyelink>(IODevice::shared_from_this()); }
};

#endif 
