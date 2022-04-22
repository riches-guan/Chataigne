//
// ssl/detail/openssl_types.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_SSL_DETAIL_OPENSSL_TYPES_HPP
#define ASIO_SSL_DETAIL_OPENSSL_TYPES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "../../detail/config.hpp"
#include "../../detail/socket_types.hpp"
#if defined(ASIO_USE_WOLFSSL)
# include <wolfssl/options.h>
#endif // defined(ASIO_USE_WOLFSSL)
#include "../../../../../../common/openssl/include/openssl/opensslconf.h"
#include "../../../../../../common/openssl/include/openssl/ssl.h"
#if !defined(OPENSSL_NO_ENGINE)
# include "../../../../../../common/openssl/include/openssl/engine.h"
#endif // !defined(OPENSSL_NO_ENGINE)
#include "../../../../../../common/openssl/include/openssl/dh.h"
#include "../../../../../../common/openssl/include/openssl/err.h"
#include "../../../../../../common/openssl/include/openssl/rsa.h"
#include "../../../../../../common/openssl/include/openssl/x509.h"
#include "../../../../../../common/openssl/include/openssl/x509v3.h"

#endif // ASIO_SSL_DETAIL_OPENSSL_TYPES_HPP
