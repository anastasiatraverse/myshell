#include <iostream>
#include <vector>
#include <string>

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
}

void parse_command_line(std::vector<std::string> &command_v){
	if(find_el(command_v, "-h") || find_el(command_v, "--help")) help_func();
}

int main(int argc, char * argv[]){
	std::vector<std::string> command_v;
	for(int i=1;i<argc;i++)
		command_v.push_back(argv[i]);

	parse_command_line(command_v);

	return 0;
}