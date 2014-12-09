
/*
    ann  An artificual neural network library
    Copyright (C) 2000  John Darrington

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef LAYER_H
#define LAYER_H

#include "ann.h"
#include "matrix.h"

#include <iostream>

// $Id: layer.h,v 1.10 2003/05/14 05:50:16 john Exp $

/* 
This class hierarchy allows a common interface to layers
within a Multi-Layer Perceptron Neural Network.
The key trick here, is that the input to a layer can be 
the output of another layer, or it can be a set of external
inputs.  Thus, both layers, and external inputs are 
derived from a common abstract base class.
This way makes it easy to construct networks of any number of
layers.
*/

namespace ann {


//  Abstract base class for an activation function
class ActivationFunction { 
public:
  virtual float operator()(float) const = 0;
  virtual void save(std::ostream &os) const = 0;
  virtual void load(std::istream &os) = 0;
};


// A network layer
//
class Nlayer : public virtual InputSource {
public:
   // Create a layer of size dim, where all neurons have 
   // the activation function af

	Nlayer(int dim, const ActivationFunction &af, bool bias=false):activationFunction(af)
	{
		dimension=dim;
		weightMatrix = 0;
		input = 0;
		hasBias = bias;
	}

   // Return a ref to the weight matrix for this layer
   matrix &getWeights() const
     { assert (weightMatrix );
     return *weightMatrix;
     }

   bool bias() const
     { return hasBias ; }

   // Return the output of this layer (input to the next layer)
   //
   const ann::vector getSource() const
{
assert(weightMatrix);
  assert(input);
  vector layerOutput(dimension);

  // first set the output to be the weighted sum
  // of the input values

  ann::vector thebias(0);
  if ( bias()) { 
    thebias.resize(1);
    thebias[0] = 1.0;
  }
  layerOutput = (*weightMatrix) * (input->getSource()).juxtapose(thebias);

  // then apply the activation function for each output
  for(VI i=layerOutput.begin(); i!=layerOutput.end() ; ++i)
    *i = activationFunction(*i);

  return layerOutput;
}


   // destroy the layer
   //
   virtual ~Nlayer()
{
delete weightMatrix;
       weightMatrix = 0;
}

private:
   Nlayer(const Nlayer &l);
   Nlayer &operator=(const Nlayer &);

protected:
   matrix *weightMatrix;
   const InputSource *input;
   const ActivationFunction &activationFunction;
public:
   bool hasBias;
};


class Layer : public Nlayer { 

 public:
   // Create a layer of size dim, where all neurons have 
   // the activation function af
   //
	// Construct a network layer with dim units
  Layer(int dim, const ActivationFunction &af, bool bias=false):Nlayer(dim,af,bias)
  {
	  dimension=dim;
	  weightMatrix = 0;
	  input = 0;
	  hasBias = bias;
  }

   // Set the input source (another layer or external input) for this layer
   //
   void setInput(const InputSource &i)
   {
	   input = &i; 

	// We have to wait until now, before defining the weight matrix for the
	// layer, because until now we didn't know what size it was.
	int inputDim = i.getDimension();
	if (bias()) ++inputDim;
	weightMatrix  = new matrix(dimension,inputDim);

	// Set weights to random values
	weightMatrix->randomise();
   }
};
} // end namespace ann

#endif









