#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include "modules.h"

void stdout_to_file(std::vector<std::string> command_v){
    std::cout<<"stdout_to_file"<<std::endl;
    std::string file_name=command_v[2];
}

void stderr_to_file(std::vector<std::string> command_v){
    std::cout<<"stderr_to_file"<<std::endl;
    std::string file_name;
}

std::string command_comments(std::string command_v){
   std::string out = "";
   bool comment = false;
   for(auto el:command_v){
        if(el=='#')comment=true;
        if (!comment)
        {
            /* code */
            out+=el;
        }
   }
   return out;
}


void check_command(std::vector<std::string> &command_v){
    // stdout_to_file(command_v);
    if(command_v[0] == "-h" || command_v[0]=="--help")std::cout<<"call help function"<<std::endl;
    if(command_v[0] == "mexit")mexit_func(command_v);
    if(command_v[0] == "mpwd")mpwd_func(command_v);
    if(command_v[0] == "merrno")merrno_func(command_v);
    if(command_v[0] == "mcd")mcd_func(command_v);
    if(command_v[0] == "mexport")mexport_func(command_v);
    if(command_v[0] == "mecho")mecho_func(command_v);
    if(command_v[0] == "myls")run_prg2(command_v);
    if(command_v[0] == "mycat")run_prg2(command_v);
    if(command_v[0] == "mycp")run_prg2(command_v);
    if(command_v[0] == "mymv")run_prg2(command_v);
    if(command_v[0] == "myrm") run_prg2(command_v);
    if(command_v[0] == "mymkdir")run_prg2(command_v);
    if(command_v[0] == "mygrep")run_prg2(command_v);
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

int main() {
    std::string command;
    boost::filesystem::path full_path(boost::filesystem::current_path());
    mainpath = full_path;
    while(true){
        get_command(command);
        std::vector<std::string> command_v = parse_command_line(command);
        check_command(command_v);
        check_for_var(command_v);
    }
    return 0;
}