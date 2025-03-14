#include "basic_sphere_interface.h"
#include "basic_shpere.h"

void SphereTriTessellation(int level,mat3x4 tri ,vertex* buf,GLuint& bufOffSet)
{
	if (level <= 0)
	{
		vec4 colorArr[5] = {
			WHITE , RED , BLUE , GREEN , YELLOW
		};
		GLuint colorIdx = 0;


		buf[bufOffSet].vertPos = tri[0];
		buf[bufOffSet + 1].vertPos = tri[1];
		buf[bufOffSet + 2].vertPos = tri[2];

		buf[bufOffSet].vertColor = colorArr[colorIdx];
		buf[bufOffSet + 1].vertColor = colorArr[colorIdx];
		buf[bufOffSet + 2].vertColor = colorArr[colorIdx];

		buf[bufOffSet].vertNorm = tri[0];
		buf[bufOffSet + 1].vertNorm = tri[1];
		buf[bufOffSet + 2].vertNorm = tri[2];

		buf[bufOffSet].vertTexCoord = vec3(tri[0]);
		buf[bufOffSet + 1].vertTexCoord = vec3(tri[1]);
		buf[bufOffSet + 2].vertTexCoord = vec3(tri[2]);

		bufOffSet += 3;
	}
	else 
	{
		//subPoint|point1_idx|point2_idx <= mindlePoint( point1_idx , point2_idx )
		vec4 subPoint01 = vec4(normalize((vec3(tri[0] + tri[1])) / 2.F), 1.F);

		vec4 subPoint12 = vec4(normalize((vec3(tri[1] + tri[2])) / 2.F), 1.F);

		vec4 subPoint20 = vec4(normalize((vec3(tri[2] + tri[0])) / 2.F), 1.F);

		mat3x4 subTri[4] = {
			{tri[0]		,subPoint01 ,subPoint20},
			{tri[1]		,subPoint12 ,subPoint01},
			{tri[2]		,subPoint20 ,subPoint12},
			{subPoint01	,subPoint12 ,subPoint20}
		};

		for(int i = 0; i < 4; i++)
		SphereTriTessellation(level-1, subTri[i] , buf, bufOffSet);


	}

}

void GenerateSphere(int level,vertex* buf,GLuint& vertexCnt)
{
	vec4 tetrahedron[6] = {
		{1.F , 0.F , 0.F , 1.F},//v0
		{0.F , 1.F , 0.F , 1.F},//v1
		{-1.F , 0.F , 0.F , 1.F},//2
		{0.F , -1.F , 0.F , 1.F},//3
		{0.F , 0.F ,  1.F , 1.F},//v4
		{0.F , 0.F , -1.F , 1.F}// v5
	};

	SphereTriTessellation(level, mat3x4(tetrahedron[0], tetrahedron[1], tetrahedron[4]),buf , vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[1], tetrahedron[2], tetrahedron[4]), buf, vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[2], tetrahedron[3], tetrahedron[4]), buf, vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[3], tetrahedron[0], tetrahedron[4]), buf, vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[1], tetrahedron[0], tetrahedron[5]), buf, vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[2], tetrahedron[1], tetrahedron[5]), buf, vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[3], tetrahedron[2], tetrahedron[5]), buf, vertexCnt);
	SphereTriTessellation(level, mat3x4(tetrahedron[0], tetrahedron[3], tetrahedron[5]), buf, vertexCnt);

}