#include <iostream>
#include <vector>
using namespace std;

struct RSA {
    int p;
    int q;
    int public_exp;
    int d;
    int n;
    int ef;
    RSA(int p, int q, int pe, int d, int n, int ef):p(p),q(q),public_exp(pe),d(d), n(n), ef(ef){}
};

vector<int> prime_numbers(int n) {
    vector<char> prime(n + 1, true);
    prime[0] = prime[1] = false;
    for (int i = 2; i * i <= n; ++i) {
        if (prime[i]) {
            for (int j = 2; j <= n / i; ++j) {
                if (prime[i * j]) {
                    prime[i * j] = false;
                }
            }
        }
    }
    vector<int> result;
    for (int i = 2; i <= n; ++i) {
        if (prime[i]) {
            result.push_back(i);
        }
    }
    return result;
}

int gcdex(int a, int b, int &x, int &y) {
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    int x1, y1;
    int d = gcdex(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

int gcd(int a, int b) {
    while (b) {
        a %= b;
        swap(a, b);
    }
    return a;
}

int inverse_element(int a, int m) {
    int x, y;
    int g = gcdex(a, m, x, y);
    cout << "a * " << x << " + b * " <<  y << " = gcd(a, m)\n";
    cout << a << " * " << x << " + " <<  m << " * " <<  y << " = gcd(" << a << "," << m << ")\n";
    if (g != 1) {
        return -1;
    } else {
        x = (x % m + m) % m;
        return x;
    }
}

unsigned long long binpow(unsigned long long n, int p) {
    if (p == 1) {
        return n;
    }
    if (p % 2) {
        return binpow(n, p - 1) * n;
    } else {
        unsigned long long res = binpow(n, p/2);
        return res * res;
    }
}

RSA rsa() {
    // at the first step generate prime numbers
    vector<int> primes = prime_numbers(50);
    int q = primes[rand() % primes.size()];
    int p = primes[rand() % primes.size()];
    int n = p * q;
    int euler_func = (p - 1) * (q - 1);
    int public_exp = 1, d = -1;

    do {
        ++public_exp;
        for (int i = public_exp; i < euler_func; ++i) {
            if (gcd(i, euler_func) == 1) {
                public_exp = i;
                break;
            }
        }
        d = inverse_element(public_exp, euler_func);
    } while (d == -1);
    return RSA(p, q, public_exp, d, n, euler_func);
}

void print_RSA(const RSA& rsa) {
    cout << "p = " << rsa.p << "\n";
    cout << "q = " << rsa.q << "\n";
    cout << "Euler function value: " << rsa.ef << "\n";
    cout << "Public key (e, n): {" << rsa.public_exp << ", " << rsa.n << "}\n";
    cout << "Private key (d, n): {" << rsa.d << ", " << rsa.n << "}\n";
}

int encrypt(const RSA& rsa, int message) {
    int c = binpow(message, rsa.public_exp);
    return c % rsa.n; 
}

int decrypt(const RSA& rsa, int e_message) {
    int m = binpow(e_message, rsa.d);
    return m % rsa.n;
}

int main() {
    srand(time(NULL));
    RSA rsa_obj = rsa();
    print_RSA(rsa_obj);
    cout << "Enter message to encrypt: ";
    int m;
    cin >> m;
    int enc = encrypt(rsa_obj, m);
    cout << "Encrypted message: " << enc << "\n";
    int dec = decrypt(rsa_obj, enc);
    cout << "Decrypted message: " << dec << "\n";
    return 0;
}