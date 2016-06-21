#include <random>
#include <chrono>

class Double_Generator
{
public:
	Double_Generator(double a, double b)
	{
		distribution = new std::uniform_real_distribution<double>(a, b);
		generator = new std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	}
	double Produce()
	{
		return (*distribution)(*generator);
	}

private:
	std::default_random_engine *generator;
	std::uniform_real_distribution<double>* distribution;
};

class Int_Generator
{
public:
	Int_Generator(int a, int b)
	{
		distribution = new std::uniform_int_distribution<int>(a, b);
		generator = new std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	}
	int Produce()
	{
		return (*distribution)(*generator);
	}

private:
	std::default_random_engine *generator;
	std::uniform_int_distribution<int>* distribution;
};


