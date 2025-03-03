1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

    > **Answer**:  In a shell, waitpid() is used to ensure all child processes complete before the shell continues accepting user input. This function blocks the parent process until each child process terminates, preventing the creation of zombie processes. If waitpid() is forgotten, child processes will continue to run in the background without the parent collecting their exit statuses, leading to resource leaks and potentially overwhelming the system with defunct processes.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

    > **Answer**:  After calling dup2() to redirect input and output in a child process, it’s necessary to close the unused pipe ends to avoid resource waste and ensure proper pipe behavior. If the unused pipe ends are left open, it could result in deadlock, incorrect data handling, or even unnecessary resource consumption. Closing unused pipes ensures that data flows as expected between processes without interference or inefficiency.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

    > **Answer**:  The cd command is implemented as a built-in command because it directly affects the current shell process's environment, specifically the working directory. If cd were an external command, it would change the directory in a child process, which would not persist once the child process exits. Implementing cd as a built-in avoids unnecessary overhead and ensures the working directory change is applied to the shell's environment.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

    > **Answer**:  To support an arbitrary number of piped commands, the shell would need to dynamically allocate memory for pipes and processes instead of using fixed-size arrays. This would allow the shell to handle any number of commands, but it introduces challenges like memory management and performance overhead. While dynamic allocation offers flexibility, it requires careful handling of memory to avoid leaks and inefficiencies in large pipelines.
