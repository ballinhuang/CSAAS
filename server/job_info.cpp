#include "job_info.h"
job_info::job_info()
{
  job_attr = map<string,job_data>();
}

int job_info::add_attr(char *name, char *value, int type)
 {
 	  job_data data = job_data(name,value,type);
    job_attr.insert(pair<string,job_data>(name,data));
    return 1;
 }

 void job_info::parsedata(string *parse)
 {
    *parse += "TYPE = ";
    switch(data_type) {
      case QSUB:  *parse += "QSUB"; break;
      case SCHEDULER:  *parse += "SCHEDULER"; break;
      default:  break;
    }
    *parse += "\n";

    for (map<string,job_data>::iterator it = job_attr.begin(); it != job_attr.end(); it++)
       *parse = *parse + it->second.name + " = " + it->second.value + "\n";
 }

void job_info::parse_job_tomom(string *parse)
{
    map<string,job_data>::iterator iter;
    iter = job_attr.find("USER");
    *parse = *parse + "User = " + iter->second.getvalue()+"\n";
    iter = job_attr.find("Script");
    *parse = *parse + "Script = "  + iter->second.getvalue()+"\n";
}

void job_info::readdata(char *message)
{
  
  string str(message);
  stringstream ss;
  ss << str;
  string name,value;
  while( ss >> name )
  {
    string equal;
    ss >> equal;
    if (name != "Script")
    {
      ss >> value;
      job_data data = job_data(name.c_str(),value.c_str(),ENV_DATA);
      job_attr.insert(pair<string,job_data>(name,data));
    }
    else
    {
      string parm="",line;
      while(ss >> value){
          if (value == "Script")
            break;
          else
            getline(ss,line);
            parm += value + " " + line + "\n";
      }
      job_data data = job_data(name.c_str(),parm.c_str(),SCRIPT_DATA);
      job_attr.insert(pair<string,job_data>(name,data));
    }
  }
}

Data_type job_info::getDataType()
{
	return data_type;
}

void job_info::setDataType(Data_type type)
{
	data_type = type;
}

void job_info::setjobid(int id){
  jobid=id;
}