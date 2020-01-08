#include <iostream>
#include <vector>
#include <string>
#include <fstream>

std::string filter_string;
bool ignore_case = false;
bool invert = false;

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

void read_file(std::string file_name, std::vector<std::string> lines){}
void filter_lines(std::vector<std::string> lines){}
void check_for_file(const std::vector<std::string> command_v){}
void check_for_regex(const std::vector<std::string> command_v){}
void check_for_string(const std::vector<std::string> command_v){}


void parse_command_line(std::vector<std::string> &command_v){
	if(find_el(command_v, "-h") || find_el(command_v, "--help")) help_func();
	if(find_el(command_v, "-i") || find_el(command_v, "--ignore-case")) ignore_case = true;
	if(find_el(command_v, "-v") || find_el(command_v, "--invert-match")) invert = true;
	check_for_file(command_v);
	check_for_regex(command_v);
	check_for_string(command_v);
}

int main(int argc, char * argv[]){
	std::vector<std::string> command_v;
	for(int i=1;i<argc;i++)
		command_v.push_back(argv[i]);

	parse_command_line(command_v);

	return 0;
}