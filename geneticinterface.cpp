#include "geneticinterface.h"

GeneticInterface::GeneticInterface(bool hasModulatoryNeurons, char plasticity, char brainOutputLogic)
{
	this->plasticity = plasticity;
	this->hasModulatoryNeurons = hasModulatoryNeurons;
	this->brainOutputLogic = brainOutputLogic;
}

GeneticInterface::~GeneticInterface()
{

}

GeneBlock GeneticInterface::getGeneBlock(Chromosome c, uint blockId)
{
	vector<bool>* chromo = c.getGenes();
	uint block = 0;

	uint start = 32*blockId;
	for(uint i = start; i < start + 32; i++)
		block = (block << 1) | (*chromo)[i];

	return block;
}

byte GeneticInterface::getId(GeneBlock block)
{
	return (block >> 24);
}

uint GeneticInterface::getValueBinary(GeneBlock block)
{
	return (block << 8) >> 8;
}

float GeneticInterface::getValue(GeneBlock block)
{
	float value;

	uint val = (block << 8) >> 8;
	uint max = 16777215;

	value = ((float)((float)val/(float)max)*2.0) - 1.0;

	return value;
}

GeneID GeneticInterface::getGeneID(GeneBlock g)
{
	byte id = getId(g);

	if(id >= 0   && id <= 12 ) return idAfferent;  //0 a 25
	if(id >= 13  && id <= 25 ) return idEfferent;  //26 a 51

	if(hasModulatoryNeurons)
	{
		if(id >= 26  && id <= 38 ) return idStandard;  //52 a 77
		if(id >= 39  && id <= 51) return idModulatory;//78 a 103
	}else
		if(id >= 26  && id <= 51 ) return idStandard;  //52 a 77

	if(id >= 52 && id <= 150) return idInput;     //104 a 179

	return idOutput;
}

float GeneticInterface::computeWeight(GeneBlock outputParam, GeneBlock inputParam)
{
	float out = getValue(outputParam);
	float in  = getValue(inputParam);

	uint outbin = getValueBinary(outputParam);
	uint inbin  = getValueBinary(inputParam);

	byte bits = 24; //24 bits value representation
	byte *o = (byte*)(&outbin);
	byte *i = (byte*)(&inbin);

//*
	byte equal[3];
	for(uint b = 0; b < 3; b++)
		equal[b] = i[b]^o[b];
//*/
/*
	byte equal[3];
	for(uint b = 1; b < 4; b++)
		equal[b-1] = i[b]^o[b];
//*/
	uint eb = 0;
	for(uint b = 0; b < 3; b++)
		for(int q = 0; q < 8; q++)
			eb += !((equal[b] >> q) & 1);

	if((eb/4)%3 == 0) eb = 0; //A try to increase the number of 0 weights to obtain more topological changes

	float weight = ((float)((uint)eb*(in + out))/((float)bits*2.0));

	return weight;
/*
	float out = outputParam;
	float in = inputParam;

	byte bits = 24; //mantissa + signal

	outputParam = (outputParam <= -0) ? outputParam - 1.0 : outputParam;
	outputParam = (outputParam >= 0) ? outputParam + 1.0 : outputParam;
	inputParam = (inputParam <= -0) ? inputParam - 1.0 : inputParam;
	inputParam = (inputParam >= 0) ? inputParam + 1.0 : inputParam;

	byte *o = reinterpret_cast<byte *>(&outputParam);
	byte *i = reinterpret_cast<byte *>(&inputParam);

	byte equal[sizeof(float)];
	for(uint b = 0; b < sizeof(float); b++)
		equal[b] = i[b]^o[b];

	uint eb = 0;
	for(uint b = 0; b < sizeof(float); b++)
		for(int q = 0; q < 8; q++)
			eb += !((equal[b] >> q) & 1);

	eb -= 8; //exponent bits: always equal

	if((eb/4)%3 == 0) eb = 0; //A try to increase the number of 0 weights to obtain more topological changes

	float weight = ((float)((uint)eb*(in + out))/((float)bits*2.0));

	return weight;
*/
}

NeuromodulatedNetwork GeneticInterface::buildNetwork(Chromosome chromoParameters, Chromosome chromoNetwork, NeuromodulatedNetwork net)
{
	float timeStep = net.getTimeStep();
	uint numberOfInputNeurons = net.getNumberOfInputs();
	uint numberOfOutputNeurons = net.getNumberOfOutputs();

	NeuromodulatedNetwork network(plasticity, hasModulatoryNeurons, brainOutputLogic, timeStep);

	//network.setStimulus(chromoParameters[0].getValue());
	network.setStimulus(getValue(getGeneBlock(chromoParameters, 0)));
	//network.setLearningRate(chromoParameters[1].getValue());
	network.setLearningRate(getValue(getGeneBlock(chromoParameters, 1)));
	//network.setPlasticityParameters(chromoParameters[2].getValue(), chromoParameters[3].getValue(), chromoParameters[4].getValue(), chromoParameters[5].getValue());
	network.setPlasticityParameters(getValue(getGeneBlock(chromoParameters, 2)), getValue(getGeneBlock(chromoParameters, 3)), getValue(getGeneBlock(chromoParameters, 4)), getValue(getGeneBlock(chromoParameters, 5)));

	uint chromoSize = chromoNetwork.getNumGenes()/32;

	vector<NeuronParameters> wParam;

	uint g = 0;
	uint currNeuron = -1;

	uint totalNeuronGenes = 0;
	for(uint g = 0; g < chromoSize; g++)
	{
		//std::cout << "id: " << getGeneID(getGeneBlock(chromoNetwork, g)) << '\n';
		if(getGeneID(getGeneBlock(chromoNetwork, g)) <= 3) totalNeuronGenes++;
	}	
	
	if(totalNeuronGenes < numberOfInputNeurons + numberOfOutputNeurons)
		return network;

	do{
		NeuronParameters ioParam;

		GeneID cellType = getGeneID(getGeneBlock(chromoNetwork,g));

		if(cellType <= 3)
		{
			if(currNeuron + 1 < numberOfInputNeurons)
				network.addCell(ntAfferent, getValue(getGeneBlock(chromoNetwork,g))); //network.addCell(ntAfferent, chromoNetwork[g].getValue());
			else if(currNeuron + 1 >= totalNeuronGenes - numberOfOutputNeurons)
				network.addCell(ntEfferent, getValue(getGeneBlock(chromoNetwork,g)) == 0 ? 0.001 : getValue(getGeneBlock(chromoNetwork,g))); //network.addCell(ntEfferent, chromoNetwork[g].getValue() == 0 ? 0.001 : chromoNetwork[g].getValue());
			else if(cellType == idStandard || cellType == idAfferent || cellType == idEfferent) //All afferent and efferent neurons should be in the network
				network.addCell(ntStandard, getValue(getGeneBlock(chromoNetwork,g)) == 0 ? 0.001 : getValue(getGeneBlock(chromoNetwork,g)));
			else //if(cellType == ntModulatory)
				network.addCell(ntModulatory, getValue(getGeneBlock(chromoNetwork,g)) == 0 ? 0.001 : getValue(getGeneBlock(chromoNetwork,g)));
		}
		/*
		switch(cellType)
		{
			case idAfferent:   network.addCell(ntAfferent, chromoNetwork[g].getValue()); break;
			case idEfferent:   network.addCell(ntEfferent, chromoNetwork[g].getValue() == 0 ? 0.001 : chromoNetwork[g].getValue()); break;
			case idStandard:   network.addCell(ntStandard, chromoNetwork[g].getValue() == 0 ? 0.001 : chromoNetwork[g].getValue()); break;
			case idModulatory: network.addCell(ntModulatory, chromoNetwork[g].getValue() == 0 ? 0.001 : chromoNetwork[g].getValue()); break;
			default: break;
		}
		//*/

		g++;
		if(cellType > 3) continue;

		wParam.push_back(ioParam);
		currNeuron++;
		if(g >= chromoSize) break;

		GeneID currId = getGeneID(getGeneBlock(chromoNetwork,g));
		while(currId == idInput || currId == idOutput)
		{
			//*
			if(/*cellType*/network.getCellType(currNeuron) == /*idAfferent*/ntAfferent)
				wParam[currNeuron].outputs.push_back(getGeneBlock(chromoNetwork,g));
			else if(/*cellType*/network.getCellType(currNeuron) == /*idEfferent*/ntEfferent)
				wParam[currNeuron].inputs.push_back(getGeneBlock(chromoNetwork,g));
			else
			{
				if(wParam[currNeuron].inputs.size() == 0)
					wParam[currNeuron].inputs.push_back(getGeneBlock(chromoNetwork,g));
				else
					wParam[currNeuron].outputs.push_back(getGeneBlock(chromoNetwork,g));
			}
			//*/
			/*
			if(currId == idInput && cellType != idAfferent)
				wParam[currNeuron].inputs.push_back(chromoNetwork[g].getValue());
			else if(currId == idOutput && cellType != idEfferent)
				wParam[currNeuron].outputs.push_back(chromoNetwork[g].getValue());
			//*/
			g++;
			if(g >= chromoSize) break;
			currId = getGeneID(getGeneBlock(chromoNetwork,g));
		}

		/*
		cout << "Neuron: " << currNeuron << " : " << cellType << "\nInputs:";
		for(uint l = 0; l < wParam[currNeuron].inputs.size(); l++)
			cout << wParam[currNeuron].inputs[l] << ", ";
		cout << endl << "Outputs: ";
		for(uint l = 0; l < wParam[currNeuron].outputs.size(); l++)
			cout << wParam[currNeuron].outputs[l] << ", ";
		cout << endl;
		*/

	}while(g < chromoSize);

	//for each output neuron
	uint nNeurons = wParam.size();
	//cout << "Num.Neurons: " << nNeurons << endl;
	for(uint on = 0; on < nNeurons; on++)
	{
		//for each output neuron output
		uint nOutputs = (wParam[on].outputs.size() >= 1) ? 1 : 0; // teste para apenas uma entrada e uma saída por neurônio
		for(uint o = 0; o < nOutputs; o++)
		{
			//for each input neuron
			for(uint in = 0; in < nNeurons; in++)
			{
				//for each input neuron input
				uint nInputs = (wParam[in].inputs.size() >= 1) ? 1 : 0; // teste para apenas uma entrada e uma saída por neurônio
				for(uint i = 0; i < nInputs; i++)
				{
					//cout << on << " -> " << in << endl;
					float weight = computeWeight(wParam[on].outputs[o], wParam[in].inputs[i]);
					network.addSynapse(on, in, weight);
				}
			}
		}
	}

	/*
	if(network.getNumberOfInputs() >= 3 && network.getNumberOfOutputs() >= 2)
	{
	ofstream brainf("brain.txt");
	brainf << network << endl << chromoNetwork;
	brainf.close();
	exit(0);
	}
	*/

	return network;
}

void GeneticInterface::setPhenotype(Individual individual, NeuromodulatedNetwork& network)
{
	network = buildNetwork(individual[0], individual[1], network);
}
