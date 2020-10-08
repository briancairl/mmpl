/**
 * @copyright 2020
 * @author Brian Cairl
 *
 * @file crtp.h
 */
#ifndef MMPL_CRTP_H
#define MMPL_CRTP_H

/**
 * @brief Macro used to implement CRTP base class  template
 *
 *        Implements derived class pointer casting messages and makes base
 *        class impossible to instance
 *
 * @param BaseTmpl  CRTP-base template
 * @param DerivedT  CRTP-derived type
 */
#define IMPLEMENT_CRTP_BASE_CLASS(BaseTmpl, DerivedT)                                                                  \
  constexpr DerivedT* derived() { return static_cast<DerivedT*>(this); }                                               \
                                                                                                                       \
  constexpr const DerivedT* derived() const { return static_cast<const DerivedT*>(this); }

#endif  // MMPL_CRTP_H