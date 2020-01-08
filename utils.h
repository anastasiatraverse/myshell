std::vector<int> history;
std::vector<std::string> hist_s;
std::vector<std::string> local_var;
std::vector<std::string> global_var;
extern char **environ;
boost::filesystem::path mainpath;
bool background = false;


bool find_el(std::vector<std::string> inv, std::string el){
	std::vector<std::string>::iterator it =std::find(inv.begin(), inv.end(), el);
	if (it != inv.end()) return true; 
	else return false;
}

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

void check_for_var(std::vector<std::string> &command_v){
	bool d = false;
	bool eq = false;
    for(auto el:command_v){
        for (auto x : el){
            if(x == '=')
                eq = true;
            if(x == '$')
            	d = true;
        }
        if(d || eq){
            local_var.push_back(el);
        }
    }
}

void find_var(const std::string var){
	for(auto el:global_var){
        std::vector<std::string> temp_var;
        split(var, temp_var, '$');
        std::string upd_var = temp_var[1];
		std::vector<std::string> v;
		split(el, v, '=');
		if(v[0] == upd_var){
			std::cout<<v[1]<<std::endl;
			history.push_back(0);
		}
	}
}


bool check_if_prog(std::vector<std::string> &command_v){
	for(auto el:command_v){
		for(int i=0;i<el.length()-1;i++){
			if(el[i]=='.' && el[i+1]=='/'){
				return true;
			}
		}
	}
	return false;
}


bool check_dir(const std::string obj){
    boost::filesystem::path fpath(obj);
    if (boost::filesystem::exists(fpath) && boost::filesystem::is_directory(fpath))
        return true;
    return false;
}