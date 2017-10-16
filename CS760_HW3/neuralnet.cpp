#include <bits/stdc++.h>

using namespace std;

vector<string> lab;
struct instance{
	vector<double> feature;
	int label;
	instance(){
		this->feature = vector<double>();
		this->label = 0;
	}
};

instance tp;

vector<instance> data;

void readData(string file,bool train);
void getData(string line);
int findNextComma(string s, int cur);
int main(int argc, char const *argv[])
{
	/* code */
	readData("sonar.arff", true);
	// for (auto x : lab) cout<<x<<endl;
	// cout<<"*****"<<endl;
	// cout<<tp.feature.size()<<endl;
	for (auto x : data){
		for (auto t : x.feature)cout<<t<<"__";
		cout<< "label:"<<x.label<<endl;
	}
	return 0;
}

void readData(string file,bool train){
	int k = 0;
	string line;
	ifstream myfile(file);
	if (myfile.is_open()){
		while(getline(myfile,line)){
			stringstream ss(line);
			string tmp;
			ss >> tmp;
			if (tmp =="@relation") continue;
			if (tmp =="@attribute") {
				if(train) {
					ss>>tmp;
					if (tmp == "'Class'"){
						ss >> tmp; // remove the left bracket
						while(ss >> tmp)lab.push_back(tmp.substr(0,tmp.size()-1));
					}
					else {
						tp.feature.push_back(0);
					}
				}
				else continue;
			}
			else if (tmp == "@data"){
				continue;
			}
			else {
				if(train)getData(line);
				// else {
				// 	// cout<<++k<<": ";
				// 	TestData(line,root);
				// }
			}
		}
		myfile.close();
	}
	else cout<< "file read error"<<endl;
}

void getData(string line){
	data.push_back(tp);
	int j= -1;
	for (int i = 0 ; i < tp.feature.size();i++){
		int start = j+1;
		j = findNextComma(line,start);
		double k = stod(line.substr(start,j-start));
		data.back().feature[i] = k;
	}
	string tmp = line.substr(j+1);
	for (int i = 0 ; i < lab.size(); i++){
		if (lab[i] == tmp){
			data.back().label = i;
			break;
		}
	}
}
int findNextComma(string line,int cur){
	for (int i = cur; i < line.size(); i++){
		if (line[i] == ',') return i;
	}
	return line.size();
}