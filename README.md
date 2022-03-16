# Naive-Bayes Classifier in C++

![C++](https://img.shields.io/badge/C%2B%2B-v14-blue.svg?style=flat&logo=c%2B%2B)
![Linux](https://svgshare.com/i/Zhy.svg)
![PyPI license](https://img.shields.io/github/license/sanchitsgupta/naive-bayes-classifier)

This is an implementation of the [Naive-Bayes Classifier](https://en.wikipedia.org/wiki/Naive_Bayes_classifier) in C++.

## Running

Compile and run the [classifier.cpp](./classifier.cpp) file.
```shell
$ g++ -o classifier classifier.cpp
$ ./classifier
```

## Dataset Info

I used a custom dataset that has faces represented in txt files. Check out [facedatatrain.txt](./datatrain/facedatatrain.txt) file to understand the data.

[datatrain](./datatrain/) directory contains the training files and [dataset](./datatest/) contains the testing ones.

> NOTE: Code was written specifically for this dataset and will require significant changes before running on some other dataset.

## Results

Accuracy: 89.3333%

Confusion Matrix
|                     | Predicted Face | Predicted Non-Face |
| ------------------- |:--------------:|:------------------:|
| **Actual Face**     | 68             | 5                  |
| **Actual Non-Face** | 11             | 66                 |

Precision: 86.076%

Recall: 93.1507%

F-measure: 89.4737%
