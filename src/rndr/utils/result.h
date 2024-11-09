/**
 * @file result.h
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright © 2024 Minimal. All rights reserved.
 */

#ifndef RNDR_RESULT_H_
#define RNDR_RESULT_H_

#include <iostream>
#include <string>
inline std::variant<bool, bool> v;

namespace rndr {
struct unexpected {
  unexpected(std::string message) : msg(message)
  {
  }
  std::string msg;
};

class ResultBase {
public:
  virtual std::string message() const = 0;

  bool                ok() const
  {
    return success_;
  }

  operator bool() const
  {
    return ok();
  }

protected:
  ResultBase(bool success) : success_(success)
  {
  }

  bool success_;
};

template <typename T>
class expected final : public ResultBase {

public:
  using type = T;

  expected(T value) : ResultBase(true), value_(std::in_place_index<1>, value)
  {
  }

  expected(unexpected failure)
      : ResultBase(false), value_(std::in_place_index<0>, failure.msg)
  {
  }

  std::string message() const override
  {
    return value_.index() == 0 ? std::get<0>(value_) : "";
  }

  T &operator*()
  {
    return std::get<1>(value_);
  }

private:
  std::variant<std::string, T> value_;
};

template <>
class expected<void> final : public ResultBase {

public:
  using type = void;

  expected() : ResultBase(true)
  {
  }

  expected(unexpected failure) : ResultBase(false), message_(failure.msg)
  {
  }

  std::string message() const override
  {
    return message_;
  }

private:
  std::string message_;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const expected<T> &result)
{
  const char *msg = result.ok() ? "Succeeded" : "Failed";
  os << msg;
  const auto message = result.message();
  if (!result.ok()) {
    if (!message.empty()) {
      os << " with message: " << message;
    }
    else {
      os << " with no message";
    }
  }
  return os;
}

using result = expected<void>;

} // namespace rndr
#endif
