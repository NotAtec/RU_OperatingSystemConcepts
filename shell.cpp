/**
	* Shell framework
	* course Operating Systems
	* Radboud University
	* v22.09.05

	Student names:
	- Jeroen Brinkhorst s1101799
	- Jeske Groenheiden s1093553
*/

/**
 * Hint: in most IDEs (Visual Studio Code, Qt Creator, neovim) you can:
 * - Control-click on a function name to go to the definition
 * - Ctrl-space to auto complete functions and variables
 */

// function/class definitions you are going to use
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vector>
#include <list>
#include <optional>

// although it is good habit, you don't have to type 'std' before many objects by including this line
using namespace std;

struct Command {
  vector<string> parts = {};
};

struct Expression {
  vector<Command> commands;
  string inputFromFile;
  string outputToFile;
  bool background = false;
};

// Parses a string to form a vector of arguments. The separator is a space char (' ').
vector<string> split_string(const string& str, char delimiter = ' ') {
  vector<string> retval;
  for (size_t pos = 0; pos < str.length(); ) {
    // look for the next space
    size_t found = str.find(delimiter, pos);
    // if no space was found, this is the last word
    if (found == string::npos) {
      retval.push_back(str.substr(pos));
      break;
    }
    // filter out consequetive spaces
    if (found != pos)
      retval.push_back(str.substr(pos, found-pos));
    pos = found+1;
  }
  return retval;
}

// wrapper around the C execvp so it can be called with C++ strings (easier to work with)
// always start with the command itself
// DO NOT CHANGE THIS FUNCTION UNDER ANY CIRCUMSTANCE
int execvp(const vector<string>& args) {
  // build argument list
  const char** c_args = new const char*[args.size()+1];
  for (size_t i = 0; i < args.size(); ++i) {
    c_args[i] = args[i].c_str();
  }
  c_args[args.size()] = nullptr;
  // replace current process with new process as specified
  int rc = ::execvp(c_args[0], const_cast<char**>(c_args));
  // if we got this far, there must be an error
  int error = errno;
  // in case of failure, clean up memory (this won't overwrite errno normally, but let's be sure)
  delete[] c_args;
  errno = error;
  return rc;
}

// Executes a command with arguments. In case of failure, returns error code.
int execute_command(const Command& cmd) {
  auto& parts = cmd.parts;
  if (parts.size() == 0)
    return EINVAL;

  int retval = execvp(parts);

  return retval ? errno : 0;
}

void display_prompt() {
  char buffer[512];
  char* dir = getcwd(buffer, sizeof(buffer));
  if (dir) {
    cout << "\e[32m" << dir << "\e[39m"; // the strings starting with '\e' are escape codes, that the terminal application interpets in this case as "set color to green"/"set color to default"
  }
  cout << "$ ";
  flush(cout);
}

string request_command_line(bool showPrompt) {
  if (showPrompt) {
    display_prompt();
  }
  string retval;
  getline(cin, retval);
  return retval;
}

// note: For such a simple shell, there is little need for a full-blown parser (as in an LL or LR capable parser).
// Here, the user input can be parsed using the following approach.
// First, divide the input into the distinct commands (as they can be chained, separated by `|`).
// Next, these commands are parsed separately. The first command is checked for the `<` operator, and the last command for the `>` operator.
Expression parse_command_line(string commandLine) {
  Expression expression;
  vector<string> commands = split_string(commandLine, '|');
  for (size_t i = 0; i < commands.size(); ++i) {
    string& line = commands[i];
    vector<string> args = split_string(line, ' ');
    if (i == commands.size() - 1 && args.size() > 1 && args[args.size()-1] == "&") {
      expression.background = true;
      args.resize(args.size()-1);
    }
    if (i == commands.size() - 1 && args.size() > 2 && args[args.size()-2] == ">") {
      expression.outputToFile = args[args.size()-1];
      args.resize(args.size()-2);
    }
    if (i == 0 && args.size() > 2 && args[args.size()-2] == "<") {
      expression.inputFromFile = args[args.size()-1];
      args.resize(args.size()-2);
    }
    expression.commands.push_back({args});
  }
  return expression;
}

// Wrapper for chdir which handles arguments 
void cd(vector<string> args) {
  if (args.size() == 1) { 
    chdir(getenv("HOME"));
  } else if (args.size() == 2) {
    chdir(args[1].c_str());
  } else {
    cerr << "cd: too many arguments" << endl;
  }
}

int execute_expression(Expression& expression) {
  // Check for empty expression
  if (expression.commands.size() == 0)
    return EINVAL;

  // Handle intern commands (like 'cd' and 'exit')
  if(expression.commands.size() == 1) {
    vector<string> cmd = expression.commands[0].parts;
    
    if(cmd[0] == "cd") {
      cd(cmd);
    } else if (cmd[0] == "exit") {
      exit(0);
    }
  }

  // External commands, executed with fork():
  // Create arrays for pipes & forks
  int size = static_cast<int>(expression.commands.size());
  int pipefds[size - 1][2];
  pid_t children[size];

  // Create pipes
  for(int i = 0; i < (size - 1); i++) {
    if (pipe(pipefds[i]) == -1) {
      perror("pipe");
      return 1;
    }
  }

  // Handle forking & command execution
  for(int i = 0; i < size; i++) {
    children[i] = fork();
    if (children[i] == 0) {
      // Redirect input to pipe or file
      if(i != 0) { 
        dup2(pipefds[i - 1][0], STDIN_FILENO);
      } else if (!expression.inputFromFile.empty()) { // If it's the first command, see if theres a file to use as input
        int inputFd = open(expression.inputFromFile.c_str(), O_RDONLY);
        dup2(inputFd, STDIN_FILENO); // Redirect the file to the input stream.
        close(inputFd);
      }

      if (expression.background) { // If backgrounded, don't allow input from stdin
        fclose(stdin);
      }

      // Redirect output to pipe or file
      if (i != (size - 1)) {
        dup2(pipefds[i][1], STDOUT_FILENO);
      } else if (!expression.outputToFile.empty()) { // If it's the last command, see if theres a file to use as output
        int outputFd = open(expression.outputToFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(outputFd, STDOUT_FILENO); // Redirect the output to file.
        close(outputFd);
      }

      // Close all pipe ends in the child process
      for (int j = 0; j < (size - 1); j++) {
        close(pipefds[j][0]);
        close(pipefds[j][1]);
      }
      

      // Execute correct command, unless the command is an internal command, in which case just pass over and abort.
      if (!(expression.commands[i].parts[0] == "cd" || expression.commands[i].parts[0] == "exit")) {
        execute_command(expression.commands[i]);
        cerr << "Error: Command '" << expression.commands[i].parts[0] << "' not found" << endl; 
      }
      
      abort();
    }
  }

  // Parent process closes all pipe ends
  for(int i = 0; i < (size - 1); i++) {
    close(pipefds[i][1]);
    close(pipefds[i][0]);
  }

  // Wait for all child processes to finish, except if running in background
  if (!expression.background) {
    for(int i = 0; i < size; i++) {
      waitpid(children[i], nullptr, 0);
    }
  }

  return 0;
}

// framework for executing "date | tail -c 5" using raw commands
// two processes are created, and connected to each other
int step1(bool showPrompt) {
  // create communication channel shared between the two processes
  // ...
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    return 1;
  }

  
  pid_t child1 = fork();
  if (child1 == 0) {
    // redirect standard output (STDOUT_FILENO) to the input of the shared communication channel
    dup2(pipefd[1], STDOUT_FILENO);
    // free non used resources (why?)
    close(pipefd[0]);

    Command cmd = {{string("date")}};
    execute_command(cmd);
    // display nice warning that the executable could not be found
    abort(); // if the executable is not found, we should abort. (why?)
  }

  pid_t child2 = fork();
  if (child2 == 0) {
    // redirect the output of the shared communication channel to the standard input (STDIN_FILENO).
    dup2(pipefd[0], STDIN_FILENO);
    // free non used resources (why?)
    close(pipefd[1]);

    Command cmd = {{string("tail"), string("-c"), string("5")}};
    execute_command(cmd);
    abort(); // if the executable is not found, we should abort. (why?)
  }

  close(pipefd[1]);
  close(pipefd[0]);

  // free non used resources (why?)
  // wait on child processes to finish (why both?)
  waitpid(child1, nullptr, 0);
  waitpid(child2, nullptr, 0);
  return 0;
}

int shell(bool showPrompt) {
  //* <- remove one '/' in front of the other '/' to switch from the normal code to step1 code
  while (cin.good()) { // When stdin gets closed, it'll enter fail state and
                       // exit this loop && exit.
    string commandLine = request_command_line(showPrompt);
    Expression expression = parse_command_line(commandLine);
    if (!cin.good()) // Extra check to exit before execution if needed.
        break;
    int rc = execute_expression(expression);
    if (rc != 0 && rc != EINVAL)
      cerr << strerror(rc) << endl;
  }
  cout << endl;

  return 0;
  /*/
  // return step1(showPrompt);
  //*/
}
