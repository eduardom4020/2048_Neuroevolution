#include "chromosome.h"

Chromosome::Chromosome()
{

}

Chromosome::Chromosome(uint minNumGenes, uint maxNumGenes)
{
	randomize(minNumGenes, maxNumGenes);
}

Chromosome::Chromosome(const Chromosome &other)
{
	this->genes = other.genes;
}

Chromosome::Chromosome(byte* serializedObject)
{
	byte* serialized = serializedObject;

	serialized += sizeof(uint);

	uint numGenes;
	memcpy(&numGenes, serialized, sizeof(uint));
	serialized += sizeof(uint);

	for(uint i = 0; i < numGenes; i++)
	{
		bool gene;
		memcpy(&gene, serialized, sizeof(bool));
		serialized += sizeof(bool);

		genes.push_back(gene);
	}
}

Chromosome::~Chromosome()
{
}

Chromosome& Chromosome::operator=(const Chromosome &other)
{
    if (this == &other)
    	return *this;

	this->genes = other.genes;

    return *this;
}

void Chromosome::setGene(uint i, bool val)
{
	genes[i] = val;
}

bool Chromosome::getGene(uint i) const
{
	return genes[i];
}

vector<bool>* Chromosome::getGenes()
{
	return &genes;
}

ostream& operator<<(ostream &out, const Chromosome &chromosome)
{
	if(chromosome.getNumGenes())
	{
		out << chromosome.getGene(0);
		for(uint i = 1; i < chromosome.getNumGenes(); i++)
			out << chromosome.getGene(i);
	}

	return out;
}

uint Chromosome::getNumGenes() const
{
	return genes.size();
}

void Chromosome::randomize(uint minNumGenes, uint maxNumGenes)
{
	if(this->getNumGenes())
		this->genes.clear();

	uint numGenes = (uint) (maxNumGenes <= minNumGenes) ? minNumGenes : util::intRand(minNumGenes, maxNumGenes);

	for(uint i = 0; i < numGenes; i++)
	{
		genes.push_back(rand()%2);
	}
}

void Chromosome::crossover(Chromosome& pair, bool multipoint, bool randBreakPoint)
{
    Chromosome &c1 = (getNumGenes() <= pair.getNumGenes()) ? *this : pair;
    Chromosome &c2 = (getNumGenes() >  pair.getNumGenes()) ? *this : pair;

    uint p1;
    if(randBreakPoint || multipoint)
		p1 = (uint) util::intRand(0, c1.getNumGenes() - 1);
	else
		p1 = c1.getNumGenes()/2;

    uint size = 0;

    if(multipoint)
    {
        uint p2 = (uint) util::intRand(p1 + 1, c1.getNumGenes() - 1);
        size = (p2 - p1);
    }
    else
        size = c1.getNumGenes() - p1;

	vector<bool> block(c1.getGenes()->begin() + p1, c1.getGenes()->begin() + p1 + size);
	//insert ( iterator position, InputIterator first, InputIterator last );
	//erase ( iterator first, iterator last );
	c1.getGenes()->erase(c1.getGenes()->begin() + p1, c1.getGenes()->begin() + p1 + size);
	c1.getGenes()->insert(c1.getGenes()->begin() + p1, c2.getGenes()->begin() + p1, c2.getGenes()->begin() + p1 + size);
	c2.getGenes()->erase(c2.getGenes()->begin() + p1, c2.getGenes()->begin() + p1 + size);
	c2.getGenes()->insert(c2.getGenes()->begin() + p1, block.begin(), block.end());

	/*
    Gene block[size];
    memcpy(&block[0],   &c1[p1], size*sizeof(Gene));
    memcpy(  &c1[p1],   &c2[p1], size*sizeof(Gene));
    memcpy(  &c2[p1], &block[0], size*sizeof(Gene));
    //*/
}

bool Chromosome::mutation(float pMutation, bool complete)
{
    int ret = 0;

	if(complete)
	{
		int rnd = util::intRand(1,1000);
		int p   = pMutation * 1000.0;
		if(rnd <= p)
		{
			ret++;
			for(uint i = 0; i < getNumGenes(); i++)
					genes[i] = rand()%2;

		}
	}else{
		for(uint i = 0; i < getNumGenes(); i++)
		{
			int rnd = util::intRand(1,1000);
			int p   = pMutation * 1000.0;
			if(rnd <= p)
			{
				ret++;
				genes[i] = 1 - genes[i];//rand()%2;
			}
		}
	}

    return ret;
}

byte* Chromosome::serialize()
{
	unsigned int size = serialSize();

	byte* serialized = new byte[size];
	byte* first = serialized;

	memcpy(serialized, &size, sizeof(uint));
	serialized += sizeof(uint);

	uint genesSize = getNumGenes();
	memcpy(serialized, &genesSize, sizeof(uint));
	serialized += sizeof(uint);

	for(uint i = 0; i < getNumGenes(); i++)
	{
		bool gene = genes[i];

		memcpy(serialized, &gene, sizeof(bool));
		serialized += sizeof(bool);
	}

	return first;
}

uint Chromosome::serialSize()
{
	return 2*sizeof(uint) + getNumGenes() * sizeof(bool);
}
