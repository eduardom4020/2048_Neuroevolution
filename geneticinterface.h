#ifndef GENETICINTERFACE_H
#define GENETICINTERFACE_H

#include "modnet.h"
#include "chromosome.h"
#include "individual.h"

typedef uint GeneBlock;

struct NeuronParameters
{
	uintv inputs, outputs;
};

enum GeneID
{
	idAfferent = 0, idEfferent, idStandard, idModulatory, idInput, idOutput
};

class GeneticInterface
{

	private:
		float computeWeight(GeneBlock outputParam, GeneBlock inputParam);

		GeneBlock getGeneBlock(Chromosome c, uint blockId);
		byte getId(GeneBlock block);
		uint getValueBinary(GeneBlock block);
		float getValue(GeneBlock block);

		GeneID getGeneID(GeneBlock g);
		NeuromodulatedNetwork buildNetwork(Chromosome chromoParameters, Chromosome chromoNetwork, NeuromodulatedNetwork net);

	public:
		bool hasModulatoryNeurons;
		char plasticity;
		char brainOutputLogic;

		GeneticInterface(bool hasModulatoryNeurons = true, char plasticity = 1, char brainOutputLogic = 0);
		~GeneticInterface();

		void setPhenotype(Individual individual, NeuromodulatedNetwork& network);
};

#endif // GENETICINTERFACE_H
