// Lattice of labels:
//
// label0   label1    label2
//       \     |      /
//        \    |     /
//         \   |    /
//           bottom
typedef enum label {
    label0 = 0,
    label1 = 1,
    label2 = 2,
    bottom = 3,
} label;

// Functions that make up the trusted computing base (TCB).

extern void noniSetLabelU8( uint8_t*, label);
extern void noniSinkU8( uint8_t*, label);
extern void noniSetLabelI8( int8_t*, label);
extern void noniSinkI8( int8_t*, label);
extern void noniSetLabelU16( uint16_t*, label);
extern void noniSinkU16( uint16_t*, label);
extern void noniSetLabelI16( int16_t*, label);
extern void noniSinkI16( int16_t*, label);
extern void noniSetLabelU32( uint32_t*, label);
extern void noniSinkU32( uint32_t*, label);
extern void noniSetLabelI32( int32_t*, label);
extern void noniSinkI32( int32_t*, label);
extern void noniSetLabelU64( uint64_t*, label);
extern void noniSinkU64( uint64_t*, label);
extern void noniSetLabelI64( int64_t*, label);
extern void noniSinkI64( int64_t*, label);

extern label cc_current_label;
