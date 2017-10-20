#include <bits/stdc++.h>

using namespace std;

vector<string> lab;
double inline sigmoid(double x){
	return 1.0/(1.0+exp(-x));
}
struct instance{
	vector<double> feature;
	int label;
	instance(){
		this->feature = vector<double>();
		this->label = 0;
	}
};

instance tp;
int n_fold;
int n_epoch;
double learning_rate;
double label_distribution;
vector<instance> data;
vector<int> Asubset; //store index of data with first label
vector<int> Bsubset; // store index of data with second label
vector<vector<int>> folds;  // split the data into folds


/*
	weights for input layers w1, and for hidden layer w2

*/
vector<vector<double>> w1;
vector<double> w2;

void readData(const string file,bool train);
void getData(const string line);
int findNextComma(const string s, int cur);
void updateWeight(const instance& ins);
void TrainNetWork();
void inline lb_dis(const vector<instance>& data){
	for (size_t i = 0 ; i < data.size(); i++){
		if (data[i].label) Bsubset.push_back(i);
		else Asubset.push_back(i);
	}
	label_distribution = double(Asubset.size())/data.size();
}
void initWeight(){
	w1 = vector<vector<double>>(tp.feature.size(),vector<double>(tp.feature.size()+1));
	w2 = w1[0];
	for (auto& x : w1){
		for (auto& w : x){
			// srand(time(NULL));
			w = double(rand()%1000000-500000)/500000;
		}
	}
	for (auto& w : w2){
		// srand(time(NULL));
		w = double(rand()%1000000-500000)/500000;
	}
}
void getfolds(){
	folds = vector<vector<int>>(n_fold,vector<int>());
	// auto rng = default_random_engine {};
	// shuffle(begin(Asubset), std::end(Asubset), rng);
	// shuffle(begin(Bsubset), std::end(Bsubset), rng);
	srand(unsigned(time(NULL)));
	random_shuffle(Asubset.begin(), Asubset.end());
	random_shuffle(Bsubset.begin(), Bsubset.end());
	int j = 0;
	for (int i = 0 ; i < Asubset.size();i++){
		folds[i % n_fold].push_back(Asubset[i]);
	}
	for (int i = 0 ; i < Bsubset.size();i++){
		folds[n_fold-j-1].push_back(Bsubset[i]);
		j = (j+1)%n_fold;
	}
	
}
int main(int argc, char const *argv[])
{
	/* code */
	readData("sonar.arff", true);
	// initWeight();
	lb_dis(data); // calculate the data distribution for the overall all data and store it in global variables
	// cout<<Asubset.size()<<endl;
	// cout<<Bsubset.size()<<endl;
	n_fold = 20;
	n_epoch = 5;
	learning_rate = 0.1;
	getfolds();
	for (auto x : folds){
		for (auto t : x)cout<<t<<lab[data[t].label]<<" ";
		cout<<endl;
	}
	TrainNetWork();
	// for (auto x : lab) cout<<x<<endl;
	// cout<<"*****"<<endl;
	// cout<<tp.feature.size()<<endl;
	// for (auto x : data){
	// 	for (auto t : x.feature)cout<<t<<"__";
	// 	cout<< "label:"<<x.label<<endl;
	// }

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

void getData(const string line){
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
int findNextComma(const string line,int cur){
	for (int i = cur; i < line.size(); i++){
		if (line[i] == ',') return i;
	}
	return line.size();
}
void TrainNetWork(){
	for (int i = 0 ; i < n_fold; i++){  // i is the index of test data folds
		initWeight();
		for (int j = 0 ; j < n_fold;j++){
			if (i == j) continue;
			else{
				for (int k = 0 ; k < folds[j].size();k++){
					updateWeight(data[folds[j][k]]);
				}
			}
		}
	}
}
void updateWeight(const instance& ins){
	vector<double> feature_vector = ins.feature;
	feature_vector.push_back(1); // add a biased unit
	vector<double> alpha(ins.feature.size(),0);
	alpha.push_back(1);
	for (int i = 0; i < feature_vector.size();i++){

	}
}