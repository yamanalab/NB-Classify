# -*- coding: utf-8 -*-

import csv
import operator
import json
import random

def loadCsv(filename):
    lines = csv.reader(open(filename, "r"))
    dataset = list(lines)
    for i in range(len(dataset)):
        dataset [i] = [str(x) for x in dataset[i]]
    return dataset

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

def run(infofile, basemodelfile, outname):
    dataInfo = loadCsv(infofile)
    
    class_names = dataInfo[0]
    attribute_values = dataInfo[1:]
    print("Loaded information on data set")
    
    print("Input the size of data set that you want to create")
    num = int(input())
    
    test = []
    for i in range(num):
        temp = []
        for j, val in enumerate(attribute_values):
            index = random.randrange(len(val))
            temp.append(val[index])
        test.append(temp)
    
    print(test)
    
    loaded = loadCsv(basemodelfile)
    print(loaded)
    
    model = {}
    model["class_prob"] = {}
    model["cond_prob"] = {}
    
    class_prob = loaded.pop(0)
    loaded.pop(0)
    cond_prob = []
    temp = []
    while loaded:
        cur = loaded.pop(0)
        if cur: temp.append(cur)
        else:
            cond_prob.append(temp)
            temp = []
    cond_prob.append(temp)
    
    
    for i, class_val in enumerate(class_names):
        model["class_prob"][class_val] = int(class_prob[i])
    
        temp = []
        for j, attribute in enumerate(attribute_values):
            temp_attr = {}
            for k, attr_val in enumerate(attribute):
                temp_attr[attr_val] = int(cond_prob[i][j][k])
            temp.append(temp_attr)
        model["cond_prob"][class_val] = temp
    
    
    with open(outname + '_model.json', 'w') as outfile:
        json.dump(model, outfile, indent=4)
    
    
    calculated_prob = classify(model['cond_prob'], model['class_prob'], test)
    print("Calculated")
    print(calculated_prob)
    
    classified = []
    for i in calculated_prob:
        classified.append(max(i.items(), key=operator.itemgetter(1))[0])
    
    
    testFilename = outname + '_train.csv'
    with open(testFilename, 'w') as csvfile:
        f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)
    
        for j, data in enumerate(test):
            temp = []
            for i in range(len(data)):
                temp.append(data[i])
            temp.append(classified[j])
            f.writerow(temp)


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description=u"train")
    ap.add_argument("-i", "--infofile", help="info filepath")
    ap.add_argument("-m", "--basemodelfile", help="base model filepath")
    ap.add_argument("-o", "--outname", default="sample", help="output name")
    args = ap.parse_args()

    run(args.infofile, args.basemodelfile, args.outname)

