#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <boost/algorithm/string.hpp>

std::string filter_string;
bool ignore_case = false;
bool invert = false;
bool regex = false;
bool file = false;

void split(std::string str, std::vector<std::string> &v, char split_char){
    std::string word = "";
    for (auto x : str){
        if (x == split_char){
            v.push_back(word);
            word = "";
        }else
            word = word + x;
    }
    v.push_back(word);
}

bool find_el(std::vector<std::string> inv, std::string el){
	std::vector<std::string>::iterator it =std::find(inv.begin(), inv.end(), el);
	if (it != inv.end()) return true; 
	else return false;
}

void help_func(){
	std::cout<<"------------------HELP INFO----------------------------------------------------"<<std::endl;
	std::cout<<"\n";
	std::cout<<"-h | --help                         - Display help info"<<std::endl;
	std::cout<<"-v | --invert-match                 - Display lines where string not happened"<<std::endl;
	std::cout<<"-i | --ignore-case                  - Ignore case in texts lines "<<std::endl;
	std::cout<<"--file=<filename>                   - Enter file name for command grep"<<std::endl;
	std::cout<<"--regexp=’<regexpression>’ | string - Enter experission for filtering text"<<std::endl;
	std::cout<<"\n";
}

void read_file(const std::string file_name, std::vector<std::string> &lines){
	std::ifstream myfile (file_name);
	std::string line;
	if (myfile.is_open()){
		while ( getline (myfile,line) ){
	      lines.push_back(line);
	    }
	    myfile.close();
	}
	else std::cerr << "Unable to open file"<<std::endl;
}

void all_lower(std::vector<std::string> &lines){
	for(int i =0 ; i<lines.size();i++){
		boost::algorithm::to_lower(lines[i]);
	}
}

void inver_filter_lines(std::vector<std::string> lines){}
void filter_lines(std::vector<std::string> lines){}

void check_command(std::vector<std::string> &command_v){
	if(file){
		std::vector<std::string> file_v;
		for(auto element:command_v)
			if(element.find("--file") != std::string::npos){
				split(element,file_v, '=');
			}
		std::vector<std::string> lines;
		read_file(file_v[1], lines);
		if(ignore_case) all_lower(lines);
	}
}

void parse_command_line(std::vector<std::string> &command_v){
	if(find_el(command_v, "-h") || find_el(command_v, "--help")) help_func();
	if(find_el(command_v, "-i") || find_el(command_v, "--ignore-case")) ignore_case = true;
	if(find_el(command_v, "-v") || find_el(command_v, "--invert-match")) invert = true;
	for(auto element:command_v){
		if(element.find("--regexp") != std::string::npos) regex = true;
		else if(element.find("--file") != std::string::npos) file = true;
		else filter_string = element;
	}
}

int main(int argc, char * argv[]){
	std::vector<std::string> command_v;
	for(int i=1;i<argc;i++)
		command_v.push_back(argv[i]);

	parse_command_line(command_v);
	check_command(command_v);

	return 0;
}