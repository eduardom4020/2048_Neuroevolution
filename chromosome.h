#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include "util.h"

typedef bool Gene;

class Chromosome
{
	private:
		vector<bool> genes;

	public:
		Chromosome();
		Chromosome(uint minNumGenes, uint maxNumGenes = 0);
		Chromosome(const Chromosome &other);
		Chromosome(byte* serializedObject);
		~Chromosome();

		Chromosome& operator=(const Chromosome &other);
		void setGene(uint i, bool val);
		bool getGene(uint i) const;
		vector<bool>* getGenes();
		friend ostream& operator<<(ostream &out, const Chromosome &chromosome);

		uint getNumGenes() const;
		void randomize(uint minNumGenes, uint maxNumGenes = 0);

        void crossover(Chromosome& pair, bool multipoint, bool randBreakPoint);
        bool mutation(float pMutation, bool complete = false);

		byte* serialize();
		uint serialSize();
};

#endif // CHROMOSOME_H
