#ifndef GENALGORITHM_H
#define GENALGORITHM_H

#include "individual.h"

class GeneticAlgorithm
{
	private:
		uint generation;

		float pCross;
		float pMutat;
		bool  compMutation;
		bool  multipoint;
		bool  randBreakPoint;

		uint bestIndividual;

		//Ex.: "f(8)3;r(1,6)"
		std::string chromosomicRule;

		vector<Individual> population;

		virtual void duplication();
		virtual void crossover();
		virtual void mutation();

		/*!Assigns atributes values from a serialized object (array of bytes).
			Argument:
			- serializedObject: A genetic algorithm object serialized into an array of bytes.
		*/
		void deserialize(byte* serializedObject);


	public:

		/*!Constructor
			Arguments:
			- crossoverProbability: Probability of Crossover
			- mutationProbability: Probability of Mutation
			- multipointCrossover: True if multipoint crossover. False, otherwise.
			- randBreakPoint: True if needed a random breakpoint in the case of singlepoint crossover. False, to break in the middle of chromosome.
		*/
		GeneticAlgorithm(float crossoverProbability = 0.6, float mutationProbability = 0.001, bool completeMutation = false, bool multipointCrossover = true, bool randBreakpoint = true);

		/*!Constructor
			Argument:
			- serializedObject: A genetic algorithm object serialized into an array of bytes.
		*/
		GeneticAlgorithm(byte* serializedObject);

		/*!Opens a genetic algorithm from file.
			Argument:
			- filename: File name.
		*/
		GeneticAlgorithm(std::string filename);

		/*!Destructor*/
		~GeneticAlgorithm();

		/*!Prints population*/
		friend ostream& operator<<(ostream &out, const GeneticAlgorithm &genetic);

		/*!Generate a random population of individuals
			Arguments:
			- numChromosomes: Number of chromosomes per individual
			- numGenes: Number of genes per individual
			- minGenValue: Smallest value assumed by a gene
			- maxGenValue: Highest value assumed by a gene
			- populationSize: Population Size
		*/
		void generateRandomPopulation(std::string chromosomicRule, uint populationSize = 100);

		/*!Adds a random individual.*/
		void addRandomIndividual();

		/*!Gets an individual.
			Arguments:
			- id: Index of the individual.

			Return:
			- A copy of the individual of index id.
		*/
		Individual getIndividual(uint id);

		/*!Returns the population size.*/
		uint getPopulationSize();

		/*!Returns the generation number of the population.*/
		uint getGeneration();

		/*!Returns the best individual of the population.*/
		Individual getBestIndividual();

		/*!Sets an individual evaluation.
			Arguments:
			- id: Index of the individual;
			- evaluation: Evaluation value of the last evaluated individual;
		*/
		void setEvaluation(uint id, float evaluation);

		/*!Returns the average of the evaluations of the population in current generation.*/
		float getEvaluationAverage();

		/*!Evolves population.*/
		void evolve();

		/*!Saves the genetic algorithm object into a file. Saves the object state (snapshot).
			Arguments:
			- filename: File name;
		*/
		void saveCheckPoint(std::string filename);

		/*!Serialize the object*/
		byte* serialize();

		/*!Returns the size (in bytes) of a serialized object*/
		uint serialSize();

};
bool sortByEvaluation(Individual a, Individual b);


#endif // GENALGORITHM_H
