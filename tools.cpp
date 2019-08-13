#include "helib/FHE.h"
#include "helib/EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include "tools.h"
 
using namespace std;
using namespace NTL;

Model readModel (string filename, vector <string> class_names, vector <vector <string> > attr_values){
  Model NB;
  NB.class_num = class_names.size(); //Number of classes
  NB.feature_num = attr_values.size(); //Number of features
  //cout << "Number of features: " << NB.feature_num << endl;
  NB.feature_value_num; //Number of possible values per feature
  for (int i = 0; i<NB.feature_num; i++){
    NB.feature_value_num.push_back(attr_values[i].size());
  } // array that contains number of possible values for each attribute

  ifstream infile(filename);
  string line;

  int num;
  int count = 0;
  int class_count = -1;

  while (getline(infile, line)){
      stringstream ss(line);

      //Reads class probabilities into class_prob
      if (count == 0){
        while (ss >> num){
          vector <long> temp;
          temp.push_back(num);
          NB.probs.push_back(temp);
          if (ss.peek()== ','){ss.ignore();}
        }
      }
      else if (((NB.feature_num*class_count)+2+class_count <= count) && (count <= ((NB.feature_num*class_count)+1+class_count+NB.feature_num))){
        while (ss >> num){
          NB.probs[class_count].push_back(num);
          if (ss.peek()== ','){ss.ignore();}
        }
      }

      else {class_count += 1;}

      count += 1;
    }
    return NB;
}

void printModel(Model NB){
  int class_num = NB.probs.size();

  cout << "Class probability" << endl;
  for (int i = 0; i<class_num; i++){
    cout << "Class " + to_string(i) + ": " << NB.probs[i][0] << endl;
  }
  cout << endl;

  cout << "Conditional probability" << endl;
  for (int i = 0; i<class_num; i++){
    cout << endl;
    cout << "Class " + to_string(i) << endl;
    int count = 1;
    for (int j = 0; j<NB.probs[i].size(); j++){
      cout << NB.probs[i][j] << " ";
    }
    cout << endl;
  }
}

Info readInfo(string filename){
  Info info;

  ifstream infile(filename);
  string line;
  int count = 0;
  info.num_features = 0;

  while (getline(infile, line)){
      stringstream ss(line);

      if (count == 0){
        count++;
        while (ss.good()){
          string substr;
          getline(ss, substr, ',');
          info.class_names.push_back(substr);
        }
      }

      else {
        vector <string> temp;
        while (ss.good()){
          string substr;
          getline(ss, substr, ',');
          temp.push_back(substr);
        }
        info.num_features += temp.size();
        info.attr_values.push_back(temp);
      }
  }
  info.class_num = info.class_names.size();

  return info;

}

vector <vector <string> > readData(string filename){
  vector <vector <string> > data;
  ifstream infile(filename);
  string line;

  while (getline(infile, line)){
    vector <string> temp;
    stringstream ss(line);
    string value;

    while (getline(ss, value, ',')){
      temp.push_back(value);
    }

    temp.pop_back(); //pops out the class label
    data.push_back(temp);
  }
  infile.close();
  return data;
}

void printData(vector <vector <string> > data){
  int i, j;
  cout << "Printing test data" << endl;
  for(i=0; i < data.size(); i++){
      cout << "Data " << i << endl;
      for (j=0; j<data[i].size();j++){
        cout << data[i][j] << " ";
      }
      cout << endl;
    }
}

vector <long> parseData(vector <string> sample, vector <vector <string> > attr_values){
  vector <long> parsed;
    for (int i =0; i < sample.size(); i++){
      int length = attr_values[i].size();
    
      for (int j=0; j < attr_values[i].size(); j++){
        if (sample[i] == attr_values[i][j]){
          parsed.push_back(1);
        }
        else {
          parsed.push_back(0);
        }
      }
    }

    return parsed;
}

void printParsedData(vector <long> parsed){
  for (int i=0; i < parsed.size(); i++)
  {
      cout << parsed[i] << ",";
  }
  cout << endl;
}

vector <long> genPermVec (int n){
  vector <long> perm_vec (n);
  iota(perm_vec.begin(), perm_vec.end(), 0);

  random_device seed_gen;
  mt19937 engine(seed_gen());
  shuffle(perm_vec.begin(), perm_vec.end(), engine);

  return perm_vec;
}

vector <long> readPermVec(string filename){
  vector <long> perm_vec;
  ifstream infile(filename);
  string line;

  while (getline(infile, line)){
    stringstream ss(line);
    int num;

    while (ss >> num){
          perm_vec.push_back(num);
          if (ss.peek()== ','){ss.ignore();}
        }
  }
  return perm_vec;
}

void writeCtxt(string filename, Ctxt ct){
  ofstream ctxt_file(filename, ios::binary);
  ctxt_file << ct;
  ctxt_file.close();
}

Ctxt readCtxt(string filename, FHEPubKey& publicKey){
  Ctxt ct(publicKey);
  ifstream ctxt_file(filename, ios::binary);
  ctxt_file >> ct;
  ctxt_file.close();

  return ct;
}


FHEcontext readContext(string filename){
  ifstream contextFile(filename, ios::binary);
  unsigned long m1, p1, r1;
  vector<long> gens, ords;
  readContextBaseBinary(contextFile, m1, p1, r1, gens, ords);
  FHEcontext context(m1,p1,r1,gens,ords);
  readContextBinary(contextFile, context);
  contextFile.close();
  return context;
}

FHEPubKey readPK(string filename,  FHEcontext& context){
  ifstream pubkeyFile(filename, ios::binary);
  FHEPubKey publicKey(context);
  readPubKeyBinary(pubkeyFile, publicKey);
  pubkeyFile.close();
  return publicKey;

}

FHESecKey readSK(string filename, FHEcontext& context){
  ifstream seckeyFile(filename, ios::binary);
  FHESecKey secretKey(context);
  readSecKeyBinary(seckeyFile, secretKey);
  seckeyFile.close();
  return secretKey;
}

void modifiedTotalSums(const EncryptedArray& ea, Ctxt& ctxt, long n)
{
  if (n == 1) return;

  Ctxt orig = ctxt;
  long k = NumBits(n), e = 1;

  for (long i = k-2; i >= 0; i--) {
    Ctxt tmp1 = ctxt;
    ea.rotate(tmp1, e);
    ctxt += tmp1; // ctxt = ctxt + (ctxt >>> e)
    e = 2*e;

    if (bit(n, i)) {
      Ctxt tmp2 = orig;
      ea.rotate(tmp2, e);
      ctxt += tmp2;
      e += 1;
    }
  }
}