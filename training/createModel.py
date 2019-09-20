import csv
import random

print("Input the number of classes: ")
num_class = int(input())
class_prob = []
count = 0
while count < num_class:
    num = random.randrange(100)
    if num >= 50:
        class_prob.append(num)
        count += 1

print("Input the number of attributes: ")
num_attr = int(input())

cond_prob = []
for i in range(num_class):
    cond_prob.append([])

total = 0
for i in range(num_attr):
    print("Current total: {}".format(total))
    print("Input the number of values for attribute {}".format(i))
    num_value = int(input())
    total += num_value

    for j in range(num_class):
        temp = []
        for k in range(num_value):
            num = random.randrange(100)
            if 30 < num < 50:
                num = random.randrange(50, 100)
            elif num <= 30:
                num = 0
            temp.append(num)
        cond_prob[j].append(temp)

print(class_prob)
print(cond_prob)



filename = 'datasets/sample' + str(total+1) + '_basemodel.csv'
with open(filename, 'w') as csvfile:
    f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)

    f.writerow(class_prob)
    for i in range(num_class):
        f.writerow([])
        for j, data in enumerate(cond_prob[i]):
            f.writerow(data)


filename = 'datasets/sample' + str(total+1) + '_info.csv'
with open(filename, 'w') as csvfile:
    f = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)

    temp = []
    for i in range(len(class_prob)):
        temp.append(i)
    f.writerow(temp)

    for i in cond_prob[0]:
        temp = []
        for j in range(len(i)):
            temp.append(j)
        f.writerow(temp)