#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

bool help = false;
bool f = false;
bool R = false;

void help_func(){
	std::cout<<"\n";
	std::cout<<"myrm [-h|--help] [-f] [-R] -- Remove files or directory"<<std::endl;
	std::cout<<"\n";
	std::cout<<"-h|--help   --  display help information"<<std::endl;
	std::cout<<"-f          --  delete files without asking for confirmation"<<std::endl;
	std::cout<<"-R          --  remove directoies"<<std::endl;
	std::cout<<"\n";
}

bool find_el(std::vector<std::string> inv, std::string el){
	std::vector<std::string>::iterator it =std::find(inv.begin(), inv.end(), el);
	if (it != inv.end()) return true; 
	else return false;
}

bool confirm(){
	char conf;
	std::cout<<"\n";
	std::cout<<"Please confirm file deletion. Y[es]/N[o]/A[ll]/C[ancel]: ";
	std::cin>>conf;
	char conf_l = tolower(conf);
	if(conf_l == 'y')return true;
	else if(conf_l == 'n')return false;
	else if(conf_l == 'c')exit(EXIT_FAILURE);
	else{
		std::cout<<"You enter incorrect symbols, please try again."<<std::endl;
		confirm();
	}
	return false;
}

bool check_dir(std::string path){
	boost::filesystem::path dir(path);
	if(boost::filesystem::exists(dir) && !boost::filesystem::is_regular_file(dir)){
		return true;
	}	
	return false;
}

void dir_entry(const boost::filesystem::path dir_path, std::vector<std::string> &v){
	for(auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(dir_path), {})){
   		std::ostringstream oss;
   		oss << entry;
   		std::string path = oss.str();
   		v.push_back(path);
   		std::cout<<entry<<std::endl;
   	}
}

void iter_remove_dir(const boost::filesystem::path dir_path){
	std::vector<std::string> v;
	dir_entry(dir_path, v);
	if(v.size()==0)
		boost::filesystem::remove(dir_path);
	else{
		for(auto el:v){
	   		if(check_dir(el)){
	   			boost::filesystem::path dir_path(el);
	   			iter_remove_dir(dir_path);
	   		}else{
	   			boost::filesystem::path file_path(el);
	   			boost::filesystem::remove(file_path);
	   		}
   		}
	}
}

void remove_files_dir(std::string path){
	if(!f){
		bool conf = confirm();
		 
		if(conf){
			boost::filesystem::path dir(path);
			if(boost::filesystem::exists(dir)){
				iter_remove_dir(dir);
				std::cout<<"Directory '"<< path <<"' deleted"<<std::endl;
			}
			else std::cerr<<"ERROR: File/directory not exists in current directory"<<std::endl;
		}
		else{
			std::cout<<"File couldn't be deleted"<<std::endl;
			exit(EXIT_SUCCESS);
		}

	}
	else{
		boost::filesystem::path file(path); 
		if(boost::filesystem::exists(file)){
			boost::filesystem::remove(file);
			std::cout<<"File/directory '"<< path <<"' deleted"<<std::endl;
		}
		else std::cerr<<"ERROR: File/directory not exists in current directory"<<std::endl;
	}
}

void remove_files(std::string path){
	if(!f){
		bool conf = confirm();
		 
		if(conf){
			boost::filesystem::wpath file(path);
			if(boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)){
				boost::filesystem::remove(file);
				std::cout<<"File '"<< path <<"' deleted"<<std::endl;
			}
			else if (!boost::filesystem::is_regular_file(file)) std::cerr<<"ERROR: It's impossible to remove directory. You should use flag '-R' "<<std::endl;
			else std::cerr<<"ERROR: File not exists in current directory"<<std::endl;
		}
		else{
			std::cout<<"File couldn't be deleted"<<std::endl;
			exit(EXIT_SUCCESS);
		}
	}
	else{
		boost::filesystem::wpath file(path); 
		if(boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)){
			boost::filesystem::remove(file);
			std::cout<<"File '"<< path <<"' deleted"<<std::endl;
		}
		else if(!boost::filesystem::is_regular_file(file)) std::cerr<<"ERROR: It's impossible to remove directory. You should use flag '-R' "<<std::endl;
		else std::cerr<<"ERROR: File not exists in current directory"<<std::endl;
	}
}

void check_command(const std::vector<std::string> inv){
	bool h1 = find_el(inv, "-h");
	bool h2 = find_el(inv, "--help");
	if ( h1||h2 ) help = true;
	if (find_el(inv, "-f")) f = true;
	if (find_el(inv, "-R")) R = true;

	if(help){
		help_func();
		exit(EXIT_SUCCESS);
	}

	if(f && R && inv.size()>2){
		remove_files_dir(inv[2]);
		exit(EXIT_SUCCESS);
	}

	if(f && inv.size()>1){
		remove_files(inv[1]);
		exit(EXIT_SUCCESS);
	}

	if(R && inv.size()>1){
		remove_files_dir(inv[1]);
		exit(EXIT_SUCCESS);
	}

	if(!R && !f){
		remove_files(inv[0]);
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char* argv[]){
	std::vector<std::string> inv;
	for(int i=1;i<argc;i++){
		inv.push_back(argv[i]);
	}	
	check_command(inv);
	return 0;
}