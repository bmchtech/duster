//
//  Basic video functions
//
//! \file tonc_oam.h
//! \author J Vijn
//! \date 20060604 - 20060604
//
// === NOTES ===
// * Basic video-IO, color, background and object functionality

module tonc.tonc_oam;

import tonc.tonc_types;
import tonc.tonc_memmap;
import tonc.tonc_memdef;
import tonc.tonc_core;


extern (C):

// --------------------------------------------------------------------
// OBJECTS
// --------------------------------------------------------------------

//! \addtogroup grpVideoObj
/*!	\{	*/

extern (D) auto OAM_CLEAR()
{
    return memset32(oam_mem, 0, OAM_SIZE / 4);
}

// --- Prototypes -----------------------------------------------------

// --- Full OAM ---
void oam_init (OBJ_ATTR* obj, uint count);
void oam_copy (OBJ_ATTR* dst, const(OBJ_ATTR)* src, uint count);

// --- Obj attr only ---
OBJ_ATTR* obj_set_attr (OBJ_ATTR* obj, ushort a0, ushort a1, ushort a2);
void obj_set_pos (OBJ_ATTR* obj, int x, int y);
void obj_hide (OBJ_ATTR* oatr);
void obj_unhide (OBJ_ATTR* obj, ushort mode);

const(ubyte)* obj_get_size (const(OBJ_ATTR)* obj);
int obj_get_width (const(OBJ_ATTR)* obj);
int obj_get_height (const(OBJ_ATTR)* obj);

void obj_copy (OBJ_ATTR* dst, const(OBJ_ATTR)* src, uint count);
void obj_hide_multi (OBJ_ATTR* obj, uint count);
void obj_unhide_multi (OBJ_ATTR* obj, ushort mode, uint count);

// --- Obj affine only ---
void obj_aff_copy (OBJ_AFFINE* dst, const(OBJ_AFFINE)* src, uint count);

void obj_aff_set (OBJ_AFFINE* oaff, FIXED pa, FIXED pb, FIXED pc, FIXED pd);
void obj_aff_identity (OBJ_AFFINE* oaff);
void obj_aff_scale (OBJ_AFFINE* oaff, FIXED sx, FIXED sy);
void obj_aff_shearx (OBJ_AFFINE* oaff, FIXED hx);
void obj_aff_sheary (OBJ_AFFINE* oaff, FIXED hy);

void obj_aff_rotate (OBJ_AFFINE* oaff, ushort alpha);
void obj_aff_rotscale (OBJ_AFFINE* oaff, FIXED sx, FIXED sy, ushort alpha);
void obj_aff_premul (OBJ_AFFINE* dst, const(OBJ_AFFINE)* src);
void obj_aff_postmul (OBJ_AFFINE* dst, const(OBJ_AFFINE)* src);

void obj_aff_rotscale2 (OBJ_AFFINE* oaff, const(AFF_SRC)* as);
void obj_rotscale_ex (OBJ_ATTR* obj, OBJ_AFFINE* oaff, const(AFF_SRC_EX)* asx);

// inverse (object -> screen) functions, could be useful
// inverses (prototypes)
void obj_aff_scale_inv (OBJ_AFFINE* oa, FIXED wx, FIXED wy);
void obj_aff_rotate_inv (OBJ_AFFINE* oa, ushort theta);
void obj_aff_shearx_inv (OBJ_AFFINE* oa, FIXED hx);
void obj_aff_sheary_inv (OBJ_AFFINE* oa, FIXED hy);

/*!	\}	*/

// --------------------------------------------------------------------
// INLINES
// --------------------------------------------------------------------

/*!	\addtogroup grpVideoObj	*/
/*! \{	*/

//! Set the attributes of an object.
OBJ_ATTR* obj_set_attr (OBJ_ATTR* obj, ushort a0, ushort a1, ushort a2);

//! Set the position of \a obj
void obj_set_pos (OBJ_ATTR* obj, int x, int y);

//! Copies \a count OAM entries from \a src to \a dst.
void oam_copy (OBJ_ATTR* dst, const(OBJ_ATTR)* src, uint count);

//! Hide an object.
void obj_hide (OBJ_ATTR* obj);

//! Unhide an object.
/*! \param obj	Object to unhide.
*	\param mode	Object mode to unhide to. Necessary because this affects
*	  the affine-ness of the object.
*/
void obj_unhide (OBJ_ATTR* obj, ushort mode);

//! Get object's sizes as a byte array
const(ubyte)* obj_get_size (const(OBJ_ATTR)* obj);

//! Get object's width
int obj_get_width (const(OBJ_ATTR)* obj);

//! Gets object's height
int obj_get_height (const(OBJ_ATTR)* obj);

// --- Affine only ---

//! Set the elements of an \a object affine matrix.
void obj_aff_set (OBJ_AFFINE* oaff, FIXED pa, FIXED pb, FIXED pc, FIXED pd);

//! Set an object affine matrix to the identity matrix
void obj_aff_identity (OBJ_AFFINE* oaff);

//! Set an object affine matrix for scaling.
void obj_aff_scale (OBJ_AFFINE* oaff, FIXED sx, FIXED sy);

void obj_aff_shearx (OBJ_AFFINE* oaff, FIXED hx);

void obj_aff_sheary (OBJ_AFFINE* oaff, FIXED hy);

// --- Inverse operations ---

void obj_aff_scale_inv (OBJ_AFFINE* oaff, FIXED wx, FIXED wy);

void obj_aff_rotate_inv (OBJ_AFFINE* oaff, ushort theta);

void obj_aff_shearx_inv (OBJ_AFFINE* oaff, FIXED hx);

void obj_aff_sheary_inv (OBJ_AFFINE* oaff, FIXED hy);

/*! \}	*/

// TONC_OAM
