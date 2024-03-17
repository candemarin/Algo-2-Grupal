#include "fachada_lollapatuza.h"
#include "PuestoDeComida.h"

FachadaLollapatuza::FachadaLollapatuza(const set<Persona> &personas, const map<IdPuesto, aed2_Puesto> &infoPuestos) {
    // TODO: Completar
    map<Nat,PuestoDeComida> puestos = map<Nat,PuestoDeComida>();
    auto itInfoPuestos = infoPuestos.begin();
    while (itInfoPuestos != infoPuestos.end()){
        puestos.insert(make_pair(itInfoPuestos->first, PuestoDeComida(itInfoPuestos->second)));
        ++itInfoPuestos;
    }
    this->lolla = Lollapatuza(puestos,personas);
}

void FachadaLollapatuza::registrarCompra(Persona persona, Producto producto, Nat cant, IdPuesto idPuesto) {
    this->lolla.vender(idPuesto, persona, producto, cant);
}

void FachadaLollapatuza::hackear(Persona persona, Producto producto) {
    this->lolla.hackear(persona, producto);
}

Nat FachadaLollapatuza::gastoTotal(Persona persona) const {
    return this->lolla.gastoTotalDe(persona);
}

Persona FachadaLollapatuza::mayorGastador() const {
    return this->lolla.personaMasGasto();
}

IdPuesto FachadaLollapatuza::menorStock(Producto producto) const {
    return this->lolla.menorStock(producto);
}

const set<Persona> &FachadaLollapatuza::personas() const {
    return this->lolla.personas();
}

Nat FachadaLollapatuza::stockEnPuesto(IdPuesto idPuesto, const Producto &producto) const {
    return this->lolla.getPuestoDeComida(idPuesto).obtenerStock(producto);
}

Nat FachadaLollapatuza::descuentoEnPuesto(IdPuesto idPuesto, const Producto &producto, Nat cantidad) const {
    return this->lolla.getPuestoDeComida(idPuesto).descuento(producto, cantidad);
}

Nat FachadaLollapatuza::gastoEnPuesto(IdPuesto idPuesto, Persona persona) const {
    return this->lolla.getPuestoDeComida(idPuesto).gastosDe(persona);
}

set<IdPuesto> FachadaLollapatuza::idsDePuestos() const {
    set<IdPuesto> puestosID;
    for (auto& par : this->lolla.puestos()) {
        puestosID.insert(get<0>(par));
    }
    return puestosID;
}