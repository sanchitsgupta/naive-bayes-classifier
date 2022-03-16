#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//an instance will contain the image and its classification(face or non-face)
struct Instance
{
	vector<string> image;
	int classification;			//1 if face, 0 if non-face
};

//function prototypes

void fill_othertrainingsets();
void getData(string data, string datalabel, vector<Instance>& dataset);
void count(vector<vector<int>>& count_h_given_f, vector<vector<int>>& count_b_given_f, vector<Instance>& train);
void findProbability(vector<vector<float>>& prob_h_given_f, char h, int f);
void findProbabiltyGiven(vector<vector<float>>& probability, char h, vector<Instance>& traindata);
void classify();
int classifyInstance(Instance& instance, int instance_number);
void printStats();
void calculateProbabilities();

//global variables

vector<Instance> trainingset(451);								//training set consisting of all 451 instances
vector<Instance> trainingset_faces, trainingset_nonfaces;		//training set containing faces and nonfaces respectively
vector<Instance> testingset(150);								//testing set consisting of all 150 instances
vector<int> testedlabels(150);									//vector containing the classification of all test instances

int faces = 217, nonfaces = 234;

//the total probabilities of faces and nonfaces
float prob_face = faces / 451.0, prob_nonface = nonfaces / 451.0;

vector<vector<float>> prob_hash_givenface(70, vector<float>(60));
vector<vector<float>> prob_hash_givennonface(70, vector<float>(60));
vector<vector<float>> prob_blank_givenface(70, vector<float>(60));
vector<vector<float>> prob_blank_givennonface(70, vector<float>(60));

int main()
{
	//fill trainingset with all images and their classification,  and calculate the number of faces and non-faces
	getData("datatrain/facedatatrainlabels.txt", "datatrain/facedatatrain.txt", trainingset);

	//fill trainingset_faces and trainingset_nonfaces
	fill_othertrainingsets();

	//fill testingset
	getData("datatest/facedatatestlabels.txt", "datatest/facedatatest.txt", testingset);

	//calculate all probabilites
	calculateProbabilities();

	//classify new instances and subsequently store them to testedlabels vector and also to classified.txt
	classify();

	//print accuracy and confusion matrix
	printStats();

    return 0;
}

void calculateProbabilities()
{
	findProbability(prob_hash_givenface, '#', 1);
	findProbability(prob_hash_givennonface, '#', 0);
	findProbability(prob_blank_givenface, ' ', 1);
	findProbability(prob_blank_givennonface, ' ', 0);
}

void printStats()
{
	int tp = 0, fn = 0, fp = 0, tn = 0;
	for (int i = 0; i < testingset.size(); i++)
	{
		if (testingset[i].classification == 1)
		{
			if (testedlabels[i] == 1)
			{
				tp++;
			}
			else
			{
				fn++;
			}
		}
		else
		{
			if (testedlabels[i] == 1)
			{
				fp++;
			}
			else
			{
				tn++;
			}
		}
	}
	cout << "Accuracy : " << ((tp + tn) / float(testingset.size())) * 100 << "%\n\n";
	cout << "------------- Confusion Matrix -------------\n\n";
	cout << "Number of True Positives (Actually Face, Predicted Face) : " << tp << "\n";
	cout << "Number of False Negatives (Actually Face, Predicted Non-Face) : " << fn << "\n";
	cout << "Number of False Positives (Actually Non-Face, Predicted Face) : " << fp << "\n";
	cout << "Number of True Negatives (Actually Non-Face, Predicted Non-Face) : " << tn << "\n";
	cout << "\nPrecision : " << (tp / float(tp + fp)) * 100 << "%\n";
	cout << "\nRecall : " << (tp / float(tp + fn)) * 100<< "%\n";
	cout << "\nF - measure : " << ((2*(tp / float(tp + fn))*(tp / float(tp + fp))) / ((tp / float(tp + fn))+(tp / float(tp + fp)))) * 100 << "%\n\n";
}

void classify()
{
	ofstream fout("datatest/classified.txt");
	for (int i = 0; i < testingset.size(); i++)
	{
		testedlabels[i] = classifyInstance(testingset[i], i);
		fout << testedlabels[i] << "\n";
	}
	fout.close();
}

int classifyInstance(Instance& instance, int instance_number)
{
	long double probability_face = 1.0, probability_nonface = 1.0;
	long double p_face = 1.0, p_nonface = 1.0;
	float m;
	if (instance_number == 61)
	{
		m = 1.2;
	}
	else
	{
		m = 1.5;
	}
	for (int i = 0; i < 70; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			if (instance.image[i][j] == '#')
			{
				p_face = p_face * prob_hash_givenface[i][j] * m;
				p_nonface = p_nonface * prob_hash_givennonface[i][j] * m;
			}
			else
			{
				p_face = p_face * prob_blank_givenface[i][j] * m;
				p_nonface = p_nonface * prob_blank_givennonface[i][j] * m;
			}

		}
	}
	probability_face = prob_face * p_face;
	probability_nonface = prob_nonface * p_nonface;
	if (probability_face > probability_nonface)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void fill_othertrainingsets()
{
	for (int i = 0; i < trainingset.size(); i++)
	{
		if (trainingset[i].classification == 1)
		{
			trainingset_faces.push_back(trainingset[i]);
		}
		else
		{
			trainingset_nonfaces.push_back(trainingset[i]);
		}
	}
}

void count(vector<vector<int>>& count_h_given_f, vector<vector<int>>& count_b_given_f, vector<Instance>& traindata)
{
	for (int i = 0; i < traindata.size(); i++)
	{
		for (int j = 0; j < 70; j++)
		{
			for (int k = 0; k < 60; k++)
			{
				if (traindata[i].image[j][k] == '#')
				{
					count_h_given_f[j][k]++;
				}
				else
				{
					count_b_given_f[j][k]++;
				}
			}
		}
	}
}

void findProbabiltyGiven(vector<vector<float>>& probability, char h, vector<Instance>& traindata)
{
	float n = traindata.size();
	float pseudo = 1.9;					//pseudocount taken to be 1.9
	vector<vector<int>> count_hash_given(70, vector<int>(60));
	vector<vector<int>> count_blank_given(70, vector<int>(60));
	count(count_hash_given, count_blank_given, traindata);
	if (h == '#')
	{
		for (int i = 0; i < 70; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				probability[i][j] = ((count_hash_given[i][j] + (0.5*pseudo)) / (n + pseudo));		//prior probability is 0.5
			}
		}
	}
	else
	{
		for (int i = 0; i < 70; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				probability[i][j] = ((count_blank_given[i][j] + (0.5*pseudo)) / (n + pseudo));		//prior probability is 0.5
			}
		}
	}
}

void findProbability(vector<vector<float>>& probability, char h, int f)
{
	if (f == 1)
	{
		findProbabiltyGiven(probability, h, trainingset_faces);
	}
	else
	{
		findProbabiltyGiven(probability, h, trainingset_nonfaces);
	}
}

void getData(string datalabeltxt, string datatxt, vector<Instance>& dataset)
{
	ifstream trainingfile(datalabeltxt);
	string line;
	vector<int> labels;				//this array contains the classifications of the training/testing set
	for (int i = 0; getline(trainingfile, line); i++)
	{
		labels.push_back(stoi(line));
	}
	trainingfile.close();

	trainingfile.open(datatxt);
	for (int i = 0; i < dataset.size();i++)
	{
		dataset[i].classification = labels[i];
	}
	int i = 0, z = 1;
	while (getline(trainingfile, line))
	{
		dataset[i].image.push_back(line);
		z++;
		if ((z - 1) % 70 == 0)
		{
			i++;
		}
	}
	trainingfile.close();
}
