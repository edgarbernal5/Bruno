#pragma once

namespace Cuado
{
	enum class PrimitiveType
	{
		Unknown = 0,
		PointList = 1,
		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,
		Topology_1_ControlPoint = 33,
		Topology_2_ControlPoint = 34,
		Topology_3_ControlPoint = 35,
		Topology_4_ControlPoint = 36,
		Topology_5_ControlPoint = 37
		//D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST
	};
}
