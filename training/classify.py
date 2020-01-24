# -*- coding: utf-8 -*-

import csv
import json
import operator

def loadCsv(filename):
    lines = csv.reader(open(filename, "r"))
    dataset = list(lines)
    label = []
    for i in range(len(dataset)):
        label.append(dataset[i][-1])
        dataset [i] = dataset[i][:-1] #strip the last value (class name)
    return dataset, label

def classify (cond_prob, class_prob, dataset):
    probs = []
    length = len(dataset[0])

    for data in dataset:
        temp = {}
        for key in cond_prob:
            prob = 0
            for i in range(length):
                prob += cond_prob[key][i][data[i]]
            final_prob = prob + class_prob[key]
            temp[key] = final_prob
        probs.append(temp)
    return probs


def run(testfile, modelfile, outname):
    dataset, labels = loadCsv(testfile)
    print('Loaded data file {0} with {1} rows'.format(testfile, len(dataset)))
    
    with open(modelfile, 'r') as f:
        trained_model = json.load(f)
    
    calculated_prob = classify(trained_model['cond_prob'], trained_model['class_prob'], dataset)
    print("Calculated")
    print(calculated_prob)
    
    classified = []
    for i in calculated_prob:
        classified.append(max(i.items(), key=operator.itemgetter(1))[0])
    
    print("Classification result")
    print(classified)
    print("Actual result")
    print(labels)
    
    positive = 0
    negative = 0
    for i in range(len(classified)):
        if classified[i]==labels[i]:
            positive += 1
        else:
            negative += 1
    
    print("True: ", positive)
    print("False: ", negative)


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description=u"train")
    ap.add_argument("-t", "--testfile", help="test filepath")
    ap.add_argument("-m", "--modelfile", help="model JSON filepath")
    ap.add_argument("-o", "--outname", default="sample", help="output name")
    args = ap.parse_args()

    run(args.testfile, args.modelfile, args.outname)

