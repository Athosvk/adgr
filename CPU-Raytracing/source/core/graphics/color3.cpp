#include <./core/graphics/color3.h>

namespace CRT
{
	const color3 Color::Black = float3::Zero();
	const color3 Color::White = float3::One();
	const color3 Color::Blue = float3(0.0f, 0.0f, 1.0f);
	const color3 Color::Red = float3(1.0f, 0.0f, 0.0f);
	const color3 Color::Green = float3(0.0f, 1.0f, 0.0f);
	const color3 Color::Yellow = float3(1.0f, 1.0f, 0.0f);
	const color3 Color::Purple = float3(1.0f, 0.0f, 1.0f);
}