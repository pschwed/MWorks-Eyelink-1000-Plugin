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

#include <boost/thread/mutex.hpp>

#include <eyelink_core/eyelink.h>
#include <eyelink_core/core_expt.h>

using namespace mw;

class Eyelink : public IODevice {
	
	static boost::mutex& EyelinkDriverLock;
    static bool Eyelink_Initialized;

protected:
    boost::shared_ptr <ScheduleTask> schedule_node;
    boost::shared_ptr <Clock> clock;
	
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
	const double e_dist;
	const double z_dist;
	boost::shared_ptr <Variable> e_time;
	
	const MWorksTime update_period;
	string tracker_ip;
		
	FSAMPLE evt;         // buffer to hold sample and event data
	char version_info[256];
	int tracker_version;
    char data_file_name[11];
	bool stopped;
	int errors;
    unsigned int ack_msg_counter;
	
	// vars for coordinate transformation
	double p4321z;
	double p43x;
	double p43y;     
	double p21x;
	double p21y;
	double d4321;
	double d4343;
	double d2121;
	double denom;
	double numer;
	double mua;
	double mub;
	double pax;
	double pay;
	double paz;
	double pbx;
	double pby;
	double pbz;
	

public:
    static const std::string TAG;
    static const std::string RX;
    static const std::string RY;
    static const std::string LX;
    static const std::string LY;
    static const std::string EX;
    static const std::string EY;
    static const std::string EZ;
    static const std::string H_RX;
    static const std::string H_RY;
    static const std::string H_LX;
    static const std::string H_LY;
    static const std::string P_RX;
    static const std::string P_RY;
    static const std::string P_LX;
    static const std::string P_LY;
    static const std::string P_R;
    static const std::string P_L;
    static const std::string EYE_DIST;
    static const std::string Z_DIST;
    static const std::string EYE_TIME;
    static const std::string UPDATE_PERIOD;
    static const std::string IP;
    
    static void describeComponent(ComponentInfo &info);
    
    explicit Eyelink(const ParameterValueMap &parameters);
    
	~Eyelink();

	virtual bool initialize();
	virtual bool update();
	virtual bool startDeviceIO();
	virtual bool stopDeviceIO();
	
};

#endif 
