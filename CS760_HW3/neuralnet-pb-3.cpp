#include <bits/stdc++.h>

using namespace std;

vector<string> lab;
double si(double x){
        return 1/(1+exp(-x));
}

struct instance{
	vector<double> feature;
	double label;
	// int fold;
	instance(){
		this->feature = vector<double>();
		this->label = 0;
	}
};

void readData(string file,bool train);
void getData(string line);
int findNextComma(string s, int cur);
void updateWeight(int k);
void TrainNetWork();
vector<double> firstLayerResult(vector<double>& feature_vector);
double secondLayerResult(vector<double>& beta);
void cmpLabel(int k);

instance tp;
int n_fold;
int n_epoch;
double learning_rate;
double label_distribution;
vector<instance> data;
vector<int> Asubset; //store index of data with first label
vector<int> Bsubset; // store index of data with second label
vector<vector<int>> folds;  // split the data into folds
int TP = 0;
int FP = 0;
unordered_map<int,pair<int,double>> result;
unordered_map<int,int> foldmatch;
double confidence;
/*
	weights for input layers w1, and for hidden layer w2

*/
vector<vector<double>> w1;
vector<double> w2;

void printweight(){
	int i  = 0;
	cout<<"w1 "<<endl;
	for (auto wi : w1){
		cout<<i++<<endl;
		
		for (auto w : wi)cout<<w<<" ";
		cout<<endl; 
	}
	cout<<"w2"<<endl;
	for (auto w : w2)cout<<w<<" " ;
	cout<<endl;
}
void inline lb_dis(vector<instance>& data){
	for (size_t i = 0 ; i < data.size(); i++){
		if (data[i].label) Bsubset.push_back(i);
		else Asubset.push_back(i);
	}
	label_distribution = double(Asubset.size())/data.size();
}
void initWeight(){
	w1.clear();
	w2.clear();
	w1 = vector<vector<double>>(tp.feature.size(),vector<double>(tp.feature.size()+1,0));
	w2 = vector<double>(tp.feature.size()+1,0);
	for (auto& x : w1){
		for (auto& w : x)w = double(rand())/RAND_MAX*2-1;	
	}
	for (auto& w : w2)w = double(rand())/RAND_MAX*2-1;
}
void getfolds(){
	folds = vector<vector<int>>(n_fold,vector<int>());
	srand(unsigned(time(NULL)));
	random_shuffle(Asubset.begin(), Asubset.end());
	random_shuffle(Bsubset.begin(), Bsubset.end());
	int j = 0;
	for (int i = 0 ; i < Asubset.size();i++){
		folds[i % n_fold].push_back(Asubset[i]);
		foldmatch[Asubset[i]] = i % n_fold;
	}
	for (int i = 0 ; i < Bsubset.size();i++){
		folds[n_fold-j-1].push_back(Bsubset[i]);
		foldmatch[Bsubset[i]] = n_fold-j-1;
		j = (j+1)%n_fold;
	}
	for (int i = 0 ; i < folds.size();i++){
		// srand(unsigned(time(NULL)));
		random_shuffle(folds[i].begin(),folds[i].end());
	}
	
}
int main(int argc, char const *argv[])
{
	/* code */
	readData("sonar.arff", true);
	// initWeight();
	lb_dis(data); // calculate the data distribution for the overall all data and store it in global variables

	n_fold = 10;
	n_epoch = 50;
	learning_rate = 0.1;
	getfolds();
	for (int i =0 ; i < 20;i++){
		confidence=(i+1)*0.05;
		TrainNetWork();
	}
	

	return 0;
}

void readData(string file,bool train){
	int k = 0;
	string line;
	ifstream myfile(file);
	if (myfile.is_open()){
		while(getline(myfile,line)){
			if (line[0] == '%') continue;
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
void TrainNetWork(){
	TP = 0;
	FP = 0;
	for (int i = 0 ; i < n_fold; i++){  // i is the index of test data folds
		initWeight();
		
		for (int e = 0 ; e < n_epoch;e++){
			for (int j = 0 ; j < n_fold;j++){
				if (i-j){
					for (int k = 0; k < folds[j].size();k++){
						updateWeight(folds[j][k]);
					}
				}

			}
		}
			for (int x :folds[i]){
			cmpLabel(x);
		}
	}
	cout<<"confidence:"<<confidence<<endl;
	cout<<"TP-rate:"<<double(TP)/Bsubset.size()<<endl;
	cout<<"FP-rate:"<<double(FP)/Asubset.size()<<endl;
		
}
void updateWeight(int k){
	vector<double> feature_vector = data[k].feature;
	feature_vector.push_back(1.0);
	vector<double> beta = firstLayerResult(feature_vector);
	double y = secondLayerResult(beta);
	double t = data[k].label;
	for (int i = 0 ; i < w1.size();i++){
		double s = (y - t) * w2[i]*beta[i]*(1-beta[i]);
		for (int j = 0 ; j < feature_vector.size();j++){
			w1[i][j] -= learning_rate*s*feature_vector[j];
		}
	}
	for (int i = 0 ; i < w2.size();i++){
		w2[i] -= learning_rate*(y-t)*beta[i];	
	}


}
vector<double> firstLayerResult(vector<double>& feature_vector){
	vector<double> res(tp.feature.size(),0);
	// res.push_back(1);
	for (int i =0; i < res.size();i++){
		for (int j = 0; j < feature_vector.size();j++){
			res[i]+= w1[i][j]*feature_vector[j];
		}
	}
	// vector<double> res = alpha;
	for (int i = 0 ; i < res.size();i++){
		res[i] = si(res[i]);
	}
	res.push_back(1.0);
	return res;
}
double secondLayerResult(vector<double>& beta){
	double s = 0;
	for (int i = 0 ; i < beta.size();i++){
		s+= beta[i]*w2[i];
	}
	return si(s);
}
void cmpLabel(int k){
	auto& t = data[k];
	vector<double> v = data[k].feature;
	v.push_back(1.0);
	vector<double> r = firstLayerResult(v);
	double res = secondLayerResult(r);
	int pLabel = (res<confidence)?0:1;
	if (pLabel && pLabel==t.label)TP++;
	if (pLabel && pLabel!=t.label)FP++;
	result[k] = {pLabel,res};
}