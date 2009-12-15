#
#	Makefile
#   boeingDemo
#
#	Created by Iowa State University on 08/18/09.
#	Copyright 2009 Iowa State University. All rights reserved.
#	
#

# Set the shell to sh
SHELL = /bin/sh

# Specify the precompiled header
precompiled_header = osg.h.gch

# Specify the objects to be generated from source files
objects =		BDScene.o \
				DeviceInputController.o \
				JugglerInterface.o \
				mainJuggler.o \
				LightsGroup.o
			
aq_objects =	KVReflector.o \
				String.o

# Specify the application binary
application = Gluggler

# Set the source directory
src_dir = ../src
aq_dir  = $(src_dir)/aq

# Setup the targets
all: $(precompiled_header) $(objects) $(aq_objects) $(application)

# Setup the VR-Juggler environment variables
# - Adds values to EXTRA_INCLUDES and EXTRA_LIBS
DZR_BASE_DIR = $(shell flagpoll doozer --get-prefix)
include $(DZR_BASE_DIR)/ext/vrjuggler/dzr.vrjuggler.mk
include $(DZR_BASE_DIR)/ext/vrjuggler/dzr.vrjuggler.glapp.mk

# Setup the OSG and FMOD includes and libs
EXTRA_INCLUDES += -I$(OSG_HOME)/include -I$(FMOD_HOME)/include $(VRJ_CXX_FLAGS) -I$(src_dir)
EXTRA_LIBS += -L$(OSG_HOME)/lib64 -losg -losgDB -losgGA -losgUtil -lOpenThreads -losgText -losgShadow -L$(FMOD_HOME)/lib64 -lfmodex64 -lfmodex64L -lfmodexp64 

# Generate the gch for the precompiled header
$(precompiled_header): $(src_dir)/osg.h
	g++ $(EXTRA_INCLUDES) -c $<

# Generate all the object files for all the source files
$(objects): %.o: $(src_dir)/%.cpp
	g++ -include $(src_dir)/osg.h $(EXTRA_INCLUDES) -c $< -o $@
	
# Generate all the object files for all the amoniq source files
$(aq_objects): %.o: $(aq_dir)/%.cpp
	g++ -include $(src_dir)/osg.h $(EXTRA_INCLUDES) -c $< -o $@

# Link the application together
$(application): $(objects) $(aq_objects)
	g++ $(objects) $(aq_objects) $(EXTRA_LIBS) $(LIBS) -o $@ 

# Methods for cleanup
# - Use clean_all when you need to regenerate the gch for the pre-compiled header
clean:
	rm -f *.o *core core.*
	rm -rf ./ii_files
	rm -f $(application)

clean_all:
	rm -f osg.h.gch *.o *core core.*
	rm -rf ./ii_files
	rm -f $(application)


