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

void setupFHE (long m, long p, long r, long L, long c, long w, long d)
{
    timer.set();

    FHEcontext context(m, p, r);
    buildModChain(context, L, c);
    FHESecKey secretKey(context);
    const FHEPubKey& publicKey = secretKey;
    secretKey.GenSecKey(w);
    addSome1DMatrices(secretKey);
    cout << "Generated the secret key for FHE " << endl;

    double t = timer.get();
    cout << "Finished setting up FHE: " << t << endl;

    double security = context.securityLevel();
    long slots = context.zMStar.getNSlots();

    cout << "slots=" << slots << ", m=" << m << ", p=" << p << ", r=" << r << ", L=" << L << ", security=" << security << endl;
    cout << "PhiM: " << context.zMStar.getPhiM() << endl;

    string str_m = "_m"+to_string(m), str_p = "_p"+to_string(p), str_L = "_L"+to_string(L);
    
    string contextFileName = "keys/context"+str_m+str_p+str_L+".bin";
    ofstream contextFile(contextFileName, ios::binary);
    writeContextBaseBinary(contextFile, context);
    writeContextBinary(contextFile, context);
    cout << "writing out context to binary file" << contextFileName << endl;

    string pubkeyFileName = "keys/pk"+str_m+str_p+str_L+".bin";
    ofstream pubkeyFile(pubkeyFileName, ios::binary);
    cout << "Writing pubkey to binary file " << pubkeyFileName << endl;
    writePubKeyBinary(pubkeyFile, publicKey);
    pubkeyFile.close();

    string seckeyFileName = "keys/sk"+str_m+str_p+str_L+".bin";
    ofstream seckeyFile(seckeyFileName, ios::binary);
    cout << "Writing seckey to binary file " << seckeyFileName << endl;
    writeSecKeyBinary(seckeyFile, secretKey);
    seckeyFile.close();

    cout << "Finsihed writing out context, pk, and sk to a file" << endl;
}

int main(int argc, char **argv)
{
    cout << "Setting up FHE" << endl;
    long m=11119, p=2, r=18, L=180, c=3, w=64, d=0;
    cout << "Generating parameters with m=" << m << ", p=" << p << ", r=" << r << ", L=" << L << endl;
    setupFHE(m, p, r, L, c, w, d);
    cout << endl << "Finished all set up" << endl;
    
    return 0;

}