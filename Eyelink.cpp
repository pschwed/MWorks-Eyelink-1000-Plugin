/*
 *  Eyelink.cpp
 *  Eyelink
 *
 *  Created by Philipp Schwedhelm on 13.12.10.
 *  Copyright 2010 German Primate Center. All rights reserved.
 *
 */

#include "Eyelink.h"
#include "boost/bind.hpp"

// External function for scheduling
void *update_(const shared_ptr<Eyelink> &gp){
	gp->update();                 
	return NULL;
}


Eyelink::Eyelink(const boost::shared_ptr <Scheduler> &a_scheduler,
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
				 const boost::shared_ptr <Variable> pl,
				 const float edist,
				 const float zdist,
				 const boost::shared_ptr <Variable> etime,
				 const MWorksTime update_time,
				 const string trackerip) {
	scheduler = a_scheduler;
	
	e_rx = rx;
	e_ry = ry;
	e_lx = lx;
	e_ly = ly;
	e_x = gx;
	e_y = gy;
	e_z = gz;
	e_time = etime;
	h_rx = hrx;
	h_ry = hry;
	h_lx = hlx;
	h_ly = hly;
	p_rx = prx;
	p_ry = pry;
	p_lx = plx;
	p_ly = ply;
	p_r = pr;
	p_l = pl;
	
	update_period = update_time;
	
	e_dist = edist;
	z_dist = zdist;
	
	tracker_ip = trackerip;
	
	// Initializes the link
	//mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Trying to find Eyelink System at %s",tracker_ip);
	if (set_eyelink_address( &tracker_ip[0] ) )
	    throw SimpleException("Failed to set Tracker to address " + tracker_ip);
	
	if(open_eyelink_connection(0))
	    throw SimpleException("Failed to connect to Tracker at " + tracker_ip);
	else {
		ELINKNODE node;
		
		// tell the tracker who we are
		eyelink_set_name((char*)("MWorks_over_Socket"));

		// verify the name
		if( eyelink_get_node(0,&node) != OK_RESULT )
			throw SimpleException("Error, EyeLink didn't acceppt our name");
		else eyemsg_printf((char*)("%s connected"), node.name);
		
		// Enable link data reception
		eyelink_reset_data(1);
	}
	
	// Eyelink should now be in "TCP-Link Open" mode

	if(eyelink_is_connected()) {
		
		tracker_version = eyelink_get_tracker_version(version_info);
		mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink %d System Version %s connected via Socket",tracker_version,version_info);
	
	}
	else {
		throw SimpleException("Error, Eyelink Connection could not be established");
	}
	
	errors = 0;
	stopped = false;
	 
}


Eyelink::~Eyelink(){
	if(!stopped) Eyelink::stopDeviceIO();
	if(eyelink_is_connected()) {
	    // Places EyeLink tracker in off-line (idle) mode
	    set_offline_mode();
		// close any open data files
	    eyecmd_printf((char*)("close_data_file"));
		
	    eyelink_close(1);         // disconnect from tracker
	
		close_eyelink_system();
		
		mprintf(M_IODEVICE_MESSAGE_DOMAIN, "Eyelink-1000 System Disconnected.");
	}
	else {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN,"Error, Eyelink Shutdown failed");
	}
}

bool Eyelink::update() {
	
	if(eyelink_is_connected())	{
		while(eyelink_get_next_data(NULL)) {
			if(eyelink_in_data_block(1,0)) { //only if data contains samples
				eyelink_get_float_data(&evt);
				
				e_time -> setValue( (float)evt.time );
				
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
					
						e_x -> setValue(pax + 0.5*(pbx-pax));
						e_y -> setValue(pay + 0.5*(pby-pay));
						e_z -> setValue(paz + 0.5*(pbz-paz));
					}
				}
				else {
					if (e_x->getValue().getFloat() != MISSING_DATA)
						e_x -> setValue((float)MISSING_DATA);
					if (e_y->getValue().getFloat() != MISSING_DATA)
						e_y -> setValue((float)MISSING_DATA);
					if (e_z->getValue().getFloat() != MISSING_DATA)
						e_z -> setValue((float)MISSING_DATA);
				}
				
				if( evt.gx[RIGHT_EYE] != MISSING_DATA &&
				    evt.gy[RIGHT_EYE] != MISSING_DATA ){
					e_rx -> setValue( evt.gx[RIGHT_EYE] );
					e_ry -> setValue( evt.gy[RIGHT_EYE] );
				}
				else {
					if (e_rx->getValue().getFloat() != MISSING_DATA)
						e_rx -> setValue((float)MISSING_DATA);
					if (e_ry->getValue().getFloat() != MISSING_DATA)
						e_ry -> setValue((float)MISSING_DATA);
				}
				
				if( evt.gx[LEFT_EYE] != MISSING_DATA &&
				    evt.gy[LEFT_EYE] != MISSING_DATA ){
					e_lx -> setValue( evt.gx[LEFT_EYE] );
					e_ly -> setValue( evt.gy[LEFT_EYE] );
				}
				else {
					if (e_lx->getValue().getFloat() != MISSING_DATA)
						e_lx -> setValue((float)MISSING_DATA);
					if (e_ly->getValue().getFloat() != MISSING_DATA)
						e_ly -> setValue((float)MISSING_DATA);
				}
				
				if( evt.hx[RIGHT_EYE] != -7936.0f &&
				    evt.hy[RIGHT_EYE] != -7936.0f ){
					h_rx -> setValue( evt.hx[RIGHT_EYE] );
					h_ry -> setValue( evt.hy[RIGHT_EYE] );
				}
				else {
					if (h_rx->getValue().getFloat() != MISSING_DATA)
						h_rx -> setValue((float)MISSING_DATA);
					if (h_ry->getValue().getFloat() != MISSING_DATA)
						h_ry -> setValue((float)MISSING_DATA);
				}
				
				if( evt.hx[LEFT_EYE] != -7936.0f &&
				    evt.hy[LEFT_EYE] != -7936.0f ){
					h_lx -> setValue( evt.hx[LEFT_EYE] );
					h_ly -> setValue( evt.hy[LEFT_EYE] );
				}
				else {
					if (h_lx->getValue().getFloat() != MISSING_DATA)
						h_lx -> setValue((float)MISSING_DATA);
					if (h_ly->getValue().getFloat() != MISSING_DATA)
						h_ly -> setValue((float)MISSING_DATA);
				}
				
				if( evt.px[RIGHT_EYE] != MISSING_DATA &&
				    evt.py[RIGHT_EYE] != MISSING_DATA ){
					p_rx -> setValue( evt.px[RIGHT_EYE] );
					p_ry -> setValue( evt.py[RIGHT_EYE] );
				}
				else {
					if (p_rx->getValue().getFloat() != MISSING_DATA)
						p_rx -> setValue((float)MISSING_DATA);
					if (p_ry->getValue().getFloat() != MISSING_DATA)
						p_ry -> setValue((float)MISSING_DATA);
				}
				
				if( evt.px[LEFT_EYE] != MISSING_DATA &&
				    evt.py[LEFT_EYE] != MISSING_DATA ){
					p_lx -> setValue( evt.px[LEFT_EYE] );
					p_ly -> setValue( evt.py[LEFT_EYE] );
				}
				else {
					if (p_lx->getValue().getFloat() != MISSING_DATA)
						p_lx -> setValue((float)MISSING_DATA);
					if (p_ly->getValue().getFloat() != MISSING_DATA)
						p_ly -> setValue((float)MISSING_DATA);
				}
				
				if( evt.pa[RIGHT_EYE] != 0 ){
					p_r -> setValue( evt.pa[RIGHT_EYE] );
				}
				else {
					if (p_r->getValue().getFloat() != 0)
						p_r -> setValue(0.0f);
				}
				
				if( evt.pa[LEFT_EYE] != 0 ){
					p_l -> setValue( evt.pa[LEFT_EYE] );
				}
				else {
					if (p_l->getValue().getFloat() != 0)
						p_l -> setValue(0.0f);
				}
			}
		}
	}
	else {
		if(++errors * update_period > (MWorksTime)1000000) { //just a quick hack
			mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Error! EyeLink Connection Lost!!");
			errors = 0;
		}
	}
	
	return true;
}


bool Eyelink::startDeviceIO(){
	
	if(eyelink_is_connected()) {
		//Eyelink to offline mode
		set_offline_mode();
		// Eyelink to record mode
		if( start_recording(0,0,1,0) ) {
			mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Warning: Eyelink does not start! Closing connection..");
			Eyelink::stopDeviceIO();
		}
		
		
		shared_ptr<Eyelink> this_one = shared_from_this();
		schedule_node = scheduler->scheduleUS(std::string(FILELINE ": ") + tag,
											  (MWorksTime)0, 
											  update_period, 
											  M_REPEAT_INDEFINITELY, 
											  boost::bind(update_, 
														  this_one),
											  M_DEFAULT_IODEVICE_PRIORITY,
											  M_DEFAULT_IODEVICE_WARN_SLOP_US,
											  M_DEFAULT_IODEVICE_FAIL_SLOP_US,
											  M_MISSED_EXECUTION_DROP);
		
	}
	else {
		mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Warning! Could not start EyeLink! Connection Lost!! (StartIO)");
		Eyelink::stopDeviceIO();
	}
	
	stopped = false;
		
	return true;
}


bool Eyelink::stopDeviceIO() {
	if(!stopped) {
		if (schedule_node) {
			schedule_node->cancel();
			schedule_node.reset();
		}
		
		if(eyelink_is_connected()) {
			//eyelink stop recording
			stop_recording();
			//eyelink has to close data file
			eyecmd_printf((char*)("close_data_file"));
			//go to eyelink offline mode
			set_offline_mode();
		}
		else mwarning(M_IODEVICE_MESSAGE_DOMAIN, "Warning! Could not stop EyeLink! Connection Lost!! (StopIO)");
		
		e_time -> setValue( 0.0f );
		e_rx -> setValue( 0.0f );
		e_ry -> setValue( 0.0f );
		e_lx -> setValue( 0.0f );
		e_ly -> setValue( 0.0f );
		e_x -> setValue( 0.0f );
		e_y -> setValue( 0.0f );
		e_z -> setValue( 0.0f );
		h_rx -> setValue( 0.0f );
		h_ry -> setValue( 0.0f );
		h_lx -> setValue( 0.0f );
		h_ly -> setValue( 0.0f );
		p_rx -> setValue( 0.0f );
		p_ry -> setValue( 0.0f );
		p_lx -> setValue( 0.0f );
		p_ly -> setValue( 0.0f );
		p_r -> setValue( 0.0f );
		p_l -> setValue( 0.0f );
		
		
		stopped = true;
	}
	
    return stopped;
}



void Eyelink::addChild(std::map<std::string, std::string> parameters,
							mw::ComponentRegistry *reg,
							shared_ptr<mw::Component> child){}
