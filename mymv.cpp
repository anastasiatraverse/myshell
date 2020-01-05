#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

bool help = false;
bool f = false;

int find_el(std::vector<std::string> &commands, const std::string el){
	std::vector<std::string>::iterator it = std::find(commands.begin(), commands.end(), el);
	if (it != commands.end()){
		commands.erase(commands.begin()+std::distance(commands.begin(), it));
		return 0;
	}else{
		return -1;
	}
}

bool rewrite(){
	std::string user_enter;
	std::cout<<"Do you want rewrite file? Y[es]/N[o]/C[ancel]"<<std::endl;
	std::cin>>user_enter;
	std::transform(user_enter.begin(), user_enter.end(), user_enter.begin(),
    [](unsigned char c){ return std::tolower(c); });
	
	if(user_enter == "y"){
		return true;
	}
	else if (user_enter == "n"){
		return false;
	}
	else if(user_enter == "c"){
		return false;
	}
	else{
		rewrite();
	}
	return false;
}

void rename_file(std::string file1, std::string file2){
	int n1 = file1.length();
	char oldname[n1 + 1];  
	strcpy(oldname, file1.c_str());

	std::cout<<"OLD name: "<<file1<<std::endl;
	std::cout<<"NEW name: "<<file2<<std::endl;

	int n2 = file2.length();
	char newname[n2 + 1];  
	strcpy(newname, file2.c_str()); 

	int result= rename(oldname, newname);
	if ( result == 0 ){
		std::cout<<"File succesfully rename"<<std::endl;
	}
	else{
		std::cerr<<"ERROR: some error happened"<<std::endl;
	}
}

bool check_dir(const std::string obj){
	boost::filesystem::path fpath(obj);
	if (boost::filesystem::exists(fpath) && boost::filesystem::is_directory(fpath))
		return true;
	return false;
}

void create_dir(const std::string dir){
	std::string dpath(dir);
	auto a = boost::filesystem::create_directory(dpath);
}

void copy_text(const std::string f1, const std::string f2){
	//file1
	std::string cstr = "";
	std::string line;
	std::ifstream file1(f1);
	if (file1.is_open()){
		while(std::getline(file1, line)){
			cstr += line + "\n";
		}
		file1.close();
	}

	//file2
	std::ofstream file2;
	file2.open(f2, std::ios_base::app | std::ios_base::out);
	file2<<cstr;
	file2.close();
}

void replace_todir( std::vector<std::string> commands, const std::string dir){
	if (!check_dir(dir))
		create_dir(dir);
	for(int i=0;i<commands.size()-1;i++){
		if (!f){
			boost::filesystem::path fpath(dir+"/"+commands[i]);
			if(boost::filesystem::exists(fpath)){
				bool user_reply = rewrite();
				if (user_reply){
					copy_text(commands[i], dir+"/"+commands[i]);
					boost::filesystem::remove(commands[i]);
				}else{
					exit(EXIT_FAILURE);
				}
			}
		}else{
			copy_text(commands[i], dir+"/"+commands[i]);
			boost::filesystem::remove(commands[i]);	
		}
		
	}
}


void help_func(){
	std::cout<<"HELP FUNCTION"<<std::endl;
}

int main(int argc, char * argv[]){
	std::vector<std::string> commands;
	for(int i = 1;  i<argc;i++)
		commands.push_back(argv[i]);

	//check command line 
	if (commands.size()>0){
		if (find_el(commands, "-h") == 0 || find_el(commands, "--help") == 0){
			help = true;
		}
		if (find_el(commands, "-f") == 0){
			f = true;
		}
	}
	//first var 	
	if(commands.size()==2){
		std::cout<<"VAR1"<<std::endl;
	}else if (commands.size()> 2){
		std::string dir = commands[commands.size()-1];
		replace_todir(commands, dir);
	}
	return 0;
}