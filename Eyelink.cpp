/*
 *  Eyelink.cpp
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#include "Eyelink.h"
#include <boost/bind.hpp>

// Allocate the lock on the heap so it's never destructed
boost::mutex& Eyelink::EyelinkDriverLock = *(new boost::mutex);

bool Eyelink::Eyelink_Initialized = false;


const std::string Eyelink::TAG("tag");
const std::string Eyelink::RX("eye_rx");
const std::string Eyelink::RY("eye_ry");
const std::string Eyelink::LX("eye_lx");
const std::string Eyelink::LY("eye_ly");
const std::string Eyelink::EX("eye_x");
const std::string Eyelink::EY("eye_y");
const std::string Eyelink::EZ("eye_z");
const std::string Eyelink::H_RX("href_rx");
const std::string Eyelink::H_RY("href_ry");
const std::string Eyelink::H_LX("href_lx");
const std::string Eyelink::H_LY("href_ly");
const std::string Eyelink::P_RX("pupil_rx");
const std::string Eyelink::P_RY("pupil_ry");
const std::string Eyelink::P_LX("pupil_lx");
const std::string Eyelink::P_LY("pupil_ly");
const std::string Eyelink::P_R("pupil_size_r");
const std::string Eyelink::P_L("pupil_size_l");
const std::string Eyelink::EYE_DIST("eye_dist");
const std::string Eyelink::Z_DIST("z_dist");
const std::string Eyelink::EYE_TIME("eye_time");
const std::string Eyelink::UPDATE_PERIOD("data_interval");
const std::string Eyelink::IP("tracker_ip");


// External function for scheduling
void *update_(const shared_ptr<Eyelink> &gp){
	gp->update();                 
	return NULL;
}


void Eyelink::describeComponent(ComponentInfo &info) {
    IODevice::describeComponent(info);
    
    info.setSignature("iodevice/eyelink");
    info.setDisplayName("Eyelink 1k, Socket Link");
    info.setDescription(
                        "Eyelink 1000 Plugin. INSTRUCTIONS BELOW\n"
                        "REQUIRED PARAMETERS:\n"
                        "eye_dist = Distance between eyes in arbitrary (eyetracker) units (1500 is ok to start with)\n"
                        "z_dist = Distance from Screen in same arbitrary units (-1000 is ok to start with)\n"
                        "Adjust these values such as the raw x,y,z coordinates are in the range 0..1\n"
                        "data_interval should not be too small (i.e. not shorter than 0.5 ms)\n"
                        "tracker_ip contains the trackers IP, check for working connection using SR-Research's 'simpleexample'\n"
                        "OUTPUT:\n"
                        "eye_x/_y/_z is the midpoint of the shortest connecting line that is orthogonal to both gaze vectors "
                        "assuming the tracker runs in binocular mode. Otherwise these values will be empty.\n"
                        "all other output parameters are specified and described in the Eyelink 1000 manual.\n"
                        );
    
    info.addParameter(RX);
    info.addParameter(RY);
    info.addParameter(LX);
    info.addParameter(LY);
    info.addParameter(EX);
    info.addParameter(EY);
    info.addParameter(EZ);
    info.addParameter(H_RX);
    info.addParameter(H_RY);
    info.addParameter(H_LX);
    info.addParameter(H_LY);
    info.addParameter(P_RX);
    info.addParameter(P_RY);
    info.addParameter(P_LX);
    info.addParameter(P_LY);
    info.addParameter(P_R);
    info.addParameter(P_L);
    info.addParameter(EYE_DIST, true, "1500");
    info.addParameter(Z_DIST, true, "-1000");
    info.addParameter(EYE_TIME);
    info.addParameter(UPDATE_PERIOD, true, "1ms");
    info.addParameter(IP, true, "10.1.1.2");
}


Eyelink::Eyelink(const ParameterValueMap &parameters) :
IODevice(parameters),
e_rx(parameters[RX]),
e_ry(parameters[RY]),
e_lx(parameters[LX]),
e_ly(parameters[LY]),
e_x(parameters[EX]),
e_y(parameters[EY]),
e_z(parameters[EZ]),
h_rx(parameters[H_RX]),
h_ry(parameters[H_RY]),
h_lx(parameters[H_LX]),
h_ly(parameters[H_LY]),
p_rx(parameters[P_RX]),
p_ry(parameters[P_RY]),
p_lx(parameters[P_LX]),
p_ly(parameters[P_LY]),
p_r(parameters[P_R]),
p_l(parameters[P_L]),
e_dist(parameters[EYE_DIST]),
z_dist(parameters[Z_DIST]),
e_time(parameters[EYE_TIME]),
update_period(parameters[UPDATE_PERIOD]),
tracker_ip(parameters[IP].str()),
clock(Clock::instance()),
errors(0),
ack_msg_counter(0)
{ }


bool Eyelink::initialize(){
	
    boost::mutex::scoped_lock lock(EyelinkDriverLock);
	
	if (Eyelink_Initialized) {
        merror(M_IODEVICE_MESSAGE_DOMAIN,"Eyelink was previously initialized! Trying again, but this is dangerous!!");
    }
	
    Eyelink_Initialized = false;
    
	// Initializes the link
	//mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Trying to find Eyelink System at %s",tracker_ip);
	if (set_eyelink_address( &tracker_ip[0] ) )
	    merror(M_IODEVICE_MESSAGE_DOMAIN,"Failed to set Tracker to address %s", tracker_ip.c_str());
	
	if(open_eyelink_connection(0))
	    merror(M_IODEVICE_MESSAGE_DOMAIN,"Failed to connect to Tracker at %s", tracker_ip.c_str());
	else {
		ELINKNODE node;
        
        // generate data file name
        time_t now = time(NULL);
        struct tm* t = gmtime(&now);
        
        sprintf(data_file_name, "%02d%06d.edf",(t->tm_year-100),t->tm_yday*1440 + t->tm_hour*60 + t->tm_min);
        //YYMMMMMM : YY=Years since 2k, MMMMMM=Minutes in current year
        
        if ( open_data_file( data_file_name ) ) {
            mwarning(M_IODEVICE_MESSAGE_DOMAIN,"Eyelink datafile setting failed (%s)",data_file_name);
        }
        else {
            mprintf(M_IODEVICE_MESSAGE_DOMAIN,"Eyelink logs to local file %s",data_file_name);
        }
		
		// tell the tracker who we are
		eyelink_set_name((char*)("MWorks_over_Socket"));
		
		// verify the name
		if( eyelink_get_node(0,&node) != OK_RESULT )
			merror(M_IODEVICE_MESSAGE_DOMAIN,"Error, EyeLink doesn't respond");
		else eyemsg_printf((char*)("%s connected"), node.name);
		
		// Enable link data reception
		eyelink_reset_data(1);
	}
	
	// Eyelink should now be in "TCP-Link Open" mode
	
	if(eyelink_is_connected()) {
		
		tracker_version = eyelink_get_tracker_version(version_info);
		mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink %d System Version %s connected via Socket",tracker_version,version_info);
		
		Eyelink_Initialized = true;
        stopped = true;
	}
	else {
		merror(M_IODEVICE_MESSAGE_DOMAIN,"Error, Eyelink Connection could not be established");
	}
	
	return Eyelink_Initialized;
}


Eyelink::~Eyelink(){
    
    boost::mutex::scoped_lock lock(EyelinkDriverLock);
    
	if (Eyelink_Initialized) {
        
		if (!stopped) {
			mwarning(M_IODEVICE_MESSAGE_DOMAIN,"Eyelink is still running !");
            //eyelink stop recording
			if(eyelink_is_connected()) { stop_recording(); }
		}
        
		if (schedule_node) {
			schedule_node->cancel();
			schedule_node.reset();
		}
		
		if(eyelink_is_connected()) {
			
			// Places EyeLink tracker in off-line (idle) mode
			set_offline_mode();
			// close any open data files
			
			if ( close_data_file() == 0 ) {
                mprintf(M_IODEVICE_MESSAGE_DOMAIN,"Eyelink closed data file %s.",data_file_name);
            }
			
			if (eyelink_close(1)) // disconnect from tracker
			{
				merror(M_IODEVICE_MESSAGE_DOMAIN, "Could not close Eyelink connection");
			}
            
			//close_eyelink_system();
			
            mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink %d System Version %s disconnected.",tracker_version,version_info);
            
		}
		else {
			mwarning(M_IODEVICE_MESSAGE_DOMAIN,"Error, Eyelink Shutdown failed");
		}
		
		Eyelink_Initialized = false;
        
    }
}

bool Eyelink::update() {
    MWTime inputtime;
    
    boost::mutex::scoped_lock lock(EyelinkDriverLock);
	if(eyelink_is_connected())	{
		while(eyelink_get_next_data(NULL)) {
			if(eyelink_in_data_block(1,0)) { //only if data contains samples
				eyelink_get_float_data(&evt);
                
                inputtime = this->clock->getCurrentTimeUS();
                
                // occasionally, send the current time together with the sample time back to the tracker (and log it there)
                if ( ack_msg_counter++ % 256 == 0 )
                    eyemsg_printf((char*)"SAMPLE %ld received %ld",(long double)evt.time ,(long double)inputtime);
				
                // now update all the variables
				e_time -> setValue( (float)evt.time ,inputtime);
				
				if( evt.gx[RIGHT_EYE] != MISSING_DATA &&
                   evt.gy[RIGHT_EYE] != MISSING_DATA &&
                   evt.gx[LEFT_EYE] != MISSING_DATA &&
                   evt.gy[LEFT_EYE] != MISSING_DATA ) {
					
					p4321z = -z_dist / e_dist;
					
					p43x = evt.gx[LEFT_EYE]/e_dist;
					p43y = evt.gy[LEFT_EYE]/e_dist;     
					
					p21x = evt.gx[RIGHT_EYE]/e_dist - 1;
					p21y = evt.gy[RIGHT_EYE]/e_dist;
					
					d4321 = p43x * p21x + p43y * p21y + p4321z * p4321z;
					d4343 = p43x * p43x + p43y * p43y + p4321z * p4321z;
					d2121 = p21x * p21x + p21y * p21y + p4321z * p4321z;
					
					denom = d2121 * d4343 - d4321 * d4321;
					
					if (abs(denom) > 1e-6) {
						
						numer = p43x * d4321 - p21x * d4343;
                        
						mua = numer / denom;
						mub = (p43x + d4321 * mua) / d4343;
                        
						pax = 1 + mua * p21x;
						pay = mua * p21y;
						paz = -p4321z + mua * p4321z;
						pbx = mub * p43x;
						pby = mub * p43y;
						pbz = -p4321z + mub * p4321z;
                        
						e_x -> setValue(pax + 0.5*(pbx-pax),inputtime);
						e_y -> setValue(pay + 0.5*(pby-pay),inputtime);
						e_z -> setValue(paz + 0.5*(pbz-paz),inputtime);
					}
				}
				else {
					if (e_x->getValue().getFloat() != MISSING_DATA)
						e_x -> setValue((float)MISSING_DATA,inputtime);
					if (e_y->getValue().getFloat() != MISSING_DATA)
						e_y -> setValue((float)MISSING_DATA,inputtime);
					if (e_z->getValue().getFloat() != MISSING_DATA)
						e_z -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.gx[RIGHT_EYE] != MISSING_DATA &&
                   evt.gy[RIGHT_EYE] != MISSING_DATA ){
					e_rx -> setValue( evt.gx[RIGHT_EYE] ,inputtime);
					e_ry -> setValue( evt.gy[RIGHT_EYE] ,inputtime);
				}
				else {
					if (e_rx->getValue().getFloat() != MISSING_DATA)
						e_rx -> setValue((float)MISSING_DATA,inputtime);
					if (e_ry->getValue().getFloat() != MISSING_DATA)
						e_ry -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.gx[LEFT_EYE] != MISSING_DATA &&
                   evt.gy[LEFT_EYE] != MISSING_DATA ){
					e_lx -> setValue( evt.gx[LEFT_EYE] ,inputtime);
					e_ly -> setValue( evt.gy[LEFT_EYE] ,inputtime);
				}
				else {
					if (e_lx->getValue().getFloat() != MISSING_DATA)
						e_lx -> setValue((float)MISSING_DATA,inputtime);
					if (e_ly->getValue().getFloat() != MISSING_DATA)
						e_ly -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.hx[RIGHT_EYE] != -7936.0f &&
                   evt.hy[RIGHT_EYE] != -7936.0f ){
					h_rx -> setValue( evt.hx[RIGHT_EYE] ,inputtime);
					h_ry -> setValue( evt.hy[RIGHT_EYE] ,inputtime);
				}
				else {
					if (h_rx->getValue().getFloat() != MISSING_DATA)
						h_rx -> setValue((float)MISSING_DATA,inputtime);
					if (h_ry->getValue().getFloat() != MISSING_DATA)
						h_ry -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.hx[LEFT_EYE] != -7936.0f &&
                   evt.hy[LEFT_EYE] != -7936.0f ){
					h_lx -> setValue( evt.hx[LEFT_EYE] ,inputtime);
					h_ly -> setValue( evt.hy[LEFT_EYE] ,inputtime);
				}
				else {
					if (h_lx->getValue().getFloat() != MISSING_DATA)
						h_lx -> setValue((float)MISSING_DATA,inputtime);
					if (h_ly->getValue().getFloat() != MISSING_DATA)
						h_ly -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.px[RIGHT_EYE] != MISSING_DATA &&
                   evt.py[RIGHT_EYE] != MISSING_DATA ){
					p_rx -> setValue( evt.px[RIGHT_EYE] ,inputtime);
					p_ry -> setValue( evt.py[RIGHT_EYE] ,inputtime);
				}
				else {
					if (p_rx->getValue().getFloat() != MISSING_DATA)
						p_rx -> setValue((float)MISSING_DATA,inputtime);
					if (p_ry->getValue().getFloat() != MISSING_DATA)
						p_ry -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.px[LEFT_EYE] != MISSING_DATA &&
                   evt.py[LEFT_EYE] != MISSING_DATA ){
					p_lx -> setValue( evt.px[LEFT_EYE] ,inputtime);
					p_ly -> setValue( evt.py[LEFT_EYE] ,inputtime);
				}
				else {
					if (p_lx->getValue().getFloat() != MISSING_DATA)
						p_lx -> setValue((float)MISSING_DATA,inputtime);
					if (p_ly->getValue().getFloat() != MISSING_DATA)
						p_ly -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.pa[RIGHT_EYE] != 0 ){
					p_r -> setValue( evt.pa[RIGHT_EYE] ,inputtime);
				}
				else {
					if (p_r->getValue().getFloat() != 0)
						p_r -> setValue((float)MISSING_DATA,inputtime);
				}
				
				if( evt.pa[LEFT_EYE] != 0 ){
					p_l -> setValue( evt.pa[LEFT_EYE] ,inputtime);
				}
				else {
					if (p_l->getValue().getFloat() != 0)
						p_l -> setValue((float)MISSING_DATA,inputtime);
				}
			}
		}
	}
	else {
		if(++errors * update_period > (MWorksTime)1000000) { //just a quick hack but impossible to ignore by the user
			merror(M_IODEVICE_MESSAGE_DOMAIN, "Error! EyeLink Connection Lost!!");
			errors = 0;
		}
	}
	
	return true;
}


bool Eyelink::startDeviceIO(){
    boost::mutex::scoped_lock lock(EyelinkDriverLock);
	
	if(eyelink_is_connected() && stopped) {
		//Eyelink to offline mode
		set_offline_mode();
		// Eyelink to record mode
		if( start_recording(0,1,1,0) ) {
			merror(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink does not start!");
		}
		
        
		shared_ptr<Scheduler> scheduler = Scheduler::instance();
		shared_ptr<Eyelink> this_one = component_shared_from_this<Eyelink>();
		schedule_node = scheduler->scheduleUS(std::string(FILELINE ": ") + getTag(),
											  (MWorksTime)0, 
											  update_period, 
											  M_REPEAT_INDEFINITELY, 
											  boost::bind(update_, 
														  this_one),
											  M_DEFAULT_IODEVICE_PRIORITY,
											  M_DEFAULT_IODEVICE_WARN_SLOP_US,
											  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
											  M_MISSED_EXECUTION_DROP);
		
		stopped = false;
		mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink successfully started.");
	}
	else {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Warning! Could not start EyeLink! (StartIO)");
	}
    
	return !stopped;
}


bool Eyelink::stopDeviceIO() {
    boost::mutex::scoped_lock lock(EyelinkDriverLock);
	
	if(!stopped) {
		
		if (schedule_node) {
			schedule_node->cancel();
			schedule_node.reset();
		}
		
		if(eyelink_is_connected()) {
			//eyelink stop recording
			stop_recording();
			//go to eyelink offline mode
			set_offline_mode();
		}
		else mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Warning! Could not stop EyeLink! Connection Lost!! (StopIO)");
		
		e_time -> setValue( (float)MISSING_DATA );
		e_rx -> setValue( (float)MISSING_DATA );
		e_ry -> setValue( (float)MISSING_DATA );
		e_lx -> setValue( (float)MISSING_DATA );
		e_ly -> setValue( (float)MISSING_DATA );
		e_x -> setValue( (float)MISSING_DATA );
		e_y -> setValue( (float)MISSING_DATA );
		e_z -> setValue( (float)MISSING_DATA );
		h_rx -> setValue( (float)MISSING_DATA );
		h_ry -> setValue( (float)MISSING_DATA );
		h_lx -> setValue( (float)MISSING_DATA );
		h_ly -> setValue( (float)MISSING_DATA );
		p_rx -> setValue( (float)MISSING_DATA );
		p_ry -> setValue( (float)MISSING_DATA );
		p_lx -> setValue( (float)MISSING_DATA );
		p_ly -> setValue( (float)MISSING_DATA );
		p_r -> setValue( (float)MISSING_DATA );
		p_l -> setValue( (float)MISSING_DATA );
		
		
		stopped = true;
		mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink successfully stopped.");
	}
	
    return stopped;
}

