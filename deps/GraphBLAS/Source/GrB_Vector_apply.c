//------------------------------------------------------------------------------
// GrB_Vector_apply: apply a unary operator to a vector
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "GB.h"

GrB_Info GrB_Vector_apply           // w<mask> = accum (w, op(u))
(
    GrB_Vector w,                   // input/output vector for results
    const GrB_Vector mask,          // optional mask for w, unused if NULL
    const GrB_BinaryOp accum,       // optional accum for z=accum(w,t)
    const GrB_UnaryOp op,           // operator to apply to the entries
    const GrB_Vector u,             // first input:  vector u
    const GrB_Descriptor desc       // descriptor for w and mask
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    WHERE ("GrB_Vector_apply (w, mask, accum, op, u, desc)") ;
    RETURN_IF_NULL_OR_UNINITIALIZED (w) ;
    RETURN_IF_UNINITIALIZED (mask) ;
    RETURN_IF_NULL_OR_UNINITIALIZED (u) ;

    // get the descriptor
    GET_DESCRIPTOR (info, desc, C_replace, Mask_comp, ignore0, ignore1) ;

    //--------------------------------------------------------------------------
    // apply the operator; do not transpose
    //--------------------------------------------------------------------------

    return (GB_apply (
        (GrB_Matrix) w,     C_replace,      // w and its descriptor
        (GrB_Matrix) mask,  Mask_comp,      // mask and its descriptor
        accum,                              // optional accum for Z=accum(C,T)
        op,                                 // operator to apply to the entries
        (GrB_Matrix) u,     false)) ;       // u, not transposed
}

