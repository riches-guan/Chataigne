//
// ip/resolver_query_base.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IP_RESOLVER_QUERY_BASE_HPP
#define ASIO_IP_RESOLVER_QUERY_BASE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "../detail/config.hpp"
#include "../ip/resolver_base.hpp"

#include "../detail/push_options.hpp"

namespace asio {
namespace ip {

/// The resolver_query_base class is used as a base for the
/// basic_resolver_query class templates to provide a common place to define
/// the flag constants.
class resolver_query_base : public resolver_base
{
protected:
  /// Protected destructor to prevent deletion through this type.
  ~resolver_query_base()
  {
  }
};

} // namespace ip
} // namespace asio

#include "../detail/pop_options.hpp"

#endif // ASIO_IP_RESOLVER_QUERY_BASE_HPP