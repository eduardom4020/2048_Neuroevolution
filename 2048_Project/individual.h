#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "chromosome.h"

class GeneticAlgorithm;

class Individual
{
	friend class GeneticAlgorithm;

	private:
		float evaluation;
		vector<Chromosome> chromosomes;

		void deserialize(byte* serializedObject);

	protected:
		void setEvaluation(float ev);

	public:

		Individual();
		Individual(std::string chromoRule);
		Individual(const Individual & other);
		Individual(byte* serializedObject);
		~Individual();

		Individual& operator=(const Individual &other);
		Chromosome& operator[](uint i);
		friend ostream& operator<<(ostream &out, const Individual &individual);

		uint getNumGenes() const;
		uint getNumChromosomes() const;
		float getEvaluation() const;

		bool crossover(Individual& pair, float pCrossover, bool multipoint = true, bool randBreakPoint = true);
		bool mutation(float pMutation, bool complete = false);

		bool open(std::string filename);
		void save(std::string filename);

		byte* serialize();
		uint serialSize();
};

#endif // INDIVIDUAL_H
