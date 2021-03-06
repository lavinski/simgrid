/* Copyright (c) 2004-2021. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef SIMGRID_KERNEL_RESOURCE_RESOURCE_HPP
#define SIMGRID_KERNEL_RESOURCE_RESOURCE_HPP

#include "src/kernel/lmm/maxmin.hpp" // Constraint
#include <simgrid/forward.h>
#include <xbt/signal.hpp>
#include <xbt/str.h>
#include <xbt/utility.hpp>

#include <string>

namespace simgrid {
namespace kernel {
namespace resource {

/** @ingroup SURF_interface
 * @brief SURF resource interface class
 * @details This is the ancestor class of every resources in SimGrid, such as links, CPU or disk
 */
class XBT_PUBLIC Resource {
  std::string name_ = "unnamed";
  bool is_on_       = true;

protected:
  struct Metric {
    double peak;           /**< The peak of the metric, ie its max value */
    double scale;          /**< Current availability of the metric according to the profiles, in [0,1] */
    profile::Event* event; /**< The associated profile event associated to the metric */
  };
  profile::Event* state_event_ = nullptr;

public:
  Resource(const std::string& name) : name_(name){};
  virtual ~Resource() = default;

  /** @brief Get the name of the current Resource */
  const std::string& get_name() const { return name_; }
  /** @brief Get the name of the current Resource */
  const char* get_cname() const { return name_.c_str(); }

  bool operator==(const Resource& other) const { return name_ == other.name_; }

  /** @brief Apply an event of external load event to that resource */
  virtual void apply_event(profile::Event* event, double value) = 0;

  /** @brief Check if the current Resource is used (if it currently serves an action) */
  virtual bool is_used() const = 0;

  /** @brief Check if the current Resource is active */
  virtual bool is_on() const { return is_on_; }
  /** @brief Turn on the current Resource */
  virtual void turn_on() { is_on_ = true; }
  /** @brief Turn off the current Resource */
  virtual void turn_off() { is_on_ = false; }
  /** @brief setup the profile file with states events (ON or OFF). The profile must contain boolean values. */
  virtual void set_state_profile(profile::Profile* profile);
};

template <class AnyResource> class Resource_T : public Resource {
  Model* model_                = nullptr;
  lmm::Constraint* constraint_ = nullptr;

public:
  using Resource::Resource;
  AnyResource* set_model(Model* model)
  {
    model_ = model;
    return static_cast<AnyResource*>(this);
  }

  Model* get_model() const { return model_; }

  AnyResource* set_constraint(lmm::Constraint* constraint)
  {
    constraint_ = constraint;
    return static_cast<AnyResource*>(this);
  }

  lmm::Constraint* get_constraint() const { return constraint_; }

  /** @brief returns the current load due to activities (in flops per second, byte per second or similar)
   *
   * The load due to external usages modeled by profile files is ignored.*/
  virtual double get_load() const { return constraint_->get_usage(); }
};

} // namespace resource
} // namespace kernel
} // namespace simgrid

namespace std {
template <> class hash<simgrid::kernel::resource::Resource> {
public:
  std::size_t operator()(const simgrid::kernel::resource::Resource& r) const
  {
    return (std::size_t)xbt_str_hash(r.get_cname());
  }
};
} // namespace std

#endif
