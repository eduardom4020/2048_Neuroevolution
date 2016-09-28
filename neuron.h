#ifndef NEURON_H
#define NEURON_H

enum NeuronType
{
	ntAfferent, ntEfferent, ntStandard, ntModulatory
};

class Neuron
{
	protected:
		NeuronType type;

	public:
		float time;
		float state;
		float gain;

		Neuron(NeuronType type, float internal = 1.0);
		Neuron(const Neuron& other);
		virtual ~Neuron();

		Neuron& operator=(const Neuron &other);

		NeuronType getType();

		float getGain();
};

class Interneuron : public Neuron
{
	public:
		Interneuron(bool modulatory = false, float time = 1.0);
		Interneuron(const Interneuron& other);
		~Interneuron();

		Interneuron& operator=(const Interneuron &other);
};

#endif // NEURON_H
