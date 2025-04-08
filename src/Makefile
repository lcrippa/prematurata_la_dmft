MCC=mcc

# LIBRARIES TO BE INCLUDED
LIB_ED=edipack2_cbinding

FLAG = -O2 -D_MPI

#NO NEED TO CHANGE DOWN HERE, only expert mode.
#########################################################################

ifdef LIB_ED
GLOB_INC+=$(shell pkg-config --cflags ${LIB_ED})
GLOB_LIB+=$(shell pkg-config --libs ${LIB_ED})
endif

all: hm_bethe

clean:
	$(RM) *.o hm_bethe

hm_bethe.mc:;

%.mc.o: %.mc
	$(MCC) -c $< -o $@

hm_bethe: hm_bethe.mc.o util.cpp
	$(CXX) $(FLAG)  $^ -o $@ ${GLOB_INC} ${GLOB_LIB}
