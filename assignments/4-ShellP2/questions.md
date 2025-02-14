1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  You use fork to create a new child process before calling execvp so that the original process remains unaffected. If we called execvp directly, it would replace the current process image, meaning our shell (or any calling program) would cease to exist. By using fork, the parent can retain control, manage multiple child processes, and handle execution errors or job management.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it typically returns -1, indicating that a new process could not be created due to resource limitations (e.g., too many processes running). My implementation checks the return value of fork() and, if it fails, prints an error message (perror("fork failed")) and exits or handles the error gracefully by preventing the command from running.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the command in the directories listed in the PATH environment variable. If the command name does not contain a /, execvp() iterates through each directory in PATH, attempting to execute the command. If the command is found in one of these locations, it is executed; otherwise, an error is returned.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() function ensures that the parent process waits for the child process to complete execution before continuing. Without calling wait(), the child process would become a zombie process upon termination, as its exit status would not be collected by the parent. Additionally, if multiple child processes are spawned without waiting, the system could accumulate many zombie processes, leading to resource exhaustion.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS(status) extracts the exit code of a terminated child process from the status value returned by wait(). This is important because it allows the parent process to check whether the child completed successfully (exit status 0) or encountered an error (nonzero exit status), which can influence further execution or error handling.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() detects quoted arguments (e.g., "some text" or 'some text') and ensures that they are treated as a single argument instead of being split at spaces. This is necessary because many commands require multi-word arguments (e.g., echo "Hello World") to be preserved as a single entity, rather than being interpreted as separate tokens.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  In this assignment, I improved the handling of quoted arguments, added support for command pipelines, and restructured parsing to be more modular. One unexpected challenge was ensuring proper handling of escaped characters inside quoted strings while maintaining correct tokenization. Another difficulty was refactoring the logic to handle nested parsing without breaking previously working cases.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals provide a way for processes to asynchronously communicate with one another or the operating system. They allow processes to be interrupted, terminated, or notified of specific events (e.g., segmentation faults). Unlike other IPC mechanisms (pipes, message queues, shared memory), signals are lightweight, do not require explicit setup, and can be sent between unrelated processes.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  
    SIGKILL (9): Immediately terminates a process and cannot be caught or ignored. Used when a process needs to be forcefully stopped (e.g., kill -9 <pid>).
    
    SIGTERM (15): Politely asks a process to terminate. The process can handle this signal to perform cleanup before exiting (kill <pid> sends SIGTERM by default).
    
    SIGINT (2): Sent when the user presses Ctrl+C. Typically used to stop a running process interactively (e.g., stopping a program in the terminal).

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP (19) immediately pauses a process, putting it in a stopped state. Unlike SIGINT, it cannot be caught, ignored, or handled by the process. This is because it is designed for system-level control, ensuring that a process can be stopped regardless of its state. It can only be resumed using SIGCONT.
