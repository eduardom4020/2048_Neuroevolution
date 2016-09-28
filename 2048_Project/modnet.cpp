#include "modnet.h"

NeuromodulatedNetwork::NeuromodulatedNetwork(char plasticity, bool modulation, char brainOutputLogic, float timeStep)
					  :step(timeStep), stimulus(0), learningRate(0.0)
{
	this->plasticity = plasticity;
	this->modulation = modulation;
	this->brainOutputLogic = brainOutputLogic;
	this->log = false;
	this->input_log = NULL;
	this->modulation_log = NULL;
	this->normalized = false;
	memset(&plasticityParameters[0], 0, 4*sizeof(float));
}

NeuromodulatedNetwork::NeuromodulatedNetwork(const NeuromodulatedNetwork& other)
					  :step(other.step), stimulus(other.stimulus), learningRate(other.learningRate)
{
	this->plasticity = other.plasticity;
	this->modulation = other.modulation;
	this->brainOutputLogic = other.brainOutputLogic;

	this->log = false;
	this->input_log = NULL;
	this->modulation_log = NULL;

	memcpy(&plasticityParameters[0], &(other.plasticityParameters[0]), 4*sizeof(float));

	this->normalized = other.normalized;
	this->synapses = other.synapses;
	for(uint i = 0; i < other.neurons.size(); i++)
	{
		NeuronType ntype = other.neurons[i]->getType();

		Neuron* neuron;

		if(ntype == ntStandard || ntype == ntModulatory)
		{
			if(ntype == ntModulatory) neuron = new Interneuron(true);
			else neuron = new Interneuron();

		}else{
			neuron = new Neuron(ntype);
		}

		neuron->time = other.neurons[i]->time;
		neuron->gain = other.neurons[i]->gain;
		neuron->state = other.neurons[i]->state;

		this->neurons.push_back(neuron);
	}
}

NeuromodulatedNetwork::~NeuromodulatedNetwork()
{
	for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
		delete (*it);

	neurons.clear();

	if(input_log) delete[] input_log;
	if(modulation_log) delete[] modulation_log;
}

NeuromodulatedNetwork& NeuromodulatedNetwork::operator=(const NeuromodulatedNetwork &other)
{
	this->plasticity = other.plasticity;
	this->modulation = other.modulation;
	this->brainOutputLogic = other.brainOutputLogic;

	this->log = false;
	this->input_log = NULL;
	this->modulation_log = NULL;

    if (this == &other)
    	return *this;

	this->step = other.step;
	this->stimulus = other.stimulus;
	this->learningRate = other.learningRate;
	memcpy(&plasticityParameters[0], &(other.plasticityParameters[0]), 4*sizeof(float));

	this->synapses = other.synapses;

	for(uint i = 0; i < this->neurons.size(); i++) delete neurons[i];
	neurons.clear();

	for(uint i = 0; i < other.neurons.size(); i++)
	{
		NeuronType ntype = other.neurons[i]->getType();

		Neuron* neuron;

		if(ntype == ntStandard || ntype == ntModulatory)
		{
			if(ntype == ntModulatory) neuron = new Interneuron(true);
			else neuron = new Interneuron();

		}else{
			neuron = new Neuron(ntype);
		}

		neuron->time = other.neurons[i]->time;
		neuron->gain = other.neurons[i]->gain;
		neuron->state = other.neurons[i]->state;

		this->neurons.push_back(neuron);
	}

    return *this;
}

ostream& operator<<(ostream &out, const NeuromodulatedNetwork &individual)
{
	out << "** Neuromodulated Network **\n";

	out << "Higher Stimulus: " << individual.stimulus << endl;
	out << "Learning Rate: " << individual.learningRate << endl;
	out << "Plasticity Parameters: " << individual.plasticityParameters[0];
	for(uint i = 1; i < 4; i++)
		out << ", " << individual.plasticityParameters[i];
	out << "\nPlasticity: ";
	switch(individual.plasticity)
	{
		case 0: out << "No"; break;
		case 2: out << "Full"; break;
		default: out << "Modulatory"; break;
	}
	out << "\nModulatory: ";
	if(individual.modulation) out << "Yes";
	else out << "No";
	out << "\nOutput Logic: ";
	switch(individual.brainOutputLogic)
	{
		case 1: out << "Hyperbolic Tangent"; break;
		default: out << "Average"; break;
	}

	out << "\nNeurons types: (" << individual.neurons.size() << " neurons)\n";
	for(uint i = 0; i < individual.neurons.size(); i++)
	{
		NeuronType type = individual.neurons[i]->getType();
		switch(type)
		{
			case ntAfferent:   out << "Afferent\t"; break;
			case ntEfferent:   out << "Efferent\t"; break;
			case ntStandard:   out << "Standard\t"; break;
			case ntModulatory: out << "Modulat.\t"; break;
			default: out << "Unknown\t"; break;
		}
	}
	out << endl;

	out.precision(6);
	out.setf(ios::fixed,ios::floatfield);

	for(uint i = 0; i < individual.neurons.size(); i++)
	{
		if(individual.neurons[i]->getType() == ntStandard || individual.neurons[i]->getType() == ntModulatory)
			out << ((Interneuron*)individual.neurons[i])->time << "\t";
		else
			out << "        " << "\t";
	}

	out << endl << endl;
	out << individual.synapses << endl;

	return out;
}

void NeuromodulatedNetwork::writeState(ostream& out)
{
	for(uint i = 0; i < neurons.size(); i++)
	{
		if(i < 10) out << "N  " << i << " (";
		else  out << "N " << i << " (";
		NeuronType t = neurons[i]->getType();
		if(t == ntAfferent) out << "A";
		else if(t == ntEfferent) out << "E";
		else if(t == ntStandard) out << "S";
		else out << "M";
		out.setf ( ios::scientific );
		out.precision(2);
		out.setf(ios::fixed,ios::floatfield);
		if(neurons[i]->state >= 0)
			out << "):  " << neurons[i]->state << " < ";
		else
			out << "): "  << neurons[i]->state << " < ";
		for(uint k = 0; k < synapses.width(); k++)
		{
			if(synapses[k][i] != 0)
			{
				out << k << "(" << synapses[k][i] << "), ";
			}
		}
		out << endl;
	}
}

void NeuromodulatedNetwork::initializeLog()
{
	this->log = true;

	for(uint i = 0; i < neurons.size(); i++)
	{
		if(neurons[i]->getType() == ntAfferent)
			this->input_indices.push_back(i);
		else if(neurons[i]->getType() == ntModulatory)
			this->modulation_indices.push_back(i);
	}

	this->input_log = new floatv[input_indices.size()];
	this->modulation_log = new floatv[modulation_indices.size()];
}

floatv* NeuromodulatedNetwork::getInputLog(uint &size)
{
	size = input_indices.size();
	return input_log;
}

floatv* NeuromodulatedNetwork::getModulationLog(uint &size)
{
	size = modulation_indices.size();
	return modulation_log;
}

float NeuromodulatedNetwork::getTimeStep()
{
	return step;
}

uint NeuromodulatedNetwork::getNumOfCells()
{
	return neurons.size();
}

uint NeuromodulatedNetwork::addCell(Neuron* neuron)
{
	neurons.push_back(neuron);
	synapses.insert_column();
	synapses.insert_row();

	return neurons.size() - 1;
}

uint NeuromodulatedNetwork::addCell(NeuronType type, float neuronParameter)
{
	switch(type)
	{
		case ntStandard:
			neurons.push_back(new Interneuron(false, neuronParameter));
			break;
		case ntModulatory:
			neurons.push_back(new Interneuron(true, neuronParameter));
			break;
		case ntAfferent:
			neurons.push_back(new Neuron(type, ((neuronParameter >= 0) ? 1.0 : -1.0)));
			break;
		default:
			neurons.push_back(new Neuron(type, neuronParameter));
			break;
	}

	synapses.insert_column();
	synapses.insert_row();

	return neurons.size() - 1;
}

bool NeuromodulatedNetwork::removeCell(uint id)
{
	if(id < neurons.size())
	{
		neurons.erase(neurons.begin() + id);
		synapses.erase_column(id);
		synapses.erase_row(id);

		return true;
	}
	return false;
}

bool NeuromodulatedNetwork::addSynapse(uint id1, uint id2, float weight)
{
	uint nNeurons = neurons.size();

	if(id1 < nNeurons && id2 < nNeurons && neurons[id1]->getType() != ntEfferent && neurons[id2]->getType() != ntAfferent)
	{
		synapses[id1][id2] = weight;
		return true;
	}

	return false;
}

bool NeuromodulatedNetwork::removeSynapse(uint id1, uint id2)
{
	uint nNeurons = neurons.size();

	if(id1 < nNeurons && id2 < nNeurons && neurons[id1]->getType() != ntEfferent && neurons[id2]->getType() != ntAfferent)
	{
		synapses[id1][id2] = 0.0;
		return true;
	}

	return false;
}

void NeuromodulatedNetwork::setStimulus(float stimulus)
{
	this->stimulus = stimulus;
}

float NeuromodulatedNetwork::getCellState(uint id)
{
	return neurons[id]->state;
}

NeuronType NeuromodulatedNetwork::getCellType(uint id)
{
	return neurons[id]->getType();
}

uint NeuromodulatedNetwork::getNumberOfInputs()
{
	uint ninputs = 0;

	for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
		if((*it)->getType() == ntAfferent) ninputs++;

	return ninputs;
}

uint NeuromodulatedNetwork::getNumberOfOutputs()
{
	uint ninputs = 0;

	for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
		if((*it)->getType() == ntEfferent) ninputs++;

	return ninputs;
}

bool NeuromodulatedNetwork::isOutputConnected(uint outNeuron)
{
    uint i, nId = 0;

    for(i = 0; i < neurons.size(); i++)
    {
        if(neurons[i]->getType() == ntEfferent)
        {
            if(nId == outNeuron) break;
            nId++;
        }
    }

    for(uint k = 0; k < neurons.size(); k++)
    	if(synapses[k][i] != 0) return true;

	return false;
}

bool NeuromodulatedNetwork::setInput(uint inNeuron, float value)
{
	uint nId = 0;

    for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
    {
        if((*it)->getType() == ntAfferent)
        {
            if(nId == inNeuron)
            {
            	(*it)->state = value;
            	return true;
            }
            nId++;
        }
    }
    return false;

}

uint NeuromodulatedNetwork::setInput(floatv values)
{
	uint valId = 0;

	for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
	{
		if((*it)->getType() == ntAfferent && valId < values.size())
		{
			(*it)->state = values[valId];
			valId++;
		}
	}

	return valId;
}

void NeuromodulatedNetwork::clearInput()
{
	for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
		if((*it)->getType() == ntAfferent)
			(*it)->state = 0;
}

float NeuromodulatedNetwork::getOutput(uint outNeuron)
{
    uint nId = 0;

    for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
    {
        if((*it)->getType() == ntEfferent)
        {
            if(nId == outNeuron) { return (*it)->state;}
            nId++;
        }
    }

    return 0;
}

floatv NeuromodulatedNetwork::getOutput()
{
    floatv states;

    for(Neuronv::iterator it = neurons.begin(); it != neurons.end(); it++)
    {
        //std::cout << "okantes\n";
        if((*it)->getType() == ntEfferent)
        {
            //std::cout << "okdepois\n";
            states.push_back((*it)->state);
        }
    }

    return states;
}

void NeuromodulatedNetwork::setLearningRate(float value)
{
	this->learningRate = value;
}

void NeuromodulatedNetwork::setPlasticityParameters(float* values)
{
	memcpy(&plasticityParameters[0], values, 4*sizeof(float));
}

void NeuromodulatedNetwork::setPlasticityParameters(float a, float b, float c, float d)
{
	plasticityParameters[0] = a;
	plasticityParameters[1] = b;
	plasticityParameters[2] = c;
	plasticityParameters[3] = d;
}

bool NeuromodulatedNetwork::process(float& energywaste)
{
	uint nNeurons = neurons.size();
	energywaste = 0;

	//if(!normalized)
	//normalizeWeights();
	if(log)
	{
		for(uint i = 0; i < input_indices.size(); i++)
		{
			input_log[i].push_back(neurons[input_indices[i]]->state);
		}
	}

	for(uint i = 0; i < nNeurons; i++)
	{
        if(brainOutputLogic == 0 && neurons[i]->getType() == ntEfferent)
        {
            processEfferentNeuron(i);
            if(neurons[i]->state > 100)
				neurons[i]->state = 100;
			else if(neurons[i]->state < -100)
				neurons[i]->state = -100;
        }
        else if(neurons[i]->getType() != ntAfferent)
        {
            processInterneuron(i);
            if(neurons[i]->state > 100)
				neurons[i]->state = 100;
			else if(neurons[i]->state < -100)
				neurons[i]->state = -100;
        }
        energywaste += (neurons[i]->state >= 0) ? neurons[i]->state : -neurons[i]->state;
	}

	if(log)
	{
		for(uint i = 0; i < modulation_indices.size(); i++)
		{
			modulation_log[i].push_back(neurons[modulation_indices[i]]->state);
		}
	}
/*
	ofstream deb("debug1.txt");
	for(uint i = 0; i < nNeurons; i++)
	{
		deb << neurons[i]->state << "\t";
	}
	deb.close();
*/
    bool success = true;
	if(plasticity != 0)
		success = updateWeights();

    return success;
}

float NeuromodulatedNetwork::computeInterneuronInput(uint id)
{
	uint nNeurons = neurons.size();
	float s = 0;

	for(uint i = 0; i < nNeurons; i++)
		if(neurons[i]->getType() != ntModulatory)
			s += tanh((neurons[i]->state*neurons[i]->getGain()))*synapses[i][id]; //inputs of neuron id == column id

	float r = ((stimulus - neurons[id]->state + s)/(neurons[id]->time));

	if(brainOutputLogic == 1)
		if(neurons[id]->getType() == ntEfferent)
			r = ((-neurons[id]->state + s)/(neurons[id]->time));
	//else
	//#endif
	//r = ((stimulus - neurons[id]->state + s)/(neurons[id]->time));

	return r;
}

void NeuromodulatedNetwork::processEfferentNeuron(uint id)
{

	uint nNeurons = neurons.size();

	float s = 0;
	float q = 0;

	for(uint i = 0; i < nNeurons; i++)
	{

		if(neurons[i]->getType() != ntModulatory && synapses[i][id] != 0)
		{
			s += tanh((neurons[i]->state*neurons[i]->getGain()))*synapses[i][id]; //inputs of neuron id == column id
			q++;
		}
	}

	if(q > 0) s = s/q;

	neurons[id]->state = s;

}

void NeuromodulatedNetwork::processInterneuron(uint id)
{
    //4th Order Runge-Kutta ====================================================
	double k1, k2, k3, k4;

    float state = neurons[id]->state;

    k1 = computeInterneuronInput(id);

    neurons[id]->state = state + k1*step/2.0;
    k2 = computeInterneuronInput(id);

    neurons[id]->state = state + k2*step/2.0;
    k3 = computeInterneuronInput(id);

    neurons[id]->state = state + k3*step;
    k4 = computeInterneuronInput(id);

    neurons[id]->state = state + (k1 + 2*k2 + 2*k3 + k4)*step/6.0;
}

void NeuromodulatedNetwork::affectSynapses(float p)
{
	for(uint i = 0; i < synapses.width(); i++)
	{
		if(neurons[i]->getType() == ntAfferent)
		{
			for(uint j = 0; j < synapses.height(); j++)
				if(synapses[i][j] != 0) synapses[i][j] *= p;//util::floatRand(-1.0,1.0);
		}
	}
}

void NeuromodulatedNetwork::normalizeWeights()
{
	normalized = true;

	uint nNeurons = neurons.size();
	float sWeights[nNeurons];

	for(uint j = 0; j < nNeurons; j++)
	{
		sWeights[j] = 0;
		for(uint i = 0; i < nNeurons; i++)
		{
			if(neurons[i]->getType() != ntModulatory) sWeights[j] += synapses[i][j]*synapses[i][j];
		}

	}

	for(uint j = 0; j < nNeurons; j++)
	{
		if(sWeights[j] != 0)
		{
			for(uint i = 0; i < nNeurons; i++)
			{
				if(neurons[i]->getType() != ntModulatory) synapses[i][j] /= sqrt(sWeights[j]);
			}
		}
	}
}

bool NeuromodulatedNetwork::updateWeights()
{
	uint nNeurons = neurons.size();

	vector<uint> modulatoryIndexes;
	vector<uint> nonAfferentIndexes;
	vector<uint> nonModulatingIndexes;

	for(uint n = 0; n < nNeurons; n++)
	{
		if(neurons[n]->getType() != ntAfferent)
		{
			nonAfferentIndexes.push_back(n);
		}

		if(neurons[n]->getType() == ntModulatory)
			modulatoryIndexes.push_back(n);
		else
			nonModulatingIndexes.push_back(n);
	}

	for(uint i = 0; i < nonAfferentIndexes.size(); i++)
	{
		float mod;

		if(plasticity == 2) //full plastic; no modulation
			mod = 1.0;
		else //plastic only with modulatory action
			mod = 0;

		if(modulation)
		{
			//mi = S(wji*oj), j = modulatory neuron, i = standard or modulatory neuron
			for(uint j = 0; j < modulatoryIndexes.size(); j++)
				mod += tanh(neurons[modulatoryIndexes[j]]->state)*synapses[modulatoryIndexes[j]][nonAfferentIndexes[i]];

			mod = tanh(mod);
		}

		if(mod != 0)
		{
			//dwji = tanh(mi/2.0)*n*(a*oj*oi + b*oj + c*oi + d), n = learning rate and a, b, c and d = tunable plasticity parameters, o = neuron output (state), j = non-modulatory neuron, i = non afferent neuron
			for(uint j = 0; j < nonModulatingIndexes.size(); j++)
			{
				//if synapse ji exists
				if(synapses[nonModulatingIndexes[j]][nonAfferentIndexes[i]] != 0)
				{
					float oj = neurons[nonModulatingIndexes[j]]->state;
					float oi = neurons[nonAfferentIndexes[i]]->state;
					float delta = mod;

					delta *= learningRate*(plasticityParameters[0]*oj*oi + plasticityParameters[1]*oj + plasticityParameters[2]*oi + plasticityParameters[3]);

					synapses[nonModulatingIndexes[j]][nonAfferentIndexes[i]] += delta;

					if(isnan(synapses[nonModulatingIndexes[j]][nonAfferentIndexes[i]]) || isinf(synapses[nonModulatingIndexes[j]][nonAfferentIndexes[i]]))
                        return false;

				}
			}
		}
	}

	return true;

}
