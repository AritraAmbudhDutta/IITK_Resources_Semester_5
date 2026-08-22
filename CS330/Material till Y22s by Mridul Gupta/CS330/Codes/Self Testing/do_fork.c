// Function to handle process creation (fork)
PCB *do_fork(PCB *parent) {
    
    PCB *child = create_new_pcb(parent); // Create a new Process Control Block (PCB) for the child
    add_child(parent, child);   // Add the child to the parent's list of children
    adjust_child_pid(child);    // Adjust the child's PID and allocate a kernel stack for the child
    alloc_kstack(child);
    copy_kstack(child, parent);      // Copy the parent's kernel stack to the child
    save_exec_state(child);     // Save the execution state of the child process
    add_to_ready_queue(child);      // Add the child process to the ready queue for scheduling
    return child->pid;  // Return the PID of the newly created child process
}
