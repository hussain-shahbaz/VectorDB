#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <map>

using namespace std;

// ==================== COLOR CODES ====================
#define RESET   15
#define RED     4
#define GREEN   2
#define YELLOW  6
#define BLUE    1
#define MAGENTA 5
#define CYAN    3
#define WHITE   7

void c(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

string repeat(string s, int n) {
    string result = "";
    for (int i = 0; i < n; i++) result += s;
    return result;
}
void clearScreen() { system("cls"); }

// ==================== BEAUTIFUL UI UTILITIES ====================
void printBoxHeader(const string& title) {
    c(CYAN);
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║" << string(32 - title.length()/2, ' ') << title 
         << string(32 - (title.length()+1)/2, ' ') << "║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n";
    c(RESET);
}

void printStep(int stepNum, const string& description, const string& formula = "") {
    c(GREEN);
    // Use the repeat helper instead of string constructor
    cout << "\n┌─ Step " << stepNum << " " 
         << repeat("─", 54 - to_string(stepNum).length()) << "┐\n";
    
    c(YELLOW);
    cout << "│ " << description << "\n";
    if (!formula.empty()) {
        c(CYAN);
        cout << "│ Formula: " << formula << "\n";
    }
    
    c(GREEN);
    // Use double quotes for the corners and the repeat helper for the line
    cout << "└" << repeat("─", 61) << "┘\n";
    c(RESET);
}
void printResult(const string& result) {
    c(MAGENTA);
    cout << "\n>> RESULT: " << result << "\n\n";
    c(RESET);
}

void printTeacherInsight(const string& insight, const string& connection = "") {
    c(CYAN);
    cout << "\n╔═══ Teacher Insight ═══════════════════════════════════════════╗\n";
    c(WHITE);
    cout << "║ " << insight << "\n";
    if (!connection.empty()) {
        c(YELLOW);
        cout << "║ Connection: " << connection << "\n";
    }
    c(CYAN);
    cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    c(RESET);
}

void printCrossReference(const string& module) {
    c(BLUE);
    cout << "  ℹ️  Cross-Reference: " << module << "\n";
    c(RESET);
}

// ==================== CORE MATH UTILITIES ====================
struct MathCore {
    static bool isPrime(long long n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        for (long long i = 3; i * i <= n; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
    
    static vector<long long> generatePrimes(long long start, long long end) {
        vector<long long> primes;
        for (long long i = (start < 2 ? 2 : start); i <= end; i++) {
            if (isPrime(i)) primes.push_back(i);
        }
        return primes;
    }
    
    static vector<int> sieve(int n) {
        vector<bool> prime(n + 1, true);
        prime[0] = prime[1] = false;
        for (int i = 2; i * i <= n; i++) {
            if (prime[i]) {
                for (int j = i * i; j <= n; j += i) {
                    prime[j] = false;
                }
            }
        }
        vector<int> primes;
        for (int i = 2; i <= n; i++) {
            if (prime[i]) primes.push_back(i);
        }
        return primes;
    }
    
    static long long nextPrime(long long n) {
        if (n < 2) return 2;
        n++;
        while (!isPrime(n)) n++;
        return n;
    }
    
    static long long gcd(long long a, long long b) {
        while (b) {
            long long t = b;
            b = a % b;
            a = t;
        }
        return a;
    }
    
    static long long modPow(long long base, long long exp, long long mod) {
        long long result = 1;
        base %= mod;
        while (exp > 0) {
            if (exp % 2 == 1)
                result = (result * base) % mod;
            base = (base * base) % mod;
            exp /= 2;
        }
        return result;
    }
    
    static long long modInverse(long long a, long long m) {
        long long m0 = m, x0 = 0, x1 = 1;
        if (m == 1) return 0;
        
        while (a > 1) {
            long long q = a / m;
            long long t = m;
            m = a % m;
            a = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        
        if (x1 < 0) x1 += m0;
        return x1;
    }
    
    static int mobius(int n) {
        int cnt = 0;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                n /= i;
                cnt++;
                if (n % i == 0) return 0;
            }
        }
        if (n > 1) cnt++;
        return (cnt % 2 == 0) ? 1 : -1;
    }
};

// ==================== PRIME NUMBER THEORY MODULES ====================
struct PrimeTheory {
    static void checkPrime() {
        printBoxHeader("PRIME CHECKING - TRIAL DIVISION");
        
        long long n;
        c(YELLOW);
        cout << "Enter number to check: ";
        c(RESET);
        cin >> n;
        
        printStep(1, "Check if n < 2 (not prime by definition)");
        if (n < 2) {
            c(RED);
            cout << "  " << n << " is NOT PRIME (less than 2)\n";
            c(RESET);
            printTeacherInsight("By definition, prime numbers are natural numbers > 1");
            return;
        }
        
        printStep(2, "Check divisibility from 2 to √n", "For i = 2 to √n: if n % i == 0, not prime");
        
        bool prime = true;
        for (long long i = 2; i * i <= n && prime; i++) {
            if (n % i == 0) {
                c(RED);
                cout << "  Found divisor: " << n << " = " << i << " × " << (n/i) << "\n";
                c(RESET);
                prime = false;
            }
        }
        
        if (prime) {
            c(GREEN);
            cout << "\n  ✓ " << n << " is PRIME\n";
            c(RESET);
        } else {
            c(RED);
            cout << "\n  ✗ " << n << " is COMPOSITE\n";
            c(RESET);
        }
        
        printTeacherInsight(
            "Trial division has O(√n) complexity. For large numbers,\n"
            "║ use Miller-Rabin (probabilistic) or AKS (deterministic).",
            "See Module 1.1.4 - Miller-Rabin Test"
        );
    }
    
    static void generatePrimesInRange() {
        printBoxHeader("PRIME GENERATION - SIEVE OF ERATOSTHENES");
        
        int n;
        c(YELLOW);
        cout << "Generate primes up to: ";
        c(RESET);
        cin >> n;
        
        printStep(1, "Initialize array marking all numbers as prime");
        printStep(2, "Mark multiples of each prime as composite", "For prime p: mark p², p²+p, p²+2p, ...");
        
        vector<int> primes = MathCore::sieve(n);
        
        printStep(3, "Collect remaining unmarked numbers");
        
        c(CYAN);
        cout << "\nGenerated Primes:\n";
        cout << "────────────────────────────────────────────────────────\n";
        c(GREEN);
        
        int count = 0;
        for (int p : primes) {
            cout << setw(8) << p;
            count++;
            if (count % 8 == 0) cout << "\n";
        }
        cout << "\n";
        c(RESET);
        
        printResult("Found " + to_string(primes.size()) + " primes");
        
        printTeacherInsight(
            "Sieve of Eratosthenes: O(n log log n) complexity.\n"
            "║ Memory: O(n). Efficient for generating many primes at once.",
            "Used in: Prime Distribution [Module 1.2.1]"
        );
    }
    
    static void findNextPrime() {
        printBoxHeader("FIND NEXT PRIME");
        
        long long n;
        c(YELLOW);
        cout << "Enter number: ";
        c(RESET);
        cin >> n;
        
        printStep(1, "Start from n+1 and test each number");
        long long next = MathCore::nextPrime(n);
        
        printResult("Next prime after " + to_string(n) + " is " + to_string(next));
        
        printTeacherInsight(
            "Average gap between consecutive primes near n is ln(n).\n"
            "║ This follows from the Prime Number Theorem.",
            "See Module 1.2.1 - Prime Distribution"
        );
    }
    
    static void primeDistribution() {
        printBoxHeader("PRIME DISTRIBUTION OBSERVATORY");
        
        int X, step;
        c(YELLOW);
        cout << "Upper bound X: ";
        c(RESET);
        cin >> X;
        c(YELLOW);
        cout << "Step size: ";
        c(RESET);
        cin >> step;
        
        printStep(1, "Count primes π(x) for each x");
        printStep(2, "Compare with approximation x/log(x)", "Prime Number Theorem");
        
        c(CYAN);
        cout << "\n┌─────────┬────────┬───────────┬─────────┐\n";
        cout << "│    X    │  π(X)  │  X/log(X) │  Error  │\n";
        cout << "├─────────┼────────┼───────────┼─────────┤\n";
        c(RESET);
        
        for (int x = step; x <= X; x += step) {
            int pi = MathCore::sieve(x).size();
            double approx = x / log(x);
            double error = pi - approx;
            
            c(WHITE);
            cout << "│ " << setw(7) << x << " │ " << setw(6) << pi << " │ "
                 << setw(9) << fixed << setprecision(2) << approx << " │ "
                 << setw(7) << setprecision(2) << error << " │\n";
        }
        
        c(CYAN);
        cout << "└─────────┴────────┴───────────┴─────────┘\n";
        c(RESET);
        
        printTeacherInsight(
            "π(x) fluctuates around x/log(x). The Riemann Hypothesis\n"
            "║ predicts that |π(x) - Li(x)| < √x log(x) where Li is the\n"
            "║ logarithmic integral. These fluctuations encode deep\n"
            "║ information about the distribution of zeros of ζ(s).",
            "See Module 1.2.5 - Riemann Hypothesis Mode"
        );
    }
    
    static void primeGaps() {
        printBoxHeader("PRIME GAPS & TWIN PRIME EXPLORER");
        
        int N;
        c(YELLOW);
        cout << "Generate primes up to: ";
        c(RESET);
        cin >> N;
        
        vector<int> primes = MathCore::sieve(N);
        
        printStep(1, "Calculate gaps between consecutive primes");
        
        c(CYAN);
        cout << "\n┌─────────┬─────────┬──────┬─────────────────┐\n";
        cout << "│  Prime  │   Next  │ Gap  │   Classification │\n";
        cout << "├─────────┼─────────┼──────┼─────────────────┤\n";
        c(RESET);
        
        int twinCount = 0;
        for (size_t i = 0; i + 1 < primes.size(); i++) {
            int gap = primes[i + 1] - primes[i];
            c(WHITE);
            cout << "│ " << setw(7) << primes[i] << " │ " << setw(7) << primes[i+1] 
                 << " │ " << setw(4) << gap << " │ ";
            
            if (gap == 2) {
                c(GREEN);
                cout << "Twin Prime ★   ";
                twinCount++;
            } else {
                cout << "               ";
            }
            c(WHITE);
            cout << "│\n";
        }
        
        c(CYAN);
        cout << "└─────────┴─────────┴──────┴─────────────────┘\n";
        c(RESET);
        
        printResult("Found " + to_string(twinCount) + " twin prime pairs");
        
        printTeacherInsight(
            "Twin Prime Conjecture (unproven): Infinitely many primes p\n"
            "║ where p+2 is also prime. Zhang-Maynard-Tao proved bounded\n"
            "║ gaps: infinitely many prime pairs with gap < 246.",
            "Related to: Hardy-Littlewood Conjecture"
        );
    }
    
    static void mobiusFunction() {
        printBoxHeader("MÖBIUS FUNCTION - RANDOMNESS vs STRUCTURE");
        
        int N;
        c(YELLOW);
        cout << "Compute μ(n) up to: ";
        c(RESET);
        cin >> N;
        
        printStep(1, "Calculate Möbius function μ(n)");
        
        c(CYAN);
        cout << "\n┌──────┬────────┐\n";
        cout << "│  n   │  μ(n)  │\n";
        cout << "├──────┼────────┤\n";
        c(RESET);
        
        for (int i = 1; i <= N; i++) {
            int mu = MathCore::mobius(i);
            c(WHITE);
            cout << "│ " << setw(4) << i << " │ ";
            if (mu == 1) {
                c(GREEN);
                cout << " +1   ";
            } else if (mu == -1) {
                c(RED);
                cout << " -1   ";
            } else {
                c(YELLOW);
                cout << "  0   ";
            }
            c(WHITE);
            cout << "│\n";
        }
        
        c(CYAN);
        cout << "└──────┴────────┘\n";
        c(RESET);
        
        printTeacherInsight(
            "μ(n) = 1 if n is square-free with even # of prime factors\n"
            "║ μ(n) = -1 if n is square-free with odd # of prime factors\n"
            "║ μ(n) = 0 if n has a squared prime factor\n"
            "║ Appears random but satisfies Σμ(d) = 0 for n>1 (d|n).\n"
            "║ Key to Möbius inversion formula and Dirichlet series.",
            "Used in: Analytic Number Theory, Riemann Zeta Function"
        );
    }
};

// ==================== MODULAR ARITHMETIC MODULES ====================
struct ModularArithmetic {
    static void modularExponentiation() {
        printBoxHeader("MODULAR EXPONENTIATION - FAST POWER");
        
        long long base, exp, mod;
        c(YELLOW);
        cout << "Enter base: ";
        c(RESET);
        cin >> base;
        c(YELLOW);
        cout << "Enter exponent: ";
        c(RESET);
        cin >> exp;
        c(YELLOW);
        cout << "Enter modulus: ";
        c(RESET);
        cin >> mod;
        
        printStep(1, "Initialize result = 1, reduce base mod m");
        printStep(2, "Square-and-multiply algorithm", "If exp odd: result = (result * base) % m");
        
        c(YELLOW);
        cout << "\nComputing " << base << "^" << exp << " mod " << mod << "...\n";
        c(RESET);
        
        long long result = 1, b = base % mod, e = exp;
        int step = 1;
        
        while (e > 0) {
            if (e % 2 == 1) {
                c(CYAN);
                cout << "  [" << step++ << "] exp odd: result = (" << result << " * " << b << ") mod " << mod << " = ";
                result = (result * b) % mod;
                cout << result << "\n";
                c(RESET);
            }
            c(CYAN);
            cout << "  [" << step++ << "] Square base: b = (" << b << "²) mod " << mod << " = ";
            b = (b * b) % mod;
            cout << b << ", exp = " << e << "/2 = " << (e/2) << "\n";
            c(RESET);
            e /= 2;
        }
        
        printResult(to_string(base) + "^" + to_string(exp) + " mod " + to_string(mod) + " = " + to_string(result));
        
        printTeacherInsight(
            "Fast exponentiation: O(log n) multiplications instead of O(n).\n"
            "║ Critical for RSA encryption where exponents can be 1024+ bits!",
            "Used in: RSA [Module 3.1], Diffie-Hellman [Module 3.2.3]"
        );
        printCrossReference("Module 3.1 - RSA Cryptosystem");
    }
    
    static void extendedGCD() {
        printBoxHeader("EXTENDED EUCLIDEAN ALGORITHM");
        
        long long a, b;
        c(YELLOW);
        cout << "Enter first number: ";
        c(RESET);
        cin >> a;
        c(YELLOW);
        cout << "Enter second number: ";
        c(RESET);
        cin >> b;
        
        printStep(1, "Apply Euclidean algorithm while tracking coefficients");
        printStep(2, "Find x, y such that ax + by = gcd(a,b)", "Bézout's Identity");
        
        long long orig_a = a, orig_b = b;
        long long x0 = 1, x1 = 0, y0 = 0, y1 = 1;
        
        c(CYAN);
        cout << "\n┌─────────┬──────┬──────┬──────┬──────┐\n";
        cout << "│    a    │  b   │  x   │  y   │  q   │\n";
        cout << "├─────────┼──────┼──────┼──────┼──────┤\n";
        c(RESET);
        
        while (b != 0) {
            long long q = a / b;
            c(WHITE);
            cout << "│ " << setw(7) << a << " │ " << setw(4) << b 
                 << " │ " << setw(4) << x0 << " │ " << setw(4) << y0 
                 << " │ " << setw(4) << q << " │\n";
            c(RESET);
            
            long long t = b;
            b = a % b;
            a = t;
            
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
            
            t = y0;
            y0 = y1 - q * y0;
            y1 = t;
        }
        
        c(CYAN);
        cout << "└─────────┴──────┴──────┴──────┴──────┘\n";
        c(RESET);
        
        printResult("gcd(" + to_string(orig_a) + ", " + to_string(orig_b) + ") = " + to_string(a));
        c(GREEN);
        cout << "Bézout coefficients: x = " << x1 << ", y = " << y1 << "\n";
        cout << "Verify: " << orig_a << " × " << x1 << " + " << orig_b << " × " << y1 << " = " << a << "\n";
        c(RESET);
        
        printTeacherInsight(
            "Extended GCD is the foundation of modular inverse computation.\n"
            "║ Without it, RSA and many cryptographic systems are impossible!",
            "Direct application: Module 2.2.3 - Modular Inverse"
        );
        printCrossReference("Module 2.2.3 - Modular Inverse Calculator");
    }
    
    static void modularInverse() {
        printBoxHeader("MODULAR INVERSE CALCULATOR");
        
        long long a, m;
        c(YELLOW);
        cout << "Enter number a: ";
        c(RESET);
        cin >> a;
        c(YELLOW);
        cout << "Enter modulus m: ";
        c(RESET);
        cin >> m;
        
        printStep(1, "Check if gcd(a, m) = 1", "Inverse exists ⟺ gcd = 1");
        
        long long g = MathCore::gcd(a, m);
        c(CYAN);
        cout << "  gcd(" << a << ", " << m << ") = " << g << "\n";
        c(RESET);
        
        if (g != 1) {
            c(RED);
            cout << "\n✗ ERROR: Modular inverse does not exist (gcd ≠ 1)\n";
            c(RESET);
            return;
        }
        
        printStep(2, "Apply Extended Euclidean Algorithm");
        printCrossReference("See Module 2.2.2 for detailed explanation");
        
        long long inv = MathCore::modInverse(a, m);
        
        printStep(3, "Verification: (a × inv) mod m = 1");
        c(CYAN);
        cout << "  (" << a << " × " << inv << ") mod " << m << " = " 
             << (a * inv) % m << "\n";
        c(RESET);
        
        printResult("Modular inverse of " + to_string(a) + " mod " + to_string(m) + " = " + to_string(inv));
        
        printTeacherInsight(
            "Modular inverse is THE KEY to RSA decryption!\n"
            "║ We need d such that e×d ≡ 1 (mod φ(n)).\n"
            "║ This d becomes the private key for decryption.",
            "Critical for: RSA [Module 3.1], CRT [Module 2.3.2]"
        );
        printCrossReference("Module 3.1.1 - RSA Key Generation");
    }
    
    static void chineseRemainderTheorem() {
        printBoxHeader("CHINESE REMAINDER THEOREM SOLVER");
        
        int n;
        c(YELLOW);
        cout << "How many congruences? ";
        c(RESET);
        cin >> n;
        
        vector<long long> a(n), m(n);
        
        for (int i = 0; i < n; i++) {
            c(YELLOW);
            cout << "x ≡ ";
            c(RESET);
            cin >> a[i];
            c(YELLOW);
            cout << " (mod ";
            c(RESET);
            cin >> m[i];
            c(YELLOW);
            cout << ")\n";
            c(RESET);
        }
        
        printStep(1, "Verify pairwise coprimality of moduli");
        
        bool coprime = true;
        for (int i = 0; i < n && coprime; i++) {
            for (int j = i + 1; j < n; j++) {
                long long g = MathCore::gcd(m[i], m[j]);
                c(CYAN);
                cout << "  gcd(" << m[i] << ", " << m[j] << ") = " << g;
                if (g == 1) {
                    c(GREEN);
                    cout << " ✓\n";
                } else {
                    c(RED);
                    cout << " ✗ Not coprime!\n";
                    coprime = false;
                    break;
                }
                c(RESET);
            }
        }
        
        if (!coprime) {
            c(RED);
            cout << "\n✗ CRT cannot be applied (moduli not pairwise coprime)\n";
            c(RESET);
            return;
        }
        
        printStep(2, "Compute M = product of all moduli");
        long long M = 1;
        for (int i = 0; i < n; i++) M *= m[i];
        c(CYAN);
        cout << "  M = " << M << "\n";
        c(RESET);
        
        printStep(3, "For each congruence, compute Mi and its inverse");
        
        long long result = 0;
        for (int i = 0; i < n; i++) {
            long long Mi = M / m[i];
            long long inv = MathCore::modInverse(Mi, m[i]);
            
            c(CYAN);
            cout << "\n  Congruence " << (i+1) << ": x ≡ " << a[i] << " (mod " << m[i] << ")\n";
            cout << "    Mi = M/" << m[i] << " = " << Mi << "\n";
            cout << "    Mi⁻¹ mod " << m[i] << " = " << inv << "\n";
            cout << "    Contribution: " << a[i] << " × " << Mi << " × " << inv << " = " 
                 << (a[i] * Mi % M * inv % M) << "\n";
            c(RESET);
            
            result = (result + a[i] * Mi % M * inv) % M;
        }
        
        result = (result % M + M) % M;
        
        printResult("x ≡ " + to_string(result) + " (mod " + to_string(M) + ")");
        
        printTeacherInsight(
            "CRT allows solving systems of congruences independently!\n"
            "║ Used in: RSA optimization (CRT speeds up decryption 4×),\n"
            "║ secret sharing schemes, and parallel computation.",
            "Applications: Fast RSA, Shamir Secret Sharing"
        );
    }
};

// ==================== RSA CRYPTOGRAPHY ====================
struct RSACrypto {
    long long p, q, n, phi, e, d;
    bool keysGenerated;
    
    RSACrypto() : p(0), q(0), n(0), phi(0), e(0), d(0), keysGenerated(false) {}
    
    void generateKeys() {
        printBoxHeader("RSA KEY GENERATION");
        
        int choice;
        c(YELLOW);
        cout << "1. Auto-generate primes\n";
        cout << "2. Enter primes manually\n";
        cout << "Choice: ";
        c(RESET);
        cin >> choice;
        
        if (choice == 1) {
            printStep(1, "Auto-generating prime numbers p and q");
            srand(time(0));
            p = MathCore::nextPrime(100 + rand() % 900);
            q = MathCore::nextPrime(100 + rand() % 900);
            while (q == p) q = MathCore::nextPrime(q + 1);
        } else {
            printStep(1, "Enter two distinct prime numbers");
            c(YELLOW);
            cout << "Enter prime p: ";
            c(RESET);
            cin >> p;
            c(YELLOW);
            cout << "Enter prime q: ";
            c(RESET);
            cin >> q;
            
            if (!MathCore::isPrime(p) || !MathCore::isPrime(q)) {
                c(RED);
                cout << "\n✗ ERROR: Both numbers must be prime!\n";
                c(RESET);
                return;
            }
        }
        
        c(GREEN);
        cout << "  Selected: p = " << p << ", q = " << q << "\n";
        c(RESET);
        
        printStep(2, "Calculate n = p × q", "Modulus for encryption/decryption");
        n = p * q;
        c(CYAN);
        cout << "  n = " << n << "\n";
        c(RESET);
        
        printStep(3, "Calculate φ(n) = (p-1)(q-1)", "Euler's totient function");
        phi = (p - 1) * (q - 1);
        c(CYAN);
        cout << "  φ(n) = " << phi << "\n";
        c(RESET);
        
        printStep(4, "Choose public exponent e", "1 < e < φ(n), gcd(e,φ(n)) = 1");
        e = 65537;
        if (e >= phi) e = 3;
        while (MathCore::gcd(e, phi) != 1) e += 2;
        c(CYAN);
        cout << "  e = " << e << "\n";
        c(RESET);
        
        printStep(5, "Calculate private key d = e⁻¹ mod φ(n)");
        printCrossReference("Uses Module 2.2.3 - Modular Inverse");
        d = MathCore::modInverse(e, phi);
        c(CYAN);
        cout << "  d = " << d << "\n";
        c(RESET);
        
        c(GREEN);
        cout << "\n✓ RSA Keys Generated Successfully!\n";
        c(YELLOW);
        cout << "  Public Key:  (e=" << e << ", n=" << n << ")\n";
        cout << "  Private Key: (d=" << d << ", n=" << n << ")\n";
        c(RESET);
        
        keysGenerated = true;
        
        printTeacherInsight(
            "RSA security relies on factorization hardness:\n"
            "║ Easy: n = p × q (multiply primes)\n"
            "║ Hard: Given n, find p and q (factor n)\n"
            "║ For 2048-bit n, factoring takes ~10²⁰ years with current tech!",
            "Security: Module 3.1.6 - RSA Theory & Security"
        );
    }
    
    void encrypt() {
        if (!keysGenerated) {
            c(RED);
            cout << "Generate keys first (Module 3.1.1)!\n";
            c(RESET);
            return;
        }
        
        printBoxHeader("RSA ENCRYPTION");
        
        long long msg;
        c(YELLOW);
        cout << "Enter message (number): ";
        c(RESET);
        cin >> msg;
        
        printStep(1, "Verify message < n");
        c(CYAN);
        cout << "  message = " << msg << ", n = " << n << "\n";
        c(RESET);
        
        if (msg >= n) {
            c(RED);
            cout << "\n✗ ERROR: Message must be < n\n";
            c(RESET);
            return;
        }
        
        printStep(2, "Compute ciphertext = message^e mod n");
        long long cipher = MathCore::modPow(msg, e, n);
        
        printResult("Ciphertext = " + to_string(cipher));
        
        printTeacherInsight(
            "Public key encryption allows anyone to encrypt,\n"
            "║ but only the private key holder can decrypt!",
            "Decryption: Module 3.1.3"
        );
    }
    
    void decrypt() {
        if (!keysGenerated) {
            c(RED);
            cout << "Generate keys first (Module 3.1.1)!\n";
            c(RESET);
            return;
        }
        
        printBoxHeader("RSA DECRYPTION");
        
        long long cipher;
        c(YELLOW);
        cout << "Enter ciphertext: ";
        c(RESET);
        cin >> cipher;
        
        printStep(1, "Compute plaintext = ciphertext^d mod n");
        long long plain = MathCore::modPow(cipher, d, n);
        
        printResult("Decrypted message = " + to_string(plain));
        
        printTeacherInsight(
            "RSA correctness: (m^e)^d ≡ m (mod n)\n"
            "║ Proof uses Euler's theorem: a^φ(n) ≡ 1 (mod n)",
            "Theory: Fermat's Little Theorem, Euler's Theorem"
        );
    }
};

// ==================== BLOCKCHAIN VOTING SYSTEM ====================
struct BlockchainVoting {
    struct Block {
        int index;
        string timestamp;
        string voterID;
        string vote;
        string signature;
        string prevHash;
        string hash;
        int nonce;
        
        Block(int idx, string vID, string v, string sig, string pH) 
            : index(idx), voterID(vID), vote(v), signature(sig), prevHash(pH), nonce(0) {
            timestamp = to_string(time(0));
            hash = calculateHash();
        }
        
        string calculateHash() {
            stringstream ss;
            ss << index << timestamp << voterID << vote << signature << prevHash << nonce;
            unsigned long h = 5381;
            for (char c : ss.str()) h = ((h << 5) + h) + c;
            
            stringstream result;
            result << hex << setfill('0') << setw(16) << h;
            return result.str();
        }
        
        void mine(int difficulty) {
            string target(difficulty, '0');
            while (hash.substr(0, difficulty) != target) {
                nonce++;
                hash = calculateHash();
            }
        }
    };
    
    vector<Block> chain;
    map<string, bool> voters;
    map<string, int> voteCount;
    int difficulty;
    
    BlockchainVoting() : difficulty(2) {
        chain.push_back(Block(0, "GENESIS", "GENESIS", "0", "0"));
    }
    
    void registerVoter() {
        printBoxHeader("REGISTER VOTER");
        
        string id, name;
        c(YELLOW);
        cout << "Enter Voter ID: ";
        c(RESET);
        cin >> id;
        cin.ignore();
        c(YELLOW);
        cout << "Enter Name: ";
        c(RESET);
        getline(cin, name);
        
        if (voters.find(id) != voters.end()) {
            c(RED);
            cout << "Voter already registered!\n";
            c(RESET);
            return;
        }
        
        voters[id] = false;
        c(GREEN);
        cout << "\n✓ Voter registered: " << name << " (ID: " << id << ")\n";
        c(RESET);
        
        printTeacherInsight(
            "In real blockchain voting, each voter gets RSA key pair.\n"
            "║ Public key identifies voter, private key signs votes.",
            "Crypto: Module 3.1 - RSA System"
        );
    }
    
    void castVote() {
        printBoxHeader("CAST VOTE");
        
        string id, candidate;
        c(YELLOW);
        cout << "Enter Voter ID: ";
        c(RESET);
        cin >> id;
        cin.ignore();
        
        if (voters.find(id) == voters.end()) {
            c(RED);
            cout << "Voter not registered!\n";
            c(RESET);
            return;
        }
        
        if (voters[id]) {
            c(RED);
            cout << "Already voted!\n";
            c(RESET);
            return;
        }
        
        c(YELLOW);
        cout << "Enter Candidate: ";
        c(RESET);
        getline(cin, candidate);
        
        printStep(1, "Create digital signature (simulated)");
        string sig = to_string(rand() % 10000);
        
        printStep(2, "Create new block");
        Block newBlock(chain.size(), id, candidate, sig, chain.back().hash);
        
        printStep(3, "Mine block (Proof of Work)");
        c(CYAN);
        cout << "  Mining with difficulty " << difficulty << "...\n";
        c(RESET);
        
        newBlock.mine(difficulty);
        
        c(GREEN);
        cout << "  ✓ Block mined! Nonce: " << newBlock.nonce << "\n";
        cout << "  Hash: " << newBlock.hash << "\n";
        c(RESET);
        
        printStep(4, "Add block to chain");
        chain.push_back(newBlock);
        voters[id] = true;
        voteCount[candidate]++;
        
        c(GREEN);
        cout << "\n✓ Vote recorded in blockchain!\n";
        c(RESET);
        
        printTeacherInsight(
            "Proof of Work prevents spam and tampering.\n"
            "║ Finding nonce with leading zeros requires computational work.\n"
            "║ Changing old blocks requires re-mining all subsequent blocks!",
            "Concepts: Hashing [Module 3.2.1], Crypto [Module 3.1]"
        );
    }
    
    void verifyChain() {
        printBoxHeader("BLOCKCHAIN VERIFICATION");
        
        printStep(1, "Verify each block's integrity");
        
        bool valid = true;
        for (size_t i = 1; i < chain.size(); i++) {
            c(CYAN);
            cout << "\n  Checking Block #" << i << "...\n";
            c(RESET);
            
            if (chain[i].hash != chain[i].calculateHash()) {
                c(RED);
                cout << "    ✗ Hash mismatch!\n";
                c(RESET);
                valid = false;
            } else {
                c(GREEN);
                cout << "    ✓ Hash valid\n";
                c(RESET);
            }
            
            if (chain[i].prevHash != chain[i-1].hash) {
                c(RED);
                cout << "    ✗ Chain link broken!\n";
                c(RESET);
                valid = false;
            } else {
                c(GREEN);
                cout << "    ✓ Link valid\n";
                c(RESET);
            }
            
            string target(difficulty, '0');
            if (chain[i].hash.substr(0, difficulty) != target) {
                c(RED);
                cout << "    ✗ Proof of work invalid!\n";
                c(RESET);
                valid = false;
            } else {
                c(GREEN);
                cout << "    ✓ Proof of work valid\n";
                c(RESET);
            }
        }
        
        if (valid) {
            c(GREEN);
            cout << "\n✓ BLOCKCHAIN IS VALID!\n";
            c(RESET);
        } else {
            c(RED);
            cout << "\n✗ BLOCKCHAIN COMPROMISED!\n";
            c(RESET);
        }
        
        printTeacherInsight(
            "Blockchain immutability comes from cryptographic linking.\n"
            "║ Each block contains hash of previous block.\n"
            "║ Changing any block breaks all subsequent hashes!",
            "Graph Theory: Directed Acyclic Graph (DAG)"
        );
    }
    
    void displayResults() {
        printBoxHeader("ELECTION RESULTS");
        
        c(CYAN);
        cout << "\nTotal votes cast: " << (chain.size() - 1) << "\n\n";
        c(RESET);
        
        c(CYAN);
        cout << "┌────────────────────┬────────┬────────────┐\n";
        cout << "│    Candidate       │ Votes  │ Percentage │\n";
        cout << "├────────────────────┼────────┼────────────┤\n";
        c(RESET);
        
        int total = chain.size() - 1;
        string winner = "";
        int maxVotes = 0;
        
        for (auto& p : voteCount) {
            double pct = total > 0 ? (p.second * 100.0 / total) : 0;
            c(WHITE);
            cout << "│ " << setw(18) << left << p.first << " │ " 
                 << setw(6) << right << p.second << " │ " 
                 << setw(9) << fixed << setprecision(2) << pct << "% │\n";
            
            if (p.second > maxVotes) {
                maxVotes = p.second;
                winner = p.first;
            }
        }
        
        c(CYAN);
        cout << "└────────────────────┴────────┴────────────┘\n";
        c(RESET);
        
        if (!winner.empty()) {
            c(MAGENTA);
            cout << "\n★ WINNER: " << winner << " with " << maxVotes << " votes!\n";
            c(RESET);
        }
    }
    
    void exportBlockchain() {
        string filename;
        c(YELLOW);
        cout << "Enter filename: ";
        c(RESET);
        cin >> filename;
        
        ofstream file(filename.c_str());
        if (!file) {
            c(RED);
            cout << "Cannot create file!\n";
            c(RESET);
            return;
        }
        
        file << "BLOCKCHAIN VOTING SYSTEM - EXPORT\n";
        file << "==================================\n\n";
        
        for (size_t i = 0; i < chain.size(); i++) {
            file << "Block #" << chain[i].index << "\n";
            file << "Timestamp: " << chain[i].timestamp << "\n";
            file << "Voter ID: " << chain[i].voterID << "\n";
            file << "Vote: " << chain[i].vote << "\n";
            file << "Hash: " << chain[i].hash << "\n";
            file << "Nonce: " << chain[i].nonce << "\n";
            file << "-----------------------------------\n\n";
        }
        
        file.close();
        c(GREEN);
        cout << "✓ Blockchain exported to " << filename << "\n";
        c(RESET);
    }
};

// ==================== PROBLEM SOLVER ====================
struct ProblemSolver {
    static void solveFromKeyboard() {
        printBoxHeader("INTERACTIVE PROBLEM SOLVER");
        
        cin.ignore();
        c(YELLOW);
        cout << "Enter problem (or 'help' for examples):\n";
        c(CYAN);
        cout << "> ";
        c(RESET);
        
        string problem;
        getline(cin, problem);
        
        if (problem == "help") {
            c(GREEN);
            cout << "\nSupported problem types:\n";
            cout << "  • 5^3 mod 7\n";
            cout << "  • inverse of 3 mod 7\n";
            cout << "  • is 17 prime\n";
            cout << "  • gcd of 48 and 18\n";
            c(RESET);
            return;
        }
        
        solve(problem);
    }
    
    static void solveFromFile() {
        printBoxHeader("SOLVE FROM FILE");
        
        string filename;
        c(YELLOW);
        cout << "Input file: ";
        c(RESET);
        cin >> filename;
        
        ifstream file(filename.c_str());
        if (!file) {
            c(RED);
            cout << "Cannot open file!\n";
            c(RESET);
            return;
        }
        
        string line;
        int num = 1;
        while (getline(file, line)) {
            if (!line.empty()) {
                c(YELLOW);
                cout << "\n════════════ Problem " << num++ << " ════════════\n";
                c(RESET);
                solve(line);
            }
        }
    }
    
    static void solve(const string& problem) {
        if (problem.find("mod") != string::npos) {
            long long base, exp, mod;
            char dummy;
            stringstream ss(problem);
            ss >> base >> dummy >> exp;
            string modStr;
            ss >> modStr >> mod;
            
            long long result = MathCore::modPow(base, exp, mod);
            printResult(to_string(base) + "^" + to_string(exp) + " mod " + to_string(mod) + " = " + to_string(result));
        }
        else if (problem.find("inverse") != string::npos) {
            long long a, m;
            sscanf(problem.c_str(), "inverse of %lld mod %lld", &a, &m);
            long long inv = MathCore::modInverse(a, m);
            printResult("Inverse = " + to_string(inv));
        }
        else if (problem.find("prime") != string::npos) {
            long long n;
            sscanf(problem.c_str(), "is %lld prime", &n);
            bool prime = MathCore::isPrime(n);
            c(prime ? GREEN : RED);
            cout << "\n" << n << " is " << (prime ? "PRIME ✓" : "COMPOSITE ✗") << "\n";
            c(RESET);
        }
        else if (problem.find("gcd") != string::npos) {
            long long a, b;
            sscanf(problem.c_str(), "gcd of %lld and %lld", &a, &b);
            long long g = MathCore::gcd(a, b);
            printResult("gcd = " + to_string(g));
        }
        else {
            c(RED);
            cout << "Problem type not recognized!\n";
            c(RESET);
        }
    }
};

// ==================== ALGORITHM COMPARISON ====================
struct AlgorithmComparison {
    static void primalityComparison() {
        printBoxHeader("PRIMALITY TEST COMPARISON");
        
        long long n;
        c(YELLOW);
        cout << "Enter number to test: ";
        c(RESET);
        cin >> n;
        
        c(CYAN);
        cout << "\n┌─────────────────────┬──────────┬───────────────┐\n";
        cout << "│ Algorithm           │ Result   │ Complexity    │\n";
        cout << "├─────────────────────┼──────────┼───────────────┤\n";
        c(RESET);
        
        bool prime = MathCore::isPrime(n);
        
        c(WHITE);
        cout << "│ Trial Division      │ ";
        c(prime ? GREEN : RED);
        cout << (prime ? "PRIME   " : "COMPOSITE");
        c(WHITE);
        cout << " │ O(√n)         │\n";
        
        cout << "│ Sieve (range)       │ ";
        c(prime ? GREEN : RED);
        cout << (prime ? "PRIME   " : "COMPOSITE");
        c(WHITE);
        cout << " │ O(n log log n)│\n";
        
        c(CYAN);
        cout << "└─────────────────────┴──────────┴───────────────┘\n";
        c(RESET);
        
        printTeacherInsight(
            "Trial Division: Simple but slow for large n\n"
            "║ Miller-Rabin (not shown): Probabilistic, O(k log³ n)\n"
            "║ AKS (2002): Deterministic polynomial time, O(log⁶ n)",
            "For cryptography (1024+ bit primes), use Miller-Rabin"
        );
    }
};

// ==================== THEORY & DOCUMENTATION ====================
struct Theory {
    static void showDefinitions() {
        printBoxHeader("DEFINITIONS & EXAMPLES");
        
        c(CYAN);
        cout << "\n1. MODULAR ARITHMETIC\n";
        c(WHITE);
        cout << "   a ≡ b (mod m) if m | (a-b)\n";
        cout << "   Example: 17 ≡ 2 (mod 5)\n\n";
        
        c(CYAN);
        cout << "2. PRIME NUMBERS\n";
        c(WHITE);
        cout << "   Natural number > 1 with exactly 2 divisors\n";
        cout << "   Examples: 2, 3, 5, 7, 11, 13, ...\n\n";
        
        c(CYAN);
        cout << "3. GCD (Greatest Common Divisor)\n";
        c(WHITE);
        cout << "   Largest number dividing both a and b\n";
        cout << "   Example: gcd(24, 36) = 12\n\n";
        
        c(CYAN);
        cout << "4. EULER'S TOTIENT φ(n)\n";
        c(WHITE);
        cout << "   Count of numbers < n coprime to n\n";
        cout << "   Example: φ(10) = 4  (1,3,7,9)\n\n";
        
        c(CYAN);
        cout << "5. FERMAT'S LITTLE THEOREM\n";
        c(WHITE);
        cout << "   If p prime and gcd(a,p)=1: a^(p-1) ≡ 1 (mod p)\n";
        cout << "   Example: 2^6 mod 7 = 1\n\n";
        
        c(CYAN);
        cout << "6. CHINESE REMAINDER THEOREM\n";
        c(WHITE);
        cout << "   Solves systems x ≡ ai (mod mi) for coprime mi\n\n";
        
        c(CYAN);
        cout << "7. RSA CRYPTOSYSTEM\n";
        c(WHITE);
        cout << "   Public: (e,n), Private: (d,n)\n";
        cout << "   Encrypt: c = m^e mod n\n";
        cout << "   Decrypt: m = c^d mod n\n\n";
        
        c(RESET);
    }
    
    static void showBridgeConcepts() {
        printBoxHeader("BRIDGE CONCEPTS - MODULE CONNECTIONS");
        
        c(YELLOW);
        cout << "\n🔗 How Modules Connect:\n\n";
        c(RESET);
        
        c(CYAN);
        cout << "Extended GCD [2.2.2]\n";
        c(WHITE);
        cout << "  ↓ provides\n";
        c(GREEN);
        cout << "Modular Inverse [2.2.3]\n";
        c(WHITE);
        cout << "  ↓ required by\n";
        c(MAGENTA);
        cout << "RSA Key Generation [3.1.1]\n";
        c(WHITE);
        cout << "  ↓ enables\n";
        c(BLUE);
        cout << "Blockchain Digital Signatures [4.1.2]\n\n";
        c(RESET);
        
        c(CYAN);
        cout << "Prime Generation [1.1.2]\n";
        c(WHITE);
        cout << "  ↓ feeds\n";
        c(GREEN);
        cout << "RSA Key Generation [3.1.1]\n\n";
        c(RESET);
        
        c(CYAN);
        cout << "Modular Exponentiation [2.1.5]\n";
        c(WHITE);
        cout << "  ↓ used in\n";
        c(GREEN);
        cout << "RSA Encrypt/Decrypt [3.1.2,3.1.3]\n";
        c(WHITE);
        cout << "  ↓ powers\n";
        c(BLUE);
        cout << "Blockchain Voting [4.1]\n\n";
        c(RESET);
    }
};

// ==================== MAIN MENU SYSTEM ====================
void primeNumberTheoryMenu();
void modularArithmeticMenu();
void cryptographyMenu();
void researchProjectsMenu();
void problemSolverMenu();
void theoryMenu();

void mainMenu() {
    while (true) {
        clearScreen();
        printBoxHeader("DISCRETE MATHEMATICS RESEARCH TOOLKIT");
        
        c(GREEN);
        cout << "\n┌─── Main Menu ───────────────────────────────────────────┐\n";
        c(YELLOW);
        cout << "│                                                         │\n";
        cout << "│  1. 🔢 Prime Number Theory                              │\n";
        cout << "│  2. ⚙️  Modular Arithmetic                               │\n";
        cout << "│  3. 🔐 Cryptography Suite                               │\n";
        cout << "│  4. 🌐 Research Projects (Real-World)                   │\n";
        cout << "│  5. 📝 Problem Solver & Practice                        │\n";
        cout << "│  6. 📚 Theory & Documentation                           │\n";
        cout << "│  7. ❌ Exit                                              │\n";
        cout << "│                                                         │\n";
        c(GREEN);
        cout << "└─────────────────────────────────────────────────────────┘\n";
        c(CYAN);
        cout << "\nSelect option (1-7): ";
        c(RESET);
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: primeNumberTheoryMenu(); break;
            case 2: modularArithmeticMenu(); break;
            case 3: cryptographyMenu(); break;
            case 4: researchProjectsMenu(); break;
            case 5: problemSolverMenu(); break;
            case 6: theoryMenu(); break;
            case 7:
                c(GREEN);
                cout << "\n✓ Thank you for using DM Toolkit!\n";
                cout << "  Made for University Students 🎓\n";
                c(RESET);
                return;
            default:
                c(RED);
                cout << "Invalid choice!\n";
                c(RESET);
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

void primeNumberTheoryMenu() {
    clearScreen();
    printBoxHeader("PRIME NUMBER THEORY");
    
    c(GREEN);
    cout << "\n┌─── Submenu ─────────────────────────────────────────────┐\n";
    c(YELLOW);
    cout << "│  1.1 Check if Prime                                     │\n";
    cout << "│  1.2 Generate Primes in Range                           │\n";
    cout << "│  1.3 Find Next Prime                                    │\n";
    cout << "│  1.4 Prime Distribution Observatory                     │\n";
    cout << "│  1.5 Prime Gaps Analysis                                │\n";
    cout << "│  1.6 Möbius Function                                    │\n";
    cout << "│  0. Back                                                │\n";
    c(GREEN);
    cout << "└─────────────────────────────────────────────────────────┘\n";
    c(CYAN);
    cout << "\nSelect: ";
    c(RESET);
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: PrimeTheory::checkPrime(); break;
        case 2: PrimeTheory::generatePrimesInRange(); break;
        case 3: PrimeTheory::findNextPrime(); break;
        case 4: PrimeTheory::primeDistribution(); break;
        case 5: PrimeTheory::primeGaps(); break;
        case 6: PrimeTheory::mobiusFunction(); break;
    }
}

void modularArithmeticMenu() {
    clearScreen();
    printBoxHeader("MODULAR ARITHMETIC");
    
    c(GREEN);
    cout << "\n┌─── Submenu ─────────────────────────────────────────────┐\n";
    c(YELLOW);
    cout << "│  2.1 Modular Exponentiation                             │\n";
    cout << "│  2.2 Extended Euclidean Algorithm                       │\n";
    cout << "│  2.3 Modular Inverse Calculator                         │\n";
    cout << "│  2.4 Chinese Remainder Theorem                          │\n";
    cout << "│  0. Back                                                │\n";
    c(GREEN);
    cout << "└─────────────────────────────────────────────────────────┘\n";
    c(CYAN);
    cout << "\nSelect: ";
    c(RESET);
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: ModularArithmetic::modularExponentiation(); break;
        case 2: ModularArithmetic::extendedGCD(); break;
        case 3: ModularArithmetic::modularInverse(); break;
        case 4: ModularArithmetic::chineseRemainderTheorem(); break;
    }
}

void cryptographyMenu() {
    static RSACrypto rsa;
    
    clearScreen();
    printBoxHeader("CRYPTOGRAPHY SUITE");
    
    c(GREEN);
    cout << "\n┌─── Submenu ─────────────────────────────────────────────┐\n";
    c(YELLOW);
    cout << "│  3.1 Generate RSA Keys                                  │\n";
    cout << "│  3.2 Encrypt Number                                     │\n";
    cout << "│  3.3 Decrypt Number                                     │\n";
    cout << "│  0. Back                                                │\n";
    c(GREEN);
    cout << "└─────────────────────────────────────────────────────────┘\n";
    c(CYAN);
    cout << "\nSelect: ";
    c(RESET);
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: rsa.generateKeys(); break;
        case 2: rsa.encrypt(); break;
        case 3: rsa.decrypt(); break;
    }
}

// void researchProjectsMenu() {
//     static BlockchainVoting blockchain;
    
//     clearScreen();
//     printBoxHeader("RESEARCH PROJECTS - REAL WORLD");
    
//     c(GREEN);
//     cout << "\n┌─── Blockchain Voting System ────────────────────────────┐\n";
//     c(YELLOW);
//     cout << "│  4.1 Register Voter                                     │\n";
//     cout << "│  4.2 Cast Vote                                          │\n";
// }
void researchProjectsMenu() {
    static BlockchainVoting blockchain;
    
    clearScreen();
    printBoxHeader("RESEARCH PROJECTS - REAL WORLD");
    
    c(GREEN);
    cout << "\n┌─── Blockchain Voting System ────────────────────────────┐\n";
    c(YELLOW);
    cout << "│  4.1 Register Voter                                     │\n";
    cout << "│  4.2 Cast Vote                                          │\n";
    cout << "│  4.3 Verify Blockchain                                  │\n";
    cout << "│  4.4 Display Results                                    │\n";
    cout << "│  4.5 Export Blockchain                                  │\n";
    cout << "│  0. Back                                                │\n";
    c(GREEN);
    cout << "└─────────────────────────────────────────────────────────┘\n";
    c(CYAN);
    cout << "\nSelect: ";
    c(RESET);
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: blockchain.registerVoter(); break;
        case 2: blockchain.castVote(); break;
        case 3: blockchain.verifyChain(); break;
        case 4: blockchain.displayResults(); break;
        case 5: blockchain.exportBlockchain(); break;
    }
}

void problemSolverMenu() {
    clearScreen();
    printBoxHeader("PROBLEM SOLVER & PRACTICE");
    
    c(GREEN);
    cout << "\n┌─── Submenu ─────────────────────────────────────────────┐\n";
    c(YELLOW);
    cout << "│  5.1 Solve from Keyboard                                │\n";
    cout << "│  5.2 Solve from File                                    │\n";
    cout << "│  5.3 Algorithm Comparison                               │\n";
    cout << "│  0. Back                                                │\n";
    c(GREEN);
    cout << "└─────────────────────────────────────────────────────────┘\n";
    c(CYAN);
    cout << "\nSelect: ";
    c(RESET);
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: ProblemSolver::solveFromKeyboard(); break;
        case 2: ProblemSolver::solveFromFile(); break;
        case 3: AlgorithmComparison::primalityComparison(); break;
    }
}

void theoryMenu() {
    clearScreen();
    printBoxHeader("THEORY & DOCUMENTATION");
    
    c(GREEN);
    cout << "\n┌─── Submenu ─────────────────────────────────────────────┐\n";
    c(YELLOW);
    cout << "│  6.1 Definitions & Examples                             │\n";
    cout << "│  6.2 Bridge Concepts (Module Connections)               │\n";
    cout << "│  0. Back                                                │\n";
    c(GREEN);
    cout << "└─────────────────────────────────────────────────────────┘\n";
    c(CYAN);
    cout << "\nSelect: ";
    c(RESET);
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: Theory::showDefinitions(); break;
        case 2: Theory::showBridgeConcepts(); break;
    }
}

// ==================== MAIN FUNCTION ====================
int main() {
    // Set console to support UTF-8 (for special characters)
    SetConsoleOutputCP(65001);
    
    // Seed random number generator
    srand(static_cast<unsigned int>(time(0)));
    
    // Start the main menu
    mainMenu();
    
    return 0;
}