#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
int main(int argc, char *argv[])
{
    ifstream confFile("try.conf");
    map<string, string> conf;
    if(confFile)
    {
        string line;
        while(confFile)
        {
            getline(confFile,line);
            cout << "l:"<<line << endl;
            size_t pos = line.find("=");
            if(pos != string::npos)
            {
                string key=line.substr(0,pos);
                string value=line.substr(pos+1);
                cout << "key: " << key << ", value: " << value << endl;
                conf[key] = value;
            }
        }
    }
	return 0;
}

