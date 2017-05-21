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

Data_type job_info::getDataType()
{
	return data_type;
}

void job_info::setDataType(Data_type type)
{
	data_type = type;
}