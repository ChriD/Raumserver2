# Makefile template for shared library
DTARGET := build/libraumserver.so
STARGET := build/libraumserver.a

# defining the source files for the project
SRCFILES := $(wildcard *.cpp)
SRCFILES += $(wildcard manager/*.cpp)
SRCFILES += $(wildcard request/*.cpp)
SRCFILES += $(wildcard webserver/*.cpp)
SRCFILES += $(wildcard webserver/civetweb/*.cpp)
SRCFILES += $(wildcard webserver/civetweb/*.c)

INCPATH     := -I includes/ -I ../../RaumkernelLib/source/includes/
LIBSPATH    := libs/linux_$(arch)/
LIBSPATHEXE := libs
DLIBSDEF    := -lraumkernel
SLIBSDEF    :=  $(LIBSPATH)libohNetCore.a $(LIBSPATH)libohNetDevices.a $(LIBSPATH)libohNetProxies.a $(LIBSPATH)libraumkernel.a


ifeq ($(arch),) 
  COMPILER          := g++
  ARCHITECTURE      := 
  ARCHCOMPILERFLAGS :=
  ARCHLINKERFLAGS   :=
endif

ifeq ($(arch),X64) 
  COMPILER          := g++-5
  ARCHITECTURE      := -m64
  ARCHCOMPILERFLAGS :=
  ARCHLINKERFLAGS   :=
endif

ifeq ($(arch),ARMV7HF)
  COMPILER          := arm-linux-gnueabihf-g++-5
  ARCHITECTURE      := -march=armv7-a
  ARCHCOMPILERFLAGS := -fasynchronous-unwind-tables -mapcs-frame -mthumb -mtpcs-frame -mtpcs-leaf-frame
  ARCHLINKERFLAGS   :=
endif

ifeq ($(arch),ARMV5TE)
  COMPILER          := arm-linux-gnueabi-g++-5
  ARCHITECTURE      := -march=armv5te -mtune=xscale
  ARCHCOMPILERFLAGS := -fasynchronous-unwind-tables -mapcs-frame -mthumb -mtpcs-frame -mtpcs-leaf-frame
  ARCHLINKERFLAGS   :=
endif 

#arm-none-eabi-gcc - -nostdlib -ggdb -mthumb -mcpu=cortex-m3 -mtpcs-frame -mtpcs-leaf-frame myfile.c


COMPILERFLAGS :=  -std=c++11 -fPIC -Wall -Wno-unknown-pragmas -Wno-unused-parameter -funwind-tables -Wextra -O0 -c -pthread -fno-omit-frame-pointer
#LINKERFLAGS   :=  -pthread -static-libgcc -static-libstdc++ -rdynamic -ldl -L$(LIBSPATH) -Wl,-rpath,$(LIBSPATHEXE) -Wl,-rpath-link,$(LIBSPATHEXE)
#LINKERFLAGS   :=  -pthread -static-libgcc -static-libstdc++ -rdynamic -Wl,--no-as-needed -ldl -L$(LIBSPATH) -Wl,-rpath,$(LIBSPATHEXE)
LINKERFLAGS   :=  -pthread -rdynamic -Wl,--no-as-needed -ldl -L$(LIBSPATH) -Wl,-rpath,$(LIBSPATHEXE)


DEBUG = 
ifeq ($(dbg),1)
  DEBUG = -g
endif

DLINKERFLAGS  	 := $(ARCHITECTURE) $(LINKERFLAGS) $(ARCHLINKERFLAGS) -shared
SLINKERFLAGS  	 := $(ARCHITECTURE) $(LINKERFLAGS) $(ARCHLINKERFLAGS)
DCOMPILERFLAGS   := $(ARCHITECTURE) $(COMPILERFLAGS) $(DEBUG) $(ARCHCOMPILERFLAGS) $(INCPATH)
SCOMPILERFLAGS   := $(ARCHITECTURE) $(COMPILERFLAGS) $(DEBUG) $(ARCHCOMPILERFLAGS) $(INCPATH)

RM  := rm -f 
RMR := rm -rf
AR  := ar

ARFLAGS := rcs

DOBJDIR := build/obj-dyn/
SOBJDIR := build/obj-stat/

DOBJFILES := $(addprefix $(DOBJDIR), $(SRCFILES:.cpp=.o))
SOBJFILES := $(addprefix $(SOBJDIR), $(SRCFILES:.cpp=.o))


.PHONY: all


### when calling make then build static and dynamic target
all: ${DTARGET} ${STARGET}
#all: ${DTARGET}
	

### create dynamic library
$(DTARGET): $(DOBJFILES)	
	$(COMPILER) ${DLINKERFLAGS} -o $@ $^ $(DLIBSDEF) 

$(DOBJDIR)%.o: %.cpp
	@ mkdir -p $(basename $@)
	$(COMPILER) $(DCOMPILERFLAGS) -MMD -c -o $@ $<

-include $(DOBJFILES:.o=.d)


### create static library
$(STARGET): $(SOBJFILES)
	$(AR) $(ARFLAGS) $@ $^ $(SLIBSDEF) 

$(SOBJDIR)%.o: %.cpp
	@ mkdir -p $(basename $@)
	$(COMPILER) $(SCOMPILERFLAGS) -MMD -c -o $@ $<

-include $(SOBJFILES:.o=.d)


### clear all build relevant files 
.PHONY: clean
clean:
	-${RM} ${DTARGET} ${STARGET} ${DOBJFILES} ${SOBJFILES} $(DOBJFILES:.o=.d) 
	-${RMR} ${DOBJDIR} ${SOBJDIR}
