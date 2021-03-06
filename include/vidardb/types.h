// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef STORAGE_VIDARDB_INCLUDE_TYPES_H_
#define STORAGE_VIDARDB_INCLUDE_TYPES_H_

#include <stdint.h>

namespace vidardb {

// Define all public custom types here.

// Represents a sequence number in a WAL file.
typedef uint64_t SequenceNumber;

}  //  namespace vidardb

#endif //  STORAGE_VIDARDB_INCLUDE_TYPES_H_
