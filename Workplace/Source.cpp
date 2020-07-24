#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
using namespace std;
//declare constant - problem specification, population size
const int GENE = 5;
const int POP_SIZE = 10;
//const int STANDARD_LABOUR_HOUR[GENE] = { 260, 300, 290, 270, 280 };
const int MIN_SALARY = 4000;
const int MAX_SALARY = 7500;
const int MAX_DIFF_SALARY = MAX_SALARY - MIN_SALARY;
const int MIN_EFFICIENCY = 65;
const int MAX_EFFICIENCY = 89;
const int MAX_DIFF_EFFICIENCY = MAX_EFFICIENCY - MIN_EFFICIENCY;

//declare chromosomes data structure
int chromosome[POP_SIZE][GENE];
//declare fitness data structure
float fitness[POP_SIZE];
int parents[2][GENE];
int parentsIndex[2];

//data
int salary[30] = { 800,900,1000,1100,1200,1300,850,950,1050,1150,1250,1350,1400,
1450,1500,1475,1375,1275,1175,1075,975,875,825,925,1025,1125,1225,1325,1425,1150 };
int efficiency[30] = { 65,67,69,71,73,75,67,68,70,72,84,86,87,88,89,88,86,84,82,80,73,71,71,75,77,79,81,85,87,81 };

//repair chromosome
void repairChromosome(int chromo[][GENE], int chromoRepair, int geneRepair) {
	int randNum;
	bool repeat;
	int duplicatedNum = chromo[chromoRepair][geneRepair];
	do {	//check the new value is valid or not
		repeat = false;
		randNum = rand() % 30;
		for (int i = 0; i < GENE; i++) {
			if (chromo[chromoRepair][i] == randNum) {
				repeat = true;
			}
		}
	} while (repeat);
	chromo[chromoRepair][geneRepair] = randNum;	//if valid then replace the old one
}

void initializePopulation() {
	int randNum;
	//initialize random seed
	srand(time(NULL)); //SOURCE;http://www.cplusplus.com/reference/cstdlib/srand/
	for (int c = 0; c < POP_SIZE; c++) {
		for (int i = 0; i < GENE; i++) {
			randNum = rand() % 30;
			chromosome[c][i] = randNum;
		}
		for (int i = 0; i < GENE; i++) {
			for (int j = i + 1; j < GENE; j++) {
				if (chromosome[c][i] == chromosome[c][j]) {
					repairChromosome(chromosome,c,j);
				}
			}
		}
	}
}

/*
rand(): Returns a pseudo-random integral number in the range between 0 and RAND_MAX.
RAND_MAX: This value is library-dependent, but is guaranteed to be at least 32767 on any standard library implementation.
example:
v1 = rand() % 100;         // v1 in the range 0 to 99
v2 = rand() % 100 + 1;     // v2 in the range 1 to 100
v3 = rand() % 30 + 1985;   // v3 in the range 1985-2014
*/
void printChromosome(int chromo[][GENE], int size) {
	for (int c = 0; c < size; c++) {
		cout << "\t" << c << ":\t";
		for (int i = 0; i < GENE; i++) {
			cout << chromo[c][i] << " ";
		}
		cout << endl;
	}
}


void evaluateChromosome() {
	int accumulatedSalary;
	int accumulatedEfficiency;
	float fitnessSalary;
	float fitnessEfficiency;
	int penalty;
	int penaltyTemp;
	for (int c = 0; c < POP_SIZE; c++) {
		penalty = 1;//normal is 1
		accumulatedSalary = 0;
		accumulatedEfficiency = 0;
		for (int i = 0; i < GENE; i++) {
			accumulatedSalary += salary[chromosome[c][i]];
			accumulatedEfficiency += efficiency[chromosome[c][i]];
			penaltyTemp = 1;
			for (int j = i + 1; j < GENE; j++) {
				if (chromosome[c][i] == chromosome[c][j]) {
					penaltyTemp += 1;//penalty accumulate according to the number of duplicated
				}
			}
			if (penaltyTemp > penalty) {
				penalty = penaltyTemp;//take the most duplicated one
			}
		}
		fitnessSalary = (MAX_SALARY - accumulatedSalary) / (float)MAX_DIFF_SALARY;
		fitnessEfficiency = ((accumulatedEfficiency/GENE) - MIN_EFFICIENCY) / (float)MAX_DIFF_EFFICIENCY;
		//penalty
		fitness[c] = ((fitnessSalary * 0.8) + (fitnessEfficiency * 0.2))/penalty;
		cout << "\tC" << c << "\tSalary\t" << fitnessSalary << "\tAverage Efficiency\t" << fitnessEfficiency 
			<< "\tDuplicated\t" << penalty-1 <<"\tFV\t" << fitness[c] << endl;
	}
}

void parentSelection() {
	//declare necessary variables
	int player1, player2;
	//1. For the first parent
	do
		for (int i = 0; i < 2; i++) {
			//1.1 Pick a random number to be the index for player 1
			player1 = rand() % 10;
			//1.2 Pick another random number to be the index for player 2
			do
				player2 = rand() % 10;
			while (player1 == player2);
			//1.3 Compare fitness between player 1 and 2, pick the best one to be parent 1 
			if (fitness[player1] >= fitness[player2]) {
				parentsIndex[i] = player1;
			}
			else {
				parentsIndex[i] = player2;
			}
			cout << player1 << endl;
			cout << player2 << endl;
			cout << "index parent " << i + 1 << "= " << parentsIndex[i] << endl;
		}
	while (parentsIndex[0] == parentsIndex[1]);

	for (int i = 0; i < 2; i++) {
		for (int g = 0; g < GENE; g++) {
			parents[i][g] = chromosome[parentsIndex[i]][g];
		}
	}
	cout << "\nPRINT PARENTS\n";
	printChromosome(parents, 2);
	//2. Repeat (1) for parent 2  
	//3. Print parent 1 & 2  
	//avoid the same number picked for  player 1 and player 2, 
	//avoid the same chromosome picked for parent 1  and 2 
}

//declare CROSSOVER_PROBABILITY as a constant
const float CROSSOVER_PROBABILITY = 0.9;
int children[2][GENE];
//declare children data structure
void crossover() {
	//1. Copy both parent’s chromosome to children chromosomes
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < GENE; j++) {
			children[i][j] = parents[i][j];
		}
	}
	//2. Generate a random number from 0-1. Make sure it is real value data type
	float randomCrossover = rand() % 11 / 10.0;
	cout << "\tRandom number generated = " << randomCrossover << endl;
	//3. If (2) less than crossover probability
	if (randomCrossover < CROSSOVER_PROBABILITY) {
		//2.1 generate a random crossover point
		cout << "\tCrossover happened\n" << endl;
		int crossover_point = rand() % GENE;
		for (int row = 0; row < 2; row++) {
			for (int column = crossover_point; column < GENE; column++) {
				children[0][column] = parents[1][column];
				children[1][column] = parents[0][column];
			}
		}
		cout << "\n\tChildren, crossovered at " << crossover_point << endl;
		printChromosome(children, 2);
	}
	//2.2 Crossover parent 1 and parent 2 to produce the children
	//2.3 Store in children 1 and 2
	//4. Print children 1 & 2 
}

const float MUTATION_PROBABILITY = 0.1;
//declare MUTATION_PROBABILITY as a constant 
void mutation() {
	//1. Declare necessary variables
	int mutation_bit;
	float random2;
	int randNum;
	//2. Declare mutation bit 
	for (int child = 0; child < 2; child++) {
		random2 = rand() % 11 / 10.0;
		cout << "\tRandom number generated = " << random2 << endl;
		if (random2 < MUTATION_PROBABILITY) {
			cout << "\n\tMutation happened\n";
			mutation_bit = rand() % GENE;
			cout << "\n\tMutating Children " << child << " at gene " << mutation_bit;
			do {
				randNum = rand() % 30;
				if (children[child][mutation_bit] != randNum) {
					children[child][mutation_bit] = randNum;
					randNum = -1;
				}
			}while(randNum != -1);
		}
	}
	//3. For both children 
	//3.1. Generate  number from 0-1 (real values)
	//3.2. If 3.1 less than mutation probability 
	//3.2.1. generate a mutation bit 
	//3.2.2. Print the mutation bit 
	//3.2.3. Fill new random number to the mutation bit (if condition)
	//End if 
	//End for
	//5. Print the mutated chromosomes } 
	cout << "\n\tChildren after mutation process\n";
	printChromosome(children, 2);
}

int newChromosome[POP_SIZE][GENE];
//1. A temporary data structure to hold the survivors
//2. Counter to count how many new chromosome so far (array counter) 
int newChromosomeCount = 0;

void survivalSelection() {
	//1. For each chromosome 
	for (int child = 0; child < 2; child++) {
		//1.2. For each gene
		for (int g = 0; g < GENE; g++) {
			//1.3 Copy children to the survival chromosome array
			newChromosome[newChromosomeCount][g] = children[child][g];
		}
		newChromosomeCount++;
	}
	cout << newChromosomeCount << endl;
	cout << "\n\tNew Chromosome so far" << endl;
	printChromosome(newChromosome, newChromosomeCount);
	//2. Update array counter
	//3. Print chromosome in the new population so far
}

//copy chromosome to the original data structure 
void copyChromosome() {
	//1. For each chromosome 
	for (int p = 0; p < POP_SIZE; p++) {
		//1.2. For each gene
		for (int g = 0; g < GENE; g++) {
			//1.3 Copy newChromosome to chromosome
			chromosome[p][g] = newChromosome[p][g];
		}
	}
}

int const MAXIMUM_GENERATION = 19;
//declare file
ofstream avgFitnessFile, bestFitnessFile, BestChromosomeFile;
float avgFitness = 0;
float bestFitness = 0;
int bestChromosome[GENE];

void calculateavgFitness()
{
	float totalFitness = 0;
	for (int i = 0; i < POP_SIZE; i++)
	{
		totalFitness = totalFitness + fitness[i];
	}
	avgFitness = totalFitness / POP_SIZE;
	cout << "\n\tAverage fitness: " << avgFitness;
	avgFitnessFile << avgFitness << endl;
}

void recordBestFitness()
{
	for (int i = 1; i < POP_SIZE; i++)
	{
		if (fitness[i] > bestFitness)
		{
			bestFitness = fitness[i];
			for (int j = 0; j < GENE; j++)
			{
				bestChromosome[j] = chromosome[i][j];
			}
		}
	}
	cout << "\n\tBest Fitness so far " << bestFitness;
	bestFitnessFile << bestFitness << endl;
	cout << "\n\tBest Chromosome is : ";
	for (int j = 0; j < GENE; j++)
	{
		cout << bestChromosome[j] << " ";
		BestChromosomeFile << bestChromosome[j] << " ";
	}
	BestChromosomeFile << endl;
	cout << endl;
}

int main() {
	avgFitnessFile.open("averageFitnessFile.txt");
	bestFitnessFile.open("bestFitnessFile.txt");
	BestChromosomeFile.open("bestChromosome.txt");
	cout << "\nGA START! \n";
	cout << "First generation \n\n";
	cout << "\nINITIALIZATION... \n";
	initializePopulation();
	cout << "\nPRINT INITIAL POPULATION \n";
	//getchar();
	for (int a = 0; a < MAXIMUM_GENERATION; a++) {
		cout << "\n~~~~~~~~~~~~~~~ GENERATION " << a + 1 << "~~~~~~~~~~~~~\n";
		//LAB 4
		printChromosome(chromosome, POP_SIZE);
		//getchar();
		cout << "\nEVALUATE CHROMOSOME \n";
		evaluateChromosome();
		calculateavgFitness();
		recordBestFitness();
		//getchar();
		newChromosomeCount = 0;
		for (int i = 0; i < POP_SIZE / 2; i++) {
			cout << "\nPARENT SELECTION \n";
			parentSelection();
			cout << "\nCROSSOVER \n";
			crossover();
			cout << "\nMUTATION \n";
			mutation();
			cout << "\nSURVIVAL SELECTION \n";
			survivalSelection();
		}
		cout << "\nCOPY CHROMOSOME \n";
		copyChromosome();
		//getchar();
	}
	evaluateChromosome();
	calculateavgFitness();
	recordBestFitness();
	avgFitnessFile.close();
	bestFitnessFile.close();
	BestChromosomeFile.close();
}