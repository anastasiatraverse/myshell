#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include <algorithm>
#include "modules.h"

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

void stdout_stderr_to_file(const std::vector<std::string> command_v){
    std::cout<<"stdout_stderr_to_file"<<std::endl;
}

void write_to_file(std::string result, std::string file_name){
    std::ofstream outfile;
    outfile.open(file_name);
    outfile<<result;
    outfile.close();
}

std::string exec(std::vector<std::string> command_v){
    std::string command=command_v[0];

   	if(command == "myls" ||command == "mycat"||command == "myrm"||command == "mymkdir"||command == "mymv"||command == "mycp"||command == "mygrep")
        command = "./"+command;
    const char *cmd= command.c_str();

    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void conveyor(std::vector<std::string>command_v){
	std::string result = "";
	std::string temp_result;
	std::vector<std::string> command_order;

	int p[2];
	if(pipe(p) == -1){
		std::cerr<<"ERORR: pipes creating failed!"<<std::endl;
		exit(EXIT_FAILURE);
	}
	pid_t pid;
	int status;
	if((pid = fork())<0){
		std::cerr<<"ERORR: failer to fork()"<<std::endl;
		exit(EXIT_FAILURE);
	}else if (pid > 0){
		close(p[1]);
		if ((pid = waitpid(pid, &status, 0)) < 0) {
            std::cerr << "ERROR: waitpid() error" << std::endl;
            exit(EXIT_FAILURE);
        }

        char buffer[128];
	    FILE *pipes_in = fdopen(p[0], "r");
	    do {
	    	size_t count = fread(buffer, sizeof(char), 128, pipes_in);
            if (ferror(pipes_in)) {
                break;
            }if (count >= 0)
                buffer[count] = '\0';
                std::cout<<buffer<<std::endl;
        }while (!feof(pipes_in));
	}else{
		close(p[0]);
		dup2(p[1], STDOUT_FILENO);
		std::cerr << "ERORR: Failed to exec!" << std::endl;
        exit(EXIT_FAILURE);
	}



}

void stdout_to_file(const std::vector<std::string> command_v){
	std::string result = exec(command_v);
	write_to_file(result, command_v[2]);
}

void stderr_to_file(const std::vector<std::string> command_v){
    std::cout<<"stderr_to_file"<<std::endl;
    std::string file_name;
}

std::string command_comments(const std::string command_v){
   std::string out = "";
   bool comment = false;
   for(auto el:command_v){
        if(el=='#')comment=true;
        if (!comment)
        {
            out+=el;
        }
   }
   return out;
}

void built_in_command(std::vector<std::string> &command_v){
    if(command_v[0] == "-h" || command_v[0]=="--help")help_function();
    else if(command_v[0] == "mexit")mexit_func(command_v);
    else if(command_v[0] == "mpwd")mpwd_func(command_v);
    else if(command_v[0] == "merrno")merrno_func(command_v);
    else if(command_v[0] == "mcd")mcd_func(command_v);
    else if(command_v[0] == "mexport")mexport_func(command_v);
    else if(command_v[0] == "mecho")mecho_func(command_v);
    else if(command_v[0] == "myls")run_prg2(command_v);
    else if(command_v[0] == "mycat")run_prg2(command_v);
    else if(command_v[0] == "mycp")run_prg2(command_v);
    else if(command_v[0] == "mymv")run_prg2(command_v);
    else if(command_v[0] == "myrm") run_prg2(command_v);
    else if(command_v[0] == "mymkdir")run_prg2(command_v);
    else if(command_v[0] == "mygrep")run_prg2(command_v);
}

void stdin_fromfile(const std::vector<std::string> command_v){
    std::string comm_name = command_v[0];
    std::string file_path = command_v[2];
    std::cout<<"Command name: "<<comm_name<<std::endl;
    std::cout<<"File: "<<file_path<<std::endl;

    std::ifstream inFile;
    inFile.open(file_path);
    std::string line;
    std::string out = "";
    while (inFile >> line) {
        out+=line;
    }

    std::vector<std::string> v;
    v.push_back(comm_name);
    v.push_back(out);
    built_in_command(v);
}

void check_for_command_to_var(std::string command){
	// std::string command = command_v[0];
	std::string var_name="";
	bool shell_command = false;
	std::string run = "";
	for(int i =0;i<command.size();i++){

		if(shell_command && command[i]!='=' && command[i]!='(' && command[i]!='$' && command[i]!=')'){
			run+=command[i];
		}else if((!shell_command) && command[i]!='=' && command[i]!='(' && command[i]!='$' && command[i]!=')'){
			var_name+=command[i];
		}
		if(command[i]=='=' && command[i+1]=='$' && command[i+2]=='('){
			shell_command = true;
		}
	}
	if(run.size()>0){
		std::vector<std::string> v;
		v.push_back(run);
		std::string result = exec(v);
		std::cout<<"VAR name: "<<var_name<<std::endl;
		std::cout<<"Output: "<<result<<std::endl;
		std::cout<<"Save to local var"<<std::endl;
		std::string v1 = var_name+"="+result;
		local_var.push_back(v1);
	}
}

void check_command(std::vector<std::string> command_v){
	if(find_el(command_v, "|")) conveyor(command_v);
    else if(find_el(command_v,"&")) run_in_background(command_v);
    else if(find_el(command_v,">")) stdout_to_file(command_v);
    else if(find_el(command_v, "2>")) stdout_to_file(command_v);
    else if(find_el(command_v, "<")) stdin_fromfile(command_v);
    else if(find_el(command_v, "2>&1")) stdout_to_file(command_v);
    else built_in_command(command_v);

    bool p = check_if_prog(command_v);
    if(p){
    	run_prg2(command_v);
    }
}

std::vector<std::string> parse_command_line(std::string &command){
    std::vector<std::string> command_v;
    split(command, command_v, ' ');
    return command_v;
}

void get_command(std::string &s){
    std::cout << mainpath.string() + " [myshell] $";
    getline(std::cin,s);
    hist_s.push_back(s);
}

int main(int argc, char *argv[]) {

	if (argc>2){
		return -1;
	}

	else if(argc == 2){
		std::string file_name = argv[1];
		std::ifstream script_file(file_name);
		std::string line;
		int res =0;
		while (std::getline(script_file, line)){
			std::cout<<line<<std::endl;
			std::string no_comment = command_comments(line);
			const char *c = no_comment.c_str();
			res = system(c);
		}
		return res;
	}

	else{
		std::string command;
	    boost::filesystem::path full_path(boost::filesystem::current_path());
	    mainpath = full_path;

	    while(true){
	        get_command(command);
	        check_for_command_to_var(command);
	        std::vector<std::string> command_v = parse_command_line(command);
	        check_command(command_v);
	        check_for_var(command_v);
	    }
	    return 0;
	}
    
}