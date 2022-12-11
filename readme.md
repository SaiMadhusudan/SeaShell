# C Shell

## Sai Madhusudan Gunda

## 2021111028

---

## Requirements

- GCC compiler
- Ubuntu

## Starting The terminal

run `make` followed by `./a.out`  

```bash
make
```

```bash
./a.out
```

or  
run `./s.sh` followed by `./a.out`

```bash
./s.sh
```

```bash
./a.out
```

## File

###### Files containing functions required for proper functioning of the shell

- `main.c`
  - Runs the while loop
  - It reads the commands form terminal
  - It has command handler function
  - Makes strings of understandable form for the functions
  - echo function is inbuilt in it

- `history.c`
  - Adds command to history
  - Stores maximum 20 commands
  - It will store in a `.txt` file called `.history.txt` (hidden file)
  - On using history commands prints past 10 commands

### Commands

###### Files containing functions to run a particular command

- `pwd.c`

  - Prints the working directory
  - All data related to directories and address is stored here

- `echo`
  - Prints user input after removing excess spaces/tabs
  - This is implemented in the main.c code

- `cd.c`

  - Navigate to different directories
  - ~/ is defined form the home directory which is defined by the user

- `ls.c`

  - Prints all files/sub-directories in a location
  - Flags implemented: `a` and `l`
  - Multiple directories supported
  - colors are implemented for files , directories and executable files

- `pinfo.c`

  - Prints info about the process (with given PID) (current process if none is given)

- `history.c`

  - It has history command which implements the history command which prints last 10 commands given excluding the repeated ones

- `make`
  - make file is used here

- `456.c`
  - It implements all `fg` and `bg` process related commands

- `discover.c.`
  - It is a custom made command which will recursively search for files and directories
  - flags like `d` , `f` is implemented

- `makefile`
  - contains instructions to complie all the files together

- `s.sh`
  - it is same as make but written in bash


### Other Implementations

- Colors are used whenever required
- promting of shell is printed using functions in `main.c`
- SIGNAL function checks if a background process it done and prints it near the prompt, but it will be still waiting for the current command
-piping in taken care
- functions like sig , jobs , fg , bg 
- signals like ctrl + c , ctrl + z
- ctrl + D to exit file
-  

### Assumptions

- `df` flag is not handled.
- But it can be made easily with appropriate changes
- .//// or any other address forms are printed as given as it does not affect the actual address as it  means the same thing like .// is same as ./
- Error handling is done in most of the cases like cd , ls etc;
- syntax errors are taken care with cases like ;& , ;; etc
- .history.txt is already provided with atleast 0 data
- error handling for discover is little different , if a file is not present it jsut doesn't print any address
- text wrap errors might occur sometimes so keeping the terminal wide open is suggested
## License  
Created by [Sai Madhusudan Gunda](https://www.linkedin.com/in/sai-madhusudan-gunda-45bab4223/)  

This is made as assignment for OSN course