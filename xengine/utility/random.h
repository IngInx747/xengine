#pragma once
#ifndef XE_RANDOM_H
#define XE_RANDOM_H

namespace xengine
{
	class Random
	{
	public:
		struct Series
		{
			unsigned int index;
			unsigned int modulo;
		};

	public:
		static Series GlobalSeries;

	public:
		// Generates a new random series
		static Series Seed(unsigned int index, unsigned int modulo);

		// Gets the next uint in series's random series
		static unsigned int NextInteger();
		static unsigned int NextInteger(Series& series);

		// Gets the next uint constrained within the region specified by count
		static unsigned int NextInteger(unsigned int count);
		static unsigned int NextInteger(unsigned int count, Series& series);

		// Gets the next random value as a float within the range [ 0.0, 1.0]
		static float UniformLiteral();
		static float UniformLiteral(Series& series);

		// Gets the next random value as a float within the range [-1.0, 1.0]
		static float BinaryLiteral();
		static float BinaryLiteral(Series& series);

		// Gets the next random value as an int between a specified min and max
		static int RandomBetween(int smin, int smax);
		static int RandomBetween(int smin, int smax, Series& series);

		// Gets the next random value as a float between a specified min and max
		static float RandomBetween(float smin, float smax);
		static float RandomBetween(float smin, float smax, Series& series);
	};
}

#endif // !XE_RANDOM_H
