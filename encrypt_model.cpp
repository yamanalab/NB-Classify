#include "helib/FHE.h"
#include "helib/EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include "tools.h"
#include "timer.hpp"

using namespace std;
auto timer = mytimer::timer();
using namespace NTL;

int main(int argc, char **argv){

    long m=11119, p=2, r=18, L=180, c=3, w=64, d=0;

    cout << "Reading in FHE keys with parameters m=" << m << ", p=" << p << ", r=" << r << ", L=" << L << endl;    
    string str_m = "_m"+to_string(m), str_p = "_p"+to_string(p), str_L = "_L"+to_string(L);
    
    string contextFileName = "keys/context"+str_m+str_p+str_L+".bin";
    FHEcontext context = readContext(contextFileName);
    cout << "Finished reading in context" << endl;

    string pubkeyFileName = "keys/pk"+str_m+str_p+str_L+".bin";
    FHEPubKey publicKey = readPK(pubkeyFileName, context);
    cout << "Finished reading pubkey" << endl;

    ZZX G = context.alMod.getFactorsOverZZ()[0];
    EncryptedArray ea(context, G);
    timer.print("Finished setting up FHE: ");

    long num_slots = context.zMStar.getNSlots();
    cout << "Number of slots: " << num_slots << endl;
    double act_security = context.securityLevel(); //obrain real security level
    cout << "Security: " << act_security << endl;
    long plain_mod = pow(p, r);
    cout << "Plaintext modulus: " << plain_mod << endl;
    cout << endl;

    //////////////////////////////////////////////////////////////////////////

    cout << "Reading model" << endl;

    cout << "Please input the name of your dataset" << endl;
    string dataset;
    cin >> dataset;

    string info_filename = "datasets/"+dataset+"_info.csv";
    Info info = readInfo(info_filename);
    cout << "Finished reading in the information on the dataset" << endl;

    string model_filename = "datasets/"+dataset+"_model.csv";
    Model NB = readModel(model_filename, info.class_names, info.attr_values);
    cout << "Finished reading in the plaintext model" << endl;

    int class_num = NB.class_num;
    int num_probs = info.num_features + 1;
    int num_data = num_slots/num_probs;

    /*
    string filename = "encdec_time.txt";
    ofstream myfile;
    myfile.open(filename);
    */

    for (int i=0; i<class_num; i++){
        timer.set();

        vector <long> temp = NB.probs[i];
        for (int j=0; j<num_data-1; j++){
            temp.insert(temp.end(), NB.probs[i].begin(), NB.probs[i].end());
        }
        temp.resize(num_slots);
        Ctxt ct(publicKey);
        ea.encrypt(ct, publicKey, temp);

        double encTime = timer.get();
        //myfile << encTime << "," << decTime << "\n"<< flush;
        
        cout << "Writing model ciphertext to file" << endl;
        string filename = "model/"+dataset+"_class"+to_string(i)+str_m+str_p+str_L+".bin";
        writeCtxt(filename, ct);
    }

    cout << "Finished encrypting and writing out the model" << endl;
    cout << endl;
    
    return 0;
}