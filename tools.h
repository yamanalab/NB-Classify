#include <string>
#include <vector>
#include "helib/FHE.h"
#include "helib/EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
 
using namespace std;

struct Model {
  int class_num; //Number of classes
  int feature_num; //Number of features
  vector <int> feature_value_num; //Number of values per feature
  vector <vector <long> > probs;
};

struct Info{
	int class_num;
	int num_features;
	vector <string> class_names;
	vector <vector <string> > attr_values;
};

Model readModel (string, vector <string>, vector <vector <string> >);
void printModel(Model);
Info readInfo(string);
vector <vector <string> > readData(string);
void printData(vector <vector <string> > );
vector <long> parseData(vector <string>, vector <vector <string> >);
void printParsedData(vector <long>);
vector <long> genPermVec (int);
vector <long> readPermVec (string);
void writeCtxt(string, Ctxt);
Ctxt readCtxt(string, FHEPubKey&);
FHEcontext readContext(string);
FHEPubKey readPK(string, FHEcontext&);
FHESecKey readSK(string, FHEcontext&);
void modifiedTotalSums(const EncryptedArray&, Ctxt&, long);