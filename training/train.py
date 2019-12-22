# -*- coding: utf-8 -*-

import csv
import json
import math

def loadCsv(filename):
    lines = csv.reader(open(filename, "r"))
    dataset = list(lines)
    for i in range(len(dataset)):
        dataset [i] = [str(x) for x in dataset[i]]
    return dataset

def categorize_data(data):
    categorized = {}
    for i in data:
        label = i[-1]
        if label not in categorized:
            categorized[label]=[]
        categorized[label].append(i[:-1])
    return categorized

def calc_cond_prob(data):
    data_length = len(data[0])
    cond_prob = []
    for each in data:
        temp = {}
        for item in each:
            if item in temp:
                temp[item] = temp[item] + 1
            else:
                temp[item] = 1
        for item in temp:
            temp[item] = float(temp[item])/data_length
        cond_prob.append(temp)
    return cond_prob

def calc_log_cond_prob(data, scale, attributes):
    data_length = len(data[0])
    cond_prob = []
    c = -int(math.log(3.7e-4)*10)

    for i in range(len(data)):
        each = data[i]
        temp = {}
        for item in each:
            if item in temp:
                temp[item] = temp[item] + 1
            else:
                temp[item] = 1
        att = attributes[i]
        for value in att:
            if value in temp:
                temp[value] = int(((math.log(temp[value]/data_length) * 10) + c)/c*scale)
            else:
                temp[value] = 0
        cond_prob.append(temp)
    return cond_prob

def calc_model(data, scale, attributes = []):
    cond_prob = {}
    if attributes:
        for i in data: #class
            result = calc_log_cond_prob(list(zip(*data[i])), scale, attributes)
            cond_prob[i] = result
    else:
        for i in data: #class
            result = calc_cond_prob(list(zip(*data[i])))
            cond_prob[i] = result

    return cond_prob


def run(infofile, trainfile, outname):
    train = loadCsv(trainfile)
    print('Loaded data file {0} with {1} rows'.format(trainfile, len(train)))

    dataInfo = loadCsv(infofile)
    class_names = dataInfo[0]
    attribute_values = dataInfo[1:]
    print("Loaded information on data set")
    
    print("Probabilities will be represented as an integer from 0 to N.")
    print("Please input N.")
    scale = int(input())

    categorized_train = categorize_data(train)
    class_prob = {}
    for key in categorized_train:
        class_prob[key] = len(categorized_train[key])/len(train)

    # Trains a model that is not logged or scaled. For testing performance of regular model
    trained_model={}
    trained_model['class_prob']=class_prob
    trained_model['cond_prob'] = calc_model(categorized_train, scale)
    modelFilename = outname + '_model_orig.json'
    with open(modelFilename, 'w') as outfile:
        json.dump(trained_model, outfile, indent=4)
    
    
    # Trains a model that is logged, scaled then represented as integers
    intModel = {}
    intClassProb = {}
    c = -int(math.log(3.7e-4)*10)  # Value later used to "lift" the logged probabilities so that they will be positive
    for key in categorized_train:
        temp = len(categorized_train[key])/len(train)
        intClassProb[key] = int(((math.log(temp) * 10) + c)/c*scale)
    intModel['class_prob'] = intClassProb
    intModel['cond_prob'] = calc_model(categorized_train, scale, attribute_values)
    
    # For checking performance and accuracy of the trained model over plaintext
    with open(outname + '_model.json', 'w') as outfile:
        json.dump(intModel, outfile, indent=4)
    
    # What will actually be encrypted by FHE
    with open(outname + '_model.csv', 'w') as csvfile:
        f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)
    
        f.writerow([intModel['class_prob'][i] for i in class_names])
        cond_prob = intModel['cond_prob']
        for c in class_names:
            f.writerow([])
            for i in range(len(cond_prob[c])):
                f.writerow([cond_prob[c][i][value] for value in attribute_values[i]])
    

if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description=u"train")
    ap.add_argument("-i", "--infofile", help="info filepath")
    ap.add_argument("-t", "--trainfile", help="train filepath")
    ap.add_argument("-o", "--outname", default="sample", help="output name")
    args = ap.parse_args()

    run(args.infofile, args.trainfile, args.outname)

