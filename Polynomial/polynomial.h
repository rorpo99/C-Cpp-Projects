#pragma once

#include <iostream>
#include <vector>


class Polynomial {
private:
	int min_power = 0;
	int max_power = 0;
	size_t size = 0;
	int* coefficients = nullptr;
public:
	Polynomial() = default;
	Polynomial(int, int, int*);
	Polynomial(const Polynomial&);
	~Polynomial();
	Polynomial& operator=(const Polynomial&);
	int get_min_power() const;
	int get_max_power() const;
	size_t get_size() const;
	int operator[](int) const;
	int& operator[](int);
	const Polynomial operator+() const;
	const Polynomial operator-() const;
	Polynomial& operator+=(const Polynomial&);
	Polynomial& operator-=(const Polynomial&);
	Polynomial remove_zeros();
	Polynomial& operator/=(int);
	Polynomial& operator*=(int);
	Polynomial& operator*=(const Polynomial&);
	double get(double) const;
	friend std::istream& operator>>(std::istream&, Polynomial& );
};

bool operator==(const Polynomial& lh, const Polynomial& rh);
bool operator!=(const Polynomial& lh, const Polynomial& rh);


std::istream& operator>>(std::istream& out, Polynomial& p);
std::ostream& operator<<(std::ostream& out, const Polynomial& p);

int* coefs_of_polynomial_sum_or_dif(const Polynomial& lh, const Polynomial& rh, char sign);
Polynomial operator+(const Polynomial& lh, const Polynomial& rh);
Polynomial operator-(const Polynomial& lh, const Polynomial& rh);

Polynomial operator*(const Polynomial& p, int n);
Polynomial operator*(int n, const Polynomial& p);

Polynomial operator/(const Polynomial& p, int n);

Polynomial operator*(const Polynomial& lh, const Polynomial& rh);

