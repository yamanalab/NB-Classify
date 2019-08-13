# NB-Classify
NB-Classify library enables outsourcing of data classification tasks to a cloud server without decryption of data by using FHE to allow secure computation.
The library also provides a program for training your classification model and writing it out to the required format for the program.

# Prerequisites
* Ubuntu LTS 16.04
* glibc 2.23
* g++ 5.4.0
* cmake 3.5
* [HElib](https://github.com/homenc/HElib)

# How to build
1. Build and install [HElib](https://github.com/homenc/HElib) according to Option 1 in INSTALL.md with the `-DCMAKE_BUILD_TYPE=Release` option added to Step 2.
2. Run the following commands at the root of NB-Classify library to build the library.
    ```terminal
    $ mkdir build && cd build
    $ cmake -Dhelib_DIR=$(HELIB_DIRECTORY)/helib_pack/share/cmake/helib ..
    $ make
    ```
    where `$(HELIB_DIRECTORY)` is the directory where HElib is installed.
3. Once the library is built, return to the root of library

# How to run
0. Make a directory `datasets` and put all the necessary files in the directory. Refer to "Format of data sets and other files" written below.
1. Setup FHE by generating the keys by running the following commands 
    ```terminal
    $ mkdir keys
    $ build/setup
    ```
2. Encrypt the trained classification model by running the following commands
    ```terminal
    $ mkdir model
    $ build/encrypt_model
    ```
    Input the name of the data set that you want to encrypt
    
3. To perform classification, first run `mkdir results` then run the following commands depending on your needs
    1. Classification of single data per query without any optimization:
        `build/classifySingle`
    2. Classification of single data per query with optimization for each data set:
        `build/classifySingleOpt`
    3. Classification of multiple data per query:
        `build/classifyMulti`
    After running the command, input the name of the data set and the number of classes you want to test.


# Format of data sets and other files
## List of necessary files
* `$(dataset_name)_info.csv`
* `$(dataset_name)_model.csv`
* `$(dataset_name)_test.csv`
* `$(dataset_name)_train.csv`  (unnecessary if you are going to use your own program for training, given that the model will be formatted in the correct way)

where `$(dataset_name)` is the name of the data set you will later use when running the programming.

A set of sample files is provided in the `datasets` directory. 

You can use `train.py` to train a classification model and write it out to the required format given that the `$(dataset_name)_info.csv` and `$(dataset_name)_train.csv` is formatted correctly.

## Format of $(dataset_name)_info.csv
First row contains names of the class labels. From second row, it contains the names or a value of a feature value for each attribute, one attirubte per row.

Example is given below where N is the number of classes, m is the number of feature values for attribute j.
```
label_0, label_1, ... , label_N
f_0_0, f_1_0, ... , f_m_0
f_0_1, f_1_1, ... , f_m_1
...
f_0_j, f_1_j, ... , f_m_j
```

## Format of $(dataset_name)_model.csv
First row contains the class probabilities. Then, seperated by empty line, it contains series of conditional probabilities where each line contains a conditional probability for each feature value and attribute.
Example is given below where N is the number of classes, m is the number of feature values for attribute j.
```
cl_0, cl_1, ... , cl_N

cp_0_0_0, cp_0_1_0, ... , cp_0_m_0
cp_0_0_1, cp_0_1_1, ... , cp_0_m_1
...
cp_0_0_j, cp_0_1_j, ... , cp_0_m_j

cp_1_0_0, cp_1_1_0, ... , cp_1_m_0
cp_1_0_1, cp_1_1_1, ... , cp_1_m_1
...
cp_1_0_j, cp_1_1_j, ... , cp_1_m_j


...


cp_N_0_0, cp_N_1_0, ... , cp_N_m_0
cp_N_0_1, cp_N_1_1, ... , cp_N_m_1
...
cp_N_0_j, cp_N_1_j, ... , cp_N_m_j
```

## Format of $(dataset_name)_test.csv and $(dataset_name)_train.csv
Each line contains data for classification where each value is the feature value for an attribute and the last value is the actual label. 
```
f0, f1, ..., fj, label
```

## References
* HElib:  https://github.com/homenc/HElib
