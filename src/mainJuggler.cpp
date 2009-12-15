/*
 *  mainJuggler.cpp
 *  Boeing Demo
 *
 *  Created by WATCH on 8/18/09.
 *  Copyright 2009 Iowa State University All rights reserved.
 *
 */

#include <cstdlib>
#include "JugglerInterface.h"
#include <vrj/Kernel/Kernel.h>

int main(int argc, char* argv[])
{
	// Get the kernel
	vrj::Kernel* kernel = vrj::Kernel::instance();
	
	// Instantiate an instance of the app
	JugglerInterface* application = new JugglerInterface(kernel, argc, argv);
	
	if ( argc <= 1 )
	{
		// display some usage info (holding the user by the hand stuff)
		//  this will probably go away once the kernel becomes separate
		//  and can load application plugins.
		std::cout<<"\n"<<std::flush;
		std::cout<<"\n"<<std::flush;
		
		std::cout << "Usage: " << argv[0]
		<< " vjconfigfile[0] vjconfigfile[1] ... vjconfigfile[n]\n"
		<< std::endl << std::endl;
		
		std::exit(EXIT_FAILURE);
	}
	
	kernel->init(argc, argv);
	
	// Load any config files specified on the command line
	for ( int i = 1; i < argc; ++i )
	{
   		std::cout << "Loading config file named " << argv[i];
		kernel->loadConfigFile(argv[i]);
	}
	
	kernel->start();
	
	kernel->setApplication(application);
	kernel->waitForKernelStop();
	
	delete application;
	
	return EXIT_SUCCESS;
}


