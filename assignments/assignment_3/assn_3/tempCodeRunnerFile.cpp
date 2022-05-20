    munmap(flag, sizeof(int)); // clean up space
    munmap(childs_pid, sizeof(int));
    munmap(flagchar, 10); // clean up space
    munmap(filetofind, 100);