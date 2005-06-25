
// Cantera includes
#include "MultiPhase.h"
#include "MultiPhaseEquil.h"

#include "Cabinet.h"
#include "Storage.h"

// Build as a DLL under Windows
#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#else
#define DLL_EXPORT
#endif

// Values returned for error conditions
#define ERR -999
#define DERR -999.999

typedef MultiPhase  mix_t;

template<> Cabinet<mix_t>*    Cabinet<mix_t>::__storage = 0;

inline mix_t* _mix(int i) {
    return Cabinet<mix_t>::cabinet()->item(i);
}

inline ThermoPhase* _th(int n) {
    return Storage::__storage->__thtable[n];
}

static bool checkSpecies(int i, int k) {
    try {
        if (k < 0 || k >= _mix(i)->nSpecies()) 
            throw CanteraError("checkSpecies",
                "illegal species index ("+int2str(k)+") ");
        return true;
    }
    catch (CanteraError) {
        return false;
    }
}

static bool checkElement(int i, int m) {
    try {
        if (m < 0 || m >= _mix(i)->nElements()) 
            throw CanteraError("checkElement",
                "illegal element index ("+int2str(m)+") ");
        return true;
    }
    catch (CanteraError) {
        return false;
    }
}

static bool checkPhase(int i, int n) {
    try {
        if (n < 0 || n >= int(_mix(i)->nPhases())) 
            throw CanteraError("checkPhase",
                "illegal phase index ("+int2str(n)+") ");
        return true;
    }
    catch (CanteraError) {
        return false;
    }
}

namespace Cantera {
    int _equilflag(const char* xy);
}

extern "C" {  

    int DLL_EXPORT mix_new() {
        mix_t* m = new MultiPhase;
        return Cabinet<mix_t>::cabinet()->add(m);
    }

    int DLL_EXPORT mix_del(int i) {
        Cabinet<mix_t>::cabinet()->del(i);
        return 0;
    }

    int DLL_EXPORT mix_copy(int i) {
        return Cabinet<mix_t>::cabinet()->newCopy(i);
    }

    int DLL_EXPORT mix_assign(int i, int j) {
        return Cabinet<mix_t>::cabinet()->assign(i,j);
    }

    int DLL_EXPORT mix_addPhase(int i, int j, double moles) {
        _mix(i)->addPhase(_th(j), moles);
        return 0;
    }

    int DLL_EXPORT mix_nElements(int i) {
        return _mix(i)->nElements();
     }

    int DLL_EXPORT mix_elementIndex(int i, char* name) {
        return _mix(i)->elementIndex(string(name));
    }

    int DLL_EXPORT mix_nSpecies(int i) {
        return _mix(i)->nSpecies();
    }

    int DLL_EXPORT mix_speciesIndex(int i, int k, int p) {
        return _mix(i)->speciesIndex(k, p);
    }

    doublereal DLL_EXPORT mix_nAtoms(int i, int k, int m) {
        bool ok = (checkSpecies(i,k) && checkElement(i,m));
        if (ok) 
            return _mix(i)->nAtoms(k,m);
        else
            return DERR;
    }

    double DLL_EXPORT mix_nPhases(int i) {
        return _mix(i)->nPhases();
    }

    doublereal DLL_EXPORT mix_phaseMoles(int i, int n) {
        if (!checkPhase(i, n)) return DERR;
        return _mix(i)->phaseMoles(n);
    }

    int DLL_EXPORT mix_setPhaseMoles(int i, int n, double v) {
        if (!checkPhase(i, n)) return ERR;
        if (v < 0.0) return -1;
        _mix(i)->setPhaseMoles(n, v);
        return 0;
    }

    int DLL_EXPORT mix_setMoles(int i, int nlen, double* n) {
        try {
            if (nlen < _mix(i)->nSpecies()) 
                throw CanteraError("setMoles","array size too small.");
            _mix(i)->setMoles(n);
            return 0;
        }
        catch (CanteraError) {
            return ERR;
        }
    }


    int DLL_EXPORT mix_setMolesByName(int i, char* n) {
        try {
            _mix(i)->setMolesByName(string(n));
            return 0;
        }
        catch (CanteraError) { return -1; }
    }

    int DLL_EXPORT mix_setTemperature(int i, double t) {
        if (t < 0.0) return -1;
        _mix(i)->setTemperature(t);
        return 0;
    }

    doublereal DLL_EXPORT mix_temperature(int i) {
        return _mix(i)->temperature();
    }

    int DLL_EXPORT mix_setPressure(int i, double p) {
        if (p < 0.0) return -1;
        _mix(i)->setPressure(p);
        return 0;
    }

    doublereal DLL_EXPORT mix_pressure(int i) {
        return _mix(i)->pressure();
    }

    doublereal DLL_EXPORT mix_speciesMoles(int i, int k) {
        if (!checkSpecies(i,k)) return DERR;
        return _mix(i)->speciesMoles(k);
    }

    doublereal DLL_EXPORT mix_elementMoles(int i, int m) {
        if (!checkElement(i,m)) return DERR;
        return _mix(i)->elementMoles(m);
    }

    
    doublereal DLL_EXPORT mix_equilibrate(int i, char* XY, 
        doublereal err, int maxsteps, int maxiter, int loglevel) { 
        try {
            return _mix(i)->equilibrate(_equilflag(XY), 
                err, maxsteps, maxiter, loglevel);
        }
        catch (CanteraError) {
            write_logfile("equil_err.html");
            return DERR;
        }
    }

    int DLL_EXPORT mix_getChemPotentials(int i, int lenmu, double* mu) {
        try {
            if (lenmu < _mix(i)->nSpecies()) 
                throw CanteraError("getChemPotentials","array too small");
            _mix(i)->getChemPotentials(mu);
            return 0;
        }
        catch (CanteraError) {
            return -1;
        }
    }

}
