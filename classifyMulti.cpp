#include "helib/FHE.h"
#include "helib/EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include "tools.h"
#include "timer.hpp"

auto timer = mytimer::timer(); //for measuring computation time
using namespace NTL;

int main(int argc, char **argv)
{   
    cout << "Setting up FHE" << endl;
    timer.set();

    long m=11119, p=2, r=18, L=180, c=3, w=64, d=0;

    cout << "Reading in FHE keys with parameters m=" << m << ", p=" << p << ", r=" << r << ", L=" << L << endl;
    string str_m = "_m"+to_string(m), str_p = "_p"+to_string(p), str_L = "_L"+to_string(L);
    
    //clients, CS and TA reads in context
    string contextFileName = "keys/context"+str_m+str_p+str_L+".bin";
    FHEcontext context = readContext(contextFileName);
    cout << "Finished reading in context" << endl;

    //clients, CS and TA reads in pk
    string pubkeyFileName = "keys/pk"+str_m+str_p+str_L+".bin";
    FHEPubKey publicKey = readPK(pubkeyFileName, context);
    cout << "Finished reading in pk" << endl;

    //sk is read only by the TA and no other parties
    string seckeyFileName = "keys/sk"+str_m+str_p+str_L+".bin";
    FHESecKey secretKey = readSK(seckeyFileName, context);
    cout << "Finished reading in sk" << endl;
    
    ////////////////////////////////////////////////////////////////////////
    
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

    ///////////////////////////////////////////////////////////////////////

    cout << "Please input the name of your test dataset" << endl;
    string file;
    cin >> file;

    int pos = file.find("_");
    string dataset = file.substr(0, pos);
    cout << dataset << endl;

    cout << "Reading model" << endl;

    string info_filename = "datasets/"+dataset+"_info.csv";
    Info info = readInfo(info_filename);
    cout << "Finished reading in the information on the dataset" << endl;

    int num_probs = info.num_features + 1;
    int num_data = num_slots/num_probs;
    int shiftIndex = num_data*(num_probs-1);

    int class_num = info.class_num;
    cout << "Number of classes: " << class_num << endl;

    /*
    int flag = 0;
    int class_num;
    while (flag==0){
        cout << "Input number of classes: ";
        cin >> class_num;
        if (class_num <= info.class_num) flag=1;
        else{
            cout << "Invalid class number!" << endl;
            cout << "Please input a number less than or equal to " << info.class_num << endl;
        };
    }
    */

    //reading in the encrypted classification model
    vector <Ctxt> model_ctxts;
    for (int i=0; i<class_num; i++){
        string model_filename = "model/"+dataset+"_class"+to_string(i)+str_m+str_p+str_L+".bin";
        Ctxt ct = readCtxt(model_filename, publicKey);
        model_ctxts.push_back(ct);
    }

    cout << endl;

    ////////////////////////////////////////////////////////////////////////////

    cout << "Reading data" << endl;
    string test_filename = "datasets/"+file+"_test.csv";
    vector <vector <string> > orig_data = readData(test_filename);
    cout << "Finished reading in data" << endl;

    //parsing data 
    vector <vector <long> > data;
    for (int i=0; i<orig_data.size(); i++){
        vector <long> temp = {1};
        vector <long> temp2 = parseData(orig_data[i], info.attr_values);
        temp.insert(temp.end(), temp2.begin(), temp2.end());
        temp.resize(num_slots);
        data.push_back(temp);
    }
    cout << "Finished processing data" << endl;
    cout << endl;

    ////////////////////////////////////////////////////////////////////////////

    string filename = "results/classifyMult_"+dataset+to_string(class_num)+".csv";
    ofstream myfile;
    myfile.open(filename);

    string filename2 = "results/result.txt";
    ofstream myfile2;
    myfile2.open(filename2);

    int num_test = data.size()/num_data;
    cout << "Number of values that will be packed into a ciphertext: " << num_data << endl;
    for (int i = 0; i<num_test; i++){
        timer.set();

        cout << "Classifying a query: " << i << endl;

        // Client generating the query by encrypting data and generating a permutation vector
        //Packing and encrypting data to be classified        
        vector <long> to_enc (num_slots, 0);
        for (int j = 0; j<num_data; j++){
            for (int k = 0; k<num_probs; k++){
                to_enc[(j*num_probs)+k] = data[j+(num_data*i)][k];
            }
        }
        Ctxt ct_data(publicKey);
        ea.encrypt(ct_data, publicKey, to_enc);
        vector <long> perm_vec = genPermVec(class_num);
        //cout << perm_vec << endl;
        // Client sends ct_data and perm_vec to the CS as a query

        //Upon receiving the query, the CS computes the below code
        //calculate the probability for each class
        vector <Ctxt> res_ctxts;
        for (int j=0; j<class_num; j++){            
            Ctxt res = model_ctxts[j];
            //timer.set();
            res.multiplyBy(ct_data);
            //timer.set();
            modifiedTotalSums(ea, res, num_probs);
            //double cTime = timer.get();
            //myfile << cTime << "\n" << flush;
            res_ctxts.push_back(res);
        }
        cout << "Finished calculating probability of each class" << endl;

        //The CS permutes the vector of ciphertexts to blind the real index
        cout << "Permuting the probability ciphertexts" << endl;
        vector <Ctxt> permed;
        for (int j=0; j<perm_vec.size(); j++){
            permed.push_back(res_ctxts[perm_vec[j]]);
        }

        //the CS and TA interacts with each other to perform comparisons
        cout << "Performing the argamx protocol..." << endl;
        srand (time(NULL));

        //TA
        vector <long> index (num_data, 0);  //for keeping track of the index with the higest prob.
        
        //CS
        Ctxt max = permed[0];

        for (int j=1; j<class_num;j++){

            //timer.set();

            //CS
            //generate a masking vector
            vector <long> mask(num_slots, 0);
            for (int k=1; k<=num_data; k++){
                mask[(num_probs*k)-1] = (rand()%100)+1;
            }
            ZZX mask_poly;
            ea.encode(mask_poly, mask);
            
            Ctxt ct_diff = permed[j];
            Ctxt temp = max; 
            ct_diff -= temp;
            ct_diff.multByConstant(mask_poly);
            //only ct_diff is to be sent to the TA
            
            //TA
            vector <long> diff;
            ea.decrypt(ct_diff, secretKey, diff);            
            vector <long> vec_b (num_slots, 0);
            for (int k=1; k<=num_data; k++){
                long b;
                if (diff[(num_probs*k)-1] < plain_mod/2){ b = 1; index[k-1] = j;}
                else b = 0;
                vec_b[(num_probs*k)-1] = b;
            }
            Ctxt ct_b(publicKey);
            ea.encrypt(ct_b, publicKey, vec_b);
            //TA returns this ct_b to the CS

            //CS
            Ctxt cur_max = max;
            max = permed[j];
            max.multiplyBy(ct_b);
            Ctxt ct_temp = ct_b;
            ct_temp.addConstant(to_ZZ(-1));
            ct_temp.multiplyBy(cur_max);
            max -= ct_temp;

            //double cTime = timer.get();
            //myfile << cTime << "\n" << flush;
        }

        cout << "Finished" << endl;
        cout << endl;

        //TA sends the index to the client as the final result
        cout << "Index after permutation: " << index << endl;
        vector <long> depermed (num_data, 0);
        vector <string> result (num_data);
        for (int k=0; k<num_data; k++){
            int cur = perm_vec[index[k]];
            depermed[k] = cur;
            result[k] = info.class_names[cur];
            myfile2 << info.class_names[cur] << "\n" << flush;
        }
        cout << "Classification result index: " << depermed << endl;
        cout << "Classification result: " << result << endl;

        cout << endl;

        double cTime = timer.get();
        myfile << cTime << "\n" << flush;
        timer.print("Time for classifying one data: ");
        cout << endl;
    }

    myfile << "\n" << flush;
    myfile << "Finished classifying all data \n" << flush;
    myfile.close();

    return 0;
}
