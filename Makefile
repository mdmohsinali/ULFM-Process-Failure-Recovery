 #***********************************************************************************  
 # File       : Makefile                                                            *
 # Description: Contains instructions to compile, link, and execute several files   *
 #              to reconstruct faulty communicator by performing in-order failed    *
 #              process replacement, and other supporting functions to achieve this * 
 # Author     : Md Mohsin Ali (md.ali<AT>anu.edu.au)                                *
 # Created    : August 2013                                                         *
 # Updated    : December 2015                                                       *
 # Help       : See README file                                                     *
 #***********************************************************************************

.SUFFIXES:
.PRECIOUS: %.o

HDRS=FailureRecovery.h
OBJS=FailureRecovery.o
PROG=ProcsFailureRecovery
CCFLAGS=-g -O2 -fopenmp

all: clean $(PROG) 
%: %.o $(OBJS)
	mpic++ -o $* $*.o $(OBJS) -lgomp
%.o: %.cpp $(HDRS)
	mpic++ -Wall -Wno-deprecated-declarations $(CCFLAGS) -c $*.cpp
run:
	mpirun -np 8 -am ft-enable-mpi --mca coll_ftbasic_method 1 \
		-mca shmem_mmap_enable_nfs_warning 0 ./ProcsFailureRecovery 2
clean:
	rm -f *.o $(PROG)
