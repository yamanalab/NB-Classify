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

dataName = input()
filename = '../datasets/'+ dataName + '_data.csv'
dataset = loadCsv(filename)
print('Loaded data file {0} with {1} rows'.format(filename, len(dataset)))

splitRatio = 0.67
train, test = splitDataset(dataset, splitRatio)
print("Length of training data: {0}".format(len(train)))
print("Length of test data: {0}".format(len(test)))

trainFilename = '../datasets/' + dataName + '_train.csv'
with open(trainFilename, 'w') as csvfile:
    f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)

    for data in train:
        temp = []
        for i in range(len(data)):
            temp.append(data[i])
        f.writerow(temp)

print("Finished writing out the training dataset")

testFilename = '../datasets/' + dataName + '_test.csv'
with open(testFilename, 'w') as csvfile:
    f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)

    for data in test:
        temp = []
        for i in range(len(data)):
            temp.append(data[i])
        f.writerow(temp)
print("Finished writing out the test dataset")