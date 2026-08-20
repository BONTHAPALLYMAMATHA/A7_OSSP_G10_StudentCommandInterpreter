# Student Command Interpreter

## Mini Shell with Academic Utilities

## 1. Project Description

Student Command Interpreter is a lightweight Linux-based mini shell developed in C.

It provides a command-line interface where users can execute basic Linux commands and custom file management commands.

The project demonstrates important operating system concepts such as command parsing, process creation, program execution, and parent-child process synchronization.

## 2. Features

- Show current directory using `pwd`
- List files using `ls`
- Create directories using `mkdir`
- Create files using `touch`
- Display file contents using `cat`
- Delete files using `rm`
- Change directory using `cd`
- Clear terminal using `clear`
- Display available commands using `help`
- Exit using `exit`
- Execute Linux commands such as `date`, `whoami`, and `uname`
- Error handling for invalid commands and arguments

## 3. Technologies Used

- C Programming Language
- Linux / Ubuntu
- GCC Compiler
- Make
- Linux System Calls

## 4. Operating System Concepts

### Command Parsing
User input is divided into commands and arguments using spaces.

### Process Creation
`fork()` creates a child process to execute external Linux commands.

### Program Execution
`execvp()` executes the requested Linux program.

### Parent-Child Synchronization
`waitpid()` allows the parent process to wait for the child process to finish.

### File Handling
File operations are implemented using functions such as `open()`, `fopen()`, `remove()`, and `mkdir()`.

## 5. Supported Commands

| Command | Description |
|---------|-------------|
| `pwd` | Show current directory |
| `ls` | List files |
| `mkdir <name>` | Create directory |
| `touch <name>` | Create file |
| `cat <file>` | Display file contents |
| `rm <file>` | Delete file |
| `cd <directory>` | Change directory |
| `clear` | Clear terminal |
| `help` | Show available commands |
| `date` | Display date and time |
| `whoami` | Display current user |
| `uname` | Display system information |
| `exit` | Exit the shell |

## 6. Project Structure

```text
StudentCommandInterpreter/
│
├── src/
│   └── main.c
│
├── include/
│   └── shell.h
│
├── docs/
├── tests/
├── screenshots/
├── bin/
│   └── student_shell
│
├── Makefile
├── README.md
└── .gitignore
