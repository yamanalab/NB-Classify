# -*- coding: utf-8 -*-

import csv
import random

def loadCsv(filename):
    lines = csv.reader(open(filename, "r"))
    dataset = list(lines)
    for i in range(len(dataset)):
        dataset[i] = [str(x) for x in dataset[i]]
    return dataset

def splitDataset(dataset, splitRatio):
    trainSize = int(len(dataset) * splitRatio)
    trainSet = []
    copy = list(dataset)
    while len(trainSet) < trainSize:
        index = random.randrange(len(copy))
        trainSet.append(copy.pop(index))
    return [trainSet, copy]


def run(datafile, trainfile, outname):
    dataset = loadCsv(datafile)
    print('Loaded data file {0} with {1} rows'.format(datafile, len(dataset)))
    
    splitRatio = 0.67
    train, test = splitDataset(dataset, splitRatio)
    print("Length of training data: {0}".format(len(train)))
    print("Length of test data: {0}".format(len(test)))
    
    with open(trainfile, 'w') as csvfile:
        f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)
    
        for data in train:
            temp = []
            for i in range(len(data)):
                temp.append(data[i])
            f.writerow(temp)
    
    print("Finished writing out the training dataset")
    
    testFilename = outname + '_test.csv'
    with open(testFilename, 'w') as csvfile:
        f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)
    
        for data in test:
            temp = []
            for i in range(len(data)):
                temp.append(data[i])
            f.writerow(temp)
    print("Finished writing out the test dataset")

if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description=u"train")
    ap.add_argument("-d", "--datafile", help="data filepath")
    ap.add_argument("-t", "--trainfile", help="train filepath")
    ap.add_argument("-o", "--outname", default="sample", help="output name")
    args = ap.parse_args()

    run(args.datafile, args.trainfile, args.outname)

