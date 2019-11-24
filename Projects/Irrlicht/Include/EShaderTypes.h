#ifndef __E_SHADER_TYPES_H_INCLUDED__
#define __E_SHADER_TYPES_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace video
{

enum E_SHADER_TYPE
{
	EST_VERTEX_SHADER = 0,
	EST_GEOMETRY_SHADER,
	EST_PIXEL_SHADER,
	EST_HULL_SHADER,
	EST_DOMAIN_SHADER,
	EST_COMPUTE_SHADER,

	EST_COUNT
};

//! Compile target enumeration for the addHighLevelShaderMaterial() method.
enum E_VERTEX_SHADER_TYPE
{
	EVST_VS_1_1 = 0,
	EVST_VS_2_0,
	EVST_VS_2_a,
	EVST_VS_3_0,
	EVST_VS_4_0,
	EVST_VS_4_1,
	EVST_VS_5_0,

	//! This is not a type, but a value indicating how much types there are.
	EVST_COUNT
};

//! Names for all vertex shader types, each entry corresponds to a E_VERTEX_SHADER_TYPE entry.
const c8* const VERTEX_SHADER_TYPE_NAMES[] = {
	"vs_1_1",
	"vs_2_0",
	"vs_2_a",
	"vs_3_0",
	"vs_4_0",
	"vs_4_1",
	"vs_5_0",
	0 };

//! Compile target enumeration for the addHighLevelShaderMaterial() method.
enum E_PIXEL_SHADER_TYPE
{
	EPST_PS_1_1 = 0,
	EPST_PS_1_2,
	EPST_PS_1_3,
	EPST_PS_1_4,
	EPST_PS_2_0,
	EPST_PS_2_a,
	EPST_PS_2_b,
	EPST_PS_3_0,
	EPST_PS_4_0,
	EPST_PS_4_1,
	EPST_PS_5_0,

	//! This is not a type, but a value indicating how much types there are.
	EPST_COUNT
};

//! Names for all pixel shader types, each entry corresponds to a E_PIXEL_SHADER_TYPE entry.
const c8* const PIXEL_SHADER_TYPE_NAMES[] = {
	"ps_1_1",
	"ps_1_2",
	"ps_1_3",
	"ps_1_4",
	"ps_2_0",
	"ps_2_a",
	"ps_2_b",
	"ps_3_0",
	"ps_4_0",
	"ps_4_1",
	"ps_5_0",
	0 };

//! Enum for supported geometry shader types
enum E_GEOMETRY_SHADER_TYPE
{
	EGST_GS_4_0 = 0,
	EGST_GS_4_1,
	EGST_GS_5_0,

	//! This is not a type, but a value indicating how much types there are.
	EGST_COUNT
};

//! String names for supported geometry shader types
const c8* const GEOMETRY_SHADER_TYPE_NAMES[] = {
	"gs_4_0",
	"gs_4_1",
	"gs_5_0",
	0 };

//! Enum for supported domain shader types
enum E_DOMAIN_SHADER_TYPE
{
	EDST_DS_5_0 = 0,

	//! This is not a type, but a value indicating how much types there are.
	EDST_COUNT
};

//! String names for supported domain shader types
const c8* const DOMAIN_SHADER_TYPE_NAMES[] = {
	"ds_5_0",
	0 };

//! Enum for supported hull shader types
enum E_HULL_SHADER_TYPE
{
	EHST_HS_5_0 = 0,

	//! This is not a type, but a value indicating how much types there are.
	EHST_COUNT
};

//! String names for supported hull shader types
const c8* const HULL_SHADER_TYPE_NAMES[] = {
	"hs_5_0",
	0 };

//! Enum for supported compute shader types
enum E_COMPUTE_SHADER_TYPE
{
	ECST_CS_4_0 = 0,
	ECST_CS_4_1,
	ECST_CS_5_0,

	//! This is not a type, but a value indicating how much types there are.
	ECST_COUNT
};

//! String names for supported compute shader types
const c8* const COMPUTE_SHADER_TYPE_NAMES[] = {
	"cs_4_0",
	"cs_4_1",
	"cs_5_0",
	0 };

}// end namespace video
} // end namespace irr

#endif // __E_SHADER_TYPES_H_INCLUDED__

