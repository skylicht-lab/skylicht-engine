// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"

#include "CGeometryCreator.h"
#include "CMeshBuffer.h"
#include "SMesh.h"
#include "IMesh.h"
#include "IVideoDriver.h"
#include "irrOS.h"

namespace irr
{
namespace scene
{

IMesh* CGeometryCreator::createCubeMesh(const core::vector3df& size) const
{
	if(!Driver)
		return 0;

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));

	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	// Recalculate bounding box
	buffer->getBoundingBox().reset(0, 0, 0);

	// Create indices
	const u16 u[36] = {   0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6,
            7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7};

	ib->set_used(36);

	for (u32 i=0; i<36; ++i)
		ib->setIndex(i, u[i]);

	// Create vertices
	video::SColor clr(255,255,255,255);

	vb->reallocate(12);

	video::S3DVertex Vertices[12] = {
		video::S3DVertex(0, 0, 0, -1, -1, -1, clr, 0, 1),
		video::S3DVertex(1, 0, 0, 1, -1, -1, clr, 1, 1),
		video::S3DVertex(1, 1, 0, 1, 1, -1, clr, 1, 0),
		video::S3DVertex(0, 1, 0, -1, 1, -1, clr, 0, 0),
		video::S3DVertex(1, 0, 1, 1, -1, 1, clr, 0, 1),
		video::S3DVertex(1, 1, 1, 1, 1, 1, clr, 0, 0),
		video::S3DVertex(0, 1, 1, -1, 1, 1, clr, 1, 0),
		video::S3DVertex(0, 0, 1, -1, -1, 1, clr, 1, 1),
		video::S3DVertex(0, 1, 1, -1, 1, 1, clr, 0, 1),
		video::S3DVertex(0, 1, 0, -1, 1, -1, clr, 1, 1),
		video::S3DVertex(1, 0, 1, 1, -1, 1, clr, 1, 0),
		video::S3DVertex(1, 0, 0, 1, -1, -1, clr, 0, 0)
	};

	for (u32 i = 0; i < 12; ++i)
	{
		Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
		Vertices[i].Pos *= size;

		vb->addVertex(&Vertices[i]);
		buffer->getBoundingBox().addInternalPoint(Vertices[i].Pos);
	}

	SMesh* mesh = new SMesh;
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->recalculateBoundingBox();
	return mesh;
}


// creates a hill plane
IMesh* CGeometryCreator::createHillPlaneMesh(
		const core::dimension2d<f32>& tileSize,
		const core::dimension2d<u32>& tc, video::SMaterial* material,
		f32 hillHeight, const core::dimension2d<f32>& ch,
		const core::dimension2d<f32>& textureRepeatCount) const
{
	if(!Driver)
		return 0;

	core::dimension2d<u32> tileCount = tc;
	core::dimension2d<f32> countHills = ch;

	if (countHills.Width < 0.01f)
		countHills.Width = 1.f;
	if (countHills.Height < 0.01f)
		countHills.Height = 1.f;

	// center
	const core::position2d<f32> center((tileSize.Width * tileCount.Width) * 0.5f, (tileSize.Height * tileCount.Height) * 0.5f);

	// texture coord step
	const core::dimension2d<f32> tx(
			textureRepeatCount.Width / tileCount.Width,
			textureRepeatCount.Height / tileCount.Height);

	// add one more point in each direction for proper tile count
	++tileCount.Height;
	++tileCount.Width;

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));
	
	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);
	
	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	// create vertices from left-front to right-back
	u32 x;

	f32 sx=0.f, tsx=0.f;
	for (x=0; x<tileCount.Width; ++x)
	{
		f32 sy=0.f, tsy=0.f;
		for (u32 y=0; y<tileCount.Height; ++y)
		{
			vtx.Pos.set(sx - center.X, 0, sy - center.Y);
			vtx.TCoords.set(tsx, 1.0f - tsy);

			if (core::isnotzero(hillHeight))
				vtx.Pos.Y = sinf(vtx.Pos.X * countHills.Width * core::PI / center.X) *
					cosf(vtx.Pos.Z * countHills.Height * core::PI / center.Y) *
					hillHeight;

			vb->addVertex(&vtx);
			sy += tileSize.Height;
			tsy += tx.Height;
		}
		sx += tileSize.Width;
		tsx += tx.Width;
	}

	// create indices

	for (x=0; x<tileCount.Width-1; ++x)
	{
		for (u32 y=0; y<tileCount.Height-1; ++y)
		{
			const s32 current = x*tileCount.Height + y;

			ib->addIndex(current);
			ib->addIndex(current + 1);
			ib->addIndex(current + tileCount.Height);

			ib->addIndex(current + 1);
			ib->addIndex(current + 1 + tileCount.Height);
			ib->addIndex(current + tileCount.Height);
		}
	}

	video::S3DVertex* Vertices = static_cast<video::S3DVertex*>(vb->getVertices());

	// recalculate normals
	for (u32 i=0; i<buffer->getIndexBuffer()->getIndexCount(); i+=3)
	{
		const core::vector3df normal = core::plane3d<f32>(
			Vertices[ib->getIndex(i + 0)].Pos,
			Vertices[ib->getIndex(i + 1)].Pos,
			Vertices[ib->getIndex(i + 2)].Pos).Normal;

		Vertices[ib->getIndex(i + 0)].Normal = normal;
		Vertices[ib->getIndex(i + 1)].Normal = normal;
		Vertices[ib->getIndex(i + 2)].Normal = normal;
	}

	if (material)
		buffer->getMaterial() = *material;

	buffer->recalculateBoundingBox();
	buffer->setHardwareMappingHint(EHM_STATIC);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();
	buffer->drop();
	return mesh;
}

namespace
{

// Return the position on an exponential curve. Input from 0 to 1.
float geopos(float pos)
{
	pos = core::clamp<float>(pos, 0, 1);
	pos *= 5;

	const float out = powf(2.5f, pos - 5);

	return out;
}

}

//! Create a geoplane.
IMesh* CGeometryCreator::createGeoplaneMesh(f32 radius, u32 rows, u32 columns) const
{
	using namespace core;
	using namespace video;

	rows = clamp<u32>(rows, 3, 2048);
	columns = clamp<u32>(columns, 3, 2048);

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));
	buffer->setHardwareMappingHint(scene::EHM_STATIC);

	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	vb->reallocate((rows * columns) + 1);
	ib->reallocate((((rows - 2) * columns * 2) + columns) * 3);

	S3DVertex v(0, 0, 0, 0, 1, 0, SColor(255, 255, 255, 255), 0, 0);
	const float anglestep = (2 * PI) / columns;

	u32 i, j;
	vb->addVertex(&v);
	for (j = 1; j < rows; j++)
	{
		const float len = radius * geopos((float) j/(rows-1));

		for (i = 0; i < columns; i++)
		{
			const float angle = anglestep * i;
			v.Pos = vector3df(len * sinf(angle), 0, len * cosf(angle));

			vb->addVertex(&v);
		}
	}

	// Indices
	// First the inner fan
	for (i = 0; i < columns; i++)
	{
		ib->addIndex(0);
		ib->addIndex(1 + i);

		if (i == columns - 1)
			ib->addIndex(1);
		else
			ib->addIndex(2 + i);
	}

	// Then the surrounding quads
	for (j = 0; j < rows - 2; j++)
	{
		for (i = 0; i < columns; i++)
		{
			u32 start = ((j * columns) + i) + 1;
			u32 next = start + 1;
			u32 farvalue = (((j + 1) * columns) + i) + 1;
			u32 farnext = farvalue + 1;

			if (i == columns - 1)
			{
				next = ((j * columns)) + 1;
				farnext = (((j + 1) * columns)) + 1;
			}

			ib->addIndex(start);
			ib->addIndex(farvalue);
			ib->addIndex(next);

			ib->addIndex(next);
			ib->addIndex(farvalue);
			ib->addIndex(farnext);
		}
	}

	// Done

	SMesh* mesh = new SMesh();
	buffer->recalculateBoundingBox();
	mesh->addMeshBuffer(buffer);
	buffer->drop();
	mesh->recalculateBoundingBox();

	return mesh;
}

IMesh* CGeometryCreator::createTerrainMesh(video::IImage* texture,
		video::IImage* heightmap, const core::dimension2d<f32>& stretchSize,
		f32 maxHeight, video::IVideoDriver* driver,
		const core::dimension2d<u32>& maxVtxBlockSize,
		bool debugBorders) const
{
	if (!Driver || !texture || !heightmap)
		return 0;

	// debug border
	const s32 borderSkip = debugBorders ? 0 : 1;

	video::S3DVertex vtx;
	vtx.Color.set(255,255,255,255);

	SMesh* mesh = new SMesh();

	const u32 tm = os::Timer::getRealTime()/1000;
	const core::dimension2d<u32> hMapSize= heightmap->getDimension();
	const core::dimension2d<u32> tMapSize= texture->getDimension();
	const core::position2d<f32> thRel(static_cast<f32>(tMapSize.Width) / hMapSize.Width, static_cast<f32>(tMapSize.Height) / hMapSize.Height);
	maxHeight /= 255.0f; // height step per color value

	core::position2d<u32> processed(0,0);
	while (processed.Y<hMapSize.Height)
	{
		while(processed.X<hMapSize.Width)
		{
			core::dimension2d<u32> blockSize = maxVtxBlockSize;
			if (processed.X + blockSize.Width > hMapSize.Width)
				blockSize.Width = hMapSize.Width - processed.X;
			if (processed.Y + blockSize.Height > hMapSize.Height)
				blockSize.Height = hMapSize.Height - processed.Y;

			CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));
			buffer->setHardwareMappingHint(scene::EHM_STATIC);

			IIndexBuffer* ib = buffer->getIndexBuffer();
			IVertexBuffer* vb = buffer->getVertexBuffer(0);

			vb->reallocate(blockSize.getArea());

			// add vertices of vertex block
			u32 y;
			core::vector2df pos(0.f, processed.Y*stretchSize.Height);
			const core::vector2df bs(1.f/blockSize.Width, 1.f/blockSize.Height);
			core::vector2df tc(0.f, 0.5f*bs.Y);
			for (y=0; y<blockSize.Height; ++y)
			{
				pos.X=processed.X*stretchSize.Width;
				tc.X=0.5f*bs.X;
				for (u32 x=0; x<blockSize.Width; ++x)
				{
					const f32 height = heightmap->getPixel(x+processed.X, y+processed.Y).getAverage() * maxHeight;

					vtx.Pos.set(pos.X, height, pos.Y);
					vtx.TCoords.set(tc);
					vb->addVertex(&vtx);
					pos.X += stretchSize.Width;
					tc.X += bs.X;
				}
				pos.Y += stretchSize.Height;
				tc.Y += bs.Y;
			}

			ib->reallocate((blockSize.Height-1)*(blockSize.Width-1)*6);

			// add indices of vertex block
			s32 c1 = 0;
			for (y=0; y<blockSize.Height-1; ++y)
			{
				for (u32 x=0; x<blockSize.Width-1; ++x)
				{
					const s32 c = c1 + x;

					ib->addIndex(c);
					ib->addIndex(c + blockSize.Width);
					ib->addIndex(c + 1);

					ib->addIndex(c + 1);
					ib->addIndex(c + blockSize.Width);
					ib->addIndex(c + 1 + blockSize.Width);
				}
				c1 += blockSize.Width;
			}

			video::S3DVertex* Vertices = static_cast<video::S3DVertex*>(vb->getVertices());

			// recalculate normals
			for (u32 i=0; i<ib->getIndexCount(); i+=3)
			{
				const core::vector3df normal = core::plane3d<f32>(
					Vertices[ib->getIndex(i+0)].Pos,
					Vertices[ib->getIndex(i+1)].Pos,
					Vertices[ib->getIndex(i+2)].Pos).Normal;

				Vertices[ib->getIndex(i+0)].Normal = normal;
				Vertices[ib->getIndex(i+1)].Normal = normal;
				Vertices[ib->getIndex(i+2)].Normal = normal;
			}

			if (vb->getVertexCount() > 0)
			{
				c8 textureName[64];
				// create texture for this block
				video::IImage* img = driver->createImage(texture->getColorFormat(), core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y)));
				texture->copyTo(img, core::position2di(0,0), core::recti(
					core::position2d<s32>(core::floor32(processed.X*thRel.X), core::floor32(processed.Y*thRel.Y)),
					core::dimension2d<u32>(core::floor32(blockSize.Width*thRel.X), core::floor32(blockSize.Height*thRel.Y))), 0);

				sprintf(textureName, "terrain%u_%u", tm, mesh->getMeshBufferCount());

				buffer->getMaterial().setTexture(0, driver->addTexture(textureName, img));

				if (buffer->getMaterial().getTexture(0))
				{
					c8 tmp[255];
					sprintf(tmp, "Generated terrain texture (%dx%d): %s",
						buffer->getMaterial().getTexture(0)->getSize().Width,
						buffer->getMaterial().getTexture(0)->getSize().Height,
						textureName);
					os::Printer::log(tmp);
				}
				else
					os::Printer::log("Could not create terrain texture.", textureName, ELL_ERROR);

				img->drop();
			}

			buffer->recalculateBoundingBox();
			mesh->addMeshBuffer(buffer);
			buffer->drop();

			// keep on processing
			processed.X += maxVtxBlockSize.Width - borderSkip;
		}

		// keep on processing
		processed.X = 0;
		processed.Y += maxVtxBlockSize.Height - borderSkip;
	}

	mesh->recalculateBoundingBox();
	return mesh;
}


/*
	a cylinder, a cone and a cross
	point up on (0,1.f, 0.f )
*/
IMesh* CGeometryCreator::createArrowMesh(const u32 tesselationCylinder,
						const u32 tesselationCone,
						const f32 height,
						const f32 cylinderHeight,
						const f32 width0,
						const f32 width1,
						const video::SColor vtxColor0,
						const video::SColor vtxColor1) const
{
	if(!Driver)
		return 0;

	SMesh* mesh = (SMesh*)createCylinderMesh(width0, cylinderHeight, tesselationCylinder, vtxColor0, false);

	IMesh* mesh2 = createConeMesh(width1, height-cylinderHeight, tesselationCone, vtxColor1, vtxColor0);
	for (u32 i=0; i<mesh2->getMeshBufferCount(); ++i)
	{
		scene::IMeshBuffer* buffer = mesh2->getMeshBuffer(i);

		IVertexBuffer* vb = buffer->getVertexBuffer(0);

		video::S3DVertex* Vertices = static_cast<video::S3DVertex*>(vb->getVertices());

		for (u32 j=0; j<vb->getVertexCount(); ++j)
			Vertices[j].Pos.Y += cylinderHeight;

		buffer->setDirty(EBT_VERTEX);
		buffer->recalculateBoundingBox();
		mesh->addMeshBuffer(buffer);
	}
	mesh2->drop();
	mesh->setHardwareMappingHint(EHM_STATIC);

	mesh->recalculateBoundingBox();
	return mesh;
}


/* A sphere with proper normals and texture coords */
IMesh* CGeometryCreator::createSphereMesh(f32 radius, u32 polyCountX, u32 polyCountY) const
{
	if(!Driver)
		return 0;
	// thanks to Alfaz93 who made his code available for Irrlicht on which
	// this one is based!

	// we are creating the sphere mesh here.

	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent u16 overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));

	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	ib->reallocate((polyCountX * polyCountY) * 6);

	const video::SColor clr(255, 255,255,255);

	u32 level = 0;

	for (u32 p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			const u32 curr = level + p2;
			ib->addIndex(curr + polyCountXPitch);
			ib->addIndex(curr);
			ib->addIndex(curr + 1);
			ib->addIndex(curr + polyCountXPitch);
			ib->addIndex(curr+1);
			ib->addIndex(curr + 1 + polyCountXPitch);
		}

		// the connectors from front to end
		ib->addIndex(level + polyCountX - 1 + polyCountXPitch);
		ib->addIndex(level + polyCountX - 1);
		ib->addIndex(level + polyCountX);

		ib->addIndex(level + polyCountX - 1 + polyCountXPitch);
		ib->addIndex(level + polyCountX);
		ib->addIndex(level + polyCountX + polyCountXPitch);
		level += polyCountXPitch;
	}

	const u32 polyCountSq = polyCountXPitch * polyCountY; // top point
	const u32 polyCountSq1 = polyCountSq + 1; // bottom point
	const u32 polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		ib->addIndex(polyCountSq);
		ib->addIndex(p2 + 1);
		ib->addIndex(p2);

		// create triangles which are at the bottom of the sphere

		ib->addIndex(polyCountSqM1 + p2);
		ib->addIndex(polyCountSqM1 + p2 + 1);
		ib->addIndex(polyCountSq1);
	}

	// create final triangle which is at the top of the sphere

	ib->addIndex(polyCountSq);
	ib->addIndex(polyCountX);
	ib->addIndex(polyCountX-1);

	// create final triangle which is at the bottom of the sphere

	ib->addIndex(polyCountSqM1 + polyCountX - 1);
	ib->addIndex(polyCountSqM1);
	ib->addIndex(polyCountSq1);

	// calculate the angle which separates all points in a circle
	const f64 AngleX = 2 * core::PI / polyCountX;
	const f64 AngleY = core::PI / polyCountY;

	u32 i=0;
	f64 axz;

	// we don't start at 0.

	f64 ay = 0;//AngleY / 2;

	vb->set_used((polyCountXPitch * polyCountY) + 2);

	video::S3DVertex* Vertices = static_cast<video::S3DVertex*>(vb->getVertices());

	for (u32 y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const f64 sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position

			const core::vector3df pos(static_cast<f32>(radius * cos(axz) * sinay),
						static_cast<f32>(radius * cos(ay)),
						static_cast<f32>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			core::vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = static_cast<f32>(acos(core::clamp(normal.X/sinay, -1.0, 1.0)) * 0.5 *core::RECIPROCAL_PI64);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = Vertices[i-polyCountXPitch].TCoords.X;
			Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, tu,
						static_cast<f32>(ay*core::RECIPROCAL_PI64));
			++i;
			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		Vertices[i] = video::S3DVertex(Vertices[i-polyCountX]);
		Vertices[i].TCoords.X=1.0f;
		++i;
	}

	// the vertex at the top of the sphere
	Vertices[i] = video::S3DVertex(0.0f,radius,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);

	// the vertex at the bottom of the sphere
	++i;
	Vertices[i] = video::S3DVertex(0.0f,-radius,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);

	// recalculate bounding box

	buffer->getBoundingBox().reset(Vertices[i].Pos);
	buffer->getBoundingBox().addInternalPoint(Vertices[i - 1].Pos);
	buffer->getBoundingBox().addInternalPoint(radius, 0.0f, 0.0f);
	buffer->getBoundingBox().addInternalPoint(-radius, 0.0f, 0.0f);
	buffer->getBoundingBox().addInternalPoint(0.0f, 0.0f, radius);
	buffer->getBoundingBox().addInternalPoint(0.0f, 0.0f, -radius);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();
	return mesh;
}


/* A cylinder with proper normals and texture coords */
IMesh* CGeometryCreator::createCylinderMesh(f32 radius, f32 length,
			u32 tesselation, const video::SColor& color,
			bool closeTop, f32 oblique) const
{
	if(!Driver)
		return 0;

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));

	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	const f32 recTesselation = core::reciprocal((f32)tesselation);
	const f32 recTesselationHalf = recTesselation * 0.5f;
	const f32 angleStep = (core::PI * 2.f ) * recTesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	u32 i;
	video::S3DVertex v;
	v.Color = color;

	vb->reallocate(tesselation*4+4+(closeTop?2:1));
	ib->reallocate((tesselation*2+1)*(closeTop?12:9));

	f32 tcx = 0.f;
	for ( i = 0; i <= tesselation; ++i )
	{
		const f32 angle = angleStep * i;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.X=tcx;
		v.TCoords.Y=0.f;
		vb->addVertex(&v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.Y=1.f;
		vb->addVertex(&v);

		v.Pos.X = radius * cosf(angle + angleStepHalf);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle + angleStepHalf);
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.X=tcx+recTesselationHalf;
		v.TCoords.Y=0.f;
		vb->addVertex(&v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		v.Normal = v.Pos;
		v.Normal.normalize();
		v.TCoords.Y=1.f;
		vb->addVertex(&v);

		tcx += recTesselation;
	}

	// indices for the main hull part
	const u32 nonWrappedSize = tesselation* 4;
	for (i=0; i != nonWrappedSize; i += 2)
	{
		ib->addIndex(i + 2);
		ib->addIndex(i + 0);
		ib->addIndex(i + 1);

		ib->addIndex(i + 2);
		ib->addIndex(i + 1);
		ib->addIndex(i + 3);
	}

	// two closing quads between end and start
	ib->addIndex(0);
	ib->addIndex(i);
	ib->addIndex(i + 1);

	ib->addIndex(0);
	ib->addIndex(i + 1);
	ib->addIndex(1);

	// close down
	v.Pos.X = 0.f;
	v.Pos.Y = 0.f;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = -1.f;
	v.Normal.Z = 0.f;
	v.TCoords.X = 1.f;
	v.TCoords.Y = 1.f;

	vb->addVertex(&v);

	u32 index = vb->getVertexCount() - 1;

	for ( i = 0; i != nonWrappedSize; i += 2 )
	{
		ib->addIndex(index);
		ib->addIndex(i + 0);
		ib->addIndex(i + 2);
	}

	ib->addIndex(index);
	ib->addIndex(i + 0);
	ib->addIndex(0);

	if (closeTop)
	{
		// close top
		v.Pos.X = oblique;
		v.Pos.Y = length;
		v.Pos.Z = 0.f;
		v.Normal.X = 0.f;
		v.Normal.Y = 1.f;
		v.Normal.Z = 0.f;
		v.TCoords.X = 0.f;
		v.TCoords.Y = 0.f;
		vb->addVertex(&v);

		index = vb->getVertexCount() - 1;

		for ( i = 0; i != nonWrappedSize; i += 2 )
		{
			ib->addIndex(i + 1);
			ib->addIndex(index);
			ib->addIndex(i + 3);
		}

		ib->addIndex(i + 1);
		ib->addIndex(index);
		ib->addIndex(1);
	}

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();
	buffer->drop();
	return mesh;
}


/* A cone with proper normals and texture coords */
IMesh* CGeometryCreator::createConeMesh(f32 radius, f32 length, u32 tesselation,
					const video::SColor& colorTop,
					const video::SColor& colorBottom,
					f32 oblique) const
{
	if(!Driver)
		return 0;

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));

	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	const f32 angleStep = (core::PI * 2.f ) / tesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	video::S3DVertex v;
	u32 i;

	v.Color = colorTop;
	for ( i = 0; i != tesselation; ++i )
	{
		f32 angle = angleStep * f32(i);

		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		vb->addVertex(&v);

		angle += angleStepHalf;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize();
		vb->addVertex(&v);
	}
	const u32 nonWrappedSize = vb->getVertexCount() - 1;

	// close top
	v.Pos.X = oblique;
	v.Pos.Y = length;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = 1.f;
	v.Normal.Z = 0.f;
	vb->addVertex(&v);

	u32 index = vb->getVertexCount() - 1;

	for ( i = 0; i != nonWrappedSize; i += 1 )
	{
		ib->addIndex(i);
		ib->addIndex(index);
		ib->addIndex(i + 1);
	}

	ib->addIndex(i);
	ib->addIndex(index);
	ib->addIndex(0);

	// close down
	v.Color = colorBottom;
	v.Pos.X = 0.f;
	v.Pos.Y = 0.f;
	v.Pos.Z = 0.f;
	v.Normal.X = 0.f;
	v.Normal.Y = -1.f;
	v.Normal.Z = 0.f;
	vb->addVertex(&v);

	index = vb->getVertexCount() - 1;

	for ( i = 0; i != nonWrappedSize; i += 1 )
	{
		ib->addIndex(index);
		ib->addIndex(i + 0);
		ib->addIndex(i + 1);
	}

	ib->addIndex(index);
	ib->addIndex(i + 0);
	ib->addIndex(0);

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();
	return mesh;
}


void CGeometryCreator::addToBuffer(const video::S3DVertex& v, IMeshBuffer* buffer) const
{
	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	const s32 tnidx = vb->linear_reverse_search(&v);
	const bool alreadyIn = (tnidx != -1);
	u16 nidx = (u16)tnidx;
	if (!alreadyIn)
	{
		nidx = (u16)vb->getVertexCount();
		ib->addIndex(nidx);
		vb->addVertex(&v);
	}
	else
		ib->addIndex(nidx);
}


IMesh* CGeometryCreator::createVolumeLightMesh(
	const u32 subdivideU, const u32 subdivideV,
	const video::SColor footColor, const video::SColor tailColor,
	const f32 lpDistance, const core::vector3df& lightDim) const
{
	if (!Driver)
		return 0;

	CMeshBuffer<video::S3DVertex>* buffer = new CMeshBuffer<video::S3DVertex>(Driver->getVertexDescriptor(0));

	IIndexBuffer* ib = buffer->getIndexBuffer();
	IVertexBuffer* vb = buffer->getVertexBuffer(0);

	buffer->setHardwareMappingHint(EHM_STATIC);

	const core::vector3df lightPoint(0, -(lpDistance*lightDim.Y), 0);
	const f32 ax = lightDim.X * 0.5f; // X Axis
	const f32 az = lightDim.Z * 0.5f; // Z Axis

	vb->clear();
	vb->reallocate(6 + 12 * (subdivideU + subdivideV));

	ib->clear();
	ib->reallocate(6 + 12 * (subdivideU + subdivideV));

	//draw the bottom foot.. the glowing region
	addToBuffer(video::S3DVertex(-ax, 0, az, 0, 0, 0, footColor, 0, 1), buffer);
	addToBuffer(video::S3DVertex(ax, 0, az, 0, 0, 0, footColor, 1, 1), buffer);
	addToBuffer(video::S3DVertex(ax, 0, -az, 0, 0, 0, footColor, 1, 0), buffer);

	addToBuffer(video::S3DVertex(ax, 0, -az, 0, 0, 0, footColor, 1, 0), buffer);
	addToBuffer(video::S3DVertex(-ax, 0, -az, 0, 0, 0, footColor, 0, 0), buffer);
	addToBuffer(video::S3DVertex(-ax, 0, az, 0, 0, 0, footColor, 0, 1), buffer);

	f32 tu = 0.f;
	const f32 tuStep = 1.f / subdivideU;
	f32 bx = -ax;
	const f32 bxStep = lightDim.X * tuStep;
	// Slices in X/U space
	for (u32 i = 0; i <= subdivideU; ++i)
	{
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(bx, 0.0f, -az);
		core::vector3df end2(bx, 0.0f, az);

		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= lightDim.Y;	// multiply it out by shootlength

		end1.X += bx;			// Add the original point location to the vector
		end1.Z -= az;

		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= lightDim.Y;

		end2.X += bx;
		end2.Z += az;

		addToBuffer(video::S3DVertex(bx, 0, az, 0, 0, 0, footColor, tu, 1), buffer);
		addToBuffer(video::S3DVertex(bx, 0, -az, 0, 0, 0, footColor, tu, 0), buffer);
		addToBuffer(video::S3DVertex(end2.X, end2.Y, end2.Z, 0, 0, 0, tailColor, tu, 1), buffer);

		addToBuffer(video::S3DVertex(bx, 0, -az, 0, 0, 0, footColor, tu, 0), buffer);
		addToBuffer(video::S3DVertex(end1.X, end1.Y, end1.Z, 0, 0, 0, tailColor, tu, 0), buffer);
		addToBuffer(video::S3DVertex(end2.X, end2.Y, end2.Z, 0, 0, 0, tailColor, tu, 1), buffer);

		//back side
		addToBuffer(video::S3DVertex(-end2.X, end2.Y, -end2.Z, 0, 0, 0, tailColor, tu, 1), buffer);
		addToBuffer(video::S3DVertex(-bx, 0, -az, 0, 0, 0, footColor, tu, 1), buffer);
		addToBuffer(video::S3DVertex(-bx, 0, az, 0, 0, 0, footColor, tu, 0), buffer);

		addToBuffer(video::S3DVertex(-bx, 0, az, 0, 0, 0, footColor, tu, 0), buffer);
		addToBuffer(video::S3DVertex(-end1.X, end1.Y, -end1.Z, 0, 0, 0, tailColor, tu, 0), buffer);
		addToBuffer(video::S3DVertex(-end2.X, end2.Y, -end2.Z, 0, 0, 0, tailColor, tu, 1), buffer);
		tu += tuStep;
		bx += bxStep;
	}

	f32 tv = 0.f;
	const f32 tvStep = 1.f / subdivideV;
	f32 bz = -az;
	const f32 bzStep = lightDim.Z * tvStep;
	// Slices in Z/V space
	for (u32 i = 0; i <= subdivideV; ++i)
	{
		// These are the two endpoints for a slice at the foot
		core::vector3df end1(-ax, 0.0f, bz);
		core::vector3df end2(ax, 0.0f, bz);

		end1 -= lightPoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= lightDim.Y;	// multiply it out by shootlength

		end1.X -= ax;			// Add the original point location to the vector
		end1.Z += bz;

		// Do it again for the other point.
		end2 -= lightPoint;
		end2.normalize();
		end2 *= lightDim.Y;

		end2.X += ax;
		end2.Z += bz;

		addToBuffer(video::S3DVertex(-ax, 0, bz, 0, 0, 0, footColor, 0, tv), buffer);
		addToBuffer(video::S3DVertex(ax, 0, bz, 0, 0, 0, footColor, 1, tv), buffer);
		addToBuffer(video::S3DVertex(end2.X, end2.Y, end2.Z, 0, 0, 0, tailColor, 1, tv), buffer);

		addToBuffer(video::S3DVertex(end2.X, end2.Y, end2.Z, 0, 0, 0, tailColor, 1, tv), buffer);
		addToBuffer(video::S3DVertex(end1.X, end1.Y, end1.Z, 0, 0, 0, tailColor, 0, tv), buffer);
		addToBuffer(video::S3DVertex(-ax, 0, bz, 0, 0, 0, footColor, 0, tv), buffer);

		//back side
		addToBuffer(video::S3DVertex(ax, 0, -bz, 0, 0, 0, footColor, 0, tv), buffer);
		addToBuffer(video::S3DVertex(-ax, 0, -bz, 0, 0, 0, footColor, 1, tv), buffer);
		addToBuffer(video::S3DVertex(-end2.X, end2.Y, -end2.Z, 0, 0, 0, tailColor, 1, tv), buffer);

		addToBuffer(video::S3DVertex(-end2.X, end2.Y, -end2.Z, 0, 0, 0, tailColor, 1, tv), buffer);
		addToBuffer(video::S3DVertex(-end1.X, end1.Y, -end1.Z, 0, 0, 0, tailColor, 0, tv), buffer);
		addToBuffer(video::S3DVertex(ax, 0, -bz, 0, 0, 0, footColor, 0, tv), buffer);
		tv += tvStep;
		bz += bzStep;
	}

	buffer->recalculateBoundingBox();

	//buffer->getMaterial().MaterialType = video::EMT_ONETEXTURE_BLEND;
	//buffer->getMaterial().MaterialTypeParam = pack_textureBlendFunc(video::EBF_SRC_COLOR, video::EBF_SRC_ALPHA, video::EMFN_MODULATE_1X);

	buffer->getMaterial().Lighting = false;
	buffer->getMaterial().ZWriteEnable = false;

	buffer->setDirty(EBT_VERTEX_AND_INDEX);

	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	buffer->drop();

	mesh->recalculateBoundingBox();
	return mesh;
}


} // end namespace scene
} // end namespace irr

