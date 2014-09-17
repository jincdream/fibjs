// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_SIMPLIFIED_OPERATOR_H_
#define V8_COMPILER_SIMPLIFIED_OPERATOR_H_

#include "src/compiler/machine-type.h"
#include "src/handles.h"

namespace v8 {
namespace internal {

// Forward declarations.
template <class>
class TypeImpl;
struct ZoneTypeConfig;
typedef TypeImpl<ZoneTypeConfig> Type;
class Zone;


namespace compiler {

// Forward declarations.
class Operator;
struct SimplifiedOperatorBuilderImpl;


enum BaseTaggedness { kUntaggedBase, kTaggedBase };

// An access descriptor for loads/stores of fixed structures like field
// accesses of heap objects. Accesses from either tagged or untagged base
// pointers are supported; untagging is done automatically during lowering.
struct FieldAccess {
  BaseTaggedness base_is_tagged;  // specifies if the base pointer is tagged.
  int offset;                     // offset of the field, without tag.
  Handle<Name> name;              // debugging only.
  Type* type;                     // type of the field.
  MachineType machine_type;       // machine type of the field.

  int tag() const { return base_is_tagged == kTaggedBase ? kHeapObjectTag : 0; }
};


// An access descriptor for loads/stores of indexed structures like characters
// in strings or off-heap backing stores. Accesses from either tagged or
// untagged base pointers are supported; untagging is done automatically during
// lowering.
struct ElementAccess {
  BaseTaggedness base_is_tagged;  // specifies if the base pointer is tagged.
  int header_size;                // size of the header, without tag.
  Type* type;                     // type of the element.
  MachineType machine_type;       // machine type of the element.

  int tag() const { return base_is_tagged == kTaggedBase ? kHeapObjectTag : 0; }
};


// If the accessed object is not a heap object, add this to the header_size.
static const int kNonHeapObjectHeaderSize = kHeapObjectTag;


const FieldAccess& FieldAccessOf(const Operator* op) WARN_UNUSED_RESULT;
const ElementAccess& ElementAccessOf(const Operator* op) WARN_UNUSED_RESULT;


// Interface for building simplified operators, which represent the
// medium-level operations of V8, including adding numbers, allocating objects,
// indexing into objects and arrays, etc.
// All operators are typed but many are representation independent.

// Number values from JS can be in one of these representations:
//   - Tagged: word-sized integer that is either
//     - a signed small integer (31 or 32 bits plus a tag)
//     - a tagged pointer to a HeapNumber object that has a float64 field
//   - Int32: an untagged signed 32-bit integer
//   - Uint32: an untagged unsigned 32-bit integer
//   - Float64: an untagged float64

// Additional representations for intermediate code or non-JS code:
//   - Int64: an untagged signed 64-bit integer
//   - Uint64: an untagged unsigned 64-bit integer
//   - Float32: an untagged float32

// Boolean values can be:
//   - Bool: a tagged pointer to either the canonical JS #false or
//           the canonical JS #true object
//   - Bit: an untagged integer 0 or 1, but word-sized
class SimplifiedOperatorBuilder FINAL {
 public:
  explicit SimplifiedOperatorBuilder(Zone* zone);

  const Operator* BooleanNot();

  const Operator* NumberEqual();
  const Operator* NumberLessThan();
  const Operator* NumberLessThanOrEqual();
  const Operator* NumberAdd();
  const Operator* NumberSubtract();
  const Operator* NumberMultiply();
  const Operator* NumberDivide();
  const Operator* NumberModulus();
  const Operator* NumberToInt32();
  const Operator* NumberToUint32();

  const Operator* ReferenceEqual(Type* type);

  const Operator* StringEqual();
  const Operator* StringLessThan();
  const Operator* StringLessThanOrEqual();
  const Operator* StringAdd();

  const Operator* ChangeTaggedToInt32();
  const Operator* ChangeTaggedToUint32();
  const Operator* ChangeTaggedToFloat64();
  const Operator* ChangeInt32ToTagged();
  const Operator* ChangeUint32ToTagged();
  const Operator* ChangeFloat64ToTagged();
  const Operator* ChangeBoolToBit();
  const Operator* ChangeBitToBool();

  const Operator* LoadField(const FieldAccess&);
  const Operator* StoreField(const FieldAccess&);
  const Operator* LoadElement(const ElementAccess&);
  const Operator* StoreElement(const ElementAccess&);

 private:
  Zone* zone() const { return zone_; }

  const SimplifiedOperatorBuilderImpl& impl_;
  Zone* const zone_;

  DISALLOW_COPY_AND_ASSIGN(SimplifiedOperatorBuilder);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_SIMPLIFIED_OPERATOR_H_
