#include <iostream>
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <boost/filesystem.hpp>

bool p = false;

void help_function(){
	std::cout<<"-------mymkdir [-h|--help] [-p]  <dirname> – create directory--------"<<std::endl;
	std::cout<<"\n";
	std::cout<<"-h|--help   -- display help information;"<<std::endl;
	std::cout<<"-p          -- parents or path, will also create all directories leading up to the given directory that do not exist already;"<<std::endl;
	std::cout<<"<dirname>   -- name of directory that has to be created"<<std::endl;
}

void split_by_del(std::string s_in, std::vector<std::string> &v_out){
	std::stringstream ss(s_in);
    std::string token;
    char delim ='/';
    while (std::getline(ss, token, delim)) {
        v_out.push_back(token);
    }
}

void make_dir_p(std::string dir_path){
	std::vector<std::string> dir_vec;
	split_by_del(dir_path, dir_vec);

	std::string dstFolder = "";

	for(int i=0;i<dir_vec.size();i++){
		dstFolder += dir_vec[i];
		std::string path(dstFolder);
		auto a = boost::filesystem::create_directory(path);
		dstFolder +="/";
	}
	
	std::cout<<"PATH CREATED: "<<dir_path<<std::endl;
	exit(EXIT_SUCCESS);
}

void make_dir(std::string dir_path){
	std::string dstFolder = dir_path;
	std::string path(dstFolder);
	auto a = boost::filesystem::create_directory(path);
	if(a == 1){
		std::cout<<"DIRECTORY '"<<dir_path<<"' CREATED SUCCESFULLY!"<<std::endl;
	}
	else{
		std::cerr<<"ERROR: directory already exists"<<std::endl;
		exit(EXIT_FAILURE);
	}
}

void check_command(const std::vector<std::string> in_vector){
	if(in_vector[0] == "-h"){
		help_function();
		exit(EXIT_SUCCESS);
	}
	else if(in_vector[0]=="-p") p = true;
	
	if(p){
		make_dir_p(in_vector[1]);
		exit(EXIT_SUCCESS);
	}else{
		make_dir(in_vector[0]);
		exit(EXIT_SUCCESS);
	}
}	

int main(int argc, char* argv[]){
	std::vector<std::string> in_vector;
    for(int i=1;i<argc;i++){
    	in_vector.push_back(argv[i]);
    }
    check_command(in_vector);
	return 0;
}