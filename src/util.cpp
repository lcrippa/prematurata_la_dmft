#include <vector>
#include <memory>
#include <complex>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <edipack2_cbinding.h>



std::string bold_green(const std::string& s) { return "\033[1;32m" + s + "\033[0m"; }
std::string bold_yellow(const std::string& s) { return "\033[1;33m" + s + "\033[0m"; }
std::string bold_red(const std::string& s) { return "\033[1;31m" + s + "\033[0m"; }


// Check convergence
bool check_convergence(const std::vector<std::complex<double>>& Xnew, double eps, int N1, int N2) {
    static std::vector<std::complex<double>> Xold;
    static int success = 0;
    static int check = 1;
    
    bool convergence = false;
    
    // Allocate and zero Xold if needed
    int Msum = Xnew.size();
    if (Xold.size() != Msum) {
        Xold.assign(Msum, std::complex<double>(0.0, 0.0));
    }

    double M = 0.0, S = 0.0;
    for (int i = 0; i < Msum; ++i) {
        M += std::abs(Xnew[i] - Xold[i]);
        S += std::abs(Xnew[i]);
    }

    double err = M / S;

    // Update Xold
    Xold = Xnew;

    std::ofstream fout("error.err", std::ios::app);
    fout << std::setw(5) << check << std::scientific << std::setprecision(7) << " " << err << "\n";

    if (err < eps) {
        ++success;
    } else {
        success = 0;
    }

    if (success > N1) {
        convergence = true;
    }

    if (check >= N2) {
        std::ofstream ferr("ERROR.README");
        ferr << "\n";
        std::cerr << "Not converged after " << N2 << " iterations." << std::endl;
        convergence = true;
    }

    if (convergence) {
        std::cout << bold_green("error=") << std::scientific << std::setprecision(7) << err << std::endl;
    } else if (err < eps) {
        std::cout << bold_yellow("error=") << std::scientific << std::setprecision(7) << err << std::endl;
    } else {
        std::cout << bold_red("error=") << std::scientific << std::setprecision(7) << err << std::endl;
    }

    ++check;
    return convergence;
}

extern "C" {
  
std::string inputFile; 
int Le = 1000;
int iloop = 0;
int Nb;
double wmixing = 0.5;
double Wband = 1.0;
double pi = M_PI;
double de = 2*Wband/Le;
bool converged = false;
int64_t bath_dim[1];
int total_size;
int total_size_n5;



// Solver-specific arrays
std::vector<int64_t> h_dim;
std::vector<int64_t> delta_dim;
std::vector<std::complex<double>> wm;
std::vector<double> Ebands, Dbands;
std::vector<double> Bath;
std::vector<double> Bath_prev;
std::vector<std::complex<double>> Hloc;
std::vector<std::complex<double>> Gimp_mats;
std::vector<std::complex<double>> Self_mats;
std::vector<std::complex<double>> Gloc_mats;
std::vector<std::complex<double>> Delta;



void leggi() {
  char c_input[inputFile.size()+1];
  std::strcpy(c_input, inputFile.c_str());
  read_input(c_input); 
}

void aggiungi(char lettera) {
  inputFile += lettera;
}

int ilbagnoepronto() {
  int Nb;
  Nb = get_bath_dimension();
  return Nb;
}

void lavati(int sapone) {
  Bath.resize(sapone);
  Bath_prev.resize(sapone);
  bath_dim[0]=sapone;
}

void lavatiancora() {
  Bath_prev = Bath;
}

// Return length
int quantoh() {
  return Hloc.size();
}
int quantog() {
  return Gimp_mats.size();
}
int quantos() {
  return Self_mats.size();
}
int quantol() {
  return Gloc_mats.size();
}
int quantod() {
  return Delta.size();
}
int quantow() {
  return wm.size();
}
int quantohdim() {
  return h_dim.size();
}
int quantodeltadim() {
  return delta_dim.size();
}

// Set length, init to 0
void iniziah(int quantolungo) {
  Hloc.resize(quantolungo, std::complex<double>(0.0,0.0)); 
}
void iniziag(int quantolungo) {
  Gimp_mats.resize(quantolungo, std::complex<double>(0.0,0.0)); 
}
void inizias(int quantolungo) {
  Self_mats.resize(quantolungo, std::complex<double>(0.0,0.0)); 
}
void inizial(int quantolungo) {
  Gloc_mats.resize(quantolungo, std::complex<double>(0.0,0.0)); 
}
void iniziad(int quantolungo) {
  Delta.resize(quantolungo, std::complex<double>(0.0,0.0));
}
void iniziaw(int quantolungo) {
  wm.resize(quantolungo, std::complex<double>(0.0,0.0)); 
}
void iniziahdim(int quantolungo) {
  h_dim.resize(quantolungo, 0); 
}
void iniziadeltadim(int quantolungo) {
  delta_dim.resize(quantolungo, 0); 
}


// Grow array
void crescih(double reale, double immaginario) {
  Hloc.push_back(std::complex<double>(reale,immaginario));
}
void crescig(double reale, double immaginario) {
  Gimp_mats.push_back(std::complex<double>(reale,immaginario));
}
void crescis(double reale, double immaginario) {
  Self_mats.push_back(std::complex<double>(reale,immaginario));
}
void crescil(double reale, double immaginario) {
  Gloc_mats.push_back(std::complex<double>(reale,immaginario));
}
void crescid(double reale, double immaginario) {
  Delta.push_back(std::complex<double>(reale,immaginario));
}
void cresciw(double reale, double immaginario) {
  wm.push_back(std::complex<double>(reale,immaginario));
}
void crescihdim(int valore) {
  h_dim.push_back(valore);
}
void crescideltadim(int valore) {
  delta_dim.push_back(valore);
}

//Set element
void prendih(int elemento, double reale, double immaginario) {
  Hloc[elemento] = std::complex<double>(reale,immaginario);
}
void prendig(int elemento, double reale, double immaginario) {
  Gimp_mats[elemento] = std::complex<double>(reale,immaginario);
}
void prendis(int elemento, double reale, double immaginario) {
  Self_mats[elemento] = std::complex<double>(reale,immaginario);
}
void prendil(int elemento, double reale, double immaginario) {
  Gloc_mats[elemento] = std::complex<double>(reale,immaginario);
}   
void prendid(int elemento, double reale, double immaginario) {
  Delta[elemento] = std::complex<double>(reale,immaginario);
}
void prendiw(int elemento, double reale, double immaginario) {
  wm[elemento] = std::complex<double>(reale,immaginario);
}
void prendihdim(int elemento, int valore) {
  h_dim[elemento] = valore;
}
void prendideltadim(int elemento, int valore) {
  delta_dim[elemento] = valore;
} 

//return element
double dammih(int elemento, int quale) {
  if (quale==0) {
    return Hloc[elemento].real();
  } else {
    return Hloc[elemento].imag();
  }
}

double dammig(int elemento, int quale) {
  if (quale==0) {
    return Gimp_mats[elemento].real();
  } else {
    return Gimp_mats[elemento].imag();
  }
}

double dammis(int elemento, int quale) {
  if (quale==0) {
    return Self_mats[elemento].real();
  } else {
    return Self_mats[elemento].imag();
  }
}

double dammil(int elemento, int quale) {
  if (quale==0) {
    return Gloc_mats[elemento].real();
  } else {
    return Gloc_mats[elemento].imag();
  }
}   

double dammid(int elemento, int quale) {
  if (quale==0) {
    return Delta[elemento].real();
  } else {
    return Delta[elemento].imag();
  }
}

double dammiw(int elemento, int quale) {
  if (quale==0) {
    return wm[elemento].real();
  } else {
    return wm[elemento].imag();
  }
}

int dammihdim(int elemento) {
  return h_dim[elemento];
}

int dammideltadim(int elemento) {
  return delta_dim[elemento];
} 

int dammintero(int elemento) {
  switch (elemento) {
  case 0:
    return Nspin;
  case 1:
    return Norb;
  case 2:
    return Lmats;
  case 3:
    return Le;
  }
  return 0;
}

void prendintero(int elemento, int numero) {
  switch (elemento) {
  case 0:
    Nspin = numero;
  case 1:
    Norb = numero;
  case 2:
    Lmats = numero;
  case 3:
    Le = numero;
  }
  return;
}

double dammidecimale(int elemento) {
  switch (elemento) {
  case 0:
    return beta;
  case 1:
    return xmu;
  case 2:
    return Wband;
  }
  return 0.0;
}

void prendidecimale(int elemento, double numero) {
  switch (elemento) {
  case 0:
    beta = numero;
  case 1:
    xmu  = numero;
  case 2:
    Wband  = numero;
  }
  return;
}
  
  
// linspace: Linearly spaced values
std::vector<double> linspace(double start, double end, int num) {
    std::vector<double> values(num);
    double step = (end - start) / (num - 1);
    for (int i = 0; i < num; ++i) {
        values[i] = start + i * step;
    }
    return values;
}

// arange: Range with step size
std::vector<double> arange(double start, double end, double step) {
    std::vector<double> values;
    for (double val = start; val < end; val += step) {
        values.push_back(val);
    }
    return values;
}

// dens_bethe: Semicircular Density of States
std::vector<double> dens_bethe(const std::vector<double>& energies, double W, double de) {
    std::vector<double> dos(energies.size());
    for (size_t i = 0; i < energies.size(); ++i) {
        double E = energies[i];
        if (abs(E) <= W) {
            dos[i] = de * (2.0 / M_PI / W) * sqrt(1.0 - pow(E / W, 2));
        } else {
            dos[i] = 0.0;
        }
    }
    return dos;
}  

void failados(){
    Ebands = linspace(-Wband,Wband,Le);
    Dbands = dens_bethe(Ebands,Wband,de);
}

void passalah(){
    ed_set_Hloc_single_N4(Hloc.data(), h_dim.data());
}

void iniziailsolver(){
   init_solver_site(Bath.data(), bath_dim);
}

void risolvi(){
  solve_site(Bath.data(), bath_dim, 1, 1);  
}

void prendilag(){
  get_gimp_site_n5(Gimp_mats.data(), 0, 0, wm.data(), Lmats, 0);
}

void prendilasigma(){
  get_sigma_site_n5(Self_mats.data(), 0, 0, wm.data(), Lmats, 0);
}

double dammilenergia(int elemento){
  return Ebands[elemento];
}

double dammilados(int elemento){
  return Dbands[elemento];
}

void spiaccica() {
  chi2_fitgf_single_normal_n5(Delta.data(), delta_dim.data(), Bath.data(), bath_dim, 1, 0, 1);
}

bool cisiamo() {
  return check_convergence(Delta, dmft_error, Nsuccess, Nloop); 
}

} // extern
