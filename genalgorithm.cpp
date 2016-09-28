#include "genalgorithm.h"
#include <cmath>
#include <algorithm> // sort

GeneticAlgorithm::GeneticAlgorithm(float crossoverProbability, float mutationProbability, bool completeMutation, bool multipointCrossover, bool randBreakpoint)
				: generation(0), pCross(crossoverProbability), pMutat(mutationProbability), compMutation(completeMutation), multipoint(multipointCrossover), randBreakPoint(randBreakpoint), bestIndividual(0)
{
	//srand(time(NULL));
}

GeneticAlgorithm::GeneticAlgorithm(byte* serializedObject)
				 :bestIndividual(0)
{
	deserialize(serializedObject);
}

GeneticAlgorithm::GeneticAlgorithm(string filename)
				 :bestIndividual(0)
{
	//srand(time(NULL));

	ifstream check(filename.c_str());

	if(!check.fail())
	{
		uint size;

		check.read((char*)&size, sizeof(uint));
		check.seekg(0);

		byte* serial = new byte[size];

		check.read((char*)serial, size);

		deserialize(serial);

		delete[] serial;

	}else{
		this->generation = 0;
		this->pCross = 0.6;
		this->pMutat = 0.001;
		this->multipoint = true;
	}

	check.close();
}

GeneticAlgorithm::~GeneticAlgorithm()
{

}

ostream& operator<<(ostream &out, const GeneticAlgorithm &genetic)
{
	out << "Generation " << genetic.generation << ":\n";

	for(uint i = 0; i < genetic.population.size(); i++)
		out << "*** Individual " << i << " ***\n" << genetic.population[i] << "\n\n";

	return out;
}

void GeneticAlgorithm::addRandomIndividual()
{
	try
	{
		population.push_back(Individual(chromosomicRule));
	}catch(...){
		cout << "Wrong chromosomic rule.\n";
	}
}

void GeneticAlgorithm::generateRandomPopulation(string chromosomicRule, uint populationSize)
{
	this->chromosomicRule = chromosomicRule;

	try
	{

		for(uint p = 0; p < populationSize; p++)
		{
			population.push_back(Individual(chromosomicRule));
		}

	}catch(...){

		cout << "Wrong chromosomic rule.\n";

	}
}

Individual GeneticAlgorithm::getIndividual(uint id)
{
	return population[id];
}

uint GeneticAlgorithm::getPopulationSize()
{
	return population.size();
}

uint GeneticAlgorithm::getGeneration()
{
	return generation;
}


bool sortByEvaluation(Individual a, Individual b)
{
	return (a.getEvaluation() > b.getEvaluation());
}

void GeneticAlgorithm::duplication()
{
	// sort by crescent evaluation 
	sort(population.begin(), population.end(), sortByEvaluation);


	vector<Individual> intermediatePopulation;

	//Solve fitness function ==================================================================
	uint populationSize = population.size();

	float evaluationAverage = 0;

	for(uint i = 0; i < populationSize; i++)
		evaluationAverage = evaluationAverage + population[i].getEvaluation();

	evaluationAverage = ((float)(((float)evaluationAverage)/((float)populationSize)));

	for(uint i = 0; i < populationSize; i++)
	{
		if(evaluationAverage != 0)
		{
			float fit = ((float)(((float)population[i].getEvaluation())/((float)evaluationAverage)));
			if(fit > 0.99999 && fit <= 1.0) fit = 1.0;
			population[i].setEvaluation(fit);
		}
		else
			population[i].setEvaluation(0);
	}
	//==========================================================================================

	//Copies individuals with fitness value greater than 1.0 ===================================
	for(uint i = 0; i < populationSize; i++)
	{
		while(population[i].getEvaluation() >= 1.0)
		{
			if(intermediatePopulation.size() >= populationSize) break;

			intermediatePopulation.push_back(population[i]);
			intermediatePopulation[intermediatePopulation.size() - 1].setEvaluation(0);

			population[i].setEvaluation(population[i].getEvaluation() - 1.0);
		}

		if(intermediatePopulation.size() >= populationSize) break;
	}
	//===========================================================================================

	//"Roulette" probability ==================================================================
	evaluationAverage = 0;

	for(uint i = 0; i < populationSize; i++)
		evaluationAverage = evaluationAverage + population[i].getEvaluation();

	evaluationAverage = ((float)(((float)evaluationAverage)/((float)populationSize)));

	for(uint i = 0; i < populationSize; i++)
	{
		if(evaluationAverage != 0)
			population[i].setEvaluation((((float)(((float)population[i].getEvaluation())/((float)evaluationAverage))))/((float)populationSize));
		else
			population[i].setEvaluation(0);
	}
	//==========================================================================================

	//"Roulette" ================================================================================
	uint roulette[1000];
	uint aux = 0;
	uint maxp = 0;

	for(uint i = 0; i < populationSize; i++)
	{
		population[i].setEvaluation(population[i].getEvaluation()*1000);
		for(uint j = 0; j < population[i].getEvaluation(); j++){
			if(aux >= 1000) break;
			if(population[i].getEvaluation() > population[maxp].getEvaluation()) maxp = i;
			roulette[aux] = i;
			aux++;
		}
	}

	while(aux < 1000) //Completes the roulette with the individual that has the greater probability.
	{
		roulette[aux] = maxp;
		aux++;
	}

	uint numChromosomes = intermediatePopulation[0].getNumChromosomes();

	while(intermediatePopulation.size() < populationSize)
	{
		for(uint c = 0; c < numChromosomes; c++)
		{
			if(intermediatePopulation.size() >= populationSize) break;
			intermediatePopulation.push_back(population[roulette[util::intRand(0, 999)]]);
			intermediatePopulation[intermediatePopulation.size() - 1].setEvaluation(0);
		}
		if(intermediatePopulation.size() >= populationSize) break;
	}
	//=================================================================================

	population = intermediatePopulation;
}

void GeneticAlgorithm::crossover()
{
	for(uint p = 0; p < population.size(); p += 2)
		population[p].crossover(population[p+1], pCross, multipoint, randBreakPoint);
}

void GeneticAlgorithm::mutation()
{
	for(uint i = 0; i < population.size(); i++)
		population[i].mutation(pMutat, compMutation);
}

Individual GeneticAlgorithm::getBestIndividual()
{
	return population[bestIndividual];
}

void GeneticAlgorithm::setEvaluation(uint id, float evaluation)
{
	if(evaluation > population[bestIndividual].getEvaluation())
		bestIndividual = id;

	population[id].setEvaluation(evaluation);
}

float GeneticAlgorithm::getEvaluationAverage()
{
	float eval = 0;
	for(uint i = 0; i < population.size(); i++)
		eval += population[i].getEvaluation();

	return eval/(float)population.size();
}

void GeneticAlgorithm::evolve()
{
	bool allzero = true;
	for(uint i = 0; i < population.size(); i++)
		if(population[i].getEvaluation() > 0)
			allzero = false;

	if(allzero)
	{
		uint popsize = population.size();
		population.clear();
		generateRandomPopulation(chromosomicRule, popsize);
	}
	else
	{
		Individual best = population[bestIndividual]; // elitismo parte 1
	
		duplication();
		crossover();
		mutation();
		
		population[0] = best; // elitismo parte final
	}

	generation++;
}

void GeneticAlgorithm::saveCheckPoint(string filename)
{
	ofstream check(filename.c_str());

	byte* serial = serialize();

	check.write((char*)serial, serialSize());

	delete[] serial;

	check.close();
}

uint GeneticAlgorithm::serialSize()
{
	uint size = 4*sizeof(uint) + 3*sizeof(bool) + 2*sizeof(float) + chromosomicRule.length()*sizeof(char);

	uint popsize = 0;
	for(uint i = 0; i < population.size(); i++)
		popsize += population[i].serialSize();

	return (size + popsize);
}

byte* GeneticAlgorithm::serialize()
{
	uint selfsize = serialSize();

	byte* serialized = new byte[selfsize];
	byte* first = serialized;

	memcpy(serialized, &selfsize, sizeof(uint));
	serialized += sizeof(uint);

	memcpy(serialized, &generation, sizeof(uint));
	serialized += sizeof(uint);

	memcpy(serialized, &pCross, sizeof(float));
	serialized += sizeof(float);

	memcpy(serialized, &pMutat, sizeof(float));
	serialized += sizeof(float);

	memcpy(serialized, &compMutation, sizeof(bool));
	serialized += sizeof(bool);

	memcpy(serialized, &multipoint, sizeof(bool));
	serialized += sizeof(bool);

	memcpy(serialized, &randBreakPoint, sizeof(bool));
	serialized += sizeof(bool);

	uint strsize = chromosomicRule.size();
	memcpy(serialized, &strsize, sizeof(uint));
	serialized += sizeof(uint);

	memcpy(serialized, &chromosomicRule[0], strsize*sizeof(char));
	serialized += strsize*sizeof(char);

	uint popsize = population.size();
	memcpy(serialized, &popsize, sizeof(uint));
	serialized += sizeof(uint);

	for(uint i = 0; i < popsize; i++)
	{
		uint indsize    = population[i].serialSize();
		byte* serialind = population[i].serialize();
		memcpy(serialized, serialind, indsize);
		serialized += indsize;
		delete[] serialind;
	}

	return first;
}

void GeneticAlgorithm::deserialize(byte* serializedObject)
{
	byte* serialized = serializedObject;

	//selfsize
	serialized += sizeof(uint);

	memcpy(&generation, serialized, sizeof(uint));
	serialized += sizeof(uint);

	memcpy(&pCross, serialized, sizeof(float));
	serialized += sizeof(float);

	memcpy(&pMutat, serialized, sizeof(float));
	serialized += sizeof(float);

	memcpy(&compMutation, serialized, sizeof(bool));
	serialized += sizeof(bool);

	memcpy(&multipoint, serialized, sizeof(bool));
	serialized += sizeof(bool);

	memcpy(&randBreakPoint, serialized, sizeof(bool));
	serialized += sizeof(bool);

	uint strsize;
	memcpy(&strsize, serialized, sizeof(uint));
	serialized += sizeof(uint);

	chromosomicRule.insert(0, (char*)serialized, strsize);
	serialized += strsize*sizeof(char);

	uint popsize;
	memcpy(&popsize, serialized, sizeof(uint));
	serialized += sizeof(uint);

	for(uint i = 0; i < popsize; i++)
	{
		uint indsize;
		memcpy(&indsize, serialized, sizeof(uint));

		population.push_back(Individual(serialized));

		serialized += indsize;
	}
}
