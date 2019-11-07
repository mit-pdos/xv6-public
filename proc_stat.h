//Proc stat structure addition:

struct pstat{
  int pid;  //pid of processes
  int run_time; //Run time of the process
  // int num_runs; // Number of times the process is executed
  // int current_queue;  //Current queue: Defined only for MUlti-level feedback queue
  // int ticks[5];   // Number of ticks that the process has recieved at each priority queue; -- Defined only for MLFQ
};
