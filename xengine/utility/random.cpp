#include "random.h"

#include <vendor/glm/gtx/compatibility.hpp>

namespace xengine
{
	////////////////////////////////////////////////////////////////
	// Random
	////////////////////////////////////////////////////////////////

	Random::Series Random::GlobalSeries = { 1337, 65536 };

	Random::Series Random::Seed(unsigned int index, unsigned int modulo)
	{
		return { index, modulo };
	}

	unsigned int Random::NextInteger()
	{
		unsigned int a = 1103515245;
		unsigned int c = 12345;

		GlobalSeries.index = (a * GlobalSeries.index + c) % GlobalSeries.modulo;
		return GlobalSeries.index;
	}

	unsigned int Random::NextInteger(Series& series)
	{
		unsigned int a = 1103515245;
		unsigned int c = 12345;

		series.index = (a * series.index + c) % series.modulo;
		return series.index;
	}

	unsigned int Random::NextInteger(unsigned int count)
	{
		return NextInteger() % count;
	}

	unsigned int Random::NextInteger(unsigned int count, Series& series)
	{
		return NextInteger(series) % count;
	}

	float Random::UniformLiteral()
	{
		float divisor = 1.0f / GlobalSeries.modulo;
		return static_cast<float>(NextInteger()) * divisor;
	}

	float Random::UniformLiteral(Series& series)
	{
		float divisor = 1.0f / series.modulo;
		return static_cast<float>(NextInteger(series)) * divisor;
	}

	float Random::BinaryLiteral()
	{
		return 2.0f * UniformLiteral() - 1.0f;
	}

	float Random::BinaryLiteral(Series& series)
	{
		return 2.0f * UniformLiteral(series) - 1.0f;
	}

	int Random::RandomBetween(int smin, int smax)
	{
		return smin + NextInteger() % (smax - smin + 1);
	}

	int Random::RandomBetween(int smin, int smax, Series& series)
	{
		return smin + NextInteger(series) % (smax - smin + 1);
	}

	float Random::RandomBetween(float smin, float smax)
	{
		return glm::lerp(smin, smax, UniformLiteral());
	}

	float Random::RandomBetween(float smin, float smax, Series& series)
	{
		return glm::lerp(smin, smax, UniformLiteral(series));
	}
}
