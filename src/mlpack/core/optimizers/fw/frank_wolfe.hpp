/**
 * @file frank_wolfe.hpp
 * @author Chenzhe Diao
 *
 * Frank-Wolfe Algorithm.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_OPTIMIZERS_FW_FRANK_WOLFE_HPP
#define MLPACK_CORE_OPTIMIZERS_FW_FRANK_WOLFE_HPP


#include <mlpack/prereqs.hpp>
#include "update_classic.hpp"
#include "update_span.hpp"
#include "constr_lpball.hpp"
#include "func_sq.hpp"

namespace mlpack {
namespace optimization {

/**
 * Frank-Wolfe is a technique to minimize a continuously differentiable convex
 * function \f$ f \f$ over a compact convex subset \f$ D \f$ of a vector space. 
 * It is also known as conditional gradient method.
 *
 * To find minimum of a function using Frank-Wolfe in each iteration \f$ k \f$:
 * 1. One optimize the linearized constrained problem, using LinearConstrSolver:
 * \f[
 * s_k:= arg\min_{s\in D} <s_k, \nabla f(x_k)>
 * \f]
 *
 * 2. Update \f$ x \f$ using UpdateRule:
 * \f[
 * x_{k+1} := (1-\gamma) x_k + \gamma s_k
 * \f]
 * for some \f$ \gamma \in (0, 1) \f$, or use Fully-Corrective Variant:
 * \f[
 * x_{k+1}:= arg\min_{x\in conv(s_0, \cdots, s_k)} f(x)
 * \f]
 *
 *
 * The algorithm continues until \f$ k \f$ reaches the maximum number of iterations, 
 * or when the duality gap is bounded by a certain tolerance \f$ \epsilon \f$. 
 * That is,
 *
 * \f[
 * g(x):= \max_{s\in D} <x-s, \nabla f(x)> \quad \leq \epsilon,
 * \f]
 *
 * we also know that \f$ g(x) \geq f(x) - f(x^*) \f$, where \f$ x^* \f$ is the optimal 
 * solution.
 *
 * The parameter \f$ \epsilon \f$ is specified by the tolerance parameter to the
 * constructor.
 *
 * For FrankWolfe to work, FunctionType, LinearConstrSolverType and UpdateRuleType 
 * template parameters are required.
 * These classes must implement the following functions:
 *
 * FunctionType:
 *
 *   double Evaluate(const arma::mat& coordinates);
 *   void Gradient(const arma::mat& coordinates,
 *                 arma::mat& gradient);
 *
 * LinearConstrSolverType:
 *
 *   void Optimize(const arma::mat& gradient,
 *                 arma::mat& s);
 *
 * UpdateRuleType:
 *
 *   void Update(const arma::mat& old_coords,
 *               const arma::mat& s,
 *               arma::mat& new_coords,
 *               const size_t num_iter);
 *
 * @tparam FunctionType Objective function type to be
 *     minimized.
 * @tparam LinearConstrSolverType Solver for the linear constrained problem.
 * @tparam UpdateRuleType Rule to update the solution in each iteration.
 *
 */
template<
    typename LinearConstrSolverType,
    typename UpdateRuleType>
class FrankWolfe
{
 public:
  /**
   * Construct the Frank-Wolfe optimizer with the given function and
   * parameters. Notice that the constraint domain \f$ D \f$ is input
   * at the initialization of linear_constr_solver, the function to be  
   * optimized is stored in update_rule.
   *
   * @param linear_constr_solver Solver for linear constrained problem.
   * @param update_rule Rule for updating solution in each iteration.
   * @param maxIterations Maximum number of iterations allowed (0 means no
   *     limit).
   * @param tolerance Maximum absolute tolerance to terminate algorithm.
   */
  FrankWolfe(const LinearConstrSolverType linear_constr_solver,
             const UpdateRuleType update_rule,
             const size_t maxIterations = 100000,
             const double tolerance = 1e-10);

  /**
   * Optimize the given function using FrankWolfe.  The given starting
   * point will be modified to store the finishing point of the algorithm, and
   * the final objective value is returned.
   *
   * @tparam function Function to be optimized.
   * @param iterate Starting point (will be modified).
   * @return Objective value of the final point.
   */
  template<typename FunctionType>
  double Optimize(FunctionType& function, arma::mat& iterate);

  //! Get the linear constrained solver.
  LinearConstrSolverType LinearConstrSolver()
      const { return linear_constr_solver; }
  //! Modify the linear constrained solver.
  LinearConstrSolverType& LinearConstrSolver() { return linear_constr_solver; }

  //! Get the update rule.
  UpdateRuleType UpdateRule() const { return update_rule; }
  //! Modify the update rule.
  UpdateRuleType& UpdateRule() { return update_rule; }

  //! Get the maximum number of iterations (0 indicates no limit).
  size_t MaxIterations() const { return maxIterations; }
  //! Modify the maximum number of iterations (0 indicates no limit).
  size_t& MaxIterations() { return maxIterations; }

  //! Get the tolerance for termination.
  double Tolerance() const { return tolerance; }
  //! Modify the tolerance for termination.
  double& Tolerance() { return tolerance; }

 private:
  //! The solver for constrained linear problem in first step.
  LinearConstrSolverType linear_constr_solver;

  //! The rule to update, used in the second step.
  UpdateRuleType update_rule;

  //! The maximum number of allowed iterations.
  size_t maxIterations;

  //! The tolerance for termination.
  double tolerance;
};

//! Orthogonal Matching Pursuit.
using OMP = FrankWolfe<ConstrLpBallSolver, UpdateSpan>;

} // namespace optimization
} // namespace mlpack

// Include implementation.
#include "frank_wolfe_impl.hpp"

#endif
