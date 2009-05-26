// Constraint.cpp
// Author: Frank C. Anderson, Ajay Seth
/*
 * Copyright (c)  2006, Stanford University. All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//=============================================================================
// INCLUDES
//=============================================================================
#include "Constraint.h"
#include "SimbodyEngine.h"

//=============================================================================
// STATICS
//=============================================================================
using namespace std;
//using namespace SimTK;
using namespace OpenSim;

//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/**
 * Default constructor.
 */
Constraint::Constraint() 
{
	setNull();
	setupProperties();
}

//_____________________________________________________________________________
/**
 * Destructor.
 */
Constraint::~Constraint()
{
}

//_____________________________________________________________________________
/**
 * Copy constructor.
 *
 * @param aConstraint Constraint to be copied.
 */
Constraint::Constraint(const Constraint &aConstraint) :
   AbstractConstraint(aConstraint)
{
	setNull();
	setupProperties();
	copyData(aConstraint);
}

//_____________________________________________________________________________
/**
 * Copy constructor from an AbstractConstraint.
 *
 * @param aConstraint Constraint to be copied.
 */
Constraint::Constraint(const AbstractConstraint &aConstraint) :
   AbstractConstraint(aConstraint)
{
	setNull();
	setupProperties();
	copyData(aConstraint);
}

//_____________________________________________________________________________
/**
 * Copy this Constraint and return a pointer to the copy.
 * The copy constructor for this class is used.
 *
 * @return Pointer to a copy of this Constraint.
 */
Object* Constraint::copy() const
{
	Constraint *gc = new Constraint(*this);
	return(gc);
}

//=============================================================================
// CONSTRUCTION METHODS
//=============================================================================
//_____________________________________________________________________________
/**
 * Copy data members from one Constraint to another.
 *
 * @param aConstraint Constraint to be copied.
 */
void Constraint::copyData(const Constraint &aConstraint)
{
	_isDisabledProp = aConstraint._isDisabledProp;
	_dynamicsEngine = aConstraint._dynamicsEngine;
	_index = aConstraint._index;
}

//_____________________________________________________________________________
/**
 * Copy data members from an AbstractConstraint to an Constraint.
 *
 * @param aConstraint AbstractConstraint to be copied.
 */
void Constraint::copyData(const AbstractConstraint &aConstraint)
{
	//_isDisabled = aConstraint.getIsDisabled();
}

//_____________________________________________________________________________
/**
 * Set the data members of this Constraint to their null values.
 */
void Constraint::setNull(void)
{
	setType("Constraint");
}

//_____________________________________________________________________________
/**
 * Connect properties to local pointers.
 */
void Constraint::setupProperties(void)
{
	_isDisabledProp.setName("isDisabled");
	_isDisabledProp.setValue(false);
	_propertySet.append(&_isDisabledProp);
}

//_____________________________________________________________________________
/**
 * Perform some set up functions that happen after the
 * object has been deserialized or copied.
 *
 * @param aEngine dynamics engine containing this Constraint.
 */
void Constraint::setup(AbstractDynamicsEngine* aEngine)
{
	// Base class
	AbstractConstraint::setup(aEngine);

	//_engine = dynamic_cast<SimbodyEngine*>(aEngine);
}

void Constraint::initializeState(SimTK::State& completeState)
{
	setIsDisabled(_isDisabledProp.getValueBool(), completeState);
}


AbstractDynamicsEngine* Constraint::getDynamicsEngine() const
{
	return _dynamicsEngine;
}

//=============================================================================
// OPERATORS
//=============================================================================
//_____________________________________________________________________________
/**
 * Assignment operator.
 *
 * @return Reference to this object.
 */
Constraint& Constraint::operator=(const Constraint &aConstraint)
{
	// BASE CLASS
	AbstractConstraint::operator=(aConstraint);

	copyData(aConstraint);

	return(*this);
}

//=============================================================================
// UTILITY
//=============================================================================
//_____________________________________________________________________________
/**
 * Update an existing Constraint with parameter values from a
 * new one, but only for the parameters that were explicitly
 * specified in the XML node.
 *
 * @param aConstraint Constraint to update from
 */
void Constraint::updateFromConstraint(const AbstractConstraint &aConstraint)
{
	setIsDisabled(aConstraint.getIsDisabled());
}

//=============================================================================
// GET AND SET
//=============================================================================
//-----------------------------------------------------------------------------
// DISABLE
//-----------------------------------------------------------------------------

//_____________________________________________________________________________
/**
 * Get whether or not this Constraint is disabled.
 * Simbody multibody system instance is realized every time the isDisabled
 * changes, BUT multiple sets to the same value have no cost.
 *
 * @param isDisabled If true the constraint is disabled; if false the constraint is enabled.
 */
bool Constraint::getIsDisabled() const
{
	return getEngine()->_system->updMatterSubsystem().updConstraint(_index).isDisabled(getEngine()->_s);
}

//_____________________________________________________________________________
/**
 * Set whether or not this Constraint is disabled.
 * Simbody multibody system instance is realized every time the isDisabled
 * changes, BUT multiple sets to the same value have no cost.
 *
 * @param isDisabled If true the constraint is disabled; if false the constraint is enabled.
 */
bool Constraint::setIsDisabled(bool isDisabled) {return setIsDisabled(isDisabled, getEngine()->_s);}
bool Constraint::setIsDisabled(bool isDisabled, SimTK::State& theState)
{
	SimTK::Constraint& simConstraint = getEngine()->_system->updMatterSubsystem().updConstraint(_index);
	bool modelConstraintIsDisabled = simConstraint.isDisabled(getEngine()->_s);

	// Check if we already have the correct enabling of the constraint then do nothing 
	if(isDisabled == modelConstraintIsDisabled)
		return true;

	// Otherwise we have to change the status of the constraint
	if(isDisabled)
		simConstraint.disable(theState);
	else
		simConstraint.enable(theState);
	
	//Update the property accordingly
	_isDisabledProp.setValue(isDisabled);

	return true;
}


//-----------------------------------------------------------------------------
// FORCES
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
/**
 * Ask the constraint for the forces it is imposing on the system
 * Simbody multibody system must be realized to at least position
 * Returns: the bodyForces on those bodies being constrained (constrainedBodies)
 *				a SpatialVec (6 components) describing resulting torque and force
 *			mobilityForces acting along constrained mobilities	
 *
 * @param state State of model
 * @param bodyForcesInParent is a Vector of SpatialVecs contain constraint forces
 * @param mobilityForces is a Vector of forces that act along the constrained
 *         mobilitities associated with this constraint
 */
void Constraint::calcConstraintForces(SimTK::Vector_<SimTK::SpatialVec>& bodyForcesInParent, 
									  SimTK::Vector& mobilityForces)
{
	SimTK::Constraint& simConstraint = getEngine()->_system->updMatterSubsystem().updConstraint(_index);
	simConstraint.calcConstraintForcesFromMultipliers( getEngine()->_s, 
													   simConstraint.getMultipliersAsVector(getEngine()->_s), 
		                                               bodyForcesInParent, mobilityForces);
}