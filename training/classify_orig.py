import csv
import json
import operator

def loadCsv(filename):
    lines = csv.reader(open(filename, "r"))
    dataset = list(lines)
    label = []
    for i in range(len(dataset)):
        label.append(dataset[i][-1])
        dataset [i] = dataset[i][:-1] #strip the class name (classification result)
    return dataset, label

def classify (cond_prob, class_prob, dataset):
    probs = []
    length = len(dataset[0])

    for data in dataset:
        temp = {}
        for key in cond_prob:
            prob = 1
            c = 3.7e-4
            for i in range(length):
                if data[i] in cond_prob[key][i]:
                    prob *= cond_prob[key][i][data[i]]
                else:
                    prob *= c
            final_prob = prob * class_prob[key]
            temp[key] = final_prob
        probs.append(temp)

    return probs


print("Input name of data set")
dataName = input()
filename = '../datasets/' + dataName + '_test.csv'
dataset, labels = loadCsv(filename)
print('Loaded data file {0} with {1} rows'.format(filename, len(dataset)))

with open('../datasets/' + dataName + '_model_orig.json', 'r') as f:
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