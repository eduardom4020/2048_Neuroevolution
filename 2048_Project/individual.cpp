#include "individual.h"

Individual::Individual()
		   :evaluation(0)
{

}

Individual::Individual(string chromoRule)
		   :evaluation(0)
{

	for(size_t i = 0; i < chromoRule.length(); i++)
	{
		size_t p = chromoRule.find('(', i);

		if(p == i + 1)
		{
			string smqGenes, sqGenes, srep;
			int mqGenes, qGenes, rep;

			size_t commaindex, size;

			char chromotype = chromoRule[i];
			bool randchromo = (chromotype == 'r');
			i = p + 1;

			if(randchromo)
			{
				commaindex = chromoRule.find(',', i);
				size = commaindex - i;
				smqGenes = chromoRule.substr(i, size);
				i = commaindex + 1;
			}
			else if(chromotype != 'f') throw 0;

			commaindex = chromoRule.find(')', i);
			size = commaindex - i;
			sqGenes = chromoRule.substr(i, size);
			i = commaindex + 1;

			commaindex = chromoRule.find(';', i);
			if(commaindex > chromoRule.length()) commaindex = chromoRule.length();
			size = commaindex - i;
			srep = chromoRule.substr(i, size);
			i = commaindex;

			mqGenes    = atoi(sqGenes.c_str());
			if(randchromo)
				qGenes = atoi(smqGenes.c_str());
			else
				qGenes = mqGenes;

			if(srep == "")
				rep    = 1;
			else
				rep    = atoi(srep.c_str());

			for(int i = 0; i < rep; i++)
				chromosomes.push_back(Chromosome(qGenes, mqGenes));

		}
		else throw 0;
	}

}

Individual::Individual(const Individual & other)
		   :evaluation(other.evaluation)
{
	this->chromosomes = other.chromosomes;
}

Individual::Individual(byte* serializedObject)
{
	deserialize(serializedObject);
}

Individual::~Individual()
{
	chromosomes.clear();
}

Individual& Individual::operator=(const Individual &other)
{
    if (this == &other)
    	return *this;

    this->evaluation = other.getEvaluation();
    this->chromosomes = other.chromosomes;

    return *this;
}

Chromosome& Individual::operator[](uint i)
{
	return chromosomes[i];
}

ostream& operator<<(ostream &out, const Individual &individual)
{
	if(individual.chromosomes.size() > 0)
	{

		out << "- Evaluation: " << individual.evaluation << "\n" << "Chromosome 0: " << individual.chromosomes[0];

		for(uint i = 1; i < individual.chromosomes.size(); i++)
		{
			out << "\n" << "Chromosome " << i << ": " << individual.chromosomes[i];
		}

	}

	return out;
}

uint Individual::getNumGenes() const
{
	int nGenes = 0;

	for(uint i = 1; i < chromosomes.size(); i++)
	{
		nGenes += chromosomes[i].getNumGenes();
	}

	return nGenes;
}

uint Individual::getNumChromosomes() const
{
	return chromosomes.size();
}

float Individual::getEvaluation() const
{
	return evaluation;
}

void Individual::setEvaluation(float ev)
{
	this->evaluation = ev;
}

bool Individual::crossover(Individual& pair, float pCrossover, bool multipoint, bool randBreakPoint)
{
    if(pair.getNumChromosomes() != chromosomes.size())
        return false;

    for(uint i = 0; i < chromosomes.size(); i++)
    {
        int rnd = util::intRand(1,1000);
        int prob = (int) pCrossover * 1000.0;
        if(rnd <= prob)
            chromosomes[i].crossover(pair[i], multipoint, randBreakPoint);
    }

    return true;
}

bool Individual::mutation(float pMutation, bool complete)
{
    int ret = 0;

    for(uint i = 0; i < chromosomes.size(); i++)
        ret += chromosomes[i].mutation(pMutation, complete);

    return ret;
}

bool Individual::open(std::string filename)
{
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

		check.close();

		return true;
	}

	check.close();

	return false;
}

void Individual::save(std::string filename)
{
	ofstream check(filename.c_str());

	byte* serial = serialize();

	check.write((char*)serial, serialSize());

	delete[] serial;

	check.close();
}

byte* Individual::serialize()
{
	uint numChromosomes = chromosomes.size();

	uint size = serialSize();

	byte* serialized = new byte[size];
	byte* first = serialized;

	memcpy(serialized, &size, sizeof(uint));
	serialized += sizeof(uint);

	memcpy(serialized, &(this->evaluation), sizeof(float));
	serialized += sizeof(float);

	memcpy(serialized, &numChromosomes, sizeof(uint));
	serialized += sizeof(uint);

	for(uint i = 0; i < numChromosomes; i++)
	{
		uint chromosize = chromosomes[i].serialSize();
		byte* chromoserial = chromosomes[i].serialize();
		memcpy(serialized,chromoserial, chromosize);
		serialized += chromosize;

		delete[] chromoserial;
	}

	return first;
}

uint Individual::serialSize()
{
	uint chromoSerialSize = 0;

	for(uint i = 0; i < chromosomes.size(); i++)
		chromoSerialSize += chromosomes[i].serialSize();

	uint size = 2*sizeof(uint) + sizeof(float) + chromoSerialSize;

	return size;
}

void Individual::deserialize(byte* serializedObject)
{
	uint  numChromosomes;

	byte* serialized = serializedObject;

	serialized += sizeof(uint);

	memcpy(&(this->evaluation), serialized, sizeof(float));
	serialized += sizeof(float);

	memcpy(&numChromosomes, serialized, sizeof(uint));
	serialized += sizeof(uint);

	for(uint i = 0; i < numChromosomes; i++)
	{
		uint chromosize;
		memcpy(&chromosize, serialized, sizeof(uint));

		byte* chromoserial = new byte[chromosize];
		memcpy(chromoserial, serialized, chromosize);
		serialized += chromosize;

		chromosomes.push_back(Chromosome(chromoserial));

		delete[] chromoserial;
	}
}
