#ifndef MODNET_H
#define MODNET_H

#include <vector>
#include <cmath>
#include "matrix.h"
#include "neuron.h"
#include "util.h"

typedef vector<float>  floatv;
typedef vector<Neuron*> Neuronv;
typedef vector<uint> uintv;

class NeuromodulatedNetwork
{
	private:
		//0 = No plasticity; 1 = modulatory plasticity; 2 = full plasticity
		char plasticity;
		bool modulation;

		//0 = Average; 1 = Hyperbolic Tangent
		char brainOutputLogic;

		bool normalized;

		float step;

		float stimulus;

		float learningRate;
		float plasticityParameters[4];

		Matrix  synapses;
		Neuronv neurons;

		float computeInterneuronInput(uint id);

		void processEfferentNeuron(uint id);
		void processInterneuron(uint id);

		bool updateWeights();

		void normalizeWeights();

		//log ===================
		bool log;
		uintv input_indices;
		uintv modulation_indices;
		floatv* input_log;
		floatv* modulation_log;
		//end log ===============

	public:
	    // modulação falsa desliga plasticidade, facilita evolução
		NeuromodulatedNetwork(char plasticity = 1, bool modulation = false, char brainOutputLogic = 0, float timeStep = 0.001);

		NeuromodulatedNetwork(const NeuromodulatedNetwork& other);

		~NeuromodulatedNetwork();

		NeuromodulatedNetwork& operator=(const NeuromodulatedNetwork &other);
		friend ostream& operator<<(ostream &out, const NeuromodulatedNetwork &individual);

		void writeState(ostream& out);

		void initializeLog();
		floatv* getInputLog(uint &size);
		floatv* getModulationLog(uint &size);

		float getTimeStep();

		uint getNumOfCells();

		uint addCell(Neuron* neuron);

		uint addCell(NeuronType type, float neuronParameter = 1.0);
		bool removeCell(uint id);

		bool addSynapse(uint id1, uint id2, float weight);
		bool removeSynapse(uint id1, uint id2);

		void affectSynapses(float p);

		float getCellState(uint id);
		NeuronType getCellType(uint id);

		uint getNumberOfInputs();
		uint getNumberOfOutputs();

		bool isOutputConnected(uint outNeuron);

		bool setInput(uint inNeuron, float value);
		uint setInput(floatv values);
		void clearInput();

		float  getOutput(uint outNeuron);
		floatv getOutput();
		
		void setStimulus(float stimulus);
		void setLearningRate(float value);
		void setPlasticityParameters(float* values);
		void setPlasticityParameters(float a, float b, float c, float d);

		bool process(float& energywaste);
};

#endif // MODNET_H
