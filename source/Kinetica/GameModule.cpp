#include "GameModule.h"

namespace Kinetica
{

Lazy<std::span<const GameModule::SLIFunctionHandler>> GameModule::SLIFunctions([]()
{
	const auto ptr = Offsets::ResolveID<"GameModule::SLIFunctionTable", SLIFunctionHandler *>();
	return std::span{ &ptr[0], &ptr[309] };
});

Lazy<std::span<const GameModule::SLIPropertyHandler>> GameModule::SLIProperties([]()
{
	const auto ptr = Offsets::ResolveID<"GameModule::SLIPropertyTable", SLIPropertyHandler *>();
	return std::span{ &ptr[0], &ptr[86] };
});

}