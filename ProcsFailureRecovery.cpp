/***********************************************************************************
 * ULFM-Process-Failure-Recovery Code source file.                                 *
 * Copyright (c) 2015 Md Mohsin Ali. All rights reserved.                          *
 * Licensed under the terms of the BSD License as described in the LICENSE file.   *
 * This comment must be retained in any redistributions of this source file.       *
 ***********************************************************************************/

/*********************************************************************************** 
 * File       : ProcsFailureRecovery.cpp                                           *
 * Description: Testing reconstructed communicator after a single process failure  *
 * Output     : Return the reconstructed communicator, myCommWorld                 *
 * Author     : Md Mohsin Ali (mohsin.ali<AT>anu.edu.au)                           *
 * Created    : August 2013                                                        *
 * Updated    : December 2015                                                      *
 * Help       : See README file                                                    *
 ***********************************************************************************/

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "mpi.h"
#include "mpi-ext.h"
#include "FailureRecovery.h"

// Main function
int main(int argc, char ** argv){
        // Variable declarations
	int i, rank, size, testFails, testRunCount, childFlag = 0, numFails,
            numNodeFails, sumPrevNumNodeFails = 0, verbosity = 0, totFails,
            *failedList, flag;
        MPI_Comm myCommWorld = MPI_COMM_WORLD, parent;

        // MPI initialization and setting global communicator
	MPI_Init(&argc, &argv);
        // Set value on testFails for each rank (rank + communicator size) 
        // to test after the reconstruct
        MPI_Comm_rank(myCommWorld, &rank);
        MPI_Comm_size(myCommWorld, &size);
        testFails = rank+size;
        // Getting parent
        MPI_Comm_get_parent(&parent);

        if (parent == MPI_COMM_NULL) {// parent
           if (argc == 1) {
              testRunCount = 1;
           }
           else {
              testRunCount = atoi(argv[1]);
           }
        }
        else {// child
           testRunCount = 1;
        }
        // Print rank, value on rank, and communicator size before failure for parent
        if (parent == MPI_COMM_NULL) {
           printf("@@@@@@@@@@@@@@@@@@@@@@ BEFORE FAILURE (rank, testFails) "
                  "= (%d, %d), size = %d:\n", rank, testFails, size);
        }
        MPI_Barrier(myCommWorld); // printing information of all processes to finish
        fflush(NULL);

        // For each testRunCount a process is killed
        for (i = 0; i < testRunCount; i++) {
           // Data recovery for failed processes
           if (parent == MPI_COMM_NULL && rank == 2+i) {
              printf("\n===================== Process %d is killed\n\n", 2+i);
              kill(getpid(), SIGKILL);
              fflush(NULL);
           }
           if (parent == MPI_COMM_NULL) { // parent
              totFails = numProcsFails(myCommWorld);
              OMPI_Comm_agree(myCommWorld, &flag);
              sleep(2);
              failedList = (int *) malloc(totFails*sizeof(int));
              // Call communicator reconstruct function for parent
              myCommWorld = communicatorReconstruct(myCommWorld, childFlag, failedList, 
                                        &numFails, &numNodeFails, sumPrevNumNodeFails, 
                                        argc, argv, verbosity);
           }
           else { // child
              // Call communicator reconstruct function for child
              myCommWorld = communicatorReconstruct(myCommWorld, childFlag, NULL,
                                        &numFails, &numNodeFails, sumPrevNumNodeFails, 
                                        argc, argv, verbosity);
           }
           // Change child to parent which becomes child automatically in 
           // communicatorReconstruct
           // Set testFails to -1 to newly created frocesses to test their 
           // values after recoverying the process
           if (parent != MPI_COMM_NULL) {
              testFails = -1;
              parent = MPI_COMM_NULL;
           }          
           // Current status of communicator after failure recovery
           OMPI_Comm_agree(myCommWorld, &flag);
           sleep(5);
           MPI_Comm_rank(myCommWorld, &rank);
           MPI_Comm_size(myCommWorld, &size);
           // Print rank, value on rank, and communicator size after failure recovery
           printf("###################### AFTER FAILURE (rank, testFails) "
                  "= (%d, %d), size = %d:\n", rank, testFails, size);
           MPI_Barrier(myCommWorld);// printing information of all processes to finish
           if (rank == 0) {
              printf("\n");
           }
           fflush(NULL);
        }
       
        // Finalize MPI
	MPI_Finalize();
	return 0;
}
