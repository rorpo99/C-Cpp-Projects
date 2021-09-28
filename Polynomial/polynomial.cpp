#include <algorithm>
#include <cmath>
#include <sstream>

#include "polynomial.h"

Polynomial::Polynomial(int new_min_power, int new_max_power, int* new_coefficients)
	: min_power(new_min_power), max_power(new_max_power), size(max_power - min_power + 1) {
	coefficients = new int[size];
	for (size_t i = 0; i < size; ++i) {
		coefficients[i] = new_coefficients[i];
	}
	remove_zeros();
}

Polynomial::Polynomial(Polynomial&&); ///move constructor

Polynomial::Polynomial(const Polynomial& other)
	: min_power(other.min_power), max_power(other.max_power), size(other.size), coefficients(new int[size]) {
	for (size_t i = 0; i < size; ++i) {
		coefficients[i] = other.coefficients[i];
	}
};

Polynomial::~Polynomial() {
	delete[] coefficients;
}

/// make strong
Polynomial& Polynomial::operator=(const Polynomial& other) {
	if (this == &other) {
		return *this;
	}
	delete[] coefficients;
	min_power = other.min_power;
	max_power = other.max_power;
	size = other.size;
	coefficients = new int[size];
	for (int i = 0; i < size; ++i) {
		coefficients[i] = other.coefficients[i];
	}
	return *this;
}

int Polynomial::get_min_power() const {
	return min_power;
}

int Polynomial::get_max_power() const {
	return max_power;
}

size_t Polynomial::get_size() const {
	return size;
}

int Polynomial::operator[](int i) const {
	if (i < min_power || i > max_power) {
		return 0;
	}
	return coefficients[i - min_power];
}

int& Polynomial::operator[](int i) {
	if (i > max_power) {
		int j = 0;
		int* new_coefs = new int[i - min_power + 1];
		for (; j < size; ++j) {
			new_coefs[j] = coefficients[j];
		}
		delete[] coefficients;
		max_power = i;
		size = max_power - min_power + 1;
		for (; j < size; ++j) {
			new_coefs[j] = 0;
		}
		coefficients = new_coefs;
	}
	else if (i < min_power) {
		int j = 0;
		int* new_coefs = new int[max_power - i + 1];
		for (; j < min_power - i; ++j) {
			new_coefs[j] = 0;
		}
		size = max_power - i + 1;
		for (; j < size; ++j) {
			new_coefs[j] = coefficients[j + min_power];
		}
		min_power = i;
		delete[] coefficients;
		coefficients = new_coefs;
	}
	return coefficients[i - min_power];
}

const Polynomial Polynomial::operator+() const {
	return *this;
}

const Polynomial Polynomial::operator-() const {
	Polynomial tmp = *this;
	std::for_each(tmp.coefficients, tmp.coefficients + size, [](int& el) {el = -el;});
	return tmp;
}

///private?
/// make strong exception garantii
Polynomial Polynomial::remove_zeros() {
	std::vector <int> coefs(size);
	for (size_t i = 0; i < size; ++i) {
		coefs[i] = coefficients[i];
	}
	delete[] coefficients;
	int i = 0;
	while (coefs[i] == 0) {
		++i;
		++min_power;
	}
	while (coefs[size - 1] == 0) {
		--size;
		--max_power;
	}
	size -= i;
	coefficients = new int[size];
	for (size_t j = 0; j < size; ++j) {
		coefficients[j] = coefs[j + i];
	}
	return *this;
}

bool operator==(const Polynomial& lh, const Polynomial& rh) {
	std::stringstream slh;
	slh << lh;
	std::stringstream srh;
	srh << rh;
	return slh.str() == srh.str();
}

bool operator!=(const Polynomial& lh, const Polynomial& rh) {
	return !(lh == rh);
}

/// make strong
std::istream& operator>>(std::istream& in, Polynomial& p) {
	in >> p.min_power >> p.max_power;
	p.size = p.max_power - p.min_power + 1;
	delete[] p.coefficients;
	p.coefficients = new int[p.size];
	for (int i = 0; i < p.size; ++i) {
		in >> p.coefficients[i];
	}
	return in;
}

std::ostream& operator<<(std::ostream& out, const Polynomial& p) {
	int mind = p.get_min_power();
	int size = p.get_size();
	bool first = true;
	if (size == 0) {
		out << 0;
	}
	for (int i = size - 1; i >= 0; --i) {
		int power = mind + i;
		int coef = p[power];
		if (first) {
			if (coef > 0) {
				if (coef != 1 || power == 0) {
					out << coef;

				}
				first = false;
			}
			else if (coef < 0) {
				if (coef == -1 && power != 0) {
					out << "-";
				}
				else {
					out << coef;
				}
				first = false;
			}
			else {
				continue;
			}
		}
		else {
			if (coef > 0) {
				out << "+";
				if (coef != 1 || power == 0) {
					out << coef;
				}
			}
			else if (coef < 0) {
				if (coef != -1 || power == 0) {
					out << coef;
				}
				else {
					out << "-";
				}
			}
		}
		if (coef != 0) {
			if (power == 1) {
				out << "x";
			}
			else if (power < 0 || power > 1) {
				out << "x^" << power;
			}
		}
	}
	return out;
}

///private?
int* coefs_of_polynomial_sum_or_dif(const Polynomial& lh, const Polynomial& rh, char sign) {  //funcion where polynomial lh always has the minimal power 
	int mind = std::min(lh.get_min_power(), rh.get_min_power());
	int maxd = std::max(lh.get_max_power(), rh.get_max_power());
	int* coefs = new int[maxd - mind + 1];
	for (int i = 0; i < maxd - mind; ++i) {
		coefs[i] = 0;
	}
	for (int i = 0; i < lh.get_size(); ++i) {
		coefs[i] = lh[i + lh.get_min_power()];
	}
	int diff = rh.get_min_power() - lh.get_min_power();
	if (sign == '+') {
		for (int i = 0; i < rh.get_size(); ++i) {
			coefs[i + diff] += rh[i + rh.get_min_power()];
		}
	}
	else {
		for (int i = 0; i < rh.get_size(); ++i) {
			coefs[i + diff] -= rh[i + rh.get_min_power()];
		}
	}
	return coefs;
}

Polynomial operator+(const Polynomial& lh, const Polynomial& rh) {
	Polynomial result = lh;
	result += rh;
	return result;
}

///I think, that this function is copy of operator-=. What about creating abstract operator
Polynomial& Polynomial::operator+=(const Polynomial& p) {
	Polynomial empty_p;
	if (*this == empty_p) {
		*this = p;
		return *this;
	}
	if (p == empty_p) {
		return *this;
	}
	int mind = std::min(this->get_min_power(), p.get_min_power());
	int maxd = std::max(this->get_max_power(), p.get_max_power());
	if (this->get_min_power() < p.get_min_power()) {
		coefficients = coefs_of_polynomial_sum_or_dif(*this, p, '+');
	}
	else {
		coefficients = coefs_of_polynomial_sum_or_dif(p, *this, '+');
	}
	min_power = mind;
	max_power = maxd;
	size = max_power - min_power + 1;
	remove_zeros();
	return *this;
}

Polynomial operator-(const Polynomial& lh, const Polynomial& rh) {
	Polynomial result = lh;
	result -= rh;
	return result;
}

Polynomial& Polynomial::operator-=(const Polynomial& p) {
	Polynomial empty_p;
	if (*this == empty_p) {
		*this = p;
		return *this;
	}
	if (p == empty_p) {
		return *this;
	}
	int mind = std::min(this->get_min_power(), p.get_min_power());
	int maxd = std::max(this->get_max_power(), p.get_max_power());
	if (this->get_min_power() < p.get_min_power()) {
		coefficients = coefs_of_polynomial_sum_or_dif(*this, p, '-');
	}
	else {
		coefficients = coefs_of_polynomial_sum_or_dif(p, *this, '-');
	}
	min_power = mind;
	max_power = maxd;
	size = max_power - min_power + 1;
	remove_zeros();
	return *this;
}

Polynomial operator*(const Polynomial& p, int n) {
	Polynomial result = p;
	result *= n;
	return result;
}

Polynomial operator*(int n, const Polynomial& p) {
	return p * n;
}

Polynomial& Polynomial::operator*=(int n) {
	std::for_each(coefficients, coefficients + size, [&n](int& el) {el = n * el; });
	return *this;
}

Polynomial operator/(const Polynomial& p, int n) {
	Polynomial result = p;
	result /= n;
	return result;
}

Polynomial& Polynomial::operator/=(int n) {
	for (int i = 0; i < size; ++i) {
		coefficients[i] = coefficients[i] / n;
	}
	this->remove_zeros();
	return *this;
}

Polynomial operator*(const Polynomial& lh, const Polynomial& rh) {
	Polynomial result = lh;
	result *= rh;
	return result;
}

Polynomial& Polynomial::operator*=(const Polynomial& p) {
	Polynomial empty_p;
	if (*this == empty_p || p == empty_p) {
		return *this;
	}
	int mind = get_min_power() + p.get_min_power();
	int maxd = get_max_power() + p.get_max_power();
	int* coefs = new int[maxd - mind + 1];
	for (int i = 0; i < maxd - mind + 1; ++i) {
		coefs[i] = 0;
	}
	for (int i = 0; i < get_size(); ++i) {
		for (int j = 0; j < p.get_size(); ++j) {
			coefs[i + j] += coefficients[i] * p[j + p.get_min_power()];
		}
	}
	delete[] coefficients;
	coefficients = coefs;
	min_power = mind;
	max_power = maxd;
	size = max_power - min_power + 1;
	remove_zeros();
	return *this;
}

double Polynomial::get(double x) const {
	double result = 0;
	double cur_val = pow(x, min_power);
	for (int i = 0; i < size; ++i) {
		result += coefficients[i] * cur_val;
		cur_val *= x;
	}
	return result;
}
