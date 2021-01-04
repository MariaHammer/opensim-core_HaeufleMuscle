#ifndef OPENSIM_Haeufle2014Muscle_h__
#define OPENSIM_Haeufle2014Muscle_h__
/* -------------------------------------------------------------------------- *
 *                  OpenSim:  Haeufle2014Muscle.h                  *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2017 Stanford University and the Authors                *
 * Author(s): Matthew Millard, Tom Uchida, Ajay Seth                          *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */
#include <OpenSim/Actuators/osimActuatorsDLL.h>
#include <simbody/internal/common.h>

// The parent class, Muscle.h, provides
//    1. max_isometric_force
//    2. optimal_fiber_length
//    3. tendon_slack_length
//    4. pennation_angle_at_optimal
//    5. max_contraction_velocity
//    6. ignore_tendon_compliance
//    7. ignore_activation_dynamics
#include <OpenSim/Simulation/Model/Muscle.h>

// Sub-models used by this muscle model
// TODO add Rockenfeller activation dynamic model
#include <OpenSim/Actuators/MuscleFirstOrderActivationDynamicModel.h>
#include <OpenSim/Actuators/MuscleFixedWidthPennationModel.h>

// TODO ersetzen durch F_isom, F_CE,c und F_CE,e, F_PEE, F_SEE, F_SDE
#include <OpenSim/Actuators/HaeufleActiveForceLengthCurve.h>
#include <OpenSim/Actuators/HaeufleForceVelocityCurve.h>
#include <OpenSim/Actuators/HaeufleFiberForceLengthCurve.h>
#include <OpenSim/Actuators/HaeufleTendonForceLengthCurve.h>
#include <OpenSim/Actuators/HaeufleTendonDampingCurve.h>

#ifdef SWIG
    #ifdef OSIMACTUATORS_API
        #undef OSIMACTUATORS_API
        #define OSIMACTUATORS_API
    #endif
#endif

namespace OpenSim {

//==============================================================================
//                         Haeufle2014Muscle
//==============================================================================
/**
This class implements a configurable muscle model, as described in
Haufle et al.\ (2013). 

TODO: Add more description

<B>Reference</B>

D.F.B. Haeufle, M. Guenther, A. Bayer, S. Schmitt (2014) Hill-type muscle model 
with serial damping and eccentric force–velocity relation. Journal of
Biomechanics https://doi.org/10.1016/j.jbiomech.2014.02.009.

@author Maria Hammer
@author Mike Spahr
*/

class OSIMACTUATORS_API Haeufle2014Muscle : public Muscle {
OpenSim_DECLARE_CONCRETE_OBJECT(Haeufle2014Muscle, Muscle);
public:
//==============================================================================
// PROPERTIES
//==============================================================================
    OpenSim_DECLARE_PROPERTY(fiber_damping, double,
        "The linear damping of the fiber.");
    OpenSim_DECLARE_PROPERTY(default_activation, double,
        "Assumed initial activation level if none is assigned.");
    OpenSim_DECLARE_PROPERTY(default_fiber_length, double,
        "Assumed initial fiber length if none is assigned.");
    OpenSim_DECLARE_PROPERTY(activation_time_constant, double,
        "Activation time constant (in seconds).");
    OpenSim_DECLARE_PROPERTY(deactivation_time_constant, double,
        "Deactivation time constant (in seconds).");
    OpenSim_DECLARE_PROPERTY(minimum_activation, double,
        "Activation lower bound.");
    OpenSim_DECLARE_PROPERTY(maximum_pennation_angle, double,
        "Maximum pennation angle (in radians).");
    OpenSim_DECLARE_UNNAMED_PROPERTY(ActiveForceLengthCurve,
        "Active-force-length curve.");
    OpenSim_DECLARE_UNNAMED_PROPERTY(ForceVelocityCurve,
        "Force-velocity curve.");
    OpenSim_DECLARE_UNNAMED_PROPERTY(FiberForceLengthCurve,
        "Passive-force-length curve.");
    OpenSim_DECLARE_UNNAMED_PROPERTY(TendonForceLengthCurve,
        "Tendon-force-length curve.");

//==============================================================================
// OUTPUTS
//==============================================================================
    OpenSim_DECLARE_OUTPUT(passive_fiber_elastic_force, double,
            getPassiveFiberElasticForce, SimTK::Stage::Dynamics);
    OpenSim_DECLARE_OUTPUT(passive_fiber_elastic_force_along_tendon, double,
            getPassiveFiberElasticForceAlongTendon, SimTK::Stage::Dynamics);
    OpenSim_DECLARE_OUTPUT(passive_fiber_damping_force, double,
            getPassiveFiberDampingForce, SimTK::Stage::Dynamics);
    OpenSim_DECLARE_OUTPUT(passive_fiber_damping_force_along_tendon, double,
            getPassiveFiberDampingForceAlongTendon, SimTK::Stage::Dynamics);

//==============================================================================
// CONSTRUCTORS
//==============================================================================
    /** Default constructor. Produces a non-functional empty muscle. */
    Haeufle2014Muscle();

    /** Constructs a functional muscle using default curves and activation model
    parameters. The tendon is assumed to be elastic, full fiber dynamics are
    solved, and activation dynamics are included.
        @param aName The name of the muscle.
        @param aMaxIsometricForce The force generated by the muscle when fully
    activated at its optimal resting length with a contraction velocity of zero.
        @param aOptimalFiberLength The optimal length of the muscle fiber.
        @param aTendonSlackLength The resting length of the tendon.
        @param aPennationAngle The angle of the fiber (in radians) relative to
    the tendon when the fiber is at its optimal resting length. */
    Haeufle2014Muscle(const  std::string &aName,
                                 double aMaxIsometricForce,
                                 double aOptimalFiberLength,
                                 double aTendonSlackLength,
                                 double aPennationAngle);

//==============================================================================
// GET METHODS
//==============================================================================
    /** @returns A boolean indicating whether fiber damping is being used. */
    bool getUseFiberDamping() const;

    /** @returns The fiber damping coefficient. */
    double getFiberDamping() const;

    /** @returns The default activation level that is used as an initial
    condition if none is provided by the user. */
    double getDefaultActivation() const;

    /** @returns The default fiber length that is used as an initial condition
    if none is provided by the user. */
    double getDefaultFiberLength() const;

    /** @returns The activation time constant (in seconds). */
    double getActivationTimeConstant() const;

    /** @returns The deactivation time constant (in seconds). */
    double getDeactivationTimeConstant() const;

    /** @returns The minimum activation level permitted by the muscle model.
    Note that this equilibrium model, like all equilibrium models, has a
    singularity when activation approaches 0, which means that a non-zero lower
    bound is required. */
    double getMinimumActivation() const;

    /** @returns The ActiveForceLengthCurve used by this model. */
    const ActiveForceLengthCurve& getActiveForceLengthCurve() const;
    /** @returns The ForceVelocityCurve used by this model. */
    const ForceVelocityCurve& getForceVelocityCurve() const;
    /** @returns The FiberForceLengthCurve used by this model. */
    const FiberForceLengthCurve& getFiberForceLengthCurve() const;
    /** @returns The TendonForceLengthCurve used by this model. */
    const TendonForceLengthCurve& getTendonForceLengthCurve() const;

    /** @returns The MuscleFixedWidthPennationModel owned by this model. */
    const MuscleFixedWidthPennationModel& getPennationModel() const;

    /** @returns The MuscleFirstOrderActivationDynamicModel owned by this
    model. */
    const MuscleFirstOrderActivationDynamicModel& getActivationModel() const;

    /** @returns The minimum fiber length, which is the maximum of two values:
    the smallest fiber length allowed by the pennation model, and the minimum
    fiber length on the active-force-length curve. When the fiber reaches this
    length, it is constrained to this value until the fiber velocity becomes
    positive. */
    double getMinimumFiberLength() const;

    /** @returns The minimum fiber length along the tendon, which is the maximum
    of two values: the smallest fiber length along the tendon permitted by the
    pennation model, and the minimum fiber length along the tendon on the
    active-force-length curve. When the fiber length reaches this value, it is
    constrained to this length along the tendon until the fiber velocity becomes
    positive. */
    double getMinimumFiberLengthAlongTendon() const;

    /** @param s The state of the system.
        @returns The normalized force term associated with the tendon element,
    \f$\mathbf{f}_{SE}(\hat{l}_{T})\f$, in the equilibrium equation. */
    double getTendonForceMultiplier(SimTK::State& s) const;

    /** @returns The stiffness of the muscle fibers along the tendon (N/m). */
    double getFiberStiffnessAlongTendon(const SimTK::State& s) const;

    /** @param s The state of the system.
    @returns The velocity of the fiber (m/s). */
    double getFiberVelocity(const SimTK::State& s) const;

    /** @param s The state of the system.
    @returns The time derivative of activation. */
    double getActivationDerivative(const SimTK::State& s) const;

    /** get the portion of the passive fiber force generated by the elastic
        element only (N) */
    double getPassiveFiberElasticForce(const SimTK::State& s) const;
    /** get the portion of the passive fiber force generated by the elastic
        element only, projected onto the tendon direction (N) */
    double getPassiveFiberElasticForceAlongTendon(const SimTK::State& s) const;
    /** get the portion of the passive fiber force generated by the damping
        element only (N) */
    double getPassiveFiberDampingForce(const SimTK::State& s) const;
    /** get the portion of the passive fiber force generated by the damping
        element only, projected onto the tendon direction (N) */
    double getPassiveFiberDampingForceAlongTendon(const SimTK::State& s) const;

//==============================================================================
// SET METHODS
//==============================================================================
    /** @param ignoreTendonCompliance Use a rigid (true) or elastic tendon.
        @param ignoreActivationDynamics Treat the excitation input as the
    activation signal (true) or use a first-order activation dynamic model.
        @param dampingCoefficient Specify the amount of damping to include in
    the model (must be either 0 or greater than 0.001). */
    void setMuscleConfiguration(bool ignoreTendonCompliance,
                                bool ignoreActivationDynamics,
                                double dampingCoefficient);

    /** @param dampingCoefficient Define the fiber damping coefficient. */
    void setFiberDamping(double dampingCoefficient);

    /** @param activation The default activation level that is used to
    initialize the muscle. */
    void setDefaultActivation(double activation);

    /** @param s The state of the system.
        @param activation The desired activation level. */
    void setActivation(SimTK::State& s, double activation) const override;

    /** @param fiberLength The default fiber length that is used to initialize
    the muscle. */
    void setDefaultFiberLength(double fiberLength);

    /** @param activationTimeConstant The activation time constant (in
    seconds). */
    void setActivationTimeConstant(double activationTimeConstant);

    /** @param deactivationTimeConstant The deactivation time constant (in
    seconds). */
    void setDeactivationTimeConstant(double deactivationTimeConstant);

    /** @param minimumActivation The minimum permissible activation level. */
    void setMinimumActivation(double minimumActivation);

    /** @param aActiveForceLengthCurve The ActiveForceLengthCurve used by the
    muscle model to scale active fiber force as a function of fiber length. */
    void setActiveForceLengthCurve(
        ActiveForceLengthCurve& aActiveForceLengthCurve);

    /** @param aForceVelocityCurve The ForceVelocityCurve used by the muscle
    model to calculate the derivative of fiber length. */
    void setForceVelocityCurve(ForceVelocityCurve& aForceVelocityCurve);

    /** @param aFiberForceLengthCurve The FiberForceLengthCurve used by the
    muscle model to calculate the passive force the muscle fiber generates as a
    function of fiber length. */
    void setFiberForceLengthCurve(
        FiberForceLengthCurve& aFiberForceLengthCurve);
    
    /** @param aTendonForceLengthCurve The TendonForceLengthCurve used by the
    muscle model to calculate the force exerted by the tendon as a function of
    tendon length. */
    void setTendonForceLengthCurve(
        TendonForceLengthCurve& aTendonForceLengthCurve);

    /** @param[out] s The state of the system.
        @param fiberLength The desired fiber length (m). */
    void setFiberLength(SimTK::State& s, double fiberLength) const;

//==============================================================================
// MUSCLE.H INTERFACE
//==============================================================================
    /** @param[in] s The state of the system.
        @returns The tensile force the muscle is generating (N). */
    double computeActuation(const SimTK::State& s) const override final;

    /** Computes the fiber length such that the fiber and tendon are developing
    the same force, distributing the velocity of the entire musculotendon
    actuator between the fiber and tendon according to their relative
    stiffnesses.
        @param[in,out] s The state of the system.
        @throws MuscleCannotEquilibrate
    */
    void computeInitialFiberEquilibrium(SimTK::State& s) const override {
        computeFiberEquilibrium(s, false);
    }

    /** Computes the fiber length such that the fiber and tendon are developing
        the same force, either assuming muscle-tendon velocity as provided
        by the state or zero as designated by the useZeroVelocity flag.
        @param[in,out] s         The state of the system.
        @param solveForVelocity  Flag indicating to solve for fiber velocity,
                                 which by default is false (zero fiber-velocity)
        @throws MuscleCannotEquilibrate
    */
    void computeFiberEquilibrium(SimTK::State& s, 
                                 bool solveForVelocity = false) const;

//==============================================================================
// DEPRECATED
//==============================================================================
    ///@cond DEPRECATED
    /*  Once the ignore_tendon_compliance flag is implemented correctly, get rid
    of this method as it duplicates code in calcMuscleLengthInfo,
    calcFiberVelocityInfo, and calcMuscleDynamicsInfo.
        @param activation of the muscle [0-1]
        @param fiberLength in (m)
        @param fiberVelocity in (m/s)
        @returns the force component generated by the fiber that is associated
    only with activation (the parallel element is not included) */
    double calcActiveFiberForceAlongTendon(double activation,
                                           double fiberLength,
                                           double fiberVelocity) const;

    /*  @returns a vector of the fiber state that will cause this muscle to
    reproduce the supplied boundary conditions
        \li 0: activation
        \li 1: normalized fiber length (m/lopt)
        \li 2: pennation angle (rad)
        \li 3: normalized fiber velocity ( (m/s) / (vmax * lopt))
        @param lengthMT the length of the entire musculotendon path (m)
        @param velocityMT the velocity of the entire musculotendon path (m)
        @param tendonForce the tensile (+) force applied by the tendon (N)
        @param dTendonForceDT the first time derivative of the force applied by
    the tendon (N/s) */
    SimTK::Vec4 calcFiberStateGivenBoundaryCond(double lengthMT,
                                                double velocityMT,
                                                double tendonForce,
                                                double dTendonForceDT) const;

    /*  @returns the active fiber force generated by a rigid tendon equilibrium
    muscle model
        @param s the state of the system
        @param aActivation the activation of the muscle */
    double calcInextensibleTendonActiveFiberForce(SimTK::State& s,
                                        double aActivation) const override
        final;
    ///@endcond

    /** Adjust the properties of the muscle after the model has been scaled. The
        optimal fiber length and tendon slack length are each multiplied by the
        ratio of the current path length and the path length before scaling. */
    void extendPostScale(const SimTK::State& s,
                         const ScaleSet& scaleSet) override;

//==============================================================================
// PROTECTED METHODS
//==============================================================================
protected:

    /** Gets the derivative of an actuator state by index.
        @param s The state.
        @param aStateName The name of the state to get.
        @return The value of the state derivative. */
    double getStateVariableDeriv(const SimTK::State& s,
                                 const std::string &aStateName) const;

    /** Sets the derivative of an actuator state specified by name.
        @param s The state.
        @param aStateName The name of the state to set.
        @param aValue The value to which the state should be set. */
    void setStateVariableDeriv(const SimTK::State& s,
                               const std::string &aStateName,
                               double aValue) const;

//==============================================================================
// MUSCLE INTERFACE REQUIREMENTS
//==============================================================================
    /** Calculate the position-related values associated with the muscle state
    (fiber and tendon lengths, normalized lengths, pennation angle, etc.). */
    void calcMuscleLengthInfo(const SimTK::State& s,
                              MuscleLengthInfo& mli) const override;

    /** Calculate the velocity-related values associated with the muscle state
    (fiber and tendon velocities, normalized velocities, pennation angular
    velocity, etc.). */
    void calcFiberVelocityInfo(const SimTK::State& s,
                               FiberVelocityInfo& fvi) const override;

    /** Calculate the dynamics-related values associated with the muscle state
    (from the active- and passive-force-length curves, the force-velocity curve,
    and the tendon-force-length curve). The last entry is a SimTK::Vector
    containing the passive conservative (elastic) fiber force and the passive
    non-conservative (damping) fiber force. */
    void calcMuscleDynamicsInfo(const SimTK::State& s,
                                MuscleDynamicsInfo& mdi) const override;

    /** Calculate the potential energy values associated with the muscle */
    void  calcMusclePotentialEnergyInfo(const SimTK::State& s, 
            MusclePotentialEnergyInfo& mpei) const override;

//==============================================================================
// MODELCOMPONENT INTERFACE REQUIREMENTS
//==============================================================================
    /** Sets up the ModelComponent from the model, if necessary */
    void extendConnectToModel(Model& model) override;

    /** Creates the ModelComponent so that it can be used in simulation */
    void extendAddToSystem(SimTK::MultibodySystem& system) const override;

    /** Initializes the state of the ModelComponent */
    void extendInitStateFromProperties(SimTK::State& s) const override;

    /** Sets the default state for the ModelComponent */
    void extendSetPropertiesFromState(const SimTK::State& s) override;

    /** Computes state variable derivatives */
    void computeStateVariableDerivatives(const SimTK::State& s) const override;

private:
    // The name used to access the activation state.
    static const std::string STATE_ACTIVATION_NAME;
    // The name used to access the fiber length state.
    static const std::string STATE_FIBER_LENGTH_NAME;

    // Indicates whether fiber damping is included in the model (false if
    // dampingCoefficient < 0.001).
    bool use_fiber_damping;

    void setNull();
    void constructProperties();

    // Rebuilds muscle model if any of its properties have changed.
    void extendFinalizeFromProperties() override;

    /* Calculates the fiber velocity that satisfies the equilibrium equation
    given a fixed fiber length.
        @param fiso maximum isometric force
        @param a activation
        @param fal active-force-length multiplier
        @param fpe passive-force-length multiplier
        @param fse tendon-force-length multiplier
        @param beta damping coefficient
        @param cosPhi cosine of pennation angle
        @returns [0] dlceN_dt
                 [1] err
                 [2] converged */
    SimTK::Vec3 calcDampedNormFiberVelocity(double fiso,
                                            double a,
                                            double fal,
                                            double fpe,
                                            double fse,
                                            double beta,
                                            double cosPhi) const;

    /* Calculates the force-velocity multiplier
        @param a activation
        @param fal the fiber active-force-length multiplier
        @param fp the total normalized parallel fiber force multiplier
        @param fse the tendon-force-length multiplier
        @param cosphi the cosine of the pennation angle
        @return the force-velocity multiplier */
    double calcFv(double a,
                  double fal,
                  double fp,
                  double fse,
                  double cosphi) const;

    /*  @param fiso the maximum isometric force the fiber can generate
        @param a activation
        @param fal the fiber active-force-length multiplier
        @param fv the fiber force-velocity multiplier
        @param fpe the fiber force-length multiplier
        @param dlceN the normalized fiber velocity
        @param cosphi the cosine of the pennation angle
        @returns Vec4
            [0] total fiber force
            [1] active fiber force
            [2] conservative passive fiber force
            [3] non-conservative passive fiber force */
    SimTK::Vec4 calcFiberForce(double fiso,
                               double a,
                               double fal,
                               double fv,
                               double fpe,
                               double dlceN) const;

    /*  @param fiso the maximum isometric force the fiber can generate
        @param ftendon the current tendon load
        @param cosPhi the cosine of the pennation angle
        @param fal the fiber active-force-length multiplier
        @param fv the fiber force-velocity multiplier
        @param fpe the fiber force-length multiplier
        @param dlceN the normalized fiber velocity
        @param cosphi the cosine of the pennation angle
        @returns activation required to realize ftendon */
    double calcActivation(double fiso,
                          double ftendon,
                          double cosPhi,
                          double fal,
                          double fv,
                          double fpe,
                          double dlceN) const;

    /*  @param fiso the maximum isometric force the fiber can generate
        @param a activation
        @param fal the fiber active-force-length multiplier
        @param fv the fiber force-velocity multiplier
        @param fpe the fiber force-length multiplier
        @param sinphi the sine of the pennation angle
        @param cosphi the cosine of the pennation angle
        @param lce the fiber length
        @param lceN the normalized fiber length
        @param optFibLen the optimal fiber length
        @returns the stiffness of the fiber in the direction of the fiber */
    double calcFiberStiffness(double fiso,
                              double a,
                              double fv,
                              double lceN,
                              double optFibLen) const;

    /*  @param fiso the maximum isometric force the fiber can generate
        @param a activation
        @param fal the fiber active-force-length multiplier
        @param beta damping coefficient
        @param s damping force scaling
        @param ds_d_dlce derivative of the damping scaling w.r.t. fiber length
        @param dlceN_dt normalized fiber velocity (1 is the max. fiber velocity)
        @param the maximum contraction velocity in m/s
        @return the partial derivative of fiber force w.r.t normalized fiber
    velocity */
    double calc_DFiberForce_DNormFiberVelocity(double fiso,
                                               double a,
                                               double fal,
                                               double beta,
                                               double dlceN_dt) const;

    /*  @param fiberForce the force, in Newtons, developed by the fiber
        @param fiberStiffness the stiffness, in N/m, of the fiber
        @param lce the fiber length
        @param sinphi the sine of the pennation angle
        @param cosphi the cosine of the pennation angle
        @returns the partial derivative of fiber force along the tendon with
    respect to small changes in fiber length (in the direction of the fiber) */
    double calc_DFiberForceAT_DFiberLength(double fiberForce,
                                           double fiberStiffness,
                                           double lce,
                                           double sinPhi,
                                           double cosPhi) const;

    /*  @param dFm_d_lce stiffness of the fiber in the direction of the fiber
        @param sinphi the sine of the pennation angle
        @param cosphi the cosine of the pennation angle
        @param lce the fiber length
        @returns the stiffness of the fiber in the direction of the tendon */
    double calc_DFiberForceAT_DFiberLengthAT(double dFm_d_lce,
                                             double sinPhi,
                                             double cosPhi,
                                             double lce) const;

    /*  @param dFt_d_tl the partial derivative of tendon force w.r.t. small
    changes in tendon length (i.e., tendon stiffness), in (N/m)
        @param lce the fiber length
        @param sinphi the sine of the pennation angle
        @param cosphi the cosine of the pennation angle
        @returns the partial derivative of tendon force with respect to small
    changes in fiber length */
    double calc_DTendonForce_DFiberLength(double dFt_d_tl,
                                          double lce,
                                          double sinphi,
                                          double cosphi) const;

//==============================================================================
// PRIVATE UTILITY CLASS MEMBERS
//==============================================================================

    // Subcomponents owned by the muscle. The properties of these subcomponents
    // are set in extendFinalizeFromProperties() from the properties of the
    // muscle.
    MemberSubcomponentIndex penMdlIdx{
      constructSubcomponent<MuscleFixedWidthPennationModel>("penMdl") };
    MemberSubcomponentIndex actMdlIdx{
      constructSubcomponent<MuscleFirstOrderActivationDynamicModel>("actMdl") };

    // Singularity-free inverse of ForceVelocityCurve.
    ForceVelocityInverseCurve fvInvCurve;

    // Here, I'm using the 'm_' to prevent me from trashing this variable with a
    // poorly chosen local variable.
    double m_minimumFiberLength;
    double m_minimumFiberLengthAlongTendon;

    // Returns true if the fiber length is currently shorter than the minimum
    // value allowed by the pennation model and the active force length curve
    bool isFiberStateClamped(double lce, double dlceN) const;

    // Returns the maximum of the minimum fiber length and the current fiber
    // length.
    double clampFiberLength(double lce) const;

    // Status flag returned by estimateMuscleFiberState().
    enum StatusFromEstimateMuscleFiberState {
        Success_Converged,
        Warning_FiberAtLowerBound,
        Failure_MaxIterationsReached
    };

    // Associative array of values returned by estimateMuscleFiberState():
    // solution_error, iterations, fiber_length, fiber_velocity, and
    // tendon_force.
    typedef std::map<std::string, double> ValuesFromEstimateMuscleFiberState;

    /* Solves fiber length and velocity to satisfy the equilibrium equations.
    The velocity of the entire musculotendon actuator is shared between the
    tendon and the fiber based on their relative mechanical stiffnesses.

    @param aActivation the initial activation of the muscle
    @param pathLength length of the whole musculotendon actuator
    @param pathLengtheningSpeed lengthening speed of the muscle path
    @param aSolTolerance the desired relative tolerance of the equilibrium
           solution
    @param aMaxIterations the maximum number of Newton steps allowed before we
           give up attempting to initialize the model
    @param staticSolution set to true to calculate the static equilibrium
           solution, setting fiber and tendon velocities to zero
    */
    std::pair<StatusFromEstimateMuscleFiberState,
              ValuesFromEstimateMuscleFiberState>
        estimateMuscleFiberState(const double aActivation,
                                 const double pathLength,
                                 const double pathLengtheningSpeed,
                                 const double aSolTolerance,
                                 const int aMaxIterations,
                                 bool staticSolution=false) const;

};
} //end of namespace OpenSim

#endif // OPENSIM_Haeufle2014Muscle_h__
