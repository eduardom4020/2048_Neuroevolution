#include "neuron.h"

//state, time;

//Neuron =======================================================================
Neuron::Neuron(NeuronType type, float internal)
	   :type(type)
{
	this->state = 0;
	this->time = 0;
	this->gain = 1.0;

	if(type == ntAfferent)
		this->gain = internal;
	else
		this->time = internal;
}

Neuron::Neuron(const Neuron& other)
{
	this->type  = other.type;
	this->state = other.state;
	this->gain = other.gain;
	this->time = other.time;
}

Neuron::~Neuron()
{

}

Neuron& Neuron::operator=(const Neuron &other)
{
	if (this == &other)
    	return *this;

	this->type  = other.type;
	this->state = other.state;
	this->gain = other.gain;
	this->time = other.time;

	return *this;
}

NeuronType Neuron::getType()
{
	return type;
}

float Neuron::getGain()
{
	return gain;
}

//End Neuron ===================================================================

//Interneuron ==================================================================
Interneuron::Interneuron(bool modulatory, float time)
		    :Neuron(modulatory ? ntModulatory : ntStandard, time)
{
}

Interneuron::Interneuron(const Interneuron& other)
			:Neuron(other.type)
{
	this->state = other.state;
	this->time = other.time;
}

Interneuron::~Interneuron()
{

}

Interneuron& Interneuron::operator=(const Interneuron &other)
{
	if (this == &other)
    	return *this;

	this->type = other.type;
	this->state = other.state;
	this->time = other.time;

	return *this;
}
//End Interneuron ==============================================================
