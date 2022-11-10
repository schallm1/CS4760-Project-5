# CS4760-Project-5

To run program simply run these commands:

make clean
make
./oss

------------------------------------------

This project forks user processes at random simulated times.
In shared memory, resource classes populated by instances of those resources are initialized.
From there, the called child processes will request access to those resources, one at a time via a sempahore, at random simulated times as well.
In addition to this, if those resources are uncontrolled, the child processes will register themselves at the top of the queue or if not somewhere else in the queue, and the oss will grant them access to said resource. The child processes will never make more requests than their cap.
All while this is happening, the child processes will check every 250 simulated miliseconds whether to terminate, if rng determines such, they will. Before terminating, they will release all assigned resources.
If they do not terminate prematurely, the child processes will release their shared resources anyway, update the system clock, and print a normal termination message.
After the conclusion of all of the child processes, the oss will then cleanup all shared memory and semaphores before terminating itself.
