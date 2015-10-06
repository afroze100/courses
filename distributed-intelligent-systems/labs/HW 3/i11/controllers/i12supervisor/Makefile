#
# Makefile for a Webots C controller
#
# if your controller uses several C sources files:
# C_SOURCES = my_controller.c my_clever_algo.c my_gui.c
#
# if your controller needs additional libraries:
# LIBRARIES=-L/path/to/my/library -lmy_library -lmy_other_library
# LIBRARIES=-L/usr/lib32 -m32 # 32 bits compilation
# if some special CFLAGS need to be passed, for example to find include
# files, or set optimization level:
# CFLAGS=-O3 -I/my_library_path/include
# CFLAGS=-O3 #-m32 -L/usr/lib32 # 32 bits compilation
#CFLAGS=-pg
#
# finally, on Windows, if you want that your controller opens up a DOS
# console to display the printf output:
# DOS_CONSOLE=1
WEBOTS_HOME = /usr/local/webots

C_SOURCES = i12supervisor.c i12implement.c i12webots.c
CFLAGS = -O3 -DWEBOTS -I$(WEBOTS_HOME)/include/controller/c # -DQUIET


ifndef WEBOTS_HOME_PATH
ifeq ($(OS),Windows_NT)
nullstring :=
space := $(nullstring) # a string containing a single space
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
else
WEBOTS_HOME_PATH=$(WEBOTS_HOME)
endif
endif


include $(WEBOTS_HOME_PATH)/resources/controllers/Makefile.include
